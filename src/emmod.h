

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
		OPN_OUT,
	};

public:
	EmMod();
	~EmMod();

	// I/O port functions
	void IOCALL ResetCpu(uint Address, uint Value);
	
	const Descriptor* IFCALL GetDesc() const { return &descriptor; }


private:

	static const Descriptor descriptor;
//	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
};

#endif // incl_mem_h
