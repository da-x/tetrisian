#include "stdafx.h"

#include "resource.h"
#include "buildnmb.h"
#include "tetris.h"
#include "tetrisian.h"
#include "SettingsDialog.h"
#include "HallOfFameDialog.h"
#include "TetMainWindow.h"

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	WSADATA wsad;
	WSAStartup(0x202, &wsad);
	InitCommonControls();

	GTetMainWindow tetMainWindow;
	tetMainWindow.Create("Tetrisian", 0, 
		WS_OVERLAPPEDWINDOW & ~(WS_SIZEBOX | WS_MAXIMIZEBOX) | 
		WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 
	              GRect(CW_USEDEFAULT, 0, 0, 0), NULL);

	GTetMsgLoop msgloop;
	return msgloop.Run();
}


void __cdecl WinMainCRTStartup( void )
{
    int mainret;
    char *lpszCommandLine;
    STARTUPINFO StartupInfo;

    lpszCommandLine = GetCommandLine();

    // Skip past program name (first token in command line).

    if ( *lpszCommandLine == '"' )  //Check for and handle quoted program name
    {
        // Scan, and skip over, subsequent characters until  another
        // double-quote or a null is encountered

        while( *lpszCommandLine && (*lpszCommandLine != '"') )
            lpszCommandLine++;

        // If we stopped on a double-quote (usual case), skip over it.

        if ( *lpszCommandLine == '"' )
            lpszCommandLine++;
    }
    else    // First token wasn't a quote
    {
        while ( *lpszCommandLine > ' ' )
            lpszCommandLine++;
    }

    // Skip past any white space preceeding the second token.

    while ( *lpszCommandLine && (*lpszCommandLine <= ' ') )
        lpszCommandLine++;

    StartupInfo.dwFlags = 0;
    GetStartupInfo( &StartupInfo );

    mainret = WinMain( GetModuleHandle(NULL),
                       NULL,
                       lpszCommandLine,
                       StartupInfo.dwFlags & STARTF_USESHOWWINDOW
                            ? StartupInfo.wShowWindow : SW_SHOWDEFAULT );

    ExitProcess(mainret);
}

/*
#define MAX_LOADSTRING 100

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

HINSTANCE hInst = NULL;
LPCTSTR szTitle = "Tetrisian";							
LPCTSTR szWindowClass = "Tetrisian";							

#define PDRAWBUF_SIZE	1024
COLORREF *pDrawBuf;
DTetGame tetgame;
HBITMAP hPageBitmap = NULL;
HBITMAP hBackgroundBitmap = NULL; 


ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	WSADATA wsad;
	WSAStartup(0x202, &wsad);
	InitCommonControls();

	MSG msg;
	HACCEL hAccelTable;

	MyRegisterClass(hInstance);

	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_TETRISIAN);

	while (1) 
	{
		if (PeekMessage(&msg, NULL, 0, 0, 0))
		{
			if (!GetMessage(&msg, NULL, 0, 0))
				break;
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		tetgame.process();
		Sleep(1);
	}

	return msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_TETRISIAN);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_TETRISIAN;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW & ~(WS_SIZEBOX | WS_MAXIMIZEBOX),
      CW_USEDEFAULT, 0, 287, 405, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void TakeOutRegion(const HRGN hBackground, const RECT *rect)
{
	HRGN hRegion = CreateRectRgnIndirect(rect);
	HRGN hTempRegion = CreateRectRgn(0,0,0,0);
	CombineRgn(hTempRegion, hBackground, hRegion, RGN_DIFF);
	CombineRgn(hBackground, hTempRegion, NULL, RGN_COPY);
	DeleteObject(hTempRegion);
	DeleteObject(hRegion);
}

void FillBackground(HDC hDC, CONST RECT *rect, HDC hBackDC, int nX, int nY)
{
	if (hBackDC)
		BitBlt(hDC, rect->left, rect->top, rect->right-rect->left, rect->bottom-rect->top, 
		   hBackDC, nX, nY, SRCCOPY);
	else
		FillRect(hDC, rect, (HBRUSH)GetStockObject(GRAY_BRUSH));
}	

void AddTextBetweenRegions(HDC hDC, HRGN hBackgroundRegion, const char *pStr,
                           int nX, int nY, UINT uFormat, HDC hBackground)
{
	RECT rt = {nX, nY, nX, nY};

	SelectObject(hDC, GetStockObject(DEFAULT_GUI_FONT));
	DrawText(hDC, pStr, strlen(pStr), &rt, DT_CALCRECT);
	TakeOutRegion(hBackgroundRegion, &rt);

	int nY_ = rt.top;
	int nX_ = rt.left;
	rt.bottom -= rt.top;
	rt.right -= rt.left;
	rt.left = 0;
	rt.top = 0;

	HBITMAP hBitmap = CreateCompatibleBitmap(hDC, rt.right, rt.bottom);
	HDC hBitDC = CreateCompatibleDC(hDC);
	SelectObject(hBitDC, hBitmap);
	SetBkMode(hBitDC, TRANSPARENT);

	SelectObject(hBitDC, GetStockObject(DEFAULT_GUI_FONT));
	
	FillBackground(hBitDC, &rt, hBackground, nX_-2, nY_-2);
	
	SetTextColor(hBitDC, 0xFFFFFF);
	DrawText(hBitDC, pStr, strlen(pStr), &rt, uFormat);
	BitBlt(hDC, nX_, nY_, rt.right, rt.bottom, hBitDC, 0, 0, SRCCOPY);

	DeleteObject(hBitDC);
	DeleteObject(hBitmap);
}

typedef	struct {char *title, *text;} twostrs;

BOOL CALLBACK OtherDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static twostrs *ch = NULL; 
	switch (uMsg)
	{
		case WM_INITDIALOG:
		{						
			ch = (twostrs *)lParam;
			HWND hWnd = CreateWindow("STATIC", ch->title, WS_CHILD | SS_LEFT, 
			            8, 8, 350, 30, hwndDlg, NULL, hInst, NULL);
			SendMessage(hWnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), 0);

			SetWindowText(hWnd, ch->title);
			SetWindowText(GetDlgItem(hwndDlg, IDC_EDIT1), ch->text);

			ShowWindow(hWnd, SW_SHOW);
			UpdateWindow(hWnd);
			ShowWindow(hwndDlg, SW_SHOW);
			UpdateWindow(hwndDlg);
			return TRUE;
		}	
		case WM_COMMAND: 
            switch (LOWORD(wParam)) 
            { 
                case IDOK: 
					GetWindowText(GetDlgItem(hwndDlg, IDC_EDIT1), ch->text, 128);
                    EndDialog(hwndDlg, IDOK);
                    return TRUE; 
            } 
	}
	return FALSE;
}

int ShowOtherDialog(HWND hWnd, char *title, char *text)
{
	twostrs strs;
	strs.text = text;
	strs.title = title;
	return DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_ENTERNAME), hWnd, OtherDialogProc, (DWORD)&strs);
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);

	switch (message) 
	{
		case WM_CREATE:
		{
			tetgame.setwindow(hWnd);
			SetTimer(hWnd, 0, 0, NULL);
			pDrawBuf = new COLORREF[PDRAWBUF_SIZE];
			hBackgroundBitmap = NULL;//LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
			SendMessage(hWnd, WM_INITMENU, 0, 0);
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		case WM_TIMER:
		{
			tetgame.process();
			break;
		}
		case WM_COMMAND:
		{
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 

			switch (wmId)
			{
				case IDM_ABOUT:
					DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
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
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
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
		case WM_ERASEBKGND:
		{
			break;
		}
		case WM_PAINT:
		{
			RECT rt;
			GetClientRect(hWnd, &rt);	
			
			HRGN hBackgroundRegion = CreateRectRgnIndirect(&rt);

			hdc = BeginPaint(hWnd, &ps);

			HDC hBackgroundDC = hBackgroundBitmap ? CreateCompatibleDC(hdc) : NULL;
			if (hBackgroundDC)
				SelectObject(hBackgroundDC, hBackgroundBitmap);

			tetgame.DrawBoard(hdc, hBackgroundRegion, hBackgroundDC, 2, 2);

			SelectClipRgn(hdc, hBackgroundRegion);
			InflateRect(&rt, -2, -2);	  
			FillBackground(hdc, &rt, hBackgroundDC, 0, 0);
			InflateRect(&rt, 2, 2);	 
			DrawEdge(hdc, &rt, EDGE_SUNKEN, BF_RECT);

			DeleteObject(hBackgroundDC);
			DeleteObject(hBackgroundRegion);

			EndPaint(hWnd, &ps);
			break;
		}
		case WM_KEYDOWN:
		{
			tetgame.keydown(wParam);
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		case WM_KEYUP:
		{
			tetgame.keyup(wParam);
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		case WM_DESTROY:
			delete []pDrawBuf;
			PostQuitMessage(0);
			DeleteObject(hBackgroundBitmap);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

const char *AddOrdinal(int n)
{
	switch (n % 10)
	{
		case 1:	return "st";
		case 2:	return "nd";
		case 3:	return "rd";
		default: return "th";
	}
}

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
*/