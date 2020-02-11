

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

	EmMod Sound;
	ConfigMP Config;
	ISystem* System;
	IIOBus* IoBus;
	IConfigPropBase* pb;
};

ExtModule::ExtModule()
{
	System = NULL;
	IoBus = NULL;
	pb = NULL;

	Config.Init(hinst);
}

bool ExtModule::Init(ISystem* _sys)
{
	// インターフェースの接続
	System = _sys;
	IoBus = (IIOBus*)System->QueryIF(M88IID_IOBus1);
	pb = (IConfigPropBase*)System->QueryIF(M88IID_ConfigPropBase);

	if (!IoBus || !pb) {
		return false;
	}

	// 0x108 = PC88::pres
	const static IIOBus::Connector ConnectList[] =
	{
		{ 0x108,IIOBus::portout,EmMod::RESET_CPU },
		{ 0, 0, 0 }
	};
	if (!IoBus->Connect(&Sound, ConnectList))
		return false;

	pb->Add(&Config);
	return true;
}

void ExtModule::Release()
{
	if (IoBus)
		IoBus->Disconnect(&Sound);
	if (pb)
		pb->Remove(&Config);

	Config.Release();

	delete this;
}

// ---------------------------------------------------------------------------

//	Module を作成
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
