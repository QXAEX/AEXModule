#include "../../h/api/data/NTDLL.h"
constexpr auto NTDLL_ERROR = -1;
HMODULE  NTDLL::Ntdll = GetModuleHandle(L"NTDLL.dll");
NTSTATUS NTDLL::ZwOpenProcess(PHANDLE ProcessHandle, ACCESS_MASK DesiredAccess, NTDLL::POBJECT_ATTRIBUTES ObjectAttributes, NTDLL::PCLIENT_ID ClientId)
{
	typedef NTSTATUS(NTAPI* call)(PHANDLE ProcessHandle, ACCESS_MASK DesiredAccess, NTDLL::POBJECT_ATTRIBUTES ObjectAttributes, NTDLL::PCLIENT_ID ClientId);
	static call callBack = (call)GetProcAddress(Ntdll, "ZwOpenProcess");
	if (callBack)
		return callBack(ProcessHandle, DesiredAccess, ObjectAttributes, ClientId);
	else
		return NTDLL_ERROR;
}

NTSTATUS NTDLL::ZwClose(HANDLE ProcessHandle)
{
	typedef NTSTATUS(NTAPI* call)(HANDLE ProcessHandle);
	static call callBack = (call)GetProcAddress(Ntdll, "ZwClose");
	if (callBack)
		return callBack(ProcessHandle);
	else
		return NTDLL_ERROR;
}

NTSTATUS NTDLL::ZwReadVirtualMemory(HANDLE ProcessHandle, LPCVOID address, LPVOID value, SIZE_T Rlength, SIZE_T* Olength)
{
	typedef NTSTATUS(NTAPI* call)(HANDLE ProcessHandle, LPCVOID address, LPVOID value, SIZE_T Rlength, SIZE_T* Olength);
	static call callBack = (call)GetProcAddress(Ntdll, "ZwReadVirtualMemory");
	if (callBack)
		return callBack(ProcessHandle, address, value, Rlength, Olength);
	else
		return NTDLL_ERROR;
}

NTSTATUS NTDLL::ZwWriteVirtualMemory(HANDLE ProcessHandle, LPCVOID address, LPVOID value, SIZE_T Rlength, SIZE_T* Olength)
{
	typedef NTSTATUS(NTAPI* call)(HANDLE ProcessHandle, LPCVOID address, LPVOID value, SIZE_T Rlength, SIZE_T* Olength);
	static call callBack = (call)GetProcAddress(Ntdll, "ZwWriteVirtualMemory");
	if (callBack)
		return callBack(ProcessHandle, address, value, Rlength, Olength);
	else
		return NTDLL_ERROR;
}

NTSTATUS NTDLL::ZwProtectVirtualMemory(HANDLE ProcessHandle, LPCVOID address, ULONG_PTR* size, DWORD protection, PDWORD oldProtection)
{
	typedef NTSTATUS(NTAPI* call)(HANDLE ProcessHandle, LPCVOID address, ULONG_PTR* size, ULONG protection, PULONG oldProtection);
	static call callBack = (call)GetProcAddress(Ntdll, "ZwProtectVirtualMemory");
	if (callBack)
		return callBack(ProcessHandle, address, size, protection, oldProtection);
	else
		return NTDLL_ERROR;
}

NTSTATUS NTDLL::ZwFreeVirtualMemory(HANDLE ProcessHandle, PVOID BaseAddress, DWORD* RegionSize, size_t FreeType)
{
	typedef NTSTATUS(NTAPI* call)(HANDLE ProcessHandle, PVOID BaseAddress, DWORD* RegionSize, size_t FreeType);
	static call callBack = (call)GetProcAddress(Ntdll, "ZwFreeVirtualMemory");
	if (callBack)
		return callBack(ProcessHandle, BaseAddress, RegionSize, FreeType);
	else
		return NTDLL_ERROR;
}

NTSTATUS NTDLL::ZwAllocateVirtualMemory(HANDLE ProcessHandle, PVOID* BaseAddress, size_t ZeroBits, DWORD* RegionSize, size_t AllocationType, size_t Protect)
{
	typedef NTSTATUS(NTAPI* call)(HANDLE ProcessHandle, PVOID* BaseAddress, size_t ZeroBits, DWORD* RegionSize, size_t AllocationType, size_t Protect);
	static call callBack = (call)GetProcAddress(Ntdll, "ZwAllocateVirtualMemory");
	if (callBack)
		return callBack(ProcessHandle, BaseAddress, ZeroBits, RegionSize, AllocationType, Protect);
	else
		return NTDLL_ERROR;
}

NTSTATUS NTDLL::ZwQueryVirtualMemory(HANDLE ProcessHandle, LPCVOID address, MEMORY_INFORMATION_CLASS MemoryInformationClass, PVOID MemoryInformation, SIZE_T MemoryInformationLength, PSIZE_T ReturnLength)
{
	typedef NTSTATUS(NTAPI* call)(HANDLE ProcessHandle, LPCVOID address, MEMORY_INFORMATION_CLASS MemoryInformationClass, PVOID MemoryInformation, SIZE_T MemoryInformationLength, PSIZE_T ReturnLength);
	static call callBack = (call)GetProcAddress(Ntdll, "ZwQueryVirtualMemory");
	if (callBack)
		return callBack(ProcessHandle, address, MemoryInformationClass, MemoryInformation, MemoryInformationLength, ReturnLength);
	else
		return NTDLL_ERROR;
}

NTSTATUS NTDLL::ZwSuspendThread(HANDLE ThreadHandle, PULONG PreviousSuspendCount)
{
	typedef NTSTATUS(NTAPI* call)(HANDLE ThreadHandle, PULONG PreviousSuspendCount);
	static call callBack = (call)GetProcAddress(Ntdll, "ZwSuspendThread");
	if (callBack)
		return callBack(ThreadHandle, PreviousSuspendCount);
	else
		return NTDLL_ERROR;
}

NTSTATUS NTDLL::ZwResumeThread(HANDLE ThreadHandle, PULONG PreviousSuspendCount)
{
	typedef NTSTATUS(NTAPI* call)(HANDLE ThreadHandle, PULONG PreviousSuspendCount);
	static call callBack = (call)GetProcAddress(Ntdll, "ZwResumeThread");
	if (callBack)
		return callBack(ThreadHandle, PreviousSuspendCount);
	else
		return NTDLL_ERROR;
}

NTSTATUS NTDLL::ZwGetContextThread(HANDLE ThreadHandle, PCONTEXT Context)
{
	typedef NTSTATUS(NTAPI* call)(HANDLE ThreadHandle, PCONTEXT Context);
	static call callBack = (call)GetProcAddress(Ntdll, "ZwGetContextThread");
	if (callBack)
		return callBack(ThreadHandle, Context);
	else
		return NTDLL_ERROR;
}

NTSTATUS NTDLL::ZwSetContextThread(HANDLE ThreadHandle, PCONTEXT Context)
{
	typedef NTSTATUS(NTAPI* call)(HANDLE ThreadHandle, PCONTEXT Context);
	static call callBack = (call)GetProcAddress(Ntdll, "ZwSetContextThread");
	if (callBack)
		return callBack(ThreadHandle, Context);
	else
		return NTDLL_ERROR;
}

NTSTATUS NTDLL::ZwOpenThread(PHANDLE ThreadHandle, ACCESS_MASK DesiredAccess, NTDLL::POBJECT_ATTRIBUTES ObjectAttributes, NTDLL::PCLIENT_ID ClientId)
{
	typedef NTSTATUS(NTAPI* call)(PHANDLE ThreadHandle, ACCESS_MASK DesiredAccess, NTDLL::POBJECT_ATTRIBUTES ObjectAttributes, NTDLL::PCLIENT_ID ClientId);
	static call callBack = (call)GetProcAddress(Ntdll, "ZwOpenThread");
	if (callBack)
		return callBack(ThreadHandle, DesiredAccess, ObjectAttributes, ClientId);
	else
		return NTDLL_ERROR;
}
