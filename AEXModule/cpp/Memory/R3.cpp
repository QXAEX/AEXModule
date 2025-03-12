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

size_t __stdcall MemoryR3::R3::Allocate(SIZE_T size) const
{
    PVOID address{ 0 };
    NTDLL::ZwAllocateVirtualMemory(this->hProcess, &address, 0, &size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    return (size_t)address;
}

bool __stdcall MemoryR3::R3::Free(PVOID address, SIZE_T size) const
{
    return  NTDLL::ZwFreeVirtualMemory(this->hProcess, address, &size, MEM_RELEASE) == NULL;
}

std::vector<PVOID> __stdcall MemoryR3::R3::Search(std::string buffer, PVOID startAddress, PVOID endAddress, R3_SEARCH_TYPE type, size_t limit, bool isVirtual) const
{
    std::vector<std::string> buffer_ = Text::text_split(buffer, " ");
    while (buffer_.front() == "??") buffer_.erase(buffer_.begin());
    while (buffer_.back() == "??") buffer_.erase(buffer_.end() - 1);
    Byteset data;
    Byteset vagueContent;
    data.resize(buffer_.size());
    vagueContent.resize(buffer_.size());
    for (int i = 0; i < buffer_.size(); i++) {
        if (buffer_[i] == "??") {
            data[i] = 0x0;
            vagueContent[i] = 0xFF;
        } else {
            data[i] = (UCHAR)Text::text_16_to_10(buffer_[i]);
            vagueContent[i] = 0x0;
        }
    }
    std::vector<PVOID> addrList{};
    size_t address = (size_t)startAddress;
    MEMORY_BASIC_INFORMATION MemoryInfoMation{};
    DWORD ProtectStatus = NULL;
    switch (type) {
    case R3_SEARCH_TYPE::R3_SEARCH_EXECUTE_RW: ProtectStatus = PAGE_EXECUTE_READWRITE; break;
    case R3_SEARCH_TYPE::R3_SEARCH_RW: ProtectStatus = PAGE_READWRITE; break;
    case R3_SEARCH_TYPE::R3_SEARCH_READ: ProtectStatus = PAGE_READONLY; break;
    case R3_SEARCH_TYPE::R3_SEARCH_EXECUTE_WRITE: ProtectStatus = PAGE_EXECUTE_WRITECOPY; break;
    case R3_SEARCH_TYPE::R3_SEARCH_EXECUTE_READ: ProtectStatus = PAGE_EXECUTE_READ; break;
    case R3_SEARCH_TYPE::R3_SEARCH_EXECUTE: ProtectStatus = PAGE_EXECUTE; break;
    case R3_SEARCH_TYPE::R3_SEARCH_ALL: ProtectStatus = -1; break;
    };
    bool flag;
    Byteset readData;
    PVOID preadData = nullptr;
    __int64 index[2]{}, size[2]{};
    while (NTDLL::ZwQueryVirtualMemory(this->hProcess, (LPCVOID)address, NTDLL::MemoryBasicInformation, &MemoryInfoMation, sizeof(MemoryInfoMation), nullptr) == NULL) {
        flag = ProtectStatus == MemoryInfoMation.Protect || ProtectStatus == -1;
        if (MemoryInfoMation.State == MEM_COMMIT && flag && MemoryInfoMation.BaseAddress >= startAddress && (MemoryInfoMation.BaseAddress <= endAddress || endAddress == nullptr)) {
            readData.resize(MemoryInfoMation.RegionSize);
            if (Read(MemoryInfoMation.BaseAddress, readData, MemoryInfoMation.RegionSize, isVirtual)) {
                preadData = (PVOID)readData.data();
                for (index[0] = 0, size[0] = readData.size() - data.size(); index[0] < size[0]; index[0]++) {
                    if ((UCHAR)readData[index[0]] == (UCHAR)data[0]) {
                        bool isVague = true;
                        for (index[1] = 1, size[1] = data.size() - 1; index[1] < size[1]; index[1]++) {
                            if ((UCHAR)readData[index[0] + index[1]] != (UCHAR)data[index[1]] && (UCHAR)vagueContent[index[1]] != 0xFF) {
                                isVague = false;
                                break;
                            }
                        }
                        if (isVague) {
                            addrList.push_back((PVOID)((size_t)(MemoryInfoMation.BaseAddress) + index[0]));
                            if (limit != NULL && addrList.size() == limit) break;
                        }
                    }
                }
            }
            if (limit != NULL && addrList.size() >= limit) break;
        }
        address += MemoryInfoMation.RegionSize;
    }
    return addrList;
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
