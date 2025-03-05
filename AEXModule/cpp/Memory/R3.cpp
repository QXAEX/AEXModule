#include "../../h/Memory/R3.h"
#include <winternl.h>
#include "../../h/api/data/ntdll.h"
#include <TlHelp32.h>
MemoryR3::R3::R3(DWORD pid)
{
    this->Open(pid);
}

MemoryR3::R3::~R3()
{
    this->Close();
}

bool __stdcall MemoryR3::R3::Open(DWORD pid)
{
    this->PID = pid;
    NTDLL::OBJECT_ATTRIBUTES objAttr{};
    InitializeObjectAttributes(&objAttr, NULL, 0, NULL, NULL);
    objAttr.ObjectName = NULL;
    NTDLL::CLIENT_ID clientId{};
    clientId.UniqueProcess = (HANDLE)pid;
    clientId.UniqueThread = NULL;
    NTSTATUS status = NTDLL::ZwOpenProcess(&this->hProcess, PROCESS_ALL_ACCESS, &objAttr, &clientId);
    return this->hProcess != NULL;
}

void __stdcall MemoryR3::R3::Close()
{
    NTDLL::ZwClose(this->hProcess);
    this->hProcess = NULL;

}

bool __stdcall MemoryR3::R3::Is64() const
{
    BOOL aaaa = false;
    IsWow64Process(this->hProcess, &aaaa);
    return (!aaaa);
}

PVOID __stdcall MemoryR3::R3::GetModuleBase(std::string moduleName) const
{
    DWORD processId = 0;
    std::string modName_ = moduleName;
    PVOID modaddress{ 0 };
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, this->PID);
    if (snapshot != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 processEntry = { sizeof(MODULEENTRY32) };
        if (Module32First(snapshot, &processEntry)) {
            do {
                if (Text::text_to_str(processEntry.szModule) == modName_) {
                    modaddress = processEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(snapshot, &processEntry));
        };
        return modaddress;
    };
    CloseHandle(snapshot);
    return 0;
}

bool __stdcall MemoryR3::R3::Write(PVOID address, Byteset value, bool isVirtual) const
{
    bool  flag{ 0 };
    SIZE_T data{ 0 };
    DWORD oldProtect = 0;
    if (isVirtual)VirtualProtect(address, (ULONG_PTR)value.size(), PAGE_EXECUTE_READWRITE, &oldProtect);
    flag = NTDLL::ZwWriteVirtualMemory(this->hProcess, (LPCVOID)address, (PVOID)value.data(), value.size(), &data) == NULL;
    if (isVirtual)VirtualProtect(address, (ULONG_PTR)value.size(), oldProtect, &oldProtect);
    return flag;
}

bool __stdcall MemoryR3::R3::Read(PVOID address, Byteset& value, DWORD rlength, bool isVirtual) const
{
    SIZE_T data{ 0 };
    DWORD oldProtect{ 0 };
    value.clear();
    value.resize(rlength);
    if (isVirtual)VirtualProtect(address, rlength, PAGE_EXECUTE_READWRITE, &oldProtect);
    bool flag = NTDLL::ZwReadVirtualMemory(this->hProcess, (LPCVOID)address, (PVOID)value.data(), rlength, &data) == NULL;
    if (isVirtual)VirtualProtect(address, rlength, oldProtect, &oldProtect);
    return flag;
}

bool __stdcall MemoryR3::R3::VirtualProtect(PVOID address, ULONG_PTR size, DWORD protection, PDWORD oldProtection) const
{
    return  NTDLL::ZwProtectVirtualMemory(this->hProcess, &address, &size, protection, oldProtection) == NULL;
}

size_t __stdcall MemoryR3::R3::Allocate(DWORD size) const
{
    PVOID address{ 0 };
    NTDLL::ZwAllocateVirtualMemory(this->hProcess, &address, 0, &size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    return (size_t)address;
}

bool __stdcall MemoryR3::R3::Free(PVOID address, DWORD size) const
{
    return  NTDLL::ZwFreeVirtualMemory(this->hProcess, address, &size, MEM_RELEASE) == NULL;
}

std::vector<PVOID> __stdcall MemoryR3::R3::Search(Byteset buffer, Byteset vagueContent, PVOID startAddress, PVOID endAddress, int type, size_t limit, bool isVirtual) const
{
    if (buffer.empty()) return std::vector<PVOID>();
    std::vector<PVOID> addresses{};
    size_t address = (size_t)startAddress;
    MEMORY_BASIC_INFORMATION MemoryInfoMation{};
    bool vague = !vagueContent.empty();
    while (NTDLL::ZwQueryVirtualMemory(this->hProcess, (LPCVOID)address, NTDLL::MemoryBasicInformation, &MemoryInfoMation, sizeof(MemoryInfoMation), nullptr) == 0) {
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

        if (MemoryInfoMation.State == MEM_COMMIT && flag &&
            MemoryInfoMation.BaseAddress >= startAddress &&
            (MemoryInfoMation.BaseAddress <= endAddress || endAddress == nullptr)) {
            Byteset data;
            if (Read(MemoryInfoMation.BaseAddress, data, MemoryInfoMation.RegionSize, isVirtual)) {
                __int64 index = data.find(buffer, NULL, vague, vagueContent);
                while (index != -1) {
                    addresses.push_back((PVOID)((size_t)(MemoryInfoMation.BaseAddress) + index));
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

bool __stdcall MemoryR3::R3::hookJump(PVOID address, PVOID newAddress, Byteset complement, bool isVirtual) const
{
    Byteset data;
    if (this->Is64()) {
        data.append({ 0xFF, 0x25 });
        data.append({ 0x00, 0x00 , 0x00, 0x00 });
        data.append((size_t)newAddress);
    }
    else {
        data.append({ 0xE9 });
        data.append((int)(0xFFFFFFFF - ((size_t)address - (size_t)newAddress) - 4));
    }
    data.append(complement);
    return this->Write(address, data, isVirtual);
}

Byteset __stdcall MemoryR3::R3::pointerChain(PVOID address, std::initializer_list<size_t> offsets, bool isVirtual) const
{
    Byteset buffer;
    bool flag = true;
    int readSize = (this->Is64() ? 8 : 4);
    size_t pointer = (size_t)address;
    this->Read(address, buffer, readSize, isVirtual);
    pointer = *(size_t*)buffer.data();
    if (offsets.size() > 0) {
        for (int i = 0, size = offsets.size() - 1; i < size; i++) {
            if (!this->Read((PVOID)(pointer + offsets.begin()[i]), buffer, readSize, isVirtual)) {
                flag = false;
                break;
            }
            pointer = *(size_t*)buffer.data();
        }
        if (flag && this->Read((PVOID)(pointer + offsets.begin()[offsets.size() - 1]), buffer, readSize, isVirtual)) {
            return buffer.append({ 0,0 });
        }
    }
    return pointer;
}
