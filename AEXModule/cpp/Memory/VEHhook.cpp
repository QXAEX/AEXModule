#include "../../h/Memory/VEHhook.h"
#include <cstdint>
#include <TlHelp32.h>
#include "../../h/api/data/ntdll.h"
#include <winternl.h>

struct DebugRegisters {
    uintptr_t dr0;
    uintptr_t dr1;
    uintptr_t dr2;
    uintptr_t dr3;
    uintptr_t dr4;
    uintptr_t dr5;
    union {
        uint32_t dr6;
        struct {
            bool B0 : 1;  // �ϵ� 0 ����
            bool B1 : 1;  // �ϵ� 1 ����
            bool B2 : 1;  // �ϵ� 2 ����
            bool B3 : 1;  // �ϵ� 3 ����
            bool BD : 1;  // ���ԼĴ������ʼ��
            bool BS : 1;  // ����ִ�д���
            bool BT : 1;  // �����л�����
            uint32_t reserved : (32 - 7);  // ����λ
        };
    };
    union {
        uint32_t dr7;
        struct {
            bool L0 : 1;  // ���ضϵ� 0 ����
            bool G0 : 1;  // ȫ�ֶϵ� 0 ����
            bool L1 : 1;  // ���ضϵ� 1 ����
            bool G1 : 1;  // ȫ�ֶϵ� 1 ����
            bool L2 : 1;  // ���ضϵ� 2 ����
            bool G2 : 1;  // ȫ�ֶϵ� 2 ����
            bool L3 : 1;  // ���ضϵ� 3 ����
            bool G3 : 1;  // ȫ�ֶϵ� 3 ����
            bool LE : 1;  // ���ؾ�ȷ�ϵ�����
            bool GE : 1;  // ȫ�־�ȷ�ϵ�����
            uint32_t reserved1 : 3;  // ����λ
            uint32_t RW0 : 2;  // �ϵ� 0 ����
            uint32_t LEN0 : 2; // �ϵ� 0 ����
            uint32_t RW1 : 2;  // �ϵ� 1 ����
            uint32_t LEN1 : 2; // �ϵ� 1 ����
            uint32_t RW2 : 2;  // �ϵ� 2 ����
            uint32_t LEN2 : 2; // �ϵ� 2 ����
            uint32_t RW3 : 2;  // �ϵ� 3 ����
            uint32_t LEN3 : 2; // �ϵ� 3 ����
            uint32_t reserved2 : (32 - 24);  // ����λ
        };
    };
};
static MemoryR3::R3 r3;
typedef struct INFO {
    DWORD token;//�쳣�����ʶ(�����ڻص����������hook��ַ)
    PVOID address;//�쳣��ַ
    Byteset oldData = nullptr;//ԭ����
    PVOID newAddress = nullptr;//�µ�ַ
    MemoryVEHHook::TYPE type = MemoryVEHHook::TYPE::INT3;//����ģʽ,Ĭ��INT3ģʽ
    //MemoryVEHHook::BRAKPOINT_TYPE breakpointType = MemoryVEHHook::BRAKPOINT_TYPE::T_EXECUTE;//�ϵ�����,Ĭ��ִ�жϵ�
    //MemoryVEHHook::BREAKPOINT_LENGTH breakpointLength = MemoryVEHHook::BREAKPOINT_LENGTH::L_BYTE;//�ϵ㳤��,Ĭ���ֽڶϵ�
    Byteset data = nullptr;//Ҫд�������(����HOOK_JUMPģʽ��Ч)
    struct {
        PVOID newAddress = nullptr;//�µ�ַ(����HOOK_JUMPģʽ��Ч)
        Byteset newAddrData = nullptr;//�µ�ַ����(����HOOK_JUMPģʽ��Ч)
        __int64 newAddrSize = NULL;//�µ�ַ��С(����HOOK_JUMPģʽ��Ч)
    } hookJump;//��ת�ṹ��(����HOOK_JUMPģʽ��Ч)
    MemoryVEHHook::VEHCallBack callBack;//�ص�����
    INFO(DWORD token, PVOID address, PVOID newAddress, MemoryVEHHook::TYPE type, Byteset data, MemoryVEHHook::VEHCallBack callBack) {
        this->token = token;
        this->address = address;
        this->newAddress = newAddress;
        this->type = type;
        this->data = data;
        this->callBack = callBack;
    }
}*PINFO;
static bool VehInstalled = false;//ȫ�ֱ������Ƿ��Ѱ�װVEH
static std::vector<INFO> VehInfos;
BOOL MemoryVEHHook::VEHhook::install()
{

    r3.Open(GetCurrentProcessId());
    if (VehInstalled) return false;
    exceptionHandler = AddVectoredExceptionHandler(1, ExceptionHandler);
    VehInstalled = this->exceptionHandler != nullptr;
    return this->exceptionHandler != nullptr;
}

BOOL MemoryVEHHook::VEHhook::uninstall() const
{
    r3.Close();
    if (!VehInstalled) return false;
    RemoveVectoredExceptionHandler(this->exceptionHandler);
}

MemoryVEHHook::ERROR_CODE MemoryVEHHook::VEHhook::Add(int token, PVOID address, PVOID newAddress, MemoryVEHHook::TYPE type, MemoryVEHHook::VEHCallBack callBack, Byteset data)
{
    INFO info(token, address, newAddress, type, data, callBack);
    MemoryVEHHook::ERROR_CODE error_code = MemoryVEHHook::ERROR_CODE::ERROR_NULL;
    int i = NULL;
    switch (type) {
    case MemoryVEHHook::TYPE::INT3:
        error_code = this->INT3(nullptr, &info, MemoryVEHHook::VEHhook::FNC_TRANSITIION::ADD);
        break;
    case MemoryVEHHook::TYPE::INT3_HOOKJUMP:
        error_code = this->INT3_HOOKJUMP(nullptr, &info, MemoryVEHHook::VEHhook::FNC_TRANSITIION::ADD);
        break;
    case MemoryVEHHook::TYPE::HARDWARE_BREAKPOINT:
        for(INFO item : VehInfos) if(item.type == MemoryVEHHook::TYPE::HARDWARE_BREAKPOINT) i++;
        if (i < 4) {
            error_code = this->HARDWARE_BREAKPOINT(nullptr, &info, MemoryVEHHook::VEHhook::FNC_TRANSITIION::ADD);
        } else {
            error_code = MemoryVEHHook::ERROR_CODE::HARDWARE_HOOKJUMP_LIMIT;
        }
        break;
    case MemoryVEHHook::TYPE::HARDWARE_HOOKJUMP:
        for (INFO item : VehInfos) if (item.type == MemoryVEHHook::TYPE::HARDWARE_BREAKPOINT) i++;
        if (i < 4) {
            error_code = this->HARDWARE_HOOKJUMP(nullptr, &info, MemoryVEHHook::VEHhook::FNC_TRANSITIION::ADD);
        } else {
            error_code = MemoryVEHHook::ERROR_CODE::HARDWARE_HOOKJUMP_LIMIT;
        }
        break;
    }
    if(error_code == MemoryVEHHook::ERROR_CODE::ERROR_NULL) VehInfos.push_back(info);
    return error_code;
}

MemoryVEHHook::ERROR_CODE MemoryVEHHook::VEHhook::Remove(int token)
{
    MemoryVEHHook::ERROR_CODE error_code = MemoryVEHHook::ERROR_CODE::TOKEN_INVALID;
    PINFO info = nullptr;
    for (auto& i : VehInfos) {
        if (i.token == token) {
            if (info == nullptr) return error_code;
            switch (info->type) {
            case MemoryVEHHook::TYPE::INT3:
                error_code = this->INT3(nullptr, info, MemoryVEHHook::VEHhook::FNC_TRANSITIION::REMOVE);
                break;
            case MemoryVEHHook::TYPE::INT3_HOOKJUMP:
                error_code = this->INT3_HOOKJUMP(nullptr, info, MemoryVEHHook::VEHhook::FNC_TRANSITIION::REMOVE);
                break;
            case MemoryVEHHook::TYPE::HARDWARE_BREAKPOINT:
                error_code = this->HARDWARE_BREAKPOINT(nullptr, info, MemoryVEHHook::VEHhook::FNC_TRANSITIION::REMOVE);
                break;
            case MemoryVEHHook::TYPE::HARDWARE_HOOKJUMP:
                error_code = this->HARDWARE_HOOKJUMP(nullptr, info, MemoryVEHHook::VEHhook::FNC_TRANSITIION::REMOVE);
                break;
            }
            VehInfos.erase(std::remove_if(VehInfos.begin(), VehInfos.end(), [&](const INFO& info) { return info.token == token; }), VehInfos.end());
            this->Remove(token);//�ݹ�ɾ��(����λ)
            break;
        }
    }
return error_code;
}

MemoryR3::R3* MemoryVEHHook::VEHhook::R3()
{
    return &r3;
}

MemoryVEHHook::ERROR_CODE MemoryVEHHook::VEHhook::INT3(PEXCEPTION_POINTERS ExceptionInfo, PVOID pInfo, FNC_TRANSITIION fnc_transition)
{
    PINFO Info = (PINFO)pInfo;
    MemoryVEHHook::ERROR_CODE error_code = MemoryVEHHook::ERROR_CODE::ERROR_NULL;
    switch (fnc_transition)
    {
    case FNC_TRANSITIION::NONE:
        if (Info->callBack) Info->callBack(Info->type, ExceptionInfo);
#ifdef _WIN64
        ExceptionInfo->ContextRecord->Rip = (size_t)Info->newAddress;
#else
        ExceptionInfo->ContextRecord->Eip = (size_t)Info->newAddress;
#endif
        break;
    case FNC_TRANSITIION::ADD:
        r3.Read(Info->address, Info->oldData, 1);
        r3.Write(Info->address, { 0xCC });
        break;
    case FNC_TRANSITIION::REMOVE:
        r3.Write(Info->address, Info->oldData, 1);
        break;
    }
    return error_code;
}

MemoryVEHHook::ERROR_CODE MemoryVEHHook::VEHhook::INT3_HOOKJUMP(PEXCEPTION_POINTERS ExceptionInfo, PVOID pInfo, FNC_TRANSITIION fnc_transition)
{
    PINFO Info = (PINFO)pInfo;
    MemoryVEHHook::ERROR_CODE error_code = MemoryVEHHook::ERROR_CODE::ERROR_NULL;
    switch (fnc_transition)
    {
    case FNC_TRANSITIION::NONE:
        if (Info->callBack) Info->callBack(Info->type, ExceptionInfo);
#ifdef _WIN64
        ExceptionInfo->ContextRecord->Rip = (size_t)Info->hookJump.newAddress;
#else
        ExceptionInfo->ContextRecord->Eip = (size_t)Info->hookJump.newAddress;
#endif
        break;
    case FNC_TRANSITIION::ADD:
        Info->hookJump.newAddrSize = (__int64)(Info->data.size() + 20);
        Info->hookJump.newAddress = (PVOID)r3.Allocate(Info->hookJump.newAddrSize);
        r3.Write(Info->hookJump.newAddress, Info->data);
        r3.hookJump((PVOID)((size_t)Info->hookJump.newAddress + Info->data.size()), Info->newAddress);
        INT3(ExceptionInfo, Info, MemoryVEHHook::VEHhook::FNC_TRANSITIION::ADD);
        break;
    case FNC_TRANSITIION::REMOVE:
        INT3(ExceptionInfo, Info, MemoryVEHHook::VEHhook::FNC_TRANSITIION::REMOVE);
        r3.Free(Info->hookJump.newAddress);
        break;
    }
    return error_code;
}

MemoryVEHHook::ERROR_CODE MemoryVEHHook::VEHhook::HARDWARE_BREAKPOINT(PEXCEPTION_POINTERS ExceptionInfo, PVOID pInfo, FNC_TRANSITIION fnc_transition)
{
    HANDLE hsnap = fnc_transition != FNC_TRANSITIION::NONE ?
        CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, GetCurrentProcessId()) : nullptr;
    PINFO Info = (PINFO)pInfo;
    MemoryVEHHook::ERROR_CODE error_code = MemoryVEHHook::ERROR_CODE::ERROR_NULL;
    DebugRegisters dr{ 0 };
    switch (fnc_transition)
    {
    case FNC_TRANSITIION::NONE:
        if (Info->callBack) Info->callBack(Info->type, ExceptionInfo);
#ifdef _WIN64
        ExceptionInfo->ContextRecord->Rip = (size_t)Info->newAddress;
#else
        ExceptionInfo->ContextRecord->Eip = (size_t)Info->newAddress;
#endif
        break;

    case FNC_TRANSITIION::ADD:
    {
        THREADENTRY32 te32{ 0 };
        te32.dwSize = sizeof(THREADENTRY32);
        BOOL state = Thread32First(hsnap, &te32);
        while (state) {
            if (te32.th32OwnerProcessID == GetCurrentProcessId()) {
                NTDLL::OBJECT_ATTRIBUTES objAttr{};
                InitializeObjectAttributes(&objAttr, NULL, 0, NULL, NULL);
                objAttr.ObjectName = NULL;
                NTDLL::CLIENT_ID clientId{};
                clientId.UniqueProcess = (HANDLE)te32.th32OwnerProcessID;
                clientId.UniqueThread = (HANDLE)te32.th32ThreadID;
                HANDLE hThread = NULL;
                NTDLL::ZwOpenThread(&hThread, THREAD_ALL_ACCESS, &objAttr, &clientId);

                if (hThread != NULL) {
                    CONTEXT context{ NULL };
                    context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
                    NTDLL::ZwGetContextThread(hThread, &context);
                    dr.dr0 = context.Dr0;
                    dr.dr1 = context.Dr1;
                    dr.dr2 = context.Dr2;
                    dr.dr3 = context.Dr3;
                    dr.dr6 = context.Dr6;
                    dr.dr7 = context.Dr7;
                    if (dr.dr0 == 0 || dr.dr0 == (uintptr_t)Info->address) {
                        dr.dr0 = (uintptr_t)Info->address;
                        dr.L0 = TRUE;
                        dr.RW0 = 0x3;  //��дִ�жϵ�
                        dr.LEN0 = 0x0;  // 1 �ֽڳ���
                    }
                    else if (dr.dr1 == 0 || dr.dr1 == (uintptr_t)Info->address) {
                        dr.dr1 = (uintptr_t)Info->address;
                        dr.L1 = TRUE;
                        dr.RW1 = 0x3;  // ��дִ�жϵ�
                        dr.LEN1 = 0x0;  // 1 �ֽڳ���
                    }
                    else if (dr.dr2 == 0 || dr.dr2 == (uintptr_t)Info->address) {
                        dr.dr2 = (uintptr_t)Info->address;
                        dr.L2 = TRUE;
                        dr.RW2 = 0x3;  // ��дִ�жϵ�
                        dr.LEN2 = 0x0;  // 1 �ֽڳ���
                    }
                    else if (dr.dr3 == 0 || dr.dr3 == (uintptr_t)Info->address) {
                        dr.dr3 = (uintptr_t)Info->address;
                        dr.L3 = TRUE;
                        dr.RW3 = 0x3;  // ��дִ�жϵ�
                        dr.LEN3 = 0x0;  // 1 �ֽڳ���
                    }
                    context.Dr0 = dr.dr0;
                    context.Dr1 = dr.dr1;
                    context.Dr2 = dr.dr2;
                    context.Dr3 = dr.dr3;
                    context.Dr6 = dr.dr6;
                    context.Dr7 = dr.dr7;
                    NTDLL::ZwSetContextThread(hThread, &context);
                    NTDLL::ZwClose(hThread);
                }
            }
            state = Thread32Next(hsnap, &te32);
        };
        break;
    }
    case FNC_TRANSITIION::REMOVE:
    {
        THREADENTRY32 te32{};
        te32.dwSize = sizeof(THREADENTRY32);
        BOOL state = Thread32First(hsnap, &te32);
        while (state) {
            if (te32.th32OwnerProcessID == GetCurrentProcessId()) {
                NTDLL::OBJECT_ATTRIBUTES objAttr{};
                InitializeObjectAttributes(&objAttr, NULL, 0, NULL, NULL);
                objAttr.ObjectName = NULL;
                NTDLL::CLIENT_ID clientId{};
                clientId.UniqueProcess = (HANDLE)te32.th32OwnerProcessID;
                clientId.UniqueThread = (HANDLE)te32.th32ThreadID;
                HANDLE hThread = NULL;
                NTDLL::ZwOpenThread(&hThread, THREAD_ALL_ACCESS, &objAttr, &clientId);
                if (hThread != NULL) {
                    CONTEXT context{ NULL };
                    context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
                    NTDLL::ZwGetContextThread(hThread, &context);
                    dr.dr0 = context.Dr0;
                    dr.dr1 = context.Dr1;
                    dr.dr2 = context.Dr2;
                    dr.dr3 = context.Dr3;
                    dr.dr6 = context.Dr6;
                    dr.dr7 = context.Dr7;
                    if (dr.dr0 == (uintptr_t)Info->address) {
                        dr.dr0 = 0;
                        dr.L0 = 0;
                        dr.RW0 = 0;
                        dr.LEN0 = 0;
                    }
                    else if (dr.dr1 == (uintptr_t)Info->address) {
                        dr.dr1 = 0;
                        dr.L1 = 0;
                        dr.RW1 = 0;
                        dr.LEN1 = 0;
                    }
                    else if (dr.dr2 == (uintptr_t)Info->address) {
                        dr.dr2 = 0;
                        dr.L2 = 0;
                        dr.RW2 = 0;
                        dr.LEN2 = 0;
                    }
                    else if (dr.dr3 == (uintptr_t)Info->address) {
                        dr.dr3 = 0;
                        dr.L3 = 0;
                        dr.RW3 = 0;
                        dr.LEN3 = 0;
                    }
                    context.Dr0 = dr.dr0;
                    context.Dr1 = dr.dr1;
                    context.Dr2 = dr.dr2;
                    context.Dr3 = dr.dr3;
                    context.Dr6 = dr.dr6;
                    context.Dr7 = dr.dr7;
                    NTDLL::ZwSetContextThread(hThread, &context);
                    NTDLL::ZwClose(hThread);
                }
            }
            state = Thread32Next(hsnap, &te32);
        };
        break;
    }
    }

    if (hsnap) NTDLL::ZwClose(hsnap);
    return error_code;
}

MemoryVEHHook::ERROR_CODE MemoryVEHHook::VEHhook::HARDWARE_HOOKJUMP(PEXCEPTION_POINTERS ExceptionInfo, PVOID pInfo, FNC_TRANSITIION fnc_transition)
{
    PINFO Info = (PINFO)pInfo;
    MemoryVEHHook::ERROR_CODE error_code = MemoryVEHHook::ERROR_CODE::ERROR_NULL;
    switch (fnc_transition)
    {
    case FNC_TRANSITIION::NONE:
        if (Info->callBack) Info->callBack(Info->type, ExceptionInfo);
#ifdef _WIN64
        ExceptionInfo->ContextRecord->Rip = (size_t)Info->hookJump.newAddress;
#else
        ExceptionInfo->ContextRecord->Eip = (size_t)Info->hookJump.newAddress;
#endif
        break;
    case FNC_TRANSITIION::ADD:
        Info->hookJump.newAddrSize = (__int64)(Info->data.size() + 20);
        Info->hookJump.newAddress = (PVOID)r3.Allocate(Info->hookJump.newAddrSize);
        r3.Write(Info->hookJump.newAddress, Info->data);
        r3.hookJump((PVOID)((size_t)Info->hookJump.newAddress + Info->data.size()), Info->newAddress);
        HARDWARE_BREAKPOINT(ExceptionInfo, Info, MemoryVEHHook::VEHhook::FNC_TRANSITIION::ADD);
        break;
    case FNC_TRANSITIION::REMOVE:
        HARDWARE_BREAKPOINT(ExceptionInfo, Info, MemoryVEHHook::VEHhook::FNC_TRANSITIION::REMOVE);
        r3.Free(Info->hookJump.newAddress);
        break;
    }
    return error_code;
}

LONG MemoryVEHHook::VEHhook::ExceptionHandler(PEXCEPTION_POINTERS ExceptionInfo)
{
    PVOID ExceptionAddress = ExceptionInfo->ExceptionRecord->ExceptionAddress;//��ȡ�쳣��ַ
    for (auto& info : VehInfos) {
        if (info.address == ExceptionAddress) {
            switch (info.type) {
            case MemoryVEHHook::TYPE::INT3:
                INT3(ExceptionInfo, &info, MemoryVEHHook::VEHhook::FNC_TRANSITIION::NONE);
                break;
            case MemoryVEHHook::TYPE::INT3_HOOKJUMP:
                INT3_HOOKJUMP(ExceptionInfo, &info, MemoryVEHHook::VEHhook::FNC_TRANSITIION::NONE);
                break;
            case MemoryVEHHook::TYPE::HARDWARE_BREAKPOINT:
                HARDWARE_BREAKPOINT(ExceptionInfo, &info, MemoryVEHHook::VEHhook::FNC_TRANSITIION::NONE);
                break;
            case MemoryVEHHook::TYPE::HARDWARE_HOOKJUMP:
                HARDWARE_HOOKJUMP(ExceptionInfo, &info, MemoryVEHHook::VEHhook::FNC_TRANSITIION::NONE);
                break;
            }
            return EXCEPTION_CONTINUE_EXECUTION;
        }
    }
    return EXCEPTION_CONTINUE_SEARCH;
}