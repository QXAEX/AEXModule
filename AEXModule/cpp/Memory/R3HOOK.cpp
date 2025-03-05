#include "../../h/Memory/R3HOOK.h"
MemoryR3HOOK::R3HOOK::R3HOOK()
{
    this->isInstalled = false;
    this->address = NULL;
    this->newAddress = NULL;
    this->isVirtual = true;
    this->PID = NULL;
    this->is64_ = false;
    this->isCapture = false;
}

MemoryR3HOOK::R3HOOK::R3HOOK(DWORD pid, PVOID address, Byteset complement)
{
    this->Install(this->PID, address, complement);
}

MemoryR3HOOK::R3HOOK::~R3HOOK()
{
    this->Uninstall();
}

void MemoryR3HOOK::R3HOOK::setVirtual(bool isVirtual)
{
    this->isVirtual = isVirtual;
}

void MemoryR3HOOK::R3HOOK::setCapture(bool flag)
{
    this->isCapture = flag;
}

bool MemoryR3HOOK::R3HOOK::Install(DWORD pid, PVOID address, Byteset complement)
{
    if (this->isInstalled)return false;
    this->isInstalled = false;
    this->PID = pid;
    this->address = address;
    this->complement = complement;
    this->Open(pid);
    this->is64_ = this->Is64();
    this->newAddress = (PVOID)this->Allocate(2048);
    Byteset buffer;
    long long newAddress = (size_t)this->newAddress + 4;
    if (this->is64_) {
        buffer.append({ 0x83, 0x3D, 0xCD, 0xFE, 0xFF, 0xFF, 0x0 });//cmp [0x0], 0
        buffer.append({ 0x0F, 0x8E, 0xCF, 0x01, 0x0, 0x0 });//jle
        buffer.append({ 0x48, 0xA3 }).append(newAddress); // rax
        buffer.append({ 0xA3 }).append(newAddress += 8); // eax
        buffer.append({ 0x66, 0xA3 }).append(newAddress += 4); // ax
        buffer.append({ 0xA2 }).append(newAddress += 2); // al
        newAddress = 0xFFFFFEAC;
        for (int i = 1, ii = 0; i < 16; i++) {
            buffer.append({ (UCHAR)(i >= 8 ? 0x4C : 0x48), 0x89,  (UCHAR)((i >= 8 ? 0x05 + ((i - 8) * 8) : 0x0D + ((i - 1) * 8))) }).append(static_cast<int>(newAddress + 1 - ii));
            buffer.append(i >= 8 ? Byteset({ 0x44 }) : nullptr).append({ 0x89,  (UCHAR)((i >= 8 ? 0x05 + ((i - 8) * 8) : 0x0D + ((i - 1) * 8))) }).append(static_cast<int>(newAddress + (i >= 8 ? 2 - ii : 3)));
            buffer.append({ 0x66, (UCHAR)(i >= 8 ? 0x44 : 0x89) }).append(i >= 8 ? Byteset({ 0x89 }) : nullptr).append({ (UCHAR)((i >= 8 ? 0x05 + ((i - 8) * 8) : 0x0D + ((i - 1) * 8))) }).append(static_cast<int>(newAddress - (i >= 8 ? 2 + ii : NULL)));
            buffer.append(i >= 3 ? Byteset({ (UCHAR)(i >= 8 ? 0x44 : 0x40) }) : nullptr).append({ 0x88,  (UCHAR)((i >= 8 ? 0x05 + ((i - 8) * 8) : 0x0D + ((i - 1) * 8))) }).append(static_cast<int>(newAddress - (i >= 8 ? 7 + ii : 5)));
            newAddress -= (i >= 3 && i <= 7 ? 0xC : 0xB);
            if (i >= 8) ii += 3;
        }
        if (this->isCapture) {
            buffer.append({ 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
            this->Write(this->newAddress, { 0x1 }, this->isVirtual);
        }
        else {
            buffer.append({ 0xFF, 0x0D, 0xF8, 0xFC, 0xFF, 0xFF }); // dec [0x0]
        }
    }
    else {
        buffer.append({ 0x83, 0x3D }).append((int)(this->newAddress)).append({ 0x0 });//cmp [0x0], 0
        buffer.append({ 0x0F, 0x8E, 0x83, 0x0, 0x0, 0x0 });//jng 0x0
        buffer.append({ 0xA3 }).append(static_cast<int>(newAddress)); // eax
        buffer.append({ 0x66, 0xA3 }).append(static_cast<int>(newAddress += 4)); // ax
        buffer.append({ 0xA2 }).append(static_cast<int>(newAddress += 2)); // al
        for (int i = 1; i < 8; i++) {
            buffer.append({ 0x89 }).append({ (UCHAR)(0x0D + ((i - 1) * 8)) }).append(static_cast<int>(newAddress += (i <= 4 ? 1 : 2)));
            buffer.append({ 0x66, 0x89 }).append({ (UCHAR)(0x0D + ((i - 1) * 8)) }).append(static_cast<int>(newAddress += 4));
            if (i <= 3) buffer.append({ 0x88 }).append({ (UCHAR)(0x0D + ((i - 1) * 8)) }).append(static_cast<int>(newAddress += 2));
        }
        if (this->isCapture) {
            buffer.append({ 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
            this->Write(this->newAddress, { 0x1 }, this->isVirtual);
        }
        else {
            buffer.append({ 0xFF, 0x0D }).append((int)(this->newAddress)); // dec [0x0]
        }
    }
    this->Read(address, temp, (this->is64_ ? 14 : 5) + complement.size(), this->isVirtual);
    buffer.append(temp);
    this->Write((PVOID)((long long)this->newAddress + (this->is64_ ? 300 : 100)), buffer, this->isVirtual);
    this->hookJump((PVOID)((long long)this->newAddress + (this->is64_ ? 300 : 100) + buffer.size()), (PVOID)((long long)address + (this->is64_ ? 14 : 5) + complement.size()), nullptr, this->isVirtual);
    this->hookJump(address, (PVOID)((long long)this->newAddress + (this->is64_ ? 300 : 100)), complement, this->isVirtual);
    this->isInstalled = true;
    return true;
}

void MemoryR3HOOK::R3HOOK::Capture()
{
    if (!this->isInstalled) return;
    this->Write(this->newAddress, { 0x1 }, this->isVirtual);
}

MemoryR3HOOK::R3HOOK_INFO MemoryR3HOOK::R3HOOK::ReadData() const
{
    if (!this->isInstalled) return R3HOOK_INFO();
    Byteset data;
    long long index = NULL;
    this->Read((PVOID)((size_t)this->newAddress + 4), data, sizeof(R3HOOK_INFO), this->isVirtual);
    PR3HOOK_INFO pInfo = (PR3HOOK_INFO)data.data();
    R3HOOK_INFO info = *pInfo;
    return info;
}

void MemoryR3HOOK::R3HOOK::Uninstall()
{
    if (this->isInstalled) {
        this->Write(this->address, temp, this->isVirtual);
        this->Free(this->newAddress, 2048);
        this->isInstalled = false;
    }
}
