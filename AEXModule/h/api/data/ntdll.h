#pragma once
#include <Windows.h>
namespace NTDLL {
	extern HMODULE Ntdll;// ntdll.dll模块句柄
	//unicode字符
	typedef struct UNICODE_STRING {
		USHORT Length;// 长度
		USHORT MaximumLength;// 最大长度
		PWSTR Buffer;// 缓冲区
	} *PUNICODE_STRING;
	//对象属性
	typedef struct OBJECT_ATTRIBUTES {
		ULONG Length;// 长度
		HANDLE RootDirectory;// 根目录
		PUNICODE_STRING ObjectName;// 对象名
		ULONG Attributes;// 属性
		PVOID SecurityDescriptor;// 安全描述符
		PVOID SecurityQualityOfService;// 安全质量服务
	} *POBJECT_ATTRIBUTES;
	//客户端ID
	typedef struct CLIENT_ID {
		HANDLE UniqueProcess;// 进程ID
		HANDLE UniqueThread;// 线程ID
	} *PCLIENT_ID;
	typedef struct _MEMORY_BASIC_INFORMATION {
		PVOID BaseAddress;
		PVOID AllocationBase;
		DWORD AllocationProtect;
		SIZE_T RegionSize;
		DWORD State;
		DWORD Protect;
		DWORD Type;
	} MEMORY_BASIC_INFORMATION, * PMEMORY_BASIC_INFORMATION;
	typedef enum _MEMORY_INFORMATION_CLASS {
		MemoryBasicInformation,
		MemoryWorkingSetInformation,
		MemoryMappedFilenameInformation,
		MemoryRegionInformation,
		MemoryWorkingSetExInformation,
		MemorySharedCommitInformation,
		MemoryImageInformation,
		MemoryRegionInformationEx,
		MemoryPrivilegedBasicInformation,
		MemoryEnclaveInformation,
		MemoryBasicInformationCapped,
		MemoryPhysicalContiguityInformation,
		MemoryPhysicalContiguityInformationEx,
		MemoryStateInformation,
		MemoryProtectionInformation,
		MemoryTypeInformation,
		MemoryBasicInformationEx,
		MemoryMaximumInformation
	} MEMORY_INFORMATION_CLASS;
	/*
	*  打开进程
	* @param ProcessHandle 进程句柄
	* @param DesiredAccess 访问权限
	* @param ObjectAttributes 对象属性
	* @param ClientId 客户端ID
	* @return 返回状态
	*/
	NTSTATUS ZwOpenProcess(PHANDLE ProcessHandle, ACCESS_MASK DesiredAccess, NTDLL::POBJECT_ATTRIBUTES ObjectAttributes, NTDLL::PCLIENT_ID ClientId);
	/*
	* 关闭进程
	* @param ProcessHandle 进程句柄
	* @return 返回状态
	*/
	NTSTATUS ZwClose(HANDLE ProcessHandle);
	/*
	* 读取内存
	* @param ProcessHandle 进程句柄
	* @param address 基址
	* @param value 缓冲区
	* @param Rlength 读取长度
	* @param Olength 实际读取长度
	* @return 返回状态
	*/
	NTSTATUS ZwReadVirtualMemory(HANDLE ProcessHandle, LPCVOID address, LPVOID value, SIZE_T Rlength, SIZE_T* Olength);
	/*
	* 写入内存
	* @param ProcessHandle 进程句柄
	* @param address 基址
	* @param value 缓冲区
	* @param Rlength 写入长度
	* @param Olength 实际写入长度
	* @return 返回状态
	*/
	NTSTATUS ZwWriteVirtualMemory(HANDLE ProcessHandle, LPCVOID address, LPVOID value, SIZE_T Rlength, SIZE_T* Olength);
	/*
	* 保护内存
	* @param ProcessHandle 进程句柄
	* @param address 基址
	* @param size 大小
	* @param protection 保护级别
	* @param oldProtection 旧保护级别
	* @return 返回状态
	*/
	NTSTATUS ZwProtectVirtualMemory(HANDLE ProcessHandle, LPCVOID address, ULONG_PTR* size, DWORD protection, PDWORD oldProtection);
	/*
	* 释放内存
	* @param ProcessHandle 进程句柄
	* @param BaseAddress 基址
	* @param RegionSize 大小
	* @param FreeType 释放类型
	* @return 返回状态
	*/
	NTSTATUS ZwFreeVirtualMemory(HANDLE ProcessHandle, PVOID BaseAddress, DWORD* RegionSize, size_t FreeType);
	/*
	* 分配内存
	* @param ProcessHandle 进程句柄
	* @param BaseAddress 基址
	* @param ZeroBits 置零位数
	* @param RegionSize 大小
	* @param AllocationType 分配类型
	* @param Protect 保护级别
	* @return 返回状态
	*/
	NTSTATUS ZwAllocateVirtualMemory(HANDLE ProcessHandle, PVOID* BaseAddress, size_t ZeroBits, DWORD* RegionSize, size_t AllocationType, size_t Protect);
	/*
	* 查询内存信息
	* @param ProcessHandle 进程句柄
	* @param address 基址
	* @param MemoryInformationClass 信息类别
	* @param MemoryInformation 信息
	* @param MemoryInformationLength 信息长度
	* @param ReturnLength 返回长度
	* @return 返回状态
	*/
	NTSTATUS ZwQueryVirtualMemory(HANDLE ProcessHandle, LPCVOID address, MEMORY_INFORMATION_CLASS MemoryInformationClass, PVOID MemoryInformation, SIZE_T MemoryInformationLength, PSIZE_T ReturnLength);
	/*
	* 挂起线程
	* @param ThreadHandle 线程句柄
	* @param PreviousSuspendCount 挂起次数
	* @return 返回状态
	*/
	NTSTATUS ZwSuspendThread(HANDLE ThreadHandle, PULONG PreviousSuspendCount);
	/*
	* 回复线程
	* @param ThreadHandle 线程句柄
	* @param PreviousSuspendCount 挂起次数
	* @return 返回状态
	*/
	NTSTATUS ZwResumeThread(HANDLE ThreadHandle, PULONG PreviousSuspendCount);
	/*
	* 获取线程上下文
	* @param ThreadHandle 线程句柄
	* @param Context 线程上下文
	* @return 返回状态
	*/
	NTSTATUS ZwGetContextThread(HANDLE ThreadHandle, PCONTEXT Context);
	/*
	* 设置线程上下文
	* @param ThreadHandle 线程句柄
	* @param Context 线程上下文
	* @return 返回状态
	*/
	NTSTATUS ZwSetContextThread(HANDLE ThreadHandle, PCONTEXT Context);
	/*
	* 打开线程
	* @param ThreadHandle 线程句柄
	* @param DesiredAccess 访问权限
	* @param ObjectAttributes 对象属性
	* @param ClientId 客户端ID
	* @return 返回状态
	*/
	NTSTATUS ZwOpenThread(PHANDLE ThreadHandle, ACCESS_MASK DesiredAccess, NTDLL::POBJECT_ATTRIBUTES ObjectAttributes, NTDLL::PCLIENT_ID ClientId);
};