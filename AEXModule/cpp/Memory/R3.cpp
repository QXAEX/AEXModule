#include "../../h/Memory/R3.h"
#include "../../h/api/data/ntdll.h"
#include "../../h/Thread.h"
#include <winternl.h>
#include <TlHelp32.h>
#include <immintrin.h>
#include <emmintrin.h>
#include <mutex>
#include <algorithm>

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
    PVOID address{ nullptr };
    NTDLL::ZwAllocateVirtualMemory(this->hProcess, &address, 0, &size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    return (size_t)address;
}

bool __stdcall MemoryR3::R3::Free(PVOID address, SIZE_T size) const
{
    return  NTDLL::ZwFreeVirtualMemory(this->hProcess, address, &size, MEM_RELEASE) == NULL;
}

//std::vector<PVOID> __stdcall MemoryR3::R3::Search(std::string buffer, bool ifThread, PVOID startAddress, PVOID endAddress, R3_SEARCH_TYPE type, size_t limit, bool isVirtual) const
//{
//    std::vector<std::string> buffer_ = Text::text_split(buffer, " ");
//    while (!buffer_.empty() && buffer_.front() == "??") buffer_.erase(buffer_.begin());
//    while (!buffer_.empty() && buffer_.back() == "??") buffer_.erase(buffer_.end() - 1);
//    const size_t patternSize = buffer_.size();
//    if (patternSize == 0) return {};
//    std::vector<uint8_t> pattern;
//    std::vector<uint8_t> mask;
//    pattern.reserve(patternSize);
//    mask.reserve(patternSize);
//    for (const auto& byte : buffer_) {
//        if (byte == "??") {
//            pattern.emplace_back(0);
//            mask.emplace_back(0);
//        }
//        else {
//            pattern.emplace_back(static_cast<uint8_t>(Text::text_16_to_10(byte)));
//            mask.emplace_back(0xFF);
//        }
//    }
//    const __m128i patternReg = _mm_loadu_si128(reinterpret_cast<const __m128i*>(pattern.data()));
//    const __m128i maskReg = _mm_loadu_si128(reinterpret_cast<const __m128i*>(mask.data()));
//    std::vector<PVOID> addrList;
//    std::mutex listMutex;
//    size_t address = reinterpret_cast<size_t>(startAddress);
//    const size_t endAddr = reinterpret_cast<size_t>(endAddress);
//    MEMORY_BASIC_INFORMATION mbi{};
//    const DWORD protectStatus = [&] {
//        switch (type) {
//        case R3_SEARCH_EXECUTE_RW: return PAGE_EXECUTE_READWRITE;
//        case R3_SEARCH_RW: return PAGE_READWRITE;
//        case R3_SEARCH_READ: return PAGE_READONLY;
//        case R3_SEARCH_EXECUTE_WRITE: return PAGE_EXECUTE_WRITECOPY;
//        case R3_SEARCH_EXECUTE_READ: return PAGE_EXECUTE_READ;
//        case R3_SEARCH_EXECUTE: return PAGE_EXECUTE;
//        default: return -1;
//        }
//        }();
//    if (ifThread) {
//        DWORD threadCode = 0;
//        Thread thread;
//        while (NTDLL::ZwQueryVirtualMemory(hProcess, reinterpret_cast<LPCVOID>(address), NTDLL::MemoryBasicInformation, &mbi, sizeof(mbi), nullptr) == 0)
//        {
//            const auto baseAddr = mbi.BaseAddress;
//            const auto regionSize = mbi.RegionSize;
//            const auto currentProtect = mbi.Protect;
//            const auto currentState = mbi.State;
//            thread.add(threadCode++, [=, &addrList, &listMutex](PThread thread) {
//                Byteset regionData;
//                if (currentState == MEM_COMMIT &&
//                    (protectStatus == static_cast<DWORD>(-1) || currentProtect == protectStatus) &&
//                    reinterpret_cast<size_t>(baseAddr) >= address &&
//                    (endAddr == 0 || reinterpret_cast<size_t>(baseAddr) <= endAddr))
//                {
//                    regionData.resize(regionSize);
//                    if (Read(baseAddr, regionData, static_cast<DWORD>(regionSize), isVirtual)) {
//                        const uint8_t* scanStart = regionData.data();
//                        const uint8_t* scanEnd = scanStart + regionSize - patternSize;
//                        for (; scanStart <= scanEnd; ++scanStart) {
//                            if (mask[0] && (*scanStart != pattern[0])) continue;
//                            const __m128i target = _mm_loadu_si128(reinterpret_cast<const __m128i*>(scanStart));
//                            const __m128i matched = _mm_cmpeq_epi8(_mm_and_si128(target, maskReg), patternReg);
//                            if (_mm_movemask_epi8(matched) == 0xFFFF) {
//                                const size_t offset = scanStart - regionData.data();
//                                {
//                                    std::lock_guard<std::mutex> lock(listMutex);
//                                    addrList.emplace_back(
//                                        reinterpret_cast<uint8_t*>(baseAddr) + offset);
//                                }
//                                if (limit && addrList.size() >= limit) return;
//                            }
//                        }
//                    }
//                }
//                });
//            address += mbi.RegionSize;
//            if (endAddr && address > endAddr) break;
//        }
//        thread.wait();
//        std::sort(addrList.begin(), addrList.end());
//        addrList.erase(std::unique(addrList.begin(), addrList.end()), addrList.end());
//    }
//    else {
//        while (NTDLL::ZwQueryVirtualMemory(hProcess, reinterpret_cast<LPCVOID>(address), NTDLL::MemoryBasicInformation, &mbi, sizeof(mbi), nullptr) == 0)
//        {
//            if (mbi.State == MEM_COMMIT &&
//                (protectStatus == static_cast<DWORD>(-1) || mbi.Protect == protectStatus) &&
//                reinterpret_cast<size_t>(mbi.BaseAddress) >= address &&
//                (endAddr == 0 || reinterpret_cast<size_t>(mbi.BaseAddress) <= endAddr))
//            {
//                Byteset regionData;
//                const size_t regionSize = mbi.RegionSize;
//                regionData.resize(regionSize);
//                if (Read(mbi.BaseAddress, regionData, static_cast<DWORD>(regionSize), isVirtual)) {
//                    const uint8_t* scanStart = regionData.data();
//                    const uint8_t* scanEnd = scanStart + regionSize - patternSize;
//                    for (; scanStart <= scanEnd; ++scanStart) {
//                        if (mask[0] && (*scanStart != pattern[0])) continue;
//                        const __m128i target = _mm_loadu_si128(reinterpret_cast<const __m128i*>(scanStart));
//                        const __m128i matched = _mm_cmpeq_epi8(_mm_and_si128(target, maskReg), patternReg);
//                        if (_mm_movemask_epi8(matched) == 0xFFFF) {
//                            const size_t offset = scanStart - regionData.data();
//                            addrList.emplace_back(reinterpret_cast<uint8_t*>(mbi.BaseAddress) + offset);
//                            if (limit && addrList.size() >= limit) return addrList;
//                        }
//                    }
//                }
//            }
//            address += mbi.RegionSize;
//            if (endAddr && address > endAddr) break;
//        }
//    }
//    return addrList;
//}
std::vector<PVOID> __stdcall MemoryR3::R3::Search(std::string buffer, bool ifThread, PVOID startAddress, PVOID endAddress, R3_SEARCH_TYPE type, size_t limit, bool isVirtual) const
{
    if (buffer.empty()) return std::vector<PVOID>();
    std::vector<PVOID> addresses{};
    size_t address = (size_t)startAddress;
    MEMORY_BASIC_INFORMATION MemoryInfoMation{};
    const DWORD protectStatus = [&] {
        switch (type) {
        case R3_SEARCH_EXECUTE_RW: return PAGE_EXECUTE_READWRITE;
        case R3_SEARCH_RW: return PAGE_READWRITE;
        case R3_SEARCH_READ: return PAGE_READONLY;
        case R3_SEARCH_EXECUTE_WRITE: return PAGE_EXECUTE_WRITECOPY;
        case R3_SEARCH_EXECUTE_READ: return PAGE_EXECUTE_READ;
        case R3_SEARCH_EXECUTE: return PAGE_EXECUTE;
        default: return -1;
        }
     }();
    std::vector<std::string> buffer_ = Text::text_split(buffer, " ");
    while (!buffer_.empty() && buffer_.front() == "??") buffer_.erase(buffer_.begin());
    while (!buffer_.empty() && buffer_.back() == "??") buffer_.erase(buffer_.end() - 1);
    const size_t patternSize = buffer_.size();
    std::vector<uint8_t> pattern;
    std::vector<uint8_t> mask;
    pattern.reserve(patternSize);
    mask.reserve(patternSize);
    Byteset findHeader;
    {
        int cout[2] = {0,0};
        for (const auto& byte : buffer_) {
            if (byte == "??") {
                pattern.emplace_back(0);
                mask.emplace_back(0);
                if (cout[1] == NULL) {
                    cout[1] = cout[0];
                }
            }
            else {
                pattern.emplace_back(static_cast<uint8_t>(Text::text_16_to_10(byte)));
                mask.emplace_back(0xFF);
            }
            cout[0]++;
        }
        if (cout[1] == NULL) cout[1] = cout[0];
        for (cout[0] = 0; cout[0] < cout[1]; cout[0]++) findHeader += (uint8_t)pattern[cout[0]];
    }
    if (ifThread) {
        DWORD threadCount = std::thread::hardware_concurrency();
        __int64 MaxAddr = (this->Is64() ? 0x00007FFFFFFFFFFF : 0xFFFFFFFF) / threadCount;
        Thread threadPool;
        for (DWORD i = 0; i < threadCount; i++) {
            const PVOID addrStart = (PVOID)(i * MaxAddr);
            const PVOID addrEnd = (PVOID)((i + 1) * MaxAddr);
            threadPool.add(i, [=, &addresses](PThread thread) {
                MEMORY_BASIC_INFORMATION threadMbi;
                size_t threadAddr = (size_t)addrStart;
                size_t threadEnd = (size_t)addrEnd;
                if (endAddress != nullptr && threadEnd > (size_t)endAddress) {
                    threadEnd = (size_t)endAddress;
                }
                while (threadAddr < threadEnd && (limit == 0 || addresses.size() < limit)) {
                    if (NTDLL::ZwQueryVirtualMemory(this->hProcess, (LPCVOID)threadAddr,
                        NTDLL::MemoryBasicInformation, &threadMbi, sizeof(threadMbi), nullptr) != 0)
                        break;
                    if (threadMbi.State != MEM_COMMIT) {
                        threadAddr += threadMbi.RegionSize;
                        continue;
                    }
                    if (protectStatus != static_cast<DWORD>(-1) && threadMbi.Protect != protectStatus) {
                        threadAddr += threadMbi.RegionSize;
                        continue;
                    }
                    Byteset regionData;
                    if (Read(threadMbi.BaseAddress, regionData, threadMbi.RegionSize, isVirtual)) {
                        const size_t headerSize = findHeader.size();
                        std::boyer_moore_searcher searcher(findHeader.begin(), findHeader.end());
                        auto begin = regionData.begin();
                        auto end = regionData.end();
                        while (begin < end) {
                            auto matchPos = std::search(begin, end, searcher);
                            if (matchPos == end) break;
                            size_t offset = std::distance(regionData.begin(), matchPos);
                            bool fullMatch = true;
                            for (size_t i = 0; i < patternSize; ++i) {
                                if (mask[i] && regionData[offset + i] != pattern[i]) {
                                    fullMatch = false;
                                    break;
                                }
                            }
                            if (fullMatch) {
                                addresses.emplace_back((BYTE*)threadMbi.BaseAddress + offset);
                                if (limit && addresses.size() >= limit) {
                                    return;
                                }
                            }
                            begin = matchPos + (fullMatch ? patternSize : headerSize);
                        }
                    }
                    threadAddr += threadMbi.RegionSize;
                }
            });
        }
        threadPool.wait();
    }
    else {
        while (NTDLL::ZwQueryVirtualMemory(this->hProcess, (LPCVOID)address, NTDLL::MemoryBasicInformation, &MemoryInfoMation, sizeof(MemoryInfoMation), nullptr) == 0) {
            if (MemoryInfoMation.State == MEM_COMMIT && (protectStatus == static_cast<DWORD>(-1) || MemoryInfoMation.Protect == protectStatus) && MemoryInfoMation.BaseAddress >= startAddress && (MemoryInfoMation.BaseAddress <= endAddress || endAddress == nullptr)) {
                Byteset data;
                if (Read(MemoryInfoMation.BaseAddress, data, MemoryInfoMation.RegionSize, isVirtual)) {
                    const size_t headerSize = findHeader.size();
                    const size_t patternLength = patternSize;
                    const auto* pPattern = pattern.data();
                    const auto* pMask = mask.data();
                    static std::boyer_moore_searcher headerSearcher(findHeader.begin(),findHeader.end());
                    auto begin = data.begin();
                    auto end = data.end();
                    while (begin < end) {
                        auto headerPos = std::search(
                            begin, end,
                            headerSearcher
                        );
                        if (headerPos == end) 
                            break;
                        const size_t offset = std::distance(data.begin(), headerPos);
                        if (offset > (data.size() - patternLength)) {
                            begin = headerPos + 1;
                            continue;
                        }
                        bool match = true;
                        size_t i = headerSize;
                        for (; i < patternLength && (i % 16 != 0); ++i) {
                            if (pMask[i] && data[offset + i] != pPattern[i]) {
                                match = false;
                                break;
                            }
                        }
                        if (match) {
                            const __m128i* dataVec = reinterpret_cast<const __m128i*>(&data[offset + i]);
                            const __m128i* patternVec = reinterpret_cast<const __m128i*>(&pPattern[i]);
                            const __m128i* maskVec = reinterpret_cast<const __m128i*>(&pMask[i]);
                            for (; i <= patternLength - 16; i += 16) {
                                __m128i dataChunk = _mm_loadu_si128(dataVec++);
                                __m128i patternChunk = _mm_loadu_si128(patternVec++);
                                __m128i maskChunk = _mm_loadu_si128(maskVec++);
                                __m128i cmp = _mm_cmpeq_epi8(dataChunk, patternChunk);
                                __m128i result = _mm_or_si128(cmp, maskChunk);
                                if (!_mm_test_all_ones(result)) {
                                    match = false;
                                    break;
                                }
                            }
                        }
                        if (match) {
                            for (; i < patternLength; ++i) {
                                if (pMask[i] && data[offset + i] != pPattern[i]) {
                                    match = false;
                                    break;
                                }
                            }
                        }
                        if (match) {
                            addresses.emplace_back((BYTE*)MemoryInfoMation.BaseAddress + offset);
                            if (limit && addresses.size() >= limit) break;
                        }
                        begin = headerPos + (match ? patternLength : headerSize);
                    }
                }
            }
            if (limit && addresses.size() >= limit) break;
            address += MemoryInfoMation.RegionSize;
        };
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
    int readSize  = this->Is64() ? sizeof(uint64_t) : sizeof(uint32_t);
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
