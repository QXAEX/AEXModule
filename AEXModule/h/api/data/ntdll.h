#pragma once
#include <Windows.h>
namespace NTDLL {
	extern HMODULE Ntdll;// ntdll.dllģ����
	//unicode�ַ�
	typedef struct UNICODE_STRING {
		USHORT Length;// ����
		USHORT MaximumLength;// ��󳤶�
		PWSTR Buffer;// ������
	} *PUNICODE_STRING;
	//��������
	typedef struct OBJECT_ATTRIBUTES {
		ULONG Length;// ����
		HANDLE RootDirectory;// ��Ŀ¼
		PUNICODE_STRING ObjectName;// ������
		ULONG Attributes;// ����
		PVOID SecurityDescriptor;// ��ȫ������
		PVOID SecurityQualityOfService;// ��ȫ��������
	} *POBJECT_ATTRIBUTES;
	//�ͻ���ID
	typedef struct CLIENT_ID {
		HANDLE UniqueProcess;// ����ID
		HANDLE UniqueThread;// �߳�ID
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
	*  �򿪽���
	* @param ProcessHandle ���̾��
	* @param DesiredAccess ����Ȩ��
	* @param ObjectAttributes ��������
	* @param ClientId �ͻ���ID
	* @return ����״̬
	*/
	NTSTATUS ZwOpenProcess(PHANDLE ProcessHandle, ACCESS_MASK DesiredAccess, NTDLL::POBJECT_ATTRIBUTES ObjectAttributes, NTDLL::PCLIENT_ID ClientId);
	/*
	* �رս���
	* @param ProcessHandle ���̾��
	* @return ����״̬
	*/
	NTSTATUS ZwClose(HANDLE ProcessHandle);
	/*
	* ��ȡ�ڴ�
	* @param ProcessHandle ���̾��
	* @param address ��ַ
	* @param value ������
	* @param Rlength ��ȡ����
	* @param Olength ʵ�ʶ�ȡ����
	* @return ����״̬
	*/
	NTSTATUS ZwReadVirtualMemory(HANDLE ProcessHandle, LPCVOID address, LPVOID value, SIZE_T Rlength, SIZE_T* Olength);
	/*
	* д���ڴ�
	* @param ProcessHandle ���̾��
	* @param address ��ַ
	* @param value ������
	* @param Rlength д�볤��
	* @param Olength ʵ��д�볤��
	* @return ����״̬
	*/
	NTSTATUS ZwWriteVirtualMemory(HANDLE ProcessHandle, LPCVOID address, LPVOID value, SIZE_T Rlength, SIZE_T* Olength);
	/*
	* �����ڴ�
	* @param ProcessHandle ���̾��
	* @param address ��ַ
	* @param size ��С
	* @param protection ��������
	* @param oldProtection �ɱ�������
	* @return ����״̬
	*/
	NTSTATUS ZwProtectVirtualMemory(HANDLE ProcessHandle, LPCVOID address, ULONG_PTR* size, DWORD protection, PDWORD oldProtection);
	/*
	* �ͷ��ڴ�
	* @param ProcessHandle ���̾��
	* @param BaseAddress ��ַ
	* @param RegionSize ��С
	* @param FreeType �ͷ�����
	* @return ����״̬
	*/
	NTSTATUS ZwFreeVirtualMemory(HANDLE ProcessHandle, PVOID BaseAddress, DWORD* RegionSize, size_t FreeType);
	/*
	* �����ڴ�
	* @param ProcessHandle ���̾��
	* @param BaseAddress ��ַ
	* @param ZeroBits ����λ��
	* @param RegionSize ��С
	* @param AllocationType ��������
	* @param Protect ��������
	* @return ����״̬
	*/
	NTSTATUS ZwAllocateVirtualMemory(HANDLE ProcessHandle, PVOID* BaseAddress, size_t ZeroBits, DWORD* RegionSize, size_t AllocationType, size_t Protect);
	/*
	* ��ѯ�ڴ���Ϣ
	* @param ProcessHandle ���̾��
	* @param address ��ַ
	* @param MemoryInformationClass ��Ϣ���
	* @param MemoryInformation ��Ϣ
	* @param MemoryInformationLength ��Ϣ����
	* @param ReturnLength ���س���
	* @return ����״̬
	*/
	NTSTATUS ZwQueryVirtualMemory(HANDLE ProcessHandle, LPCVOID address, MEMORY_INFORMATION_CLASS MemoryInformationClass, PVOID MemoryInformation, SIZE_T MemoryInformationLength, PSIZE_T ReturnLength);
	/*
	* �����߳�
	* @param ThreadHandle �߳̾��
	* @param PreviousSuspendCount �������
	* @return ����״̬
	*/
	NTSTATUS ZwSuspendThread(HANDLE ThreadHandle, PULONG PreviousSuspendCount);
	/*
	* �ظ��߳�
	* @param ThreadHandle �߳̾��
	* @param PreviousSuspendCount �������
	* @return ����״̬
	*/
	NTSTATUS ZwResumeThread(HANDLE ThreadHandle, PULONG PreviousSuspendCount);
	/*
	* ��ȡ�߳�������
	* @param ThreadHandle �߳̾��
	* @param Context �߳�������
	* @return ����״̬
	*/
	NTSTATUS ZwGetContextThread(HANDLE ThreadHandle, PCONTEXT Context);
	/*
	* �����߳�������
	* @param ThreadHandle �߳̾��
	* @param Context �߳�������
	* @return ����״̬
	*/
	NTSTATUS ZwSetContextThread(HANDLE ThreadHandle, PCONTEXT Context);
	/*
	* ���߳�
	* @param ThreadHandle �߳̾��
	* @param DesiredAccess ����Ȩ��
	* @param ObjectAttributes ��������
	* @param ClientId �ͻ���ID
	* @return ����״̬
	*/
	NTSTATUS ZwOpenThread(PHANDLE ThreadHandle, ACCESS_MASK DesiredAccess, NTDLL::POBJECT_ATTRIBUTES ObjectAttributes, NTDLL::PCLIENT_ID ClientId);
};