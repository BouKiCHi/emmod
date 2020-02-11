

#ifndef incl_mem_h
#define incl_mem_h

#include "device.h"
#include "if/ifcommon.h"
#include "loginfo.h"

// ---------------------------------------------------------------------------

class EmMod : public Device
{
public:
	enum
	{
		RESET_CPU = 0,
		PORT31,
		PORT71,
		PORT99,
		PORTE2,
		PORTE3,
	};

public:
	EmMod();
	~EmMod();
	
	void SetIoAccess(IIOAccess* access);
	bool SetMemoryManager(IMemoryManager* manager);
	void Reset();
	void AllocMainRAM();
	void AllocERAM(int eramSize);
	void IOCALL Out31(uint Address, uint Value);
	void IOCALL Out71(uint Address, uint Value);
	void IOCALL Out99(uint Address, uint Value);
	void IOCALL OutE2(uint Address, uint Value);
	void IOCALL OutE3(uint Address, uint Value);
	void UpdateMemoryBus();

	void Update00Read();
	void Update60Read();
	void Update00Write();

	void OutputDebugInfo(const char* format, ...);


	// I/O port functions
	void IOCALL ResetCpu(uint Address, uint Value);

	
	const Descriptor* IFCALL GetDesc() const { return &descriptor; }


private:
	bool n80sw;
	unsigned char port31;
	unsigned char port71;
	unsigned char port99;
	unsigned char portE2;
	unsigned char portE3;

	int eramBanks;

	IIOAccess* io;

	uint8* ram;
	uint8* eram;

	IMemoryManager* mm;
	int mid;

	static const Descriptor descriptor;
//	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
};

#endif // incl_mem_h
