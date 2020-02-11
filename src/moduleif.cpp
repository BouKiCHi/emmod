

#include "headers.h"
#include "if/ifcommon.h"
#include "if/ifguid.h"
#include "emmod.h"
#include "config.h"

#define EXTDEVAPI	__declspec(dllexport)

HINSTANCE hinst;

// ---------------------------------------------------------------------------

class ExtModule : public IModule
{
public:
	ExtModule();
	~ExtModule() {}

	bool Init(ISystem* system);
	void IFCALL Release();

	void* IFCALL QueryIF(REFIID) { return 0; }

private:

	EmMod Mod;
	ConfigMP Config;
	ISystem* System;
	IIOBus* IoBus;
	IIOAccess* IoAccess;
	IMemoryManager* mm;
	IConfigPropBase* pb;
};

ExtModule::ExtModule()
{
	System = NULL;
	IoBus = NULL;
	IoAccess = NULL;
	pb = NULL;

	Config.Init(hinst);
}

bool ExtModule::Init(ISystem* _sys)
{
	// �C���^�[�t�F�[�X�̐ڑ�
	System = _sys;
	IoBus = (IIOBus*)System->QueryIF(M88IID_IOBus1);
	IoAccess = (IIOAccess*)System->QueryIF(M88IID_IOAccess1);
	pb = (IConfigPropBase*)System->QueryIF(M88IID_ConfigPropBase);
	mm = (IMemoryManager*)System->QueryIF(M88IID_MemoryManager1);

	if (!IoBus || !IoAccess || !pb) {
		return false;
	}

	Mod.SetIoAccess(IoAccess);
	if (!Mod.SetMemoryManager(mm)) return false;

	// 0x108 = PC88::pres
	const static IIOBus::Connector ConnectList[] =
	{
		{ 0x108,IIOBus::portout,EmMod::RESET_CPU },
		{ 0x31,IIOBus::portout,EmMod::PORT31 },
		{ 0x71,IIOBus::portout,EmMod::PORT71 },
		{ 0x99,IIOBus::portout,EmMod::PORT99 },
		{ 0xE2,IIOBus::portout,EmMod::PORTE2 },
		{ 0xE3,IIOBus::portout,EmMod::PORTE3 },
		{ 0, 0, 0 } // �I�[
	};
	if (!IoBus->Connect(&Mod, ConnectList))
		return false;

	pb->Add(&Config);
	return true;
}

void ExtModule::Release()
{
	if (IoBus)
		IoBus->Disconnect(&Mod);
	if (pb)
		pb->Remove(&Config);

	Config.Release();

	delete this;
}

// ---------------------------------------------------------------------------

//	Module ���쐬
extern "C" EXTDEVAPI IModule * __cdecl M88CreateModule(ISystem * system)
{
	ExtModule* module = new ExtModule;

	if (module)
	{
		if (module->Init(system))
			return module;
		delete module;
	}
	return 0;
}

BOOL APIENTRY DllMain(HANDLE hmod, DWORD rfc, LPVOID)
{
	switch (rfc)
	{
	case DLL_PROCESS_ATTACH:
		hinst = (HINSTANCE)hmod;
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;

	case DLL_PROCESS_DETACH:
		break;
	}
	return true;
}
