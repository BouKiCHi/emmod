

#ifndef incl_mem_h
#define incl_mem_h

#include "device.h"
#include "if/ifcommon.h"
#include "comm/CommMemory.h"

// ---------------------------------------------------------------------------

class EmMod : public Device
{
public:
	enum
	{
		RESET_CPU = 0,
		PORT31,
		PORT70,
		PORT71,
		PORT78,
		PORT99,
		PORTE2,
		PORTE3,
	};

public:
	EmMod();
	~EmMod();

	void Reset();
	void AllocMainRAM();
	void AllocERAM(int eramSize);

	void SetIoAccess(IIOAccess* access);
	bool SetMemoryManager(IMemoryManager* manager);


	bool OpenMemory();
	void StartThread();
	void CloseThread();

	static unsigned int __stdcall ReceiveThread(void* p_this);

	void ReceiveCommand();
	const char *GetCommandName(int Command);


	void IOCALL Out31(uint Address, uint Value);
	void IOCALL Out70(uint Address, uint Value);
	void IOCALL Out71(uint Address, uint Value);
	void IOCALL Out78(uint Address, uint Value);
	void IOCALL Out99(uint Address, uint Value);
	void IOCALL OutE2(uint Address, uint Value);
	void IOCALL OutE3(uint Address, uint Value);

	static uint MEMCALL MRead(void*, uint);
	static void MEMCALL MWrite(void*, uint, uint);


	uint ReadMemory(uint);
	void WriteMemory(uint a, uint d);

	uint ReadMemory80(uint a);
	void WriteMemory80(uint a, uint d);

	void OutputDebugInfo(const char* format, ...);


	// I/O port functions
	void IOCALL ResetCpu(uint Address, uint Value);

	
	const Descriptor* IFCALL GetDesc() const { return &descriptor; }


private:
	HANDLE threadHandle;
	bool runThread;

	uint8* p00r;
	uint8* p00w;
	uint8* p60r;


	bool n80sw;
	uint8 port31;
	uint8 port71;
	uint8 port99;
	uint8 portE2;
	uint8 portE3;

	uint txtwnd;


	int eramBanks;

	IIOAccess* io;

	CommMemory* cm;

	uint8* ram;
	uint8* eram;

	IMemoryManager* mm;
	int mid;

	static const Descriptor descriptor;
//	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
};

#endif // incl_mem_h
