////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "buildnmb.h"
#include "Tetrisian.h"
#include "TetRegData.h"
#include "TetBoard.h"
#include "TetPlayers.h"
#include "TetGameInstance.h"
#include "TetMainWindow.h"
#include "HallOfFameDialog.h"
#include "SettingsDialog.h"

////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

////////////////////////////////////////////////////////////////////////////////////////////

void GTetMsgLoop::Idle()
{
	//tetgame.process();	
	GMsgLoop::Idle();
}

////////////////////////////////////////////////////////////////////////////////////////////

GTetMainWindow::GTetMainWindow()
{
	SetEvents(this);
}

bool GTetMainWindow::OnCreate(GWindow *pWindow)
{
	tetgame.setwindow(this);
	SetTimer(*this, 0, 0, NULL);

	m_pDrawBuf = new COLORREF[PDRAWBUF_SIZE];

	//hBackgroundBitmap = NULL;//LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
	SendMessage(*this, WM_INITMENU, 0, 0);

	m_mainMenu.Create(MAKEINTRESOURCE(IDC_TETRISIAN));
	m_mainMenu.SetToWindow(this);

	GPoint pnt = apiAdjustSize(tetgame.m_playLocal.calcwindowsize(), true);
	SetWindowPos(*this, NULL, 0, 0, pnt.GetX(), pnt.GetY(), SWP_NOMOVE);

	apiShowWindow();
	apiUpdateWindow();
	return true;
}

void GTetMainWindow::OnDestroy(GWindow *pWindow)
{
}

LRESULT GTetMainWindow::OnCommand(GWindow *pWindow, HWND hwndFrom, UINT idFrom, UINT code)
{
	return 0;
}

LRESULT GTetMainWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_TIMER:
		{
			tetgame.process();
			break;
		}
		case WM_KEYDOWN:
		{
			tetgame.keydown(wParam);
			break;
		}
		case WM_KEYUP:
		{
			tetgame.keyup(wParam);
			break;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
		case WM_COMMAND:
		{
			int wmId, wmEvent;
			HINSTANCE hInst = GetModuleHandle(NULL);
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 

			switch (wmId)
			{
				case IDM_ABOUT:
					//DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
					break;
				case IDM_EXIT:
					DestroyWindow(hWnd);
					break;

				case ID_GAME_START:
				{					
					tetgame.user_startgame();					
					break;
				}
				case ID_GAME_END:
				{
					tetgame.user_endgame();
					break;
				}
				case ID_GAME_LSP:
				{
					tetgame.setgamemode(0);
					break;
				}
				case ID_GAME_NGC:
				{
					tetgame.setgamemode(1);
					break;
				}
				case ID_GAME_NGS:
				{
					tetgame.setgamemode(2);
					break;
				}
				case ID_PAUSE:
				{
					tetgame.m_playLocal.user_pause();
					break;
				}
				case IDM_GAME_SETTINGS:
				{
					if (hSettingsDialogWnd)
						SetFocus(hSettingsDialogWnd);
					else
						CreateDialog(hInst, MAKEINTRESOURCE(IDD_SETTINGSDIALOG), hWnd, SettingsDialogProc);
						
					break;			
				}				
				case IDM_GAME_HALLOFFAME:
				{
					if (hHallOfFameDialogWnd)
						SetFocus(hHallOfFameDialogWnd);
					else
						CreateDialog(hInst, MAKEINTRESOURCE(IDD_HALLOFFAME), hWnd, HallOfFameDialogProc);
					
					break;
				}
			}
			break;
		}
		case WM_INITMENU:
		{
			HMENU hMenu = GetMenu(hWnd);
			bool bGame = tetgame.isgameplaying();
			int nGame = 0;

			EnableMenuItem(hMenu, ID_GAME_END, MF_BYCOMMAND | (bGame ? MF_ENABLED : MF_GRAYED));
			EnableMenuItem(hMenu, ID_PAUSE, MF_BYCOMMAND | (bGame ? MF_ENABLED : MF_GRAYED));
			EnableMenuItem(hMenu, ID_GAME_START, MF_BYCOMMAND | (bGame ? MF_GRAYED : MF_ENABLED));

			switch (tetgame.getgamemode())
			{
				case 0: nGame = ID_GAME_LSP; break;
				case 1: nGame = ID_GAME_NGC; break;
				case 2: nGame = ID_GAME_NGS; break;
			}			
			
			CheckMenuRadioItem(hMenu, ID_GAME_LSP, ID_GAME_NGS, nGame, MF_BYCOMMAND);

			DrawMenuBar(hWnd);
			
			break;
		}
	}
	return GWindow::WndProc(hWnd, uMsg, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////////////////

const char *pszMonths[12][2] = 
{{"Jan", "January"},
 {"Feb", "February"},
 {"Mar", "March"},
 {"Apr", "April"},
 {"May", "May"},
 {"Jun", "June"},
 {"Jul", "July"},
 {"Aug", "August"},
 {"Sep", "September"},
 {"Oct", "October"},
 {"Nov", "November"},
 {"Dec", "December"}};


LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:			
			SendMessage(GetDlgItem(hDlg, IDOK), WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

			return TRUE;		
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hDlg, &ps);

			SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
			SetBkMode(hdc, TRANSPARENT);

			char ch[1000], chMonth[10] = "Dec";
			int nYear=0, nDay=0;
			nDay = 18;
			nYear = 1999;
			//sscanf(__DATE__, "%s %d %d", chMonth, &nDay, &nYear);
			for (int i=0; i < 12; i++)
				if (strcmp(chMonth, pszMonths[i][0]) == 0)
				{
					strcpy(chMonth, pszMonths[i][1]);
					break;
				}

			wsprintf(ch, "%s %s/%d\n"
			            "Dan Aloni, Callistasoft (c) 1999\n\n"
						"Compiled on the %d%s of %s %d at "__TIME__" CET.\n",
			             TETRISIAN_NAME, TETRISIAN_VERSION, TETRISIAN_BUILDNUMBER,
						 nDay, AddOrdinal(nDay), chMonth, nYear);

			RECT rect = {60, 15, 0 ,0};
			DrawText(hdc, ch, strlen(ch), &rect, DT_NOCLIP);

			EndPaint(hDlg, &ps);
			break;
		}
		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}
