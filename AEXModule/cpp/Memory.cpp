#include "../h/Memory.h"
#include <winternl.h>
#include <thread>
#ifdef _WIN64
#define GetIntAddress(Ptr) ((uintptr_t)Ptr & 0xFFFFFFFFFFFFF000)
#else
#define GetIntAddress(Ptr) ((uintptr_t)Ptr & 0xFFFFF000)
#endif
Memory::API::API()
{
    HMODULE ntdll = LoadLibraryA("ntdll.dll");
    HMODULE kernel32 = LoadLibraryA("kernel32.dll");
    this->if_init = ntdll != NULL && kernel32 != NULL;
    if (this->if_init)
    {
        this->ZwOpenProcess = (zwOpenProcess)GetProcAddress(ntdll, "ZwOpenProcess");
        this->ZwClose = (zwClose)GetProcAddress(ntdll, "ZwClose");
        this->ZwWriteVirtualMemory = (zwWriteVirtualMemory)GetProcAddress(ntdll, "ZwWriteVirtualMemory");
        this->ZwReadVirtualMemory = (zwReadVirtualMemory)GetProcAddress(ntdll, "ZwReadVirtualMemory");
        this->ZwProtectVirtualMemory = (zwProtectVirtualMemory)GetProcAddress(ntdll, "ZwProtectVirtualMemory");
        this->ZwAllocateVirtualMemory = (zwAllocateVirtualMemory)GetProcAddress(ntdll, "ZwAllocateVirtualMemory");
        this->ZwFreeVirtualMemory = (zwFreeVirtualMemory)GetProcAddress(ntdll, "ZwFreeVirtualMemory");
        this->ZwQueryVirtualMemory = (zwQueryVirtualMemory)GetProcAddress(ntdll, "ZwQueryVirtualMemory");
        this->CreateToolhelp32Snapshot = (createToolhelp32Snapshot)GetProcAddress(kernel32, "CreateToolhelp32Snapshot");
        this->Thread32First = (thread32First)GetProcAddress(kernel32, "Thread32First");
        this->Thread32Next = (thread32Next)GetProcAddress(kernel32, "Thread32Next");
        this->RtlWow64GetProcessMachines = (rtlWow64GetProcessMachines)GetProcAddress(ntdll, "RtlWow64GetProcessMachines");
        this->ZwSuspendThread = (zwSuspendThread)GetProcAddress(ntdll, "ZwSuspendThread");
        this->ZwResumeThread = (zwResumeThread)GetProcAddress(ntdll, "ZwResumeThread");
        this->ZwGetContextThread = (zwGetContextThread)GetProcAddress(ntdll, "ZwGetContextThread");
        this->ZwSetContextThread = (zwSetContextThread)GetProcAddress(ntdll, "ZwSetContextThread");
        this->ZwOpenThread = (zwOpenThread)GetProcAddress(ntdll, "ZwOpenThread");
    }
}

Memory::R3::R3(DWORD pid)
{
    this->Open(pid);
}

Memory::R3::~R3()
{
    this->Close();
}

bool Memory::R3::Open(DWORD pid)
{
    if (!this->if_init)return false;
    this->PID = pid;
    OBJECT_ATTRIBUTES objAttr{};
    InitializeObjectAttributes(&objAttr, NULL, 0, NULL, NULL);
    objAttr.ObjectName = NULL;
    CLIENT_ID clientId{};
    clientId.UniqueProcess = (HANDLE)pid;
    clientId.UniqueThread = NULL;
    NTSTATUS status = ZwOpenProcess(&this->hProcess, PROCESS_ALL_ACCESS, &objAttr, &clientId);
    return this->hProcess != NULL;
}

void Memory::R3::Close() const
{
    if (!this->if_init)return;
    ZwClose(this->hProcess);
}

bool Memory::R3::Is64() const
{
    BOOL aaaa = false;
    IsWow64Process(this->hProcess, &aaaa);
    return (!aaaa);
}

size_t Memory::R3::GetModuleBase(std::string moduleName) const
{
    DWORD processId = 0;
    std::string modName_ = moduleName;
    __int64 modaddress{ 0 };
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, (HANDLE)this->PID);
    if (snapshot != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 processEntry = { sizeof(MODULEENTRY32) };
        if (Module32First(snapshot, &processEntry)) {
            do {
                if (Text::text_to_str(processEntry.szModule) == modName_) {
                    modaddress = (__int64)processEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(snapshot, &processEntry));
        };
        return modaddress;
    };
    CloseHandle(snapshot);
    return 0;
}

bool Memory::R3::Write(size_t address, Byteset value, bool isVirtual) const
{
    if (!this->if_init)return false;
    bool  flag{ 0 };
    SIZE_T data{ 0 };
    DWORD oldProtect = 0;
    if (isVirtual)VirtualProtect(address, value.size(), PAGE_EXECUTE_READWRITE, &oldProtect);
    flag = ZwWriteVirtualMemory(this->hProcess, (LPCVOID)address, (PVOID)value.data(), value.size(), &data) == NULL;
    if (isVirtual)VirtualProtect(address, value.size(), oldProtect, &oldProtect);
    return flag;
}

bool Memory::R3::Read(size_t address, Byteset& value, SIZE_T rlength, bool isVirtual) const
{
    if (!this->if_init)return false;
    SIZE_T data{ 0 };
    DWORD oldProtect{ 0 };
    value.clear();
    value.resize(rlength);
    if(isVirtual)VirtualProtect(address, rlength, PAGE_EXECUTE_READWRITE, &oldProtect);
    bool flag = ZwReadVirtualMemory(this->hProcess, (LPCVOID)address, (PVOID)value.data(), rlength, &data) == NULL;
    if (isVirtual)VirtualProtect(address, rlength, oldProtect, &oldProtect);
    return flag;
}

bool Memory::R3::VirtualProtect(size_t address, size_t size, DWORD protection, PDWORD oldProtection) const
{
    if (!this->if_init) return false;
    return ZwProtectVirtualMemory(this->hProcess, &address, &size, protection, oldProtection) == NULL;
}

size_t Memory::R3::Allocate(size_t size) const
{
    if (!this->if_init)return 0;
    size_t address{ 0 };
    ZwAllocateVirtualMemory(this->hProcess, &address, 0, &size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    return address;
}

bool Memory::R3::Free(size_t address, size_t size) const
{
    if (!this->if_init)return false;
    return ZwFreeVirtualMemory(this->hProcess, &address, &size, MEM_RELEASE) == NULL;
}

std::vector<size_t> Memory::R3::Search(Byteset buffer, Byteset vagueContent, size_t startAddress, size_t endAddress, int type, size_t limit, bool isVirtual) const
{
    if (!this->if_init || buffer.empty()) return std::vector<size_t>();
    std::vector<size_t> addresses{};
    size_t address = startAddress;
    MEMORY_BASIC_INFORMATION MemoryInfoMation{};
    bool vague = !vagueContent.empty();
    while (ZwQueryVirtualMemory(this->hProcess, (LPCVOID)address, MemoryBasicInformation, &MemoryInfoMation, sizeof(MemoryInfoMation), nullptr) == 0) {
        bool flag = false;
        switch (type)
        {
            case 0: // 可读可写可执行
                flag = (MemoryInfoMation.Protect == PAGE_EXECUTE_READWRITE);
                break;
            case 1: // 可读写
                flag = (MemoryInfoMation.Protect == PAGE_READWRITE);
                break;
            case 2: // 只读
                flag = (MemoryInfoMation.Protect == PAGE_READONLY);
                break;
            case 3: // 可写可执行
                flag = (MemoryInfoMation.Protect == PAGE_EXECUTE_WRITECOPY);
                break;
            case 4: // 可读可执行
                flag = (MemoryInfoMation.Protect == PAGE_EXECUTE_READ);
                break;
            case 5: // 可执行
                flag = (MemoryInfoMation.Protect == PAGE_EXECUTE);
                break;
            case -1: // ALL
                flag = true;
                break;
            default:
                flag = false;
                break;
        }

        if (MemoryInfoMation.State == MEM_COMMIT && true &&
            (size_t)(MemoryInfoMation.BaseAddress) >= startAddress &&
            ((size_t)(MemoryInfoMation.BaseAddress) <= endAddress || endAddress == -1)) {
            Byteset data;
            if (Read((size_t)MemoryInfoMation.BaseAddress, data, MemoryInfoMation.RegionSize, isVirtual)) {
                size_t index = data.find(buffer, NULL, vague, vagueContent);
                while (index != -1) {
                    addresses.push_back((size_t)(MemoryInfoMation.BaseAddress) + index);
                    if (limit != NULL && addresses.size() >= limit) break;
                    index = data.find(buffer, index + 1, vague, vagueContent);
                }
            }

            if (limit != NULL && addresses.size() >= limit) break;
        }

        address += MemoryInfoMation.RegionSize;
    }

    return addresses;
}

bool Memory::R3::hookJump(size_t address, size_t newAddress, Byteset complement, bool isVirtual) const
{
    Byteset data;
    if (this->Is64()) {
        data.append({ 0xFF, 0x25 });
        data.append({ 0x00, 0x00 , 0x00, 0x00 });
        data.append(newAddress);
    } else {
        data.append({ 0xE9 });
        data.append((int)(0xFFFFFFFF - (address - newAddress) - 4));
    }
    data.append(complement);
    return this->Write(address, data, isVirtual);
}

Byteset Memory::R3::pointerChain(size_t address, std::initializer_list<size_t> offsets, bool isVirtual) const
{
    Byteset buffer;
    bool flag = true;
    int readSize = (this->Is64() ? 8 : 4);
    size_t pointer = address;
    this->Read(pointer, buffer, readSize, isVirtual);
    pointer = *(size_t *)buffer.data();
    for (int i = 0, size = offsets.size() - 1; i < size; i++) {
        if (!this->Read(pointer + offsets.begin()[i], buffer, readSize, isVirtual)) {
            flag = false;
            break;
        }
        pointer = *(size_t*)buffer.data();
    }
    if (flag && this->Read(pointer + offsets.begin()[offsets.size() - 1], buffer, readSize, isVirtual)) {
        return buffer.append({ 0,0 });
    }
    return pointer;
}

Memory::R3HOOK::R3HOOK()
{
    this->isInstalled = false;
    this->address = NULL;
    this->newAddress = NULL;
    this->isVirtual = false;
    this->PID = NULL;
    this->is64_ = false;
    this->isCapture = false;
}

Memory::R3HOOK::R3HOOK(DWORD pid, size_t address, Byteset complement)
{
    this->Install(this->PID, address, complement);
}

Memory::R3HOOK::~R3HOOK()
{
    this->Uninstall();
}

void Memory::R3HOOK::setVirtual(bool isVirtual)
{
    this->isVirtual = isVirtual;
}

void Memory::R3HOOK::setCapture(bool flag)
{
    this->isCapture = flag;
}

bool Memory::R3HOOK::Install(DWORD pid, size_t address, Byteset complement)
{
    if (this->isInstalled)return false;
    this->isInstalled = false;
    this->PID = pid;
    this->address = address;
    this->complement = complement;
    this->Open(pid);
    this->is64_ = this->Is64();
    this->newAddress = this->Allocate(2048);
    Byteset buffer;
    long long newAddress = this->newAddress + 4;
    if (this->is64_) {
        buffer.append({ 0x83, 0x3D, 0xCD, 0xFE, 0xFF, 0xFF, 0x0 });//cmp [0x0], 0
        buffer.append({ 0x0F, 0x8E, 0xCF, 0x01, 0x0, 0x0 });//jle
        buffer.append({ 0x48, 0xA3 }).append(newAddress); // rax
        buffer.append({ 0xA3 }).append(newAddress += 8); // eax
        buffer.append({ 0x66, 0xA3 }).append(newAddress += 4); // ax
        buffer.append({ 0xA2 }).append(newAddress += 2); // al
        newAddress = 0xFFFFFEAC;
        for (int i = 1,ii = 0; i < 16; i++) {
            buffer.append({ (UCHAR)(i >= 8 ? 0x4C : 0x48), 0x89,  (UCHAR)((i >= 8 ? 0x05 + ((i - 8) * 8) : 0x0D + ((i - 1) * 8))) }).append(static_cast<int>(newAddress + 1 - ii ));
            buffer.append(i >= 8 ? Byteset({ 0x44 }) : nullptr).append({ 0x89,  (UCHAR)((i >= 8 ? 0x05 + ((i - 8) * 8) : 0x0D + ((i - 1) * 8))) }).append(static_cast<int>(newAddress +(i>=8? 2 - ii : 3)));
            buffer.append({ 0x66, (UCHAR)(i >= 8 ? 0x44 : 0x89) }).append(i >= 8 ? Byteset({ 0x89 }) : nullptr).append({ (UCHAR)((i >= 8 ? 0x05 + ((i - 8) * 8) : 0x0D + ((i - 1) * 8))) }).append(static_cast<int>(newAddress - (i >= 8 ? 2 + ii : NULL)));
            buffer.append(i >= 3 ? Byteset({ (UCHAR)(i >= 8 ? 0x44 : 0x40) }) : nullptr).append({ 0x88,  (UCHAR)((i >= 8 ? 0x05 + ((i - 8) * 8) : 0x0D + ((i - 1) * 8))) }).append(static_cast<int>(newAddress - (i >= 8 ? 7 + ii: 5) ));
            newAddress -= (i>= 3 && i <= 7 ? 0xC : 0xB);
            if (i>=8) ii+=3;
        }
        if (this->isCapture) {
            buffer.append({ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 });
        } else {
            buffer.append({ 0xFF, 0x0D, 0xF8, 0xFC, 0xFF, 0xFF }); // dec [0x0]
        }
    } else {
        buffer.append({ 0x83, 0x3D }).append(static_cast<int>(this->newAddress)).append({ 0x0 });//cmp [0x0], 0
        buffer.append({ 0x0F, 0x8E, 0x83, 0x0, 0x0, 0x0 });//jng 0x0
        buffer.append({ 0xA3 }).append(static_cast<int>(newAddress)); // eax
        buffer.append({ 0x66, 0xA3 }).append(static_cast<int>(newAddress+=4)); // ax
        buffer.append({ 0xA2 }).append(static_cast<int>(newAddress += 2)); // al
        for (int i = 1;i<8;i++) {
            buffer.append({ 0x89 }).append({ (UCHAR)(0x0D + ((i - 1) * 8)) }).append(static_cast<int>(newAddress += (i <= 4 ? 1 : 2)));
            buffer.append({ 0x66, 0x89 }).append({ (UCHAR)(0x0D + ((i - 1) * 8)) }).append(static_cast<int>(newAddress += 4));
            if (i <= 3) buffer.append({ 0x88 }).append({ (UCHAR)(0x0D + ((i - 1) * 8)) }).append(static_cast<int>(newAddress += 2));
        }
        if (this->isCapture) {
            buffer.append({ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 });
        } else {
            buffer.append({ 0xFF, 0x0D }).append(static_cast<int>(this->newAddress)); // dec [0x0]
        }
    }
    this->Read(address, temp, (this->is64_ ? 14 : 5) + complement.size(), this->isVirtual);
    this->Write(this->newAddress + (this->is64_ ? 300 : 100), buffer, this->isVirtual);
    this->hookJump(this->newAddress + (this->is64_ ? 300 : 100) + buffer.size(), address + (this->is64_ ? 14 : 5) + complement.size(), nullptr, this->isVirtual);
    if (this->isCapture)buffer.append(temp);
    this->hookJump(address, this->newAddress + (this->is64_ ? 300 : 100), complement, this->isVirtual);
    this->isInstalled = true;
    return true;
}

void Memory::R3HOOK::Capture()
{
    if (!this->isInstalled) return;
    this->Write(this->newAddress, {0x1}, this->isVirtual);
}
#ifdef _WIN64
Memory::R3HOOK_INFO Memory::R3HOOK::ReadData() const
{
    if (!this->isInstalled) return R3HOOK_INFO();
    Byteset data;
    long long index = NULL;
    this->Read(this->newAddress + 4, data, sizeof(R3HOOK_INFO), this->isVirtual);
    R3HOOK_INFO info{};
    PR3HOOK_INFO pInfo = (PR3HOOK_INFO)data.data();
    info.rax = pInfo->rax;
    info.eax = pInfo->eax;
    info.ax = pInfo->ax;
    info.al = pInfo->al;
    info.rcx = pInfo->rcx;
    info.ecx = pInfo->ecx;
    info.cx = pInfo->cx;
    info.cl = pInfo->cl;
    info.rdx = pInfo->rdx;
    info.edx = pInfo->edx;
    info.dx = pInfo->dx;
    info.dl = pInfo->dl;
    info.rbx = pInfo->rbx;
    info.ebx = pInfo->ebx;
    info.bx = pInfo->bx;
    info.bl = pInfo->bl;
    info.rsp = pInfo->rsp;
    info.esp = pInfo->esp;
    info.sp = pInfo->sp;
    info.spl = pInfo->spl;
    info.rbp = pInfo->rbp;
    info.ebp = pInfo->ebp;
    info.bp = pInfo->bp;
    info.rsi = pInfo->rsi;
    info.si = pInfo->si;
    info.edi = pInfo->edi;
    info.di = pInfo->di;
    info.r8 = pInfo->r8;
    info.r8d = pInfo->r8d;
    info.r8w = pInfo->r8w;
    info.r8b = pInfo->r8b;
    info.r9 = pInfo->r9;
    info.r9d = pInfo->r9d;
    info.r9w = pInfo->r9w;
    info.r9b = pInfo->r9b;
    info.r10 = pInfo->r10;
    info.r10d = pInfo->r10d;
    info.r10w = pInfo->r10w;
    info.r10b = pInfo->r10b;
    info.r11 = pInfo->r11;
    info.r11d = pInfo->r11d;
    info.r11w = pInfo->r11w;
    info.r11b = pInfo->r11b;
    info.r12 = pInfo->r12;
    info.r12d = pInfo->r12d;
    info.r12w = pInfo->r12w;
    info.r12b = pInfo->r12b;
    info.r13 = pInfo->r13;
    info.r13d = pInfo->r13d;
    info.r13w = pInfo->r13w;
    info.r13b = pInfo->r13b;
    info.r14 = pInfo->r14;
    info.r14d = pInfo->r14d;
    info.r14w = pInfo->r14w;
    info.r14b = pInfo->r14b;
    info.r15 = pInfo->r15;
    info.r15d = pInfo->r15d;
    info.r15w = pInfo->r15w;
    info.r15b = pInfo->r15b;
    return info;
}
#else
Memory::R3HOOK_INFO Memory::R3HOOK::ReadData() const{
    if (!this->isInstalled) return R3HOOK_INFO();
    Byteset data;
    this->Read(this->newAddress + 4, data, sizeof(R3HOOK_INFO), this->isVirtual);
    R3HOOK_INFO info{};
    PR3HOOK_INFO pInfo = (PR3HOOK_INFO)data.data();
    info.eax = pInfo->eax;
    info.ax = pInfo->ax;
    info.al = pInfo->al;
    info.ecx = pInfo->ecx;
    info.cx = pInfo->cx;    
    info.cl = pInfo->cl;
    info.edx = pInfo->edx;
    info.dx = pInfo->dx;
    info.dl = pInfo->dl;
    info.ebx = pInfo->ebx;
    info.bx = pInfo->bx;
    info.bl = pInfo->bl;
    info.esp = pInfo->esp;
    info.sp = pInfo->sp;
    info.ebp = pInfo->ebp;
    info.bp = pInfo->bp;
    info.esi = pInfo->esi;
    info.si = pInfo->si;
    info.edi = pInfo->edi;
    info.di = pInfo->di;
    return info;
}
#endif
void Memory::R3HOOK::Uninstall()
{
    if (this->isInstalled) {
        this->Write(this->address, temp, this->isVirtual);
        this->Free(this->newAddress, 2048);
        this->isInstalled = false;
    }
}

Memory::EmbeddedCall::EmbeddedCall()
{
    this->PID = NULL;
    this->size = NULL;
    this->address = NULL;
}

Memory::EmbeddedCall::EmbeddedCall(DWORD pid, size_t size)
{
    this->Init(pid, size);
}

Memory::EmbeddedCall::~EmbeddedCall()
{
    this->Unit();
}

bool Memory::EmbeddedCall::Init(DWORD pid, size_t size)
{
    this->PID = pid;
    this->Open(this->PID);
    return this->AllocateMemory(size);
}

void Memory::EmbeddedCall::Unit()
{
    if (this->address != NULL) {
        this->Free((size_t)this->address, this->size);
    }
}

bool Memory::EmbeddedCall::AllocateMemory(size_t size)
{
    if (this->address == NULL) {
        this->Free((size_t)this->address, this->size);
    }
    this->size = size;
    this->address = (PVOID)this->Allocate(this->size);
    return this->address != NULL;
}

bool Memory::EmbeddedCall::SetCode(Byteset code,size_t index)
{
    if (this->address == NULL) {
        return false;
    }
    return this->Write((size_t)this->address, code, false);
}

static HANDLE VehHThread = NULL;
static bool VehInstalled;
static int hardwareBreakpointCount = 0;//硬件断点计数
static std::vector<Memory::VEHHOOK_HANDLE> VehInfos;
LONG NTAPI Memory::VEHHook::ExceptionHandler(PEXCEPTION_POINTERS ExceptionInfo) {
    PVOID ExceptionAddress = ExceptionInfo->ExceptionRecord->ExceptionAddress;//获取异常地址
    for (int i = 0,size = VehInfos.size(); i < size; i++) {
        if (VehInfos[i].address == ExceptionAddress) {
            bool jmp = VehInfos[i].callBack(ExceptionInfo);
            VEHHook* veh = (VEHHook*)VehInfos[i].veh;
            #ifdef _WIN64
            switch (VehInfos[i].type)
            {
            case VEH_TYPE::INT3:
                if (!jmp) {
                    ExceptionInfo->ContextRecord->Rip -= 1;
                    veh->Write((size_t)ExceptionAddress, VehInfos[i].oldAddrData,true);
                    VehInfos[i].isInstalled = false;
                }
                else {
                    ExceptionInfo->ContextRecord->Rip += VehInfos[i].size;
                }
                break;
            case VEH_TYPE::INT3_HOOKJUMP:
                ExceptionInfo->ContextRecord->Rip = (DWORD)VehInfos[i].newAddress;
                break;
            case VEH_TYPE::HARDWARE_BREAKPOINT:
                ExceptionInfo->ContextRecord->Rip += VehInfos[i].size;
                break;
            case VEH_TYPE::HAEDWARE_HOOKJUMP:
                ExceptionInfo->ContextRecord->Rip = (DWORD64)VehInfos[i].newAddress;
                break;
            }
            #else
            switch (VehInfos[i].type)
            {
            case VEH_TYPE::INT3:
                if (!jmp) {
                    ExceptionInfo->ContextRecord->Eip -= 1;
                    veh->Write((size_t)ExceptionAddress, VehInfos[i].oldAddrData,true);
                    VehInfos[i].isInstalled = false;
                }
                else {
                    ExceptionInfo->ContextRecord->Eip += VehInfos[i].size;
                }
                break;
            case VEH_TYPE::INT3_HOOKJUMP:
                ExceptionInfo->ContextRecord->Eip = (DWORD)VehInfos[i].newAddress;
                break;
            case VEH_TYPE::HARDWARE_BREAKPOINT:
                ExceptionInfo->ContextRecord->Eip += VehInfos[i].size;
                break;
            case VEH_TYPE::HAEDWARE_HOOKJUMP:
                ExceptionInfo->ContextRecord->Eip = (DWORD64)VehInfos[i].newAddress;
                break;
            }
            #endif
            return EXCEPTION_CONTINUE_EXECUTION;
        }
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

Memory::VEHHook::VEHHook() : exceptionHandler(nullptr) {
}

Memory::VEHHook::~VEHHook() {
    Uninstall();
};

bool Memory::VEHHook::Install() {
    this->Close();
    this->Open(GetCurrentProcessId());
    if (VehInstalled) return false;
    exceptionHandler = AddVectoredExceptionHandler(1, ExceptionHandler);
    VehInstalled = this->exceptionHandler != nullptr;
    return this->exceptionHandler != nullptr;
};

bool Memory::VEHHook::Uninstall() {
    //HARDWARE_BREAKPOINT
    bool hasHardwareBreakpoint = false;
    if (this->exceptionHandler) {
        for (int i = 0, size = VehInfos.size(); i < size; i++) {
            if (!(hasHardwareBreakpoint) && VehInfos[i].type == VEH_TYPE::HARDWARE_BREAKPOINT) {
                this->DelAddress(VehInfos[i].code);
                return this->Uninstall();//重载卸载,防异常报错
            }
            else if(VehInfos[i].type != VEH_TYPE::HARDWARE_BREAKPOINT) {
                this->DelAddress(VehInfos[i].code);
            }
        };
        RemoveVectoredExceptionHandler(this->exceptionHandler);
        this->exceptionHandler = nullptr;
    }
    VehInstalled = this->exceptionHandler != nullptr;
    return this->exceptionHandler == nullptr;
};

DWORD Memory::VEHHook::AddAddress(VEHHOOK_PARAM param)
{
    DWORD ret = NULL;
    if (!VehInstalled) ret = -4; else if (param.address == NULL) ret = -1; else if (param.callBack == nullptr) ret = -2;
    for (int i = 0, size = VehInfos.size(); i < size; i++) if (VehInfos[i].code == param.code || VehInfos[i].address == param.address) {
        ret = -3; 
        break;
    }
    if (ret == NULL) {
        VEHHOOK_HANDLE handle{};
        handle.code = param.code;
        handle.address = param.address;
        handle.callBack = param.callBack;
        handle.type = param.type;
        handle.size = param.size;
        handle.newAddrData = param.data;
        handle.veh = this;
        handle.isInstalled = true;
        HANDLE hsnap = nullptr;
        THREADENTRY32 te32{};
        BOOL state = FALSE;
        switch (handle.type) {
            case VEH_TYPE::INT3:
                this->Read((size_t)handle.address, handle.oldAddrData, 1);
                this->Write((size_t)handle.address, { 0xCC }, true);
                break;
            case VEH_TYPE::INT3_HOOKJUMP:
                handle.newAddrSize = param.data.size() + 50;
                handle.newAddress = (PVOID)this->Allocate(handle.newAddrSize);
                this->Write((size_t)handle.newAddress, param.data, true);
                this->hookJump((size_t)(handle.newAddress) + param.data.size(), (size_t)(handle.address) + handle.size);
                this->Read((size_t)handle.address, handle.oldAddrData, 1);
                this->Write((size_t)handle.address, { 0xCC }, true);
                break;
            case VEH_TYPE::HARDWARE_BREAKPOINT:
                if (hardwareBreakpointCount == 4) break;
                hsnap = this->API::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, (HANDLE)GetCurrentProcessId());
                te32 = { 0 };
                te32.dwSize = sizeof(THREADENTRY32);
                state = this->API::Thread32First(hsnap, &te32);
                if (state == TRUE) hardwareBreakpointCount++;
                while (state) {
                    if (te32.th32OwnerProcessID == GetCurrentProcessId()) {
                        OBJECT_ATTRIBUTES objAttr{};
                        InitializeObjectAttributes(&objAttr, NULL, 0, NULL, NULL);
                        objAttr.ObjectName = NULL;
                        CLIENT_ID clientId{};
                        clientId.UniqueProcess = (HANDLE)te32.th32OwnerProcessID;
                        clientId.UniqueThread = (HANDLE)te32.th32ThreadID;
                        HANDLE hThread = NULL;
                        this->API::ZwOpenThread(&hThread, THREAD_ALL_ACCESS, &objAttr, &clientId);
                        if (hThread != NULL) {
                            CONTEXT context{ NULL };
                            context.ContextFlags = CONTEXT_ALL;
                            this->API::ZwGetContextThread(hThread, &context);
                            if (context.Dr0 == NULL) {
                                context.Dr0 = (size_t)handle.address;
                                context.Dr7 |= 0x3; // 启用Dr0的局部断点
                            }
                            else if (context.Dr1 == NULL) {
                                context.Dr1 = (size_t)handle.address;
                                context.Dr7 |= 0xC;// 启用Dr1的局部断点
                            }
                            else if (context.Dr2 == NULL) {
                                context.Dr2 = (size_t)handle.address;
                                context.Dr7 = 0x30; // 启用Dr2的局部断点
                            }
                            else if (context.Dr3 == NULL) {
                                context.Dr3 = (size_t)handle.address;
                                context.Dr7 = 0xC0; // 启用Dr3的局部断点
                            }
                            this->API::ZwSetContextThread(hThread, &context);
                            this->API::ZwClose(hThread);
                        }
                    }
                    state = this->API::Thread32Next(hsnap, &te32);
                };
                this->API::ZwClose(hsnap);
                break;
            case VEH_TYPE::HAEDWARE_HOOKJUMP:
                handle.newAddrSize = param.data.size() + 50;
                handle.newAddress = (PVOID)this->Allocate(handle.newAddrSize);
                this->Write((size_t)handle.newAddress, param.data, true);
                this->hookJump((size_t)(handle.newAddress) + param.data.size(), (size_t)(handle.address) + handle.size);
                if (hardwareBreakpointCount == 4) break;
                hsnap = this->API::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, (HANDLE)GetCurrentProcessId());
                te32.dwSize = sizeof(THREADENTRY32);
                state = this->API::Thread32First(hsnap, &te32);
                if (state == TRUE) hardwareBreakpointCount++;
                while (state) {
                    if (te32.th32OwnerProcessID == GetCurrentProcessId()) {
                        OBJECT_ATTRIBUTES objAttr{};
                        InitializeObjectAttributes(&objAttr, NULL, 0, NULL, NULL);
                        objAttr.ObjectName = NULL;
                        CLIENT_ID clientId{};
                        clientId.UniqueProcess = (HANDLE)te32.th32OwnerProcessID;
                        clientId.UniqueThread = (HANDLE)te32.th32ThreadID;
                        HANDLE hThread = NULL;
                        this->API::ZwOpenThread(&hThread, THREAD_ALL_ACCESS, &objAttr, &clientId);
                        if (hThread != NULL) {
                            CONTEXT context{ NULL };
                            context.ContextFlags = CONTEXT_ALL;
                            this->API::ZwGetContextThread(hThread, &context);
                            context.Dr7 = NULL;
                            if (context.Dr0 == NULL) {
                                context.Dr0 = (size_t)handle.address;
                                context.Dr7 = 0x3; // 启用Dr0的局部断点
                            }
                            else if (context.Dr1 == NULL) {
                                context.Dr1 = (size_t)handle.address;
                                context.Dr7 = 0xF;// 启用Dr1的局部断点
                            }
                            else if (context.Dr2 == NULL) {
                                context.Dr2 = (size_t)handle.address;
                                context.Dr7 = 0x3F; // 启用Dr2的局部断点
                            }
                            else if (context.Dr3 == NULL) {
                                context.Dr3 = (size_t)handle.address;
                                context.Dr7 = 0xFF; // 启用Dr3的局部断点
                            }
                            this->API::ZwSetContextThread(hThread, &context);
                            this->API::ZwClose(hThread);
                        }
                    }
                    state = this->API::Thread32Next(hsnap, &te32);
                };
                this->API::ZwClose(hsnap);
                break;
        }
        VehInfos.push_back(handle);
    }
    return ret;
};

bool Memory::VEHHook::DelAddress(DWORD code)
{
    bool flag = false;
    BOOL state = FALSE, flag_ = FALSE;
    HANDLE hsnap = nullptr;
    THREADENTRY32 te32 = { 0 };
    for (int i = 0, size = VehInfos.size(); i < size; i++) {
        if (VehInfos[i].code == code) {
            if (VehInfos[i].isInstalled) {
                switch (VehInfos[i].type)
                {
                case VEH_TYPE::INT3:
                    this->Write((size_t)VehInfos[i].address, VehInfos[i].oldAddrData, true);
                    break;
                case VEH_TYPE::INT3_HOOKJUMP:
                    this->Write((size_t)VehInfos[i].address, VehInfos[i].oldAddrData, true);
                    this->Free((size_t)VehInfos[i].newAddress, VehInfos[i].newAddrSize);
                    break;
                case VEH_TYPE::HARDWARE_BREAKPOINT:
                    hsnap = this->API::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, (HANDLE)GetCurrentProcessId());
                    te32.dwSize = sizeof(THREADENTRY32);
                    state = this->API::Thread32First(hsnap, &te32);
                    while (state) {
                        if (te32.th32OwnerProcessID == GetCurrentProcessId()) {
                            OBJECT_ATTRIBUTES objAttr{};
                            InitializeObjectAttributes(&objAttr, NULL, 0, NULL, NULL);
                            objAttr.ObjectName = NULL;
                            CLIENT_ID clientId{};
                            clientId.UniqueProcess = (HANDLE)te32.th32OwnerProcessID;
                            clientId.UniqueThread = (HANDLE)te32.th32ThreadID;
                            HANDLE hThread = NULL;
                            this->API::ZwOpenThread(&hThread, THREAD_ALL_ACCESS, &objAttr, &clientId);
                            if (hThread != NULL) {
                                CONTEXT context{ NULL };
                                context.ContextFlags = CONTEXT_ALL;
                                this->API::ZwGetContextThread(hThread, &context);
                                if (context.Dr0 == (size_t)VehInfos[i].address)context.Dr0 = NULL, flag_ = true;
                                else if (context.Dr1 == (size_t)VehInfos[i].address)context.Dr1 = NULL, flag_ = true;
                                else if (context.Dr2 == (size_t)VehInfos[i].address)context.Dr2 = NULL, flag_ = true;
                                else if (context.Dr3 == (size_t)VehInfos[i].address)context.Dr3 = NULL, flag_ = true;
                                if (context.Dr0 == NULL && context.Dr1 != NULL) {
                                    context.Dr0 = context.Dr1;
                                    context.Dr7 = 0x3;
                                    context.Dr1 = NULL;
                                }
                                if (context.Dr1 == NULL && context.Dr2 != NULL) {
                                    context.Dr1 = context.Dr2;
                                    context.Dr7 = 0xF;
                                    context.Dr2 = NULL;
                                }
                                if (context.Dr2 == NULL && context.Dr3 != NULL) {
                                    context.Dr2 = context.Dr3;
                                    context.Dr7 = 0x3F;
                                    context.Dr3 = NULL;
                                }
                                if (context.Dr0 == NULL && context.Dr1 == NULL && context.Dr2 == NULL && context.Dr3 == NULL) {
                                    context.Dr7 = NULL;
                                }
                                this->API::ZwSetContextThread(hThread, &context);
                                this->API::ZwClose(hThread);
                            }
                        }
                        state = this->API::Thread32Next(hsnap, &te32);
                    };
                    if (flag_) hardwareBreakpointCount--;
                    this->API::ZwClose(hsnap);
                    break;
                case VEH_TYPE::HAEDWARE_HOOKJUMP:
                    this->Free((size_t)VehInfos[i].newAddress, VehInfos[i].newAddrSize);
                    hsnap = this->API::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, (HANDLE)GetCurrentProcessId());
                    te32.dwSize = sizeof(THREADENTRY32);
                    state = this->API::Thread32First(hsnap, &te32);
                    while (state) {
                        if (te32.th32OwnerProcessID == GetCurrentProcessId()) {
                            OBJECT_ATTRIBUTES objAttr{};
                            InitializeObjectAttributes(&objAttr, NULL, 0, NULL, NULL);
                            objAttr.ObjectName = NULL;
                            CLIENT_ID clientId{};
                            clientId.UniqueProcess = (HANDLE)te32.th32OwnerProcessID;
                            clientId.UniqueThread = (HANDLE)te32.th32ThreadID;
                            HANDLE hThread = NULL;
                            this->API::ZwOpenThread(&hThread, THREAD_ALL_ACCESS, &objAttr, &clientId);
                            if (hThread != NULL) {
                                CONTEXT context{ NULL };
                                context.ContextFlags = CONTEXT_ALL;
                                this->API::ZwGetContextThread(hThread, &context);
                                if (context.Dr0 == (size_t)VehInfos[i].address)context.Dr0 = NULL, flag_ = true;
                                else if (context.Dr1 == (size_t)VehInfos[i].address)context.Dr1 = NULL, flag_ = true;
                                else if (context.Dr2 == (size_t)VehInfos[i].address)context.Dr2 = NULL, flag_ = true;
                                else if (context.Dr3 == (size_t)VehInfos[i].address)context.Dr3 = NULL, flag_ = true;
                                if (context.Dr0 == NULL && context.Dr1 != NULL) {
                                    context.Dr0 = context.Dr1;
                                    context.Dr7 = 0x3;
                                    context.Dr1 = NULL;
                                }
                                if (context.Dr1 == NULL && context.Dr2 != NULL) {
                                    context.Dr1 = context.Dr2;
                                    context.Dr7 = 0xF;
                                    context.Dr2 = NULL;
                                }
                                if (context.Dr2 == NULL && context.Dr3 != NULL) {
                                    context.Dr2 = context.Dr3;
                                    context.Dr7 = 0x3F;
                                    context.Dr3 = NULL;
                                }
                                if (context.Dr0 == NULL && context.Dr1 == NULL && context.Dr2 == NULL && context.Dr3 == NULL) {
                                    context.Dr7 = NULL;
                                }
                                this->API::ZwSetContextThread(hThread, &context);
                                this->API::ZwClose(hThread);
                            }
                        }
                        state = this->API::Thread32Next(hsnap, &te32);
                    };
                    if (flag_) hardwareBreakpointCount--;
                    this->API::ZwClose(hsnap);
                    break;
                }
            }
            VehInfos.erase(VehInfos.begin() + i);
            flag = true;
            break;
        }
    }
    return flag;
};

bool Memory::VEHHook::ModAddress(DWORD code, Byteset data)
{
    bool flag = false;
    for (int i = 0, size = VehInfos.size(); i < size; i++) {
        if (VehInfos[i].code == code) {
            flag = true;
            switch (VehInfos[i].type)
            {
            case VEH_TYPE::INT3:
                if (!VehInfos[i].isInstalled) {
                    this->Read((size_t)VehInfos[i].address, VehInfos[i].oldAddrData, 1);
                    this->Write((size_t)VehInfos[i].address, { 0xCC }, true);
                }
                break;
            case VEH_TYPE::INT3_HOOKJUMP:
                this->Free((size_t)VehInfos[i].newAddress, VehInfos[i].newAddrSize);
                VehInfos[i].newAddrSize = data.size() + 50;
                VehInfos[i].newAddress = (PVOID)this->Allocate(VehInfos[i].newAddrSize);
                this->Write((size_t)VehInfos[i].newAddress, data, true);
                break;
            case VEH_TYPE::HAEDWARE_HOOKJUMP:
                this->Free((size_t)VehInfos[i].newAddress, VehInfos[i].newAddrSize);
                VehInfos[i].newAddrSize = data.size() + 50;
                VehInfos[i].newAddress = (PVOID)this->Allocate(VehInfos[i].newAddrSize);
                this->Write((size_t)VehInfos[i].newAddress, data, true);
                break;
            }
            break;
        }
    }
    return flag;
};

Memory::VEHHOOK_INFO Memory::VEHHook::GetRegisters(PEXCEPTION_POINTERS info)
{
    VEHHOOK_INFO info_{};
    #ifdef _WIN64
    info_.rip = info->ContextRecord->Rip;
    info_.rax = info->ContextRecord->Rax;
    info_.rbx = info->ContextRecord->Rbx;
    info_.rcx = info->ContextRecord->Rcx;
    info_.rdx = info->ContextRecord->Rdx;
    info_.rsi = info->ContextRecord->Rsi;
    info_.rdi = info->ContextRecord->Rdi;
    info_.rbp = info->ContextRecord->Rbp;
    info_.rsp = info->ContextRecord->Rsp;
    info_.r8 = info->ContextRecord->R8;
    info_.r9 = info->ContextRecord->R9;
    info_.r10 = info->ContextRecord->R10;
    info_.r11 = info->ContextRecord->R11;
    info_.r12 = info->ContextRecord->R12;
    info_.r13 = info->ContextRecord->R13;
    info_.r14 = info->ContextRecord->R14;
    info_.r15 = info->ContextRecord->R15;
    info_.xmm0 = info->ContextRecord->Xmm0;
    info_.xmm1 = info->ContextRecord->Xmm1;
    info_.xmm2 = info->ContextRecord->Xmm2;
    info_.xmm3 = info->ContextRecord->Xmm3;
    info_.xmm4 = info->ContextRecord->Xmm4;
    info_.xmm5 = info->ContextRecord->Xmm5;
    info_.xmm6 = info->ContextRecord->Xmm6;
    info_.xmm7 = info->ContextRecord->Xmm7;
    info_.xmm8 = info->ContextRecord->Xmm8;
    info_.xmm9 = info->ContextRecord->Xmm9;
    info_.xmm10 = info->ContextRecord->Xmm10;
    info_.xmm11 = info->ContextRecord->Xmm11;
    info_.xmm12 = info->ContextRecord->Xmm12;
    info_.xmm13 = info->ContextRecord->Xmm13;
    info_.xmm14 = info->ContextRecord->Xmm14;
    info_.xmm15 = info->ContextRecord->Xmm15;
    #else
    info_.eip = info->ContextRecord->Eip;
    info_.eax = info->ContextRecord->Eax;
    info_.ebx = info->ContextRecord->Ebx;
    info_.ecx = info->ContextRecord->Ecx;
    info_.edx = info->ContextRecord->Edx;
    info_.esi = info->ContextRecord->Esi;
    info_.edi = info->ContextRecord->Edi;
    info_.ebp = info->ContextRecord->Ebp;
    info_.esp = info->ContextRecord->Esp;
    #endif
    return info_;
};

size_t Memory::VEHHook::GetModuleBase(std::string moduleName) const
{
    return this->R3::GetModuleBase(moduleName);
}

Memory::PVEHHOOK_HANDLE Memory::VEHHook::GetHandle(PVOID address)
{
    for (int i = 0, size = VehInfos.size(); i < size; i++) {
        if (VehInfos[i].address == address) {
            return &VehInfos[i];
        }
    }
    return PVEHHOOK_HANDLE();
}