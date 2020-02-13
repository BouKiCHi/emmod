
#include "headers.h"
#include "config.h"
#include "resource.h"

// ---------------------------------------------------------------------------

ConfigMP::ConfigMP()
{
	base = NULL;
	hinst = NULL;
	gate.SetDestination(PageGate, this);
}

ConfigMP::~ConfigMP()
{
}

bool ConfigMP::Init(HINSTANCE _hinst)
{
	hinst = _hinst;
	return true;
}


bool IFCALL ConfigMP::Setup(IConfigPropBase* _base, PROPSHEETPAGE* psp)
{
	base = _base;

	memset(psp, 0, sizeof(PROPSHEETPAGE));
	psp->dwSize = sizeof(PROPSHEETPAGE);
	psp->dwFlags = 0;
	psp->hInstance = hinst;
	psp->pszTemplate = MAKEINTRESOURCE(IDD_CONFIG);
	psp->pszIcon = 0;
	psp->pfnDlgProc = (DLGPROC)(void*)gate;
	psp->lParam = 0;
	return true;
}


BOOL ConfigMP::PageProc(HWND hdlg, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_NOTIFY:
		switch (((NMHDR*)lp)->code)
		{
		case PSN_SETACTIVE:
			base->PageSelected(this);
			break;

		case PSN_APPLY:
			base->Apply();
			return PSNRET_NOERROR;
		}
		return TRUE;
	}

	return FALSE;
}

BOOL CALLBACK ConfigMP::PageGate
(ConfigMP* config, HWND hwnd, UINT m, WPARAM w, LPARAM l)
{
	if (config) {
		return config->PageProc(hwnd, m, w, l);
	} else {
		return FALSE;
	}
}

// M88のデバッグ版でダイアログが動作しなかったので追加。意味はなかった。
INT_PTR ConfigMP::PageGate2(HWND hwnd, UINT m, WPARAM w, LPARAM l)
{
	ConfigMP* config = NULL;

	if (m == WM_INITDIALOG) {
		PROPSHEETPAGE* pPage = (PROPSHEETPAGE*)l;
		config = reinterpret_cast<ConfigMP*>(pPage->lParam);
		if (config) {
			::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)config);
		}
	}
	else {
		config = (ConfigMP*)::GetWindowLongPtr(hwnd, GWLP_USERDATA);
	}

	if (config) {
		return config->PageProc(hwnd, m, w, l);
	}
	else {
		return FALSE;
	}
}

