#include "stdafx.h"
#include "HallOfFameDialog.h"
#include "resource.h"

#include "TetRegData.h"
#include "TetBoard.h"
#include "TetPlayers.h"
#include "TetGameInstance.h"

HWND hHallOfFameDialogWnd = NULL;

BOOL CALLBACK HallOfFameDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			hHallOfFameDialogWnd = hwndDlg;
			tetgame.m_RegData.datahalloffame(false);

			HWND hCtrlWnd = GetDlgItem(hwndDlg, IDC_LIST1);
			LVCOLUMN lc;
			int i=0;
			lc.mask = LVCF_WIDTH | LVCF_TEXT; 
			lc.cx = 40;
			lc.pszText = "Place";
			ListView_InsertColumn(hCtrlWnd, i++, &lc);
			lc.cx = 130;
			lc.pszText = "Name";
			ListView_InsertColumn(hCtrlWnd, i++, &lc);
			lc.cx = 60;
			lc.pszText = "Score";
			ListView_InsertColumn(hCtrlWnd, i++, &lc);
			lc.cx = 40;
			lc.pszText = "Lines";
			ListView_InsertColumn(hCtrlWnd, i++, &lc);
			lc.cx = 80;
			lc.pszText = "Time Elapsed";
			ListView_InsertColumn(hCtrlWnd, i++, &lc);

			char ch[128];
			LVITEM li;
			memset(&li, 0, sizeof(li));
			li.mask = LVIF_TEXT;
			li.pszText = ch;
			GTetRegData::GHallOfFame (&HallOfFame)[10] = tetgame.m_RegData.HallOfFame;
			for (i=0; i < 10; i++)
			{
				li.iItem = i;
				li.iSubItem = 0;
				wsprintf(ch, "%d", i+1);
				li.iItem = ListView_InsertItem(hCtrlWnd, &li);
				
				if (HallOfFame[i].bUseEntry)
				{
					wsprintf(ch, "%s", (char *)HallOfFame[i].strName);
					ListView_SetItemText(hCtrlWnd, i, 1, ch);
					wsprintf(ch, "%d", HallOfFame[i].nScore);
					ListView_SetItemText(hCtrlWnd, i, 2, ch);
					wsprintf(ch, "%d", HallOfFame[i].nLines);
					ListView_SetItemText(hCtrlWnd, i, 3, ch);
					int nGameTime = HallOfFame[i].nElapsed;
					wsprintf(ch, "%02d:%02d:%02d", (int)nGameTime/3600, 
					  ((int)nGameTime % 3600)/60, (int)nGameTime % 60);
					ListView_SetItemText(hCtrlWnd, i, 4, ch);
				}
			}

			ShowWindow(hwndDlg, SW_SHOW);
			UpdateWindow(hwndDlg);
			return TRUE;
		}	
		case WM_DESTROY:
		{
			hHallOfFameDialogWnd = NULL;
			return TRUE;
		}
		case WM_COMMAND: 
		{
            switch (LOWORD(wParam)) 
            { 
				case IDCANCEL:
                case IDOK: 
                    DestroyWindow(hwndDlg); 
                    return TRUE; 
            } 
			break;
		}
		case WM_KEYDOWN:
		{
			if (wParam == VK_ESCAPE)
				DestroyWindow(hwndDlg); 			
			break;
		}
	}
	return FALSE;
}
