#include "gpv_decrypt.h"

void init_console()
{
	AllocConsole();

	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		init_console();
		init_gpv_decrypt();
	}

	if (fdwReason == DLL_PROCESS_DETACH)
	{
	}

	return TRUE;
}
