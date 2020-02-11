

#include "headers.h"
#include "emmod.h"

// コンストラクタ
EmMod::EmMod() : Device(0)
{
}

EmMod::~EmMod()
{
}

// ---------------------------------------------------------------------------
//	I/O ポートを監視

void IFCALL EmMod::ResetCpu(uint Address, uint Value)
{
	OutputDebugString("ResetCPU\n");
}

// ---------------------------------------------------------------------------
//	device description
//
const Device::Descriptor EmMod::descriptor = { /*indef*/ 0, outdef };

const Device::OutFuncPtr EmMod::outdef[] =
{
	STATIC_CAST(Device::OutFuncPtr, &EmMod::ResetCpu),
};

/*
const Device::InFuncPtr SoundLog::indef[] =
{
	0,
};
*/

