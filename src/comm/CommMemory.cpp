#include "headers.h"
#include "CommMemory.h"

bool CommMemory::Open(const char* PipeName)
{
    Handle = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, MemorySize, PipeName);
    if (Handle == NULL) return false;

    Buffer = (BYTE *)MapViewOfFile(Handle, FILE_MAP_ALL_ACCESS, 0, 0, 0);

    if (Buffer == NULL) {
        Close();
        return false;
    }

    return true;
}

void CommMemory::Close()
{
    if (Buffer != NULL) {
        UnmapViewOfFile(Buffer);
        Buffer = NULL;
    }
    if (Handle != NULL) {
        CloseHandle(Handle);
        Handle = NULL;
    }
}

BYTE *CommMemory::GetSharedMemory()
{
    return Buffer;
}

bool CommMemory::RecvCheck()
{
    return (Buffer[0] != PacketId);
}

void CommMemory::RecvDone()
{
    PacketId = Buffer[0];
    Buffer[1] = PacketId;
}

void CommMemory::SendData(const BYTE* data, int length)
{
    memcpy(Buffer + 0x10, data, length);
}

void CommMemory::RecvData(BYTE* data, int length)
{
    memcpy(data, Buffer + 0x10, length);
}


void CommMemory::Recv()
{
    Command = Buffer[2];
    Bank = Buffer[3];
    Address = Buffer[4] | (Buffer[5]<<8);
    Length = Buffer[6] | (Buffer[7] << 8);
}

void CommMemory::SetMemoryAddress(BYTE Bank, WORD Address, WORD Length)
{
    this->Bank = Bank;
    this->Address = Address;
    this->Length = Length;
}

void CommMemory::SendCommand(BYTE Command)
{
    Buffer[2] = Command;
    Buffer[3] = Bank;
    Buffer[4] = Address & 0xff;
    Buffer[5] = (Address>>8) & 0xff;
    Buffer[6] = Length & 0xff;
    Buffer[7] = (Length >> 8) & 0xff;

    PacketId++;
    Buffer[0] = PacketId;
}
