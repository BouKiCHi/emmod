

#include "headers.h"
#include "emmod.h"
#include "comm/CommMemory.h"

EmMod::EmMod() : Device(0) {
	mid = 0;
	eramBanks = 0;
	ram = NULL;
	eram = NULL;
	io = NULL;
	mm = NULL;
	n80sw = false;
	port31 = port71 = port99 = portE2 = portE3 = 0;
	txtwnd = 0x0000;
	threadHandle = NULL;
	cm = NULL;
	p00r = NULL;
	p60r = NULL;
	p00w = NULL;
	runThread = false;
}


EmMod::~EmMod() {
	if (mm && mid != -1) {
		mm->Disconnect(mid);
	}

	if (cm) delete cm;
}

void EmMod::Reset() {
	port31 = 0;
	port71 = 0xff;
	port99 = 0;
	portE2 = 0;
	portE3 = 0;
	if (eram) { delete[] eram; eram = NULL; }
	if (ram) { delete[] ram; ram = NULL; }

	int eramSize = 0x04;

	AllocERAM(eramSize);
	AllocMainRAM();

	mm->AllocR(mid, 0, 0x8400, MRead);
	mm->AllocW(mid, 0, 0x8400, MWrite);
}

void EmMod::AllocMainRAM()
{
	ram = new uint8[0x10000];
	if (ram) {
		memset(ram, 0, 0x10000);
	}
}

void EmMod::AllocERAM(int eramSize)
{
	eram = new uint8[0x8000 * eramSize];
	if (eram) {
		eramBanks = eramSize;
		memset(eram, 0, 0x8000 * eramBanks);
	}
}

void EmMod::SetIoAccess(IIOAccess* access)
{
	io = access;
}

bool EmMod::SetMemoryManager(IMemoryManager* manager)
{
	mm = manager;
	mid = mm->Connect(this, true);
	return (mid != -1);
}

bool EmMod::OpenMemory()
{
	cm = new CommMemory;
	return cm->Open("m88mem");
}

void EmMod::StartThread()
{
	threadHandle = (HANDLE)_beginthreadex(0, 0, &EmMod::ReceiveThread, this, 0, 0);
}

void EmMod::CloseThread()
{
	runThread = false;
	WaitForSingleObject(threadHandle, INFINITE);
}

unsigned int __stdcall EmMod::ReceiveThread(void* pThis)
{
	EmMod* o = static_cast<EmMod*>(pThis);
	o->ReceiveCommand();
	return 0;
}

void  EmMod::ReceiveCommand() {
	runThread = true;

	while(runThread) {
		if (cm->RecvCheck()) {
			cm->Recv();

			const char *CommandName = GetCommandName(cm->Command);

			OutputDebugInfo("Command:%s Bank:%02x Address:%04x Length:%04x\n",
				CommandName, cm->Bank, cm->Address, cm->Length);

			int Address = cm->Address;
			int Length = cm->Length;
			int Bank = cm->Bank;

			if (0x8000 < Address) Address = 0x8000;
			if (0x8000 < Address + Length) Length = 0x8000 - Address;
			if (eramBanks - 1 < Bank) Bank = Bank % eramBanks;

			switch (cm->Command) {
			case MemoryCommand::ReadMainRam:
				cm->SendData(ram + Address, Length);
				break;
			case MemoryCommand::ReadExtRam:
				cm->SendData(&eram[Bank * 0x8000] + Address, Length);
				break;
			case MemoryCommand::WriteMainRam:
				cm->RecvData(ram + Address, Length);
				break;
			case MemoryCommand::WriteExtRam:
				cm->RecvData(&eram[Bank * 0x8000] + Address, Length);
				break;
			}

			cm->RecvDone();
		}
		Sleep(10);
	}
}

const char *EmMod::GetCommandName(int Command)
{
	switch (Command)
	{
	case MemoryCommand::NoOperation:
		return "NoOperation";
	case MemoryCommand::ReadMainRam:
		return "Read MainRam";
	case MemoryCommand::ReadExtRam:
		return "Read ExtRam";

	case MemoryCommand::WriteMainRam:
		return "Write MainRam";
	case MemoryCommand::WriteExtRam:
		return "Write ExtRam";
	default:
		return "Unknown";
	}
}

void IOCALL EmMod::ResetCpu(uint Address, uint Value)
{
	n80sw = false;
	Reset();
}


void IOCALL EmMod::Out31(uint Address, uint Value) {
	if ((Value ^ port31) & 6) {
		port31 = Value & 6;
	}
}

void IOCALL EmMod::Out70(uint Address, uint Value) {
	if (n80sw) return;
	txtwnd = Value * 0x100;
}

// N88ROM
void IOCALL EmMod::Out71(uint Address, uint Value) {
	port71 = Value;
}

// txtwnd += 0x100
void IOCALL EmMod::Out78(uint Address, uint Value) {
	txtwnd = (txtwnd + 0x100) & 0xff00;
}

// CD
void IOCALL EmMod::Out99(uint Address, uint Value) {
	//port99 = Value;
	//UpdateMemoryBus();
}

// ERAM
void IOCALL EmMod::OutE2(uint Address, uint Value) {
	portE2 = Value;
}

// ERAM
void IOCALL EmMod::OutE3(uint Address, uint Value) {
	portE3 = Value;
}

uint MEMCALL EmMod::MRead(void* p, uint a)
{
	EmMod* mp = reinterpret_cast<EmMod*>(p);
	return (a < 0x8000) ? mp->ReadMemory(a) : mp->ReadMemory80(a);
}

void MEMCALL EmMod::MWrite(void* p, uint a, uint d)
{
	EmMod* mp = reinterpret_cast<EmMod*>(p);
	if (a < 0x8000) mp->WriteMemory(a, d); else mp->WriteMemory80(a, d);
}

uint EmMod::ReadMemory(uint a) {
	uint8* read;

	if (portE2 & 0x01 && (portE3 < eramBanks))
	{
		// ERAM選択
		int bank = (portE3 % eramBanks);
		read = &eram[bank * 0x8000];
	}
	else
	{
		// メインRAM
		read = ram;

		// ROMなのでスルー
		if (!(port31 & 2)) {
			read = NULL;
		}
	}

	if (read == NULL) {
		return mm->Read8P(mid, a);
	}
	return read[a & 0x7fff];
}

void EmMod::WriteMemory(uint a, uint d) {
	uint8* write;

	if (portE2 & 0x10 && (portE3 < eramBanks))
	{
		// ERAM選択
		int bank = (portE3 % eramBanks);
		write = &eram[bank * 0x8000];
	} else {
		// メインRAM
		write = ram;
	}

	if (write != NULL) write[a & 0x7fff] = d;

	mm->Write8P(mid, a, d);
}

uint EmMod::ReadMemory80(uint a) {
	return ram[((a & 0x3ff) + txtwnd) & 0xffff];
}

void EmMod::WriteMemory80(uint a, uint d) {
	ram[((a & 0x3ff) + txtwnd) & 0xffff] = d;
	mm->Write8P(mid, a, d);
}

void EmMod::OutputDebugInfo(const char* format, ...) {
	va_list args;
	int len;
	char* buffer;

	va_start(args, format);

	len = _vscprintf(format, args) + 1;

	buffer = (char*)malloc(len * sizeof(char));
	if (buffer) {
		vsprintf(buffer, format, args);
		OutputDebugString(buffer);
		free(buffer);
	}
	va_end(args);
}

// ---------------------------------------------------------------------------
//	device description
//
const Device::Descriptor EmMod::descriptor = { /*indef*/ 0, outdef };

const Device::OutFuncPtr EmMod::outdef[] =
{
	STATIC_CAST(Device::OutFuncPtr, &EmMod::ResetCpu),
	STATIC_CAST(Device::OutFuncPtr, &EmMod::Out31),
	STATIC_CAST(Device::OutFuncPtr, &EmMod::Out70),
	STATIC_CAST(Device::OutFuncPtr, &EmMod::Out71),
	STATIC_CAST(Device::OutFuncPtr, &EmMod::Out78),
	STATIC_CAST(Device::OutFuncPtr, &EmMod::Out99),
	STATIC_CAST(Device::OutFuncPtr, &EmMod::OutE2),
	STATIC_CAST(Device::OutFuncPtr, &EmMod::OutE3),
};

/*
const Device::InFuncPtr EmMod::indef[] =
{
	0,
};
*/

