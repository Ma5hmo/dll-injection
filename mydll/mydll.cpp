#include "pch.h"
#define DLL_EXPORT
#include "mydll.h"

extern "C"
{
	void ShowMessage()
	{
		MessageBox(NULL, L"DLL attached successfully B)", L"Hacked!", MB_RIGHT);
	}
}

BOOL APIENTRY DllMain(HANDLE hModule, // Handle to DLL module
	DWORD ul_reason_for_call,
	LPVOID lpReserved) // Reserved
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		ShowMessage();
	}
	return TRUE;
}