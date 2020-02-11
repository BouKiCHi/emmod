

#include "headers.h"
#include "emmod.h"

EmMod::EmMod() : Device(0) {
	mid = 0;
	eramBanks = 0;
	ram = NULL;
	eram = NULL;
	io = NULL;
	mm = NULL;
	n80sw = false;
	port31 = port71 = port99 = portE2 = portE3 = 0;
}

EmMod::~EmMod() {
	if (mm && mid != -1) {
		mm->Disconnect(mid);
	}
}

void EmMod::SetIoAccess(IIOAccess* access)
{
	io = access;
}

bool EmMod::SetMemoryManager(IMemoryManager* manager)
{
	this->mm = manager;
	mid = manager->Connect(this, true);
	return (mid != -1);
}


void EmMod::Reset() {
	port31 = 0;
	port71 = 0xff;
	port99 = 0;
	portE2 = 0;
	portE3 = 0;
	if (eram) { delete[] eram; eram = NULL; }
	if (ram) { delete[] ram; ram = NULL; }

	int eramSize = 0x20;

	AllocERAM(eramSize);
	AllocMainRAM();

	UpdateMemoryBus();
}

void EmMod::AllocMainRAM()
{
	ram = new uint8[0x8000];
	if (ram) {
		memset(ram, 0, 0x8000);
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


void IOCALL EmMod::ResetCpu(uint Address, uint Value)
{
	// N80の状態取得方法
	n80sw = false;
	//n80sw = Io->In(0x33) != 0xff;
	//OutputDebugInfo("ResetCPU N80:%s\n", n80sw ? "Yes" : "No");

	Reset();
}


void IOCALL EmMod::Out31(uint Address, uint Value) {
	port31 = Value & 6;
	UpdateMemoryBus();
}


// N88ROM
void IOCALL EmMod::Out71(uint Address, uint Value) {
	port71 = Value;
	UpdateMemoryBus();
}

// CD
void IOCALL EmMod::Out99(uint Address, uint Value) {
	port99 = Value;
	UpdateMemoryBus();
}

// ERAM
void IOCALL EmMod::OutE2(uint Address, uint Value) {
	portE2 = Value;
	UpdateMemoryBus();
}

// ERAM
void IOCALL EmMod::OutE3(uint Address, uint Value) {
	portE3 = Value;
	UpdateMemoryBus();
}

// メモリ構成のアップデート
void EmMod::UpdateMemoryBus()
{
	Update00Read();
	Update60Read();
	Update00Write();
}

void EmMod::Update00Read() {
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

		if (!(port31 & 2)) {
			// ROMなのでスルー
			mm->ReleaseR(mid, 0, 0x6000);
			return;
		}
	}

	mm->AllocR(mid, 0, 0x6000, read);
}


void EmMod::Update60Read() {
	uint8* read;
	if (portE2 & 0x01 && (portE3 < eramBanks))
	{
		// ERAM選択
		int bank = (portE3 % eramBanks);
		read = &eram[bank * 0x8000] + 0x6000;
	}
	else
	{
		// メインRAM
		read = ram + 0x6000;

		if ((port31 & 6) == 0)
		{
			// N88ROM
			// EROM or N88E
			mm->ReleaseR(mid, 0x6000, 0x2000);
			return;
		}
		else if ((port31 & 6) == 4)
		{
			// CD or N80
			mm->ReleaseR(mid, 0x6000, 0x2000);
			return;
		}
	}

	mm->AllocR(mid, 0x6000, 0x2000, read);
}

void EmMod::Update00Write() {
	uint8* write;

	if (portE2 & 0x01 && (portE3 < eramBanks))
	{
		int bank = (portE3 % eramBanks);
		write = &eram[bank * 0x8000];
	} else {
		write = ram;
	}

	mm->AllocW(mid, 0, 0x8000, write);
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
	STATIC_CAST(Device::OutFuncPtr, &EmMod::Out71),
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

