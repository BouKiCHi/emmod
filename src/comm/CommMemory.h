#pragma once

#include <windows.h>

enum MemoryCommand {
	NoOperation,
	ReadMainRam,
	ReadExtRam,
	WriteMainRam,
	WriteExtRam,
};


class CommMemory
{
private:
	DWORD MemorySize = 16 + 65536; // 16 + 64KB
	HANDLE Handle = NULL;
	BYTE *Buffer = NULL;

public:

	BYTE PacketId = 0x00;
	BYTE Command = 0x00;
	BYTE Bank = 0x00;

	WORD Address = 0x00;
	WORD Length = 0x00;


	bool Open(const char* PipeName);
	void Close();
	BYTE *GetSharedMemory();

	bool RecvCheck();
	void RecvDone();

	void SendData(const BYTE *data, int length);
	void RecvData(BYTE* data, int length);

	void Recv();

	void SetMemoryAddress(BYTE Bank, WORD Address, WORD Length);
	void SendCommand(BYTE Command);
};

