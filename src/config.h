
#ifndef incl_config_h
#define incl_config_h

#include "if/ifcommon.h"

class ConfigMP : public IConfigPropSheet
{
public:
	ConfigMP();
	~ConfigMP();
	bool Init(HINSTANCE _hinst);
	bool IFCALL Setup(IConfigPropBase*, PROPSHEETPAGE* psp);
	
private:
	BOOL PageProc(HWND, UINT, WPARAM, LPARAM);
	static BOOL CALLBACK PageGate(HWND, UINT, WPARAM, LPARAM);

	static INT_PTR CALLBACK PageGate2(HWND, UINT, WPARAM, LPARAM);


	HINSTANCE hinst;
	IConfigPropBase* base;
	static ConfigMP* selfptr;
};

#endif // incl_config_h
