#include "../../h/Memory/EmbeddedCall.h"

MemoryEmbeddedCall::EmbeddedCall::EmbeddedCall()
{
    this->PID = NULL;
    this->size = NULL;
    this->address = NULL;
}

MemoryEmbeddedCall::EmbeddedCall::EmbeddedCall(DWORD pid, size_t size)
{
    this->Init(pid, size);
}

MemoryEmbeddedCall::EmbeddedCall::~EmbeddedCall()
{
    this->Unit();
}

bool MemoryEmbeddedCall::EmbeddedCall::Init(DWORD pid, size_t size)
{
    this->PID = pid;
    this->Open(this->PID);
    return this->AllocateMemory(size);
}

void MemoryEmbeddedCall::EmbeddedCall::Unit()
{
    if (this->address != NULL) {
        this->Free(this->address, this->size);
    }
}

bool MemoryEmbeddedCall::EmbeddedCall::AllocateMemory(size_t size)
{
    if (this->address == NULL) {
        this->Free(this->address, this->size);
    }
    this->size = size;
    this->address = (PVOID)this->Allocate(this->size);
    return this->address != NULL;
}

bool MemoryEmbeddedCall::EmbeddedCall::SetCode(Byteset code, size_t index)
{
    if (this->address == NULL) {
        return false;
    }
    return this->Write(this->address, code, false);
}