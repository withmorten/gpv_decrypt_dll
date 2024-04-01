#include "gpv_decrypt.h"

#include <MemoryMgr.h>
#include <Trampoline.h>
#include <Patterns.h>

using namespace hook;
using namespace Memory::VP;

typedef int32_t StoragePoint;

struct String
{
	char *m_data;
	size_t m_Length;
	size_t m_AllocatedLength;
};

struct FilePath
{
	StoragePoint mStoragePoint;
	String mFileName;
	String unk1;
	bool unk2;
};

char campaign_name[1024];
char campaign_gpv_path[1024];
char campaign_decrypt_path[1024];
bool is_gpv;
bool pompeii;

struct Campaign;

typedef Campaign *(*CampaignConstructor)(Campaign *, uint64_t, FilePath *, uint64_t);
CampaignConstructor Campaign__Constructor;

struct Campaign
{
	Campaign *Constructor(uint64_t pResult, FilePath *filePath, uint64_t pDlcCampaign)
	{
		strscpy(campaign_name, filePath->mFileName.m_data);

		if (streq(campaign_name, "cam0.aoe2campaign")
			|| streq(campaign_name, "cam1.aoe2campaign")
			|| streq(campaign_name, "ccam1.aoe2campaign")
			|| streq(campaign_name, "cscam1.aoe2campaign")
			|| streq(campaign_name, "cscam2.aoe2campaign")
			|| streq(campaign_name, "cscam3.aoe2campaign")
			|| streq(campaign_name, "eecam1.aoe2campaign")
			|| streq(campaign_name, "eecam2.aoe2campaign")
			|| streq(campaign_name, "eecam3.aoe2campaign")
			|| streq(campaign_name, "incam1.aoe2campaign")
			|| streq(campaign_name, "incam2.aoe2campaign")
			|| streq(campaign_name, "incam3.aoe2campaign")
			|| streq(campaign_name, "wecam1.aoe2campaign")
			|| streq(campaign_name, "wecam2.aoe2campaign")
			|| streq(campaign_name, "wecam3.aoe2campaign"))
		{
			is_gpv = true;
			pompeii = false;
		}
		else if (streq(campaign_name, "aoecam1.aoe2campaign")
			|| streq(campaign_name, "aoecam2.aoe2campaign")
			|| streq(campaign_name, "aoecam3.aoe2campaign")
			|| streq(campaign_name, "aoecam4.aoe2campaign")
			|| streq(campaign_name, "pcam1.aoe2campaign")
			|| streq(campaign_name, "pcam2.aoe2campaign")
			|| streq(campaign_name, "pcam3.aoe2campaign"))
		{
			is_gpv = true;
			pompeii = true;
		}
		else
		{
			strscpy(campaign_name, "");
			is_gpv = false;
			pompeii = false;
		}

		if (is_gpv)
		{
			strscpy(campaign_gpv_path, pompeii ? "modes\\Pompeii\\resources\\_common\\campaign\\" : "resources\\_common\\campaign\\");
			strscat(campaign_gpv_path, campaign_name);
			strscat(campaign_gpv_path, ".gpv");

			strscpy(campaign_decrypt_path, "gpv_decrypt\\");
			strscat(campaign_decrypt_path, campaign_name);

			printf("%s\n", campaign_gpv_path);
		}

		return Campaign__Constructor(this, pResult, filePath, pDlcCampaign);
	}
};

typedef void (*gpv_decrypt)(uint64_t, byte *, long);
gpv_decrypt gpv_decrypt_orig;

void gpv_decrypt_hook(uint64_t magic, byte *data, long size)
{
	gpv_decrypt_orig(magic, data, size);

	FILE *f = fopen(campaign_decrypt_path, "wb");
	fwrite(data, size, 1, f);
	fclose(f);
}

uintptr_t CAMPAIGN_CTOR_ADDRESS;
uintptr_t CAMPAIGN_CTOR_CALLLOC;
uintptr_t GPV_DECRYPT_ADDRESS;
uintptr_t GPV_DECRYPT_CALLLOC;

bool fill_addresses(char *version_str)
{
	if (streq(version_str, "101.102.42346.0 107882 Final Steam 20240312.01 ADO"))
	{
		CAMPAIGN_CTOR_ADDRESS = 0x0000000140EDD430;
		CAMPAIGN_CTOR_CALLLOC = 0x0000000141012769;
		GPV_DECRYPT_ADDRESS = 0x0000000141718DD0;
		GPV_DECRYPT_CALLLOC = 0x0000000140EDDD7D;

		return true;
	}
	
	if (streq(version_str, "101.102.43233.0 108769 Final Preview Steam 20240321.02 ADO"))
	{
		CAMPAIGN_CTOR_ADDRESS = 0x0000000140EDD030;
		CAMPAIGN_CTOR_CALLLOC = 0x0000000141012399;
		GPV_DECRYPT_ADDRESS = 0x0000000141719200;
		GPV_DECRYPT_CALLLOC = 0x0000000140EDD97D;

		return true;
	}

	if (streq(version_str, "101.102.43233.0 108769 Final Steam 20240321.03 ADO"))
	{
		CAMPAIGN_CTOR_ADDRESS = 0x0000000140EDECB0;
		CAMPAIGN_CTOR_CALLLOC = 0x0000000141013FF9;
		GPV_DECRYPT_ADDRESS = 0x000000014171AF90;
		GPV_DECRYPT_CALLLOC = 0x0000000140EDF5FD;

		return true;
	}

	return false;
}

char *strstart(char *str)
{
	char *ret; for (ret = str; *ret; ret--); return ++ret;
}

void init_gpv_decrypt()
{
	CreateDirectory("gpv_decrypt", NULL);

	bool version_found = false;

	{
		auto ver_string_pattern = pattern("20 46 69 6E 61 6C 20 53 74 65 61 6D 20"); // " Final Steam "
		auto ver_string_pattern_pup = pattern("20 46 69 6E 61 6C 20 50 72 65 76 69 65 77 20 53 74 65 61 6D 20"); // " Final Preview Steam "

		if (ver_string_pattern.count(1).size() == 1)
		{
			version_found = fill_addresses(strstart((char *)ver_string_pattern.get_first(0)));
		}
		else if (ver_string_pattern_pup.count(1).size() == 1)
		{
			version_found = fill_addresses(strstart((char *)ver_string_pattern_pup.get_first(0)));
		}
	}

	if (version_found)
	{
		printf("version found, hooking ...\n");

		// hook the constructor to get some initial data
		{
			Campaign__Constructor = (CampaignConstructor)CAMPAIGN_CTOR_ADDRESS;

			void *address = (void *)CAMPAIGN_CTOR_CALLLOC;
			auto trampoline = Trampoline::MakeTrampoline(address);

			InjectHook(address, trampoline->Jump(&Campaign::Constructor));
		}

		// then hook the function that decrypts the gpv ...
		{
			gpv_decrypt_orig = (gpv_decrypt)GPV_DECRYPT_ADDRESS;

			void *address = (void *)GPV_DECRYPT_CALLLOC;
			auto trampoline = Trampoline::MakeTrampoline(address);

			InjectHook(address, trampoline->Jump(gpv_decrypt_hook));
		}
	}
	else
	{
		printf("unknown version\n");
	}
}
