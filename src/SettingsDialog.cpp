#include "stdafx.h"
#include "SettingsDialog.h"

#include "TetRegData.h"
#include "TetBoard.h"
#include "TetPlayers.h"
#include "TetGameInstance.h"

#include "resource.h"

GTetRegData::GSettings Settings;

BOOL CALLBACK SettingsDialogChildProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
void SettingsDialogTabSelChanged(HWND hwndDlg);

HWND hSettingsDialogWnd = NULL;

struct SettingsDialog_Header
{ 
	HWND hDisplayWnd;
	HWND hTabWnd;
	int nTemplates;
    int *apRes; 
}; 
 
BOOL CALLBACK SettingsDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hTabCtrl;
	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			Settings = tetgame.m_RegData.Settings;
			SettingsDialog_Header *sdh = new SettingsDialog_Header;
			sdh->hDisplayWnd = NULL;
			sdh->nTemplates = 1;
			sdh->apRes = new int[sdh->nTemplates];
			sdh->apRes[0] = IDD_SETTINGSDIALOG_T1;
			//sdh->apRes[1] = IDD_SETTINGSDIALOG_T2;
			SetWindowLong(hwndDlg, GWL_USERDATA, (LONG) sdh); 

			hSettingsDialogWnd = hwndDlg;
			ShowWindow(hwndDlg, SW_SHOW);
			UpdateWindow(hwndDlg);

			hTabCtrl = GetDlgItem(hwndDlg, IDC_TAB1);
			sdh->hTabWnd = hTabCtrl;

			TCITEM ti;
			memset(&ti, 0, sizeof(ti));
			ti.mask = TCIF_TEXT;
			ti.pszText = "General";

			TabCtrl_DeleteItem(hTabCtrl, 0);
			TabCtrl_InsertItem(hTabCtrl, 0, &ti);
			//TabCtrl_InsertItem(hTabCtrl, 1, &ti);

			SettingsDialogTabSelChanged(hwndDlg);

			return TRUE;
		}	
		case WM_NOTIFY:
		{
			NMHDR  *hdr = (NMHDR *)lParam;
			if (wParam == IDC_TAB1)
			{
				switch(hdr->code)
				{
					case TCN_SELCHANGE:
					{
						SettingsDialogTabSelChanged(hwndDlg);
					}
				}				
			}
			break;
		}	
		case WM_DESTROY:
		{
			SettingsDialog_Header *sdh = (SettingsDialog_Header *)GetWindowLong(hwndDlg, GWL_USERDATA); 
			DestroyWindow(sdh->hDisplayWnd);
			SetWindowLong(hwndDlg, GWL_USERDATA, 0);
			delete sdh->apRes;
			delete sdh;
			
			hSettingsDialogWnd = NULL;
			tetgame.m_RegData.Settings = Settings;
			tetgame.invalidate();
			break;
		}
		case WM_COMMAND: 
            switch (LOWORD(wParam)) 
            { 
                case IDOK: 
					tetgame.m_RegData.datasettings(true);
                    DestroyWindow(hwndDlg); 
                    return TRUE; 
 
                case IDCANCEL: 
                    DestroyWindow(hwndDlg); 
                    return TRUE; 
            } 
	}
	return FALSE;
}

void SettingsDialogTabSelChanged(HWND hwndDlg)
{
	SettingsDialog_Header *sdh = (SettingsDialog_Header *)GetWindowLong(hwndDlg, GWL_USERDATA); 
	int iSel = TabCtrl_GetCurSel(sdh->hTabWnd); 

	if (sdh->hDisplayWnd != NULL) 
		DestroyWindow(sdh->hDisplayWnd); 

	sdh->hDisplayWnd = CreateDialog(GetModuleHandle(NULL), 
		MAKEINTRESOURCE(sdh->apRes[iSel]), hwndDlg, SettingsDialogChildProc);
}

GString GetWindowTextString(HWND hWnd)
{
	char ch[1024];
	ch[0] = 0;
	GetWindowText(hWnd, ch, 1024);
	return ch;
}

BOOL CALLBACK SettingsDialogChildProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			HWND hParentWnd = GetParent(hwndDlg);
			SettingsDialog_Header *sdh = (SettingsDialog_Header *)GetWindowLong(hParentWnd, GWL_USERDATA); 
			RECT rect;
			GetWindowRect(sdh->hTabWnd, &rect);
			ScreenToClient(hParentWnd, (POINT *)&rect.left);
			ScreenToClient(hParentWnd, (POINT *)&rect.right);
			
			TabCtrl_AdjustRect(sdh->hTabWnd, FALSE, &rect);

			ShowWindow(hSettingsDialogWnd, SW_SHOW);
			UpdateWindow(hSettingsDialogWnd);

			SetWindowPos(hwndDlg, HWND_TOP, 
				rect.left, rect.top,
				rect.right-rect.left, rect.bottom-rect.top, 0); 

			CheckDlgButton(hwndDlg, IDC_CHECK1, Settings.Direct.bAutomaticallyStartsNewGame);
			CheckDlgButton(hwndDlg, IDC_CHECK2, Settings.Direct.b3D);
			SetWindowText(GetDlgItem(hwndDlg, IDC_EDIT1), Settings.strDefaultUsername);

			return TRUE;
		}
		case WM_DESTROY:
		{
			Settings.Direct.bAutomaticallyStartsNewGame = IsDlgButtonChecked(hwndDlg, IDC_CHECK1);
			Settings.Direct.b3D = IsDlgButtonChecked(hwndDlg, IDC_CHECK2);
			Settings.strDefaultUsername = GetWindowTextString(GetDlgItem(hwndDlg, IDC_EDIT1));
			break;
		}
		case WM_COMMAND: 
		{
            switch (LOWORD(wParam)) 
            { 
                case IDCANCEL: 
                    DestroyWindow(hwndDlg); 
                    return TRUE; 
            } 
			break;
		}
	}
	return FALSE;
}