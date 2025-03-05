#pragma once
#include "Byteset.h"
#include "Text.h"
#include <iostream>
#include "./Memory/R3.h"
#include "./Memory/R3HOOK.h"
#include "./Memory/EmbeddedCall.h"
#include "./Memory/VEHHook.h"

/*
* �����ڴ棨ע�����Ա���У�
*/
namespace Memory {
	class API {
	public:
		typedef struct _UNICODE_STRING {
			USHORT Length;
			USHORT MaximumLength;
			PWSTR Buffer;
		} UNICODE_STRING, * PUNICODE_STRING;
		typedef struct _OBJECT_ATTRIBUTES {
			ULONG Length;
			HANDLE RootDirectory;
			PUNICODE_STRING ObjectName;
			ULONG Attributes;
			PVOID SecurityDescriptor;
			PVOID SecurityQualityOfService;
		} OBJECT_ATTRIBUTES, * POBJECT_ATTRIBUTES;
		typedef struct _CLIENT_ID {
			HANDLE UniqueProcess;
			HANDLE UniqueThread;
		} CLIENT_ID, * PCLIENT_ID;
		typedef NTSTATUS(NTAPI* zwOpenProcess)(PHANDLE ProcessHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PCLIENT_ID ClientId);
		typedef NTSTATUS(NTAPI* zwWriteVirtualMemory)(HANDLE ProcessHandle, LPCVOID address, LPVOID value, SIZE_T Rlength, SIZE_T* Olength);
		typedef NTSTATUS(NTAPI* zwReadVirtualMemory)(HANDLE ProcessHandle, LPCVOID address, LPVOID value, SIZE_T Rlength, SIZE_T* Olength);
		typedef NTSTATUS(NTAPI* zwProtectVirtualMemory)(HANDLE ProcessHandle, LPCVOID address, size_t* size, DWORD protection, PDWORD oldProtection);
		typedef NTSTATUS(NTAPI* zwAllocateVirtualMemory)(HANDLE ProcessHandle, size_t* BaseAddress, size_t ZeroBits, size_t* RegionSize, size_t AllocationType, size_t Protect);
		typedef NTSTATUS(NTAPI* zwFreeVirtualMemory)(HANDLE ProcessHandle, size_t* BaseAddress, size_t* RegionSize, size_t FreeType);
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
		typedef NTSTATUS(NTAPI* zwQueryVirtualMemory)(HANDLE ProcessHandle, LPCVOID address, MEMORY_INFORMATION_CLASS MemoryInformationClass, PVOID MemoryInformation, SIZE_T MemoryInformationLength, PSIZE_T ReturnLength);
		typedef NTSTATUS(NTAPI* zwClose)(HANDLE Handle);
		typedef HANDLE(WINAPI* createToolhelp32Snapshot)(DWORD, HANDLE);
		typedef BOOL(WINAPI* thread32First)(HANDLE hSnapshot, PTHREADENTRY32 lpte);
		typedef BOOL(WINAPI* thread32Next)(HANDLE hSnapshot, PTHREADENTRY32 lpte);
		typedef void (NTAPI* rtlWow64GetProcessMachines)(HANDLE, PUSHORT, PUSHORT);
		typedef NTSTATUS(NTAPI* zwSuspendThread)(HANDLE ThreadHandle, PULONG PreviousSuspendCount);
		typedef NTSTATUS(NTAPI* zwResumeThread)(HANDLE ThreadHandle, PULONG PreviousSuspendCount);
		typedef NTSTATUS(NTAPI* zwGetContextThread)(HANDLE ThreadHandle, PCONTEXT Context);
		typedef NTSTATUS(NTAPI* zwSetContextThread)(HANDLE ThreadHandle, PCONTEXT Context);
		typedef NTSTATUS(NTAPI* zwOpenThread)(PHANDLE ThreadHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PCLIENT_ID ClientId);
	public:
		zwOpenProcess ZwOpenProcess;
		zwWriteVirtualMemory ZwWriteVirtualMemory;
		zwReadVirtualMemory ZwReadVirtualMemory;
		zwProtectVirtualMemory ZwProtectVirtualMemory;
		zwAllocateVirtualMemory ZwAllocateVirtualMemory;
		zwFreeVirtualMemory ZwFreeVirtualMemory;
		zwQueryVirtualMemory ZwQueryVirtualMemory;
		createToolhelp32Snapshot CreateToolhelp32Snapshot;
		thread32First Thread32First;
		thread32Next Thread32Next;
		rtlWow64GetProcessMachines RtlWow64GetProcessMachines;
		zwSuspendThread ZwSuspendThread;
		zwResumeThread ZwResumeThread;
		zwGetContextThread ZwGetContextThread;
		zwSetContextThread ZwSetContextThread;
		zwOpenThread ZwOpenThread;
		zwClose ZwClose;
	public:
		BOOL if_init;
		API();
		~API() = default;
	};
	class R3: private API {
	public:
		HANDLE hProcess;
		DWORD PID;
	public:
		R3() = default;
		R3(DWORD pid);
		~R3();
		/*
		* �򿪽���
		* @param pid ����ID
		*/
		bool Open(DWORD pid);
		/*
		* �رս�������
		*/
		void Close() const;
		/*
		* �Ƿ�64λ����
		* @return �Ƿ�64λ����,true:64λ,false:32λ
		*/
		bool Is64() const;
		/*
		* ��ģ���ַ
		* @param moduleName ģ������
		* @return ģ���ַ
		*/
		size_t GetModuleBase(std::string moduleName) const;
		/*
		* д�ڴ�
		* @param address �ڴ��ַ
		* @param value д��ֵ
		* @param isVirtual �Ƿ������Ᵽ��, Ĭ��false
		* @return �Ƿ�д��ɹ�
		*/
		bool Write(size_t address, Byteset value, bool isVirtual = false) const;
		/*
		* ���ڴ�
		* @param address �ڴ��ַ
		* @param value ��ȡֵ
		* @param rlength ��ȡ����
		* @param isVirtual �Ƿ������Ᵽ��, Ĭ��false
		* @return �Ƿ��ȡ�ɹ�
		*/
		bool Read(size_t address, Byteset& value, SIZE_T rlength, bool isVirtual = false) const;
		/*
		* �޸��ڴ�����
		* @param address �ڴ��ַ
		* @param size �ڴ��С
		* @param protection ����ģʽ
		* @param oldProtection ԭ����ģʽ
		* @return �Ƿ��޸ĳɹ�
		*/
		bool VirtualProtect(size_t address, size_t size, DWORD protection, PDWORD oldProtection) const;
		/*
		* �����ڴ�ռ�
		* @param size �����С, Ĭ��1024�ֽ�
		* @return ���뵽���ڴ��ַ
		*/
		size_t Allocate(size_t size = 1024) const;
		/*
		* �ͷ��ڴ�ռ�
		* @param address �ڴ��ַ
		* @param size �ڴ��С, Ĭ��1024�ֽ�
		* @return �Ƿ��ͷųɹ�
		* ע��: �ͷŵ��ڴ��������Allocate�����
		*/
		bool Free(size_t address, size_t size = 1024) const;
		/*
		* �����ڴ�
		* @param buffer Ҫ�������ֽڴ�
		* @param vagueContent ģ�����ҵĺ����ֽڼ�,Ĭ��Ϊnullptr
		* @param startAddress �ڴ��ַ(��ʼ), Ĭ��NULL
		* @param endAddress �ڴ��ַ(����), Ĭ��-1(������������)
		* @param type ��ѯ�ڴ�����(Ĭ��-1),-1:All,0:�ɶ���д��ִ��,1:�ɶ�д,2:ֻ��,3:��д��ִ��,4:�ɶ���ִ��,5:��ִ��
		* @param limit �������� 0:������, Ĭ��10
		* @param isVirtual �Ƿ������Ᵽ��, Ĭ��false
		* @return �ҵ����ڴ��ַ����
		*/
		std::vector<size_t> Search(Byteset buffer,Byteset vagueContent = nullptr, size_t startAddress = NULL, size_t endAddress = -1,  int type = -1,size_t limit = 10, bool isVirtual = false) const;
		/*
		* hook��ת(64λhook���ֽ��볤��Ϊ14�ֽ�,32λhook���ֽ��볤��Ϊ5�ֽ�,ע�ⲹ��)
		* @param address �ڴ��ַ
		* @param newAddress ����ת��ַ
		* @param complement Ҫ������ֽڼ�,Ĭ��Ϊnullptr,�����ǰ����Ϊ64λ����,��hook�����ݳ���Ϊ16�ֽ�
		* @param isVirtual �Ƿ������Ᵽ��, Ĭ��false
		* @return �Ƿ���ת�ɹ�
		*/
		bool hookJump(size_t address, size_t newAddress, Byteset complement = nullptr, bool isVirtual = false) const;
		/*
		* ָ������
		* @param address �ڴ��ַ
		* @param offsets ָ��ƫ�Ƽ���,Ĭ��Ϊnullptr
		* @param isVirtual �Ƿ������Ᵽ��, Ĭ��false
		* @return ������ָ��ֵ
		*/
		Byteset pointerChain(size_t address, std::initializer_list<size_t> offsets = {}, bool isVirtual = false) const;
	};
	#pragma pack(push, 1)
	#ifdef _WIN64
	typedef struct _R3HOOK_INFO {
		unsigned long long rax;
		unsigned int eax;
		unsigned short ax;
		unsigned char al;
		unsigned long long rcx;
		unsigned int ecx;
		unsigned short cx;
		unsigned char cl;
		unsigned long long rdx;
		unsigned int edx;
		unsigned short dx;
		unsigned char dl;
		unsigned long long rbx;
		unsigned int ebx;
		unsigned short bx;
		unsigned char bl;
		unsigned long long rsp;
		unsigned int esp;
		unsigned short sp;
		unsigned char spl;
		unsigned long long rbp;
		unsigned int ebp;
		unsigned short bp;
		unsigned char bpl;
		unsigned long long rsi;
		unsigned int esi;
		unsigned short si;
		unsigned char sil;
		unsigned long long rdi;
		unsigned int edi;
		unsigned short di;
		unsigned char dil;
		unsigned long long r8;
		unsigned int r8d;
		unsigned short r8w;
		unsigned char r8b;
		unsigned long long r9;
		unsigned int r9d;
		unsigned short r9w;
		unsigned char r9b;
		unsigned long long r10;
		unsigned int r10d;
		unsigned short r10w;
		unsigned char r10b;
		unsigned long long r11;
		unsigned int r11d;
		unsigned short r11w;
		unsigned char r11b;
		unsigned long long r12;
		unsigned int r12d;
		unsigned short r12w;
		unsigned char r12b;
		unsigned long long r13;
		unsigned int r13d;
		unsigned short r13w;
		unsigned char r13b;
		unsigned long long r14;
		unsigned int r14d;
		unsigned short r14w;
		unsigned char r14b;
		unsigned long long r15;
		unsigned int r15d;
		unsigned short r15w;
		unsigned char r15b;
	} R3HOOK_INFO, * PR3HOOK_INFO;
	#else
	typedef struct _R3HOOK_INFO {
		unsigned int eax;
		unsigned short ax;
		unsigned char al;
		unsigned int ecx;
		unsigned short cx;
		unsigned char cl;
		unsigned int edx;
		unsigned short dx;
		unsigned char dl;
		unsigned int ebx;
		unsigned short bx;
		unsigned char bl;
		unsigned int esp;
		unsigned short sp;
		unsigned int ebp;
		unsigned short bp;
		unsigned int esi;
		unsigned short si;
		unsigned int edi;
		unsigned short di;
	} R3HOOK_INFO, * PR3HOOK_INFO; 
	#endif
	#pragma pack(pop)
	class R3HOOK : private R3 {
	private:
		bool isInstalled;
		bool is64_;
		bool isVirtual;
		bool isCapture;
		size_t address;
		Byteset complement;
		Byteset temp;
		size_t newAddress;
		DWORD PID;
	public:
		R3HOOK();
		R3HOOK(DWORD pid, size_t address, Byteset complement = nullptr);
		~R3HOOK();
		/*
		* �����Ƿ������Ᵽ��
		* @param isVirtual �Ƿ������Ᵽ��
		*/
		void setVirtual(bool isVirtual);
		/*
		* ʵʱ��ȡ(������Ҫ����Capture�����������ȡ)
		* @param flag �Ƿ�����,true:����,false:����
		*/
		void setCapture(bool flag);
		/*
		* ��װ����(64λhook���ֽ��볤��Ϊ14�ֽ�,32λhook���ֽ��볤��Ϊ5�ֽ�,ע�ⲹ��)
		* @param pid ����ID
		* @param address �ڴ��ַ
		* @param callBack ���ӻص�����
		* @param complement Ҫ������ֽڼ�,Ĭ��Ϊnullptr
		* @return �Ƿ�װ�ɹ�
		*/
		bool Install(DWORD pid, size_t address,Byteset complement = nullptr);
		/*
		* ��ȡ����
		*/
		void Capture();
		/*
		* ��ȡ����
		* @return ��ȡ������
		*/
		R3HOOK_INFO ReadData() const;
		/*
		* ж�ع���
		*/
		void Uninstall();
	};
	class EmbeddedCall : private R3 {
	private:
		typedef bool (WINAPI* embeddedCall)(int param1);
	private:
		DWORD PID;
		PVOID address;
		size_t size;

	public:
		EmbeddedCall();
		EmbeddedCall(DWORD pid,size_t size);
		~EmbeddedCall();
		/*
		* ��ʼ��
		* @param pid ����ID
		* @param size �ڴ��С
		* @return �Ƿ��ʼ���ɹ�
		*/
		bool Init(DWORD pid, size_t size);
		/*
		* �ͷ���Դ
		*/
		void Unit();
		/*
		* �����ڴ��С(�����������ڴ�ռ�)
		* @param size �ڴ��С
		* @return �Ƿ����ɹ�
		*/
		bool AllocateMemory(size_t size);
		/*
		* �������
		* @param code ����
		* @param index ��ʼλ��,Ĭ��Ϊ0
		* @return �Ƿ����óɹ�
		*/
		bool SetCode(Byteset code, size_t index = NULL);
		/*
		* ���ú���
		* @param index ������ʼλ��,��0��ʼ
		* @param args ��������
		* @return ��������ֵ
		*/
		template<typename Ret, typename... Args>
		Ret Call(int index, Args... args) {
			if (this->address == NULL) return Ret();
			PVOID addr = reinterpret_cast<PVOID>(reinterpret_cast<size_t>(this->address) + index);
			return (reinterpret_cast<Ret(WINAPI*)(Args...)>(addr))(args...);
		}
	};
	#ifdef _WIN64
	typedef struct _VEHHOOK_INFO {
		DWORD64 rax;
		DWORD64 rcx;
		DWORD64 rdx;
		DWORD64 rbx;
		DWORD64 rsp;
		DWORD64 rbp;
		DWORD64 rsi;
		DWORD64 rdi;
		DWORD64 rip;
		DWORD64 r8;
		DWORD64 r9;
		DWORD64 r10;
		DWORD64 r11;
		DWORD64 r12;
		DWORD64 r13;
		DWORD64 r14;
		DWORD64 r15;
		M128A xmm0;
		M128A xmm1;
		M128A xmm2;
		M128A xmm3;
		M128A xmm4;
		M128A xmm5;
		M128A xmm6;
		M128A xmm7;
		M128A xmm8;
		M128A xmm9;
		M128A xmm10;
		M128A xmm11;
		M128A xmm12;
		M128A xmm13;
		M128A xmm14;
		M128A xmm15;
	} VEHHOOK_INFO, * PVEHHOOK_INFO;
	#else
	typedef struct _VEHHOOK_INFO {
		DWORD eax;
		DWORD ecx;
		DWORD edx;
		DWORD ebx;
		DWORD esp;
		DWORD ebp;
		DWORD esi;
		DWORD edi;
		DWORD eip;
	} VEHHOOK_INFO, * PVEHHOOK_INFO;
	#endif
	/*
	* VEH�ص���������
	* @param info �쳣��Ϣ
	* @return �Ƿ������쳣ָ���,true:����,false:������(�������ͻỹԭ(ԭ����/int3)ִ��,����Ҫ�������������޸�) (HOOKJUMPģʽ��,����ʲô����Ӱ��ԭָ��ִ��)
	*/
	typedef bool (* VEHCallBack)(PEXCEPTION_POINTERS info);
	//����ģʽ
	enum VEH_TYPE {
		INT3,//int3�쳣�ϵ� 
		INT3_HOOKJUMP,//int3HOOK_JUMPģʽ
		HARDWARE_BREAKPOINT,//Ӳ���ϵ�(����,��ģʽ����4����ַ,�����������ʧ��)
		HAEDWARE_HOOKJUMP,//Ӳ��HOOK_JUMPģʽ
	};
	typedef struct _VEHHOOK_PARAM{
		DWORD code;//�쳣�����ʶ(�����ڻص����������hook��ַ)
		PVOID address;//�쳣��ַ
		VEHCallBack callBack;//�ص�����
		DWORD type = INT3;//����ģʽ,Ĭ��INT3ģʽ
		DWORD size = NULL;//�����С,��hook��ָ�����м����ֽھ������
		Byteset data = nullptr;//Ҫд�������(����HOOK_JUMPģʽ��Ч)
	}VEHHOOK_PARAM, * PVEHHOOK_PARAM;
	typedef struct _VEHHOOK_HANDLE {
		PVOID veh;//VEHHook����ָ��
		DWORD code;//�쳣�����ʶ(�����ڻص����������hook��ַ)
		PVOID address;//�쳣��ַ
		VEHCallBack callBack;//�ص�����
		DWORD type;//����ģʽ
		bool isInstalled;//�Ƿ�װ(����ģʽΪINT3ʱ,���쳣�����ص��������ؼ����Զ�ж��)
		Byteset oldAddrData;//ԭ��ַ����
		DWORD size;//�����С,���ָ��Ϊmov eax,0,���Ҳ����������еĵ�һλ,�����СҪ�����е���һ��ָ���м�ĳ���
		HANDLE threadId;//�߳�ID(����HARDWARE_BREAKPOINTģʽ��Ч)
		PVOID newAddress;//�µ�ַ(����HOOK_JUMPģʽ��Ч)
		Byteset newAddrData;//�µ�ַ����(����HOOK_JUMPģʽ��Ч)
		DWORD newAddrSize;//�µ�ַ��С(����HOOK_JUMPģʽ��Ч)
	} VEHHOOK_HANDLE, * PVEHHOOK_HANDLE;
	class VEHHook : private API, public R3 {
	private:
		PVOID exceptionHandler;
		static LONG NTAPI ExceptionHandler(PEXCEPTION_POINTERS ExceptionInfo);
	public:
		VEHHook();
		~VEHHook();
		/*
		* ��װVEH����
		* @return �Ƿ�װ�ɹ�,ȫ��ֻ������װһ��VEH����,�ظ���װ�᷵��ʧ��
		*/
		bool Install();
		/*
		* ж��VEH����
		* @return �Ƿ�ж�سɹ�
		*/
		bool Uninstall();
		/*
		* �����쳣��ַ
		* @param param �����ṹ��
		* @return ����CODE(0:�ɹ�,-1:��ַ�޸�ʧ��,-2:�ص�����Ϊ��,-3:code���ַ�ظ�,-4:VEH����δ��װ,-5: ����Ӳ���ϵ�Ĵ�������ռ��)
		*/
		DWORD AddAddress(VEHHOOK_PARAM param);
		/*
		* ɾ���쳣��ַ
		* @param code �쳣�����ʶ(�����ڻص����������hook��ַ)
		* @return true=�ɹ�,false=ʧ��
		*/
		bool DelAddress(DWORD code);
		/*
		* �޸��쳣��ַ(��ԭ�����������쳣��code����,�����޸Ĵ���,HOOK_JUMPģʽ��,�޸��µ�ַ�Լ����ڴ�����)
		* @param code �쳣�����ʶ(�����ڻص����������hook��ַ)
		* @param newAddress �µ�ַ
		* @param data Ҫд�������(����HOOK_JUMPģʽ��Ч)
		* @return �Ƿ��޸ĳɹ�
		*/
		bool ModAddress(DWORD code,Byteset data = nullptr);
		/*
		* ��ȡ�Ĵ���
		* @param info �쳣��Ϣ
		* @return �Ĵ���ָ��
		*/
		VEHHOOK_INFO GetRegisters(PEXCEPTION_POINTERS info);
		/*
		* ��ģ���ַ
		* @param moduleName ģ������
		* @return ģ���ַ
		*/
		size_t GetModuleBase(std::string moduleName) const;
		/*
		* ȡ��ַ�ڲ��쳣������Ϣ
		* @param address ��ַ
		* @return �ڲ��쳣������Ϣ
		*/
		PVEHHOOK_HANDLE GetHandle(PVOID address);
	};
};