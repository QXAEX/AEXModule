#include "../../h/api/data/kernel32.h"
HMODULE KERNEL32::Kernel32 = GetModuleHandle(L"kernel32.dll");
HANDLE WINAPI KERNEL32::CreateToolhelp32Snapshot(DWORD dwFlags, DWORD th32ProcessID) {
	typedef HANDLE(WINAPI* call)(DWORD dwFlags, DWORD th32ProcessID);
	static call callBack = (call)GetProcAddress(KERNEL32::Kernel32, "CreateToolhelp32Snapshot");
	return callBack(dwFlags, th32ProcessID);
}

BOOL WINAPI KERNEL32::Thread32First(HANDLE hSnapshot, PTHREADENTRY32 lpte)
{
	typedef BOOL(WINAPI* call)(HANDLE hSnapshot, PTHREADENTRY32 lpte);
	static call callBack = (call)GetProcAddress(KERNEL32::Kernel32, "Thread32First");
	return callBack(hSnapshot, lpte);
}

BOOL WINAPI KERNEL32::Thread32Next(HANDLE hSnapshot, PTHREADENTRY32 lpte)
{
	typedef BOOL(WINAPI* call)(HANDLE hSnapshot, PTHREADENTRY32 lpte);
	static call callBack = (call)GetProcAddress(KERNEL32::Kernel32, "Thread32Next");
	return callBack(hSnapshot, lpte);
}
