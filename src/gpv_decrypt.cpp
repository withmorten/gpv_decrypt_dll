#include "gpv_decrypt.h"

#include <MemoryMgr.h>
#include <Trampoline.h>
#include <Patterns.h>

long fsize(FILE *stream)
{
	long pos = ftell(stream);
	fseek(stream, 0, SEEK_END);
	long size = ftell(stream);
	fseek(stream, pos, SEEK_SET);
	return size;
}

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
long campaign_size;

struct Campaign;

typedef Campaign *(*CampaignConstructor)(Campaign *, uint64_t, FilePath *, uint64_t);
CampaignConstructor Campaign__Constructor = (CampaignConstructor)CAMPAIGN_CTOR_ADDRESS;

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
			campaign_size = 0;
		}

		if (is_gpv)
		{
			// printf("%s\n", campaign_name);

			strscpy(campaign_gpv_path, pompeii ? "modes\\Pompeii\\resources\\_common\\campaign\\" : "resources\\_common\\campaign\\");
			strscat(campaign_gpv_path, campaign_name);
			strscat(campaign_gpv_path, ".gpv");

			strscpy(campaign_decrypt_path, "gpv_decrypt\\");
			strscat(campaign_decrypt_path, campaign_name);

			FILE *f = fopen(campaign_gpv_path, "rb");
			campaign_size = fsize(f) - 4 - sizeof(size_t); // sizeof(magic)
			fclose(f);

			printf("%s, %d\n", campaign_gpv_path, campaign_size);
		}

		return Campaign__Constructor(this, pResult, filePath, pDlcCampaign);
	}
};

typedef void (*gpv_decrypt)(uint64_t, byte *, long);
gpv_decrypt gpv_decrypt_orig = (gpv_decrypt)GPV_DECRYPT_ADDRESS;

void gpv_decrypt_hook(uint64_t magic, byte *data, long size)
{
	gpv_decrypt_orig(magic, data, size);

	FILE *f = fopen(campaign_decrypt_path, "wb");
	fwrite(data, size, 1, f);
	fclose(f);
}

void init_gpv_decrypt()
{
	// hook the constructor to get some initial data
	{
		void *address = (void *)CAMPAIGN_CTOR_CALLLOC;
		auto trampoline = Trampoline::MakeTrampoline(address);

		InjectHook(address, trampoline->Jump(&Campaign::Constructor));
	}

	// then hook the function that decrypts the gpv ...
	{
		void *address = (void *)GPV_DECRYPT_CALLLOC;
		auto trampoline = Trampoline::MakeTrampoline(address);

		InjectHook(address, trampoline->Jump(gpv_decrypt_hook));
	}
}
