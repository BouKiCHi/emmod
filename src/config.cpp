
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

// Ý’è‚Ì‰Šú‰»‚ðs‚¤
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
		break;
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
		break;
	case WM_COMMAND:
		return TRUE;
		break;

		/*case WM_COMMAND:
			if (HIWORD(wp) == BN_CLICKED) {
					base->PageChanged(hdlg);
					return TRUE;
			}
		break; */
	}

	return FALSE;
}

BOOL CALLBACK ConfigMP::PageGate
(ConfigMP* config, HWND hwnd, UINT m, WPARAM w, LPARAM l)
{
	return config->PageProc(hwnd, m, w, l);
}

void ConfigMP::Release(void) {
}
