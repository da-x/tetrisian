////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "TetRegData.h"
#include "TetBoard.h"
#include "TetPlayers.h"
#include "TetGameInstance.h"
#include "TetPlayerWindow.h"

////////////////////////////////////////////////////////////////////////////////////////////

GTetPlayerWindow::GTetPlayerWindow()
{
	m_pPlayer = NULL;
	m_hBackgroundBitmap = NULL;
	SetEvents(this);
}

bool GTetPlayerWindow::Create(GTetPlayer *pPlayer)
{
	m_pPlayer = pPlayer;

	GWindow *pWindow = pPlayer->getparentwindow();	
	GRect rect;

	return GWindow::Create("Temp", 0,  WS_OVERLAPPEDWINDOW | WS_CHILD & ~WS_SIZEBOX, 
	       rect, pWindow);
}

bool GTetPlayerWindow::OnCreate(GWindow *pWindow)
{
	setdock(m_pPlayer->getdockdefault());

	GPoint pntSize = m_pPlayer->calcwindowsize();
	RECT rectX = {0, 0, pntSize.GetX(), pntSize.GetY()};

	AdjustWindowRectEx(&rectX, apiGetLong(GWL_STYLE), FALSE, apiGetLong(GWL_EXSTYLE));
	SetWindowPos(*this, NULL, 0, 0, rectX.right-rectX.left, rectX.bottom-rectX.top, SWP_NOMOVE);

	apiShowWindow();
	apiUpdateWindow();
	return true;
}

void GTetPlayerWindow::OnDestroy(GWindow *pWindow)
{		
}

LRESULT GTetPlayerWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_ERASEBKGND:
		{
			return 0;
		}
		case WM_PAINT:
		{
			RECT rt;
			GetClientRect(hWnd, &rt);	

			PAINTSTRUCT ps;
			HDC hDC = BeginPaint(hWnd, &ps);

			HRGN hBackgroundRegion = CreateRectRgnIndirect(&rt);

			HDC hBackgroundDC = m_hBackgroundBitmap ? CreateCompatibleDC(hDC) : NULL;
			if (hBackgroundDC)
				SelectObject(hBackgroundDC, m_hBackgroundBitmap);

			if (m_pPlayer)
				m_pPlayer->DrawBoard(hDC, hBackgroundRegion, hBackgroundDC, 2, 2);

			SelectClipRgn(hDC, hBackgroundRegion);
			InflateRect(&rt, -2, -2);	  
			FillBackground(hDC, &rt, hBackgroundDC, 0, 0);
			InflateRect(&rt, 2, 2);	 
			DrawEdge(hDC, &rt, EDGE_SUNKEN, BF_RECT);

			DeleteObject(hBackgroundDC);
			DeleteObject(hBackgroundRegion);

			EndPaint(hWnd, &ps);
			break;
		}
	}
	return GWindow::WndProc(hWnd, uMsg, wParam, lParam);
}

void GTetPlayerWindow::setdock(bool bDock) 
{
	int nStyle = apiGetLong(GWL_STYLE);
	int nExStyle = apiGetLong(GWL_EXSTYLE);
	if (bDock)
	{
		nStyle = WS_CHILD;
		nExStyle = 0;
		apiSetParent(*m_pPlayer->getparentwindow());
	}
	else
	{
		nStyle = WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX;
		nExStyle = WS_EX_TOOLWINDOW;
		apiSetParent(NULL);
	}

	apiSetLong(GWL_STYLE, nStyle);
	apiSetLong(GWL_EXSTYLE, nExStyle);

	m_bDockWindow = bDock;
}

////////////////////////////////////////////////////////////////////////////////////////////
