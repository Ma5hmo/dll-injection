#include "pch.h"

#define PROCESS_TO_INJECT L"notepad.exe"
#define DLL_NAME "mydll.dll"
#define KERNEL32_DLL_PATH "C:\\Windows\\system32\\kernel32.dll"

DWORD GetProcessIdByName(const WCHAR* processName) {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE) {
		printf("createToolhelp32Snapshot err - %d\n", GetLastError());
		return 0;
	}

	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hSnapshot, &pe32)) {
		CloseHandle(hSnapshot);
		printf("process32First err - %d\n", GetLastError());
		return 0;
	}

	do {
		if (strcmp((LPSTR)pe32.szExeFile, (LPSTR)processName) == 0) // should work with WCHAR aswell
		{
			CloseHandle(hSnapshot);
			return pe32.th32ProcessID;
		}
	} while (Process32Next(hSnapshot, &pe32));

	CloseHandle(hSnapshot);
	return 0;
}

int main()
{
	CHAR dllPath[MAX_PATH] = {0};
	// mydll.dll has to be at the same path of the executed file for this to work
	DWORD dllPathSize = GetFullPathNameA(DLL_NAME, MAX_PATH, dllPath, NULL);
	// the dll of LoadLibraryA is Kernel32.dll, we will use it to get the memory address of the function
	PVOID addrLoadLibrary = (PVOID)GetProcAddress(GetModuleHandleA(KERNEL32_DLL_PATH), "LoadLibraryA");

	printf("getting notepad.exe pid...\n");
	DWORD pid = 0;
	do
	{
		pid = GetProcessIdByName(PROCESS_TO_INJECT);
	} while (pid == 0); // until notepad.exe gets opened
	printf("found! PID=%d\n", pid);

	HANDLE proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	// allocate memory inside notepad.exe
	PVOID memAddr = (PVOID)VirtualAllocEx(proc, NULL, dllPathSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (NULL == memAddr) {
		printf("memaddr err - %d\n", GetLastError());
		return 0;
	}
	// write the DLL path to the memory allocated
	BOOL check = WriteProcessMemory(proc, memAddr, dllPath, dllPathSize, NULL);
	if (0 == check) {
		printf("writeToProcMem err - %d\n", GetLastError());
		return 0;
	}

	// create a thread that runs LoadLibrary so we load our own DLL into notepad.exe
	HANDLE hRemote = CreateRemoteThread(proc, NULL, NULL,
		(LPTHREAD_START_ROUTINE)addrLoadLibrary, // LoadLibrary function from Kernel32.dll
		memAddr, // memory address of our dll name inside the remote process memory
		NULL, NULL);
	if (NULL == hRemote) {
		printf("thread err - %d\n", GetLastError());
		return 0;
	}
	printf("Created thread successfully.\n");
	WaitForSingleObject(hRemote, INFINITE);
	CloseHandle(hRemote);
	return 0;
}
