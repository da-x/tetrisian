////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "TetRegData.h"
#include "TetBoard.h"
#include "TetPlayers.h"
#include "TetGameInstance.h"

////////////////////////////////////////////////////////////////////////////////////////////

GBoard::GBoard(int nX, int nY)
{
	initinstance();
	setsize(nX, nY);
}

GBoard::GBoard(const GBoard &board)
{
	initinstance();
	setsize(board.m_nX, board.m_nY);
	memcpy(m_pBoard, board.m_pBoard, m_nX*m_nY);
}

void GBoard::initinstance()
{
	m_pBoard = NULL;
	m_pDrawingObject = NULL;
	m_nX = 0;
	m_nY = 0;
}

void GBoard::makeempty(int nVal)
{
	if (m_pBoard)
	{
		if (nVal == 0)
			memset(m_pBoard, 0, m_nX*m_nY);
		else
			for (int i=0,j=m_nX*m_nY; i<j; i++)
				m_pBoard[i] = nVal;
	}
}

GBoardDrawing *GBoard::getdrawingobject() 
{
	return m_pDrawingObject;
}

GBoard::~GBoard()
{
	if (m_pDrawingObject)
		delete m_pDrawingObject;
}

void GBoard::setsize(int nX, int nY)
{	
	if (nX < 1)
		nX = 1;
	if (nY < 1)
		nY = 1;

	m_nX = nX;
	m_nY = nY;				 

	if (m_pBoard)
		free(m_pBoard);

	m_pBoard = (char *)malloc(m_nX*m_nY);	
	if (m_pBoard == NULL)
	{
		DWORD x = GetLastError();
		OutputDebugString("wert");
		m_pBoard = (char *)malloc(m_nX*m_nY);	
	}
}

void GBoard::initdrawingtools(int nWidth)
{
	if (m_pDrawingObject)
		delete m_pDrawingObject;
	m_pDrawingObject = new GBoardDrawing(this, nWidth);
}

int GBoard::checkcompletelines()
{
	int i=0,k=0;
	while (k=1+enumcompletelines(k)) i++;
	return i;
}

void GBoard::removecompletelines()
{
	int i,j,k;
	bool bComp;
	for (i=0; i < m_nY; i++)
	{
		bComp = true;
		for (j=0; j < m_nX; j++)
			if (m_pBoard[i*m_nX+j] == 0)
			{
				bComp = false;
				break;
			}
		if (bComp)
		{
			for (j=i; j > 0; j--)
				for (k=0; k < m_nX; k++)
					m_pBoard[j*m_nX+k] = m_pBoard[(j-1)*m_nX+k];
			for (k=0; k < m_nX; k++)
				m_pBoard[k] = 0;
		}
	}
}

int GBoard::enumcompletelines(int nNext)
{
	int i;
	bool bComp;
	for (i=nNext; i < m_nY; i++)
	{
		bComp = true;
		for (int j=0; j < m_nX; j++)
			if (m_pBoard[i*m_nX+j] == 0)
			{
				bComp = false;
				break;
			}
		if (bComp)
			return i;
	}
	return -1;
}		

void GBoard::set(int nX, int nY, int nVal)
{
	if (nX >= 0  &&  nX < m_nX  &&  nY >= 0  &&  nY < m_nY)
		m_pBoard[nY*m_nX+nX] = nVal;
}
	   

int GBoard::get(int nX, int nY)
{
	if (nX >= 0  &&  nX < m_nX  &&  nY >= 0  &&  nY < m_nY)
		return m_pBoard[nY*m_nX+nX];
	return 0;
}

GBoard& GBoard::operator=(const GBoard &board)
{
	if (board.m_nX == m_nX  &&  board.m_nY == m_nY)
		memcpy(m_pBoard, board.m_pBoard, m_nX*m_nY);
	return *this;
}

bool GBoard::operator ==(const GBoard &board)
{
	return (board.m_nX == m_nX  &&  board.m_nY == m_nY &&
	 (memcmp(m_pBoard, board.m_pBoard, m_nX*m_nY) == 0));		
}

////////////////////////////////////////////////////////////////////////////////////////////

GBoardDrawing::GBoardDrawing(GBoard *pBoard, int nWidth)
:m_LastDraw(pBoard->getx(), pBoard->gety())
{
	m_nWidth = 0;
	m_pBoard = pBoard;
	m_crbuffer = NULL;
	m_hPageBitmap = NULL;
	m_hPageDC = NULL;
	m_bDrawSkeleton = false;
	m_bInvalidateBoard = true;	
	m_b3D = getdraw3d();
	memset(m_hBlockBitmaps, 0, sizeof(m_hBlockBitmaps));
	HMODULE hGDI32 = GetModuleHandle("GDI32");
	setwidth(nWidth);
}

GBoardDrawing::~GBoardDrawing()
{
	for (int i=0; i < 9; i++)
		if (m_hBlockBitmaps[i])
			DeleteObject(m_hBlockBitmaps[i]);
	if (m_crbuffer)
		free(m_crbuffer);
	if (m_hPageDC)
		DeleteDC(m_hPageDC);
	if (m_hPageBitmap)
		DeleteObject(m_hPageBitmap);	
}

void GBoardDrawing::setposition(int nX, int nY)
{
	m_nX = nX;
	m_nY = nY;
}

void GBoardDrawing::setwidth(int nWidth)
{
	if (nWidth < 4)
		nWidth = 4;
	if (nWidth > 32)
		nWidth = 32;

	if (m_nWidth == nWidth)
		return;

	m_nWidth = nWidth;

	if (m_crbuffer)
		free(m_crbuffer);

	int ncrsize = nWidth*nWidth*3/2*3/2;
	m_crbuffer = (COLORREF *)malloc(ncrsize*sizeof(COLORREF));

	HWND hWnd = GetDesktopWindow();
	HDC hDC = GetDC(hWnd);
	HDC hBDC = CreateCompatibleDC(hDC);

	int i;
	for (i=0; i < 10; i++)
	{
		if (m_hBlockBitmaps[i])
			DeleteObject(m_hBlockBitmaps[i]);
		m_hBlockBitmaps[i] = CreateCompatibleBitmap(hDC, m_nWidth*3/2, m_nWidth*3/2);
		SelectObject(hBDC, m_hBlockBitmaps[i]);
		DrawTetrisianBlock(hBDC, i, 0, 0, m_nWidth, m_crbuffer, ncrsize); 
	}

	if (m_hPageBitmap)
		DeleteObject(m_hPageBitmap);	
		
	m_hPageBitmap = CreateCompatibleBitmap(hDC, getdrawwidth(), getdrawheight());

	if (m_hPageDC)
		DeleteDC(m_hPageDC);

	m_hPageDC = CreateCompatibleDC(hDC);
	SelectObject(m_hPageDC, m_hPageBitmap);		 

	DeleteDC(hBDC);
	ReleaseDC(hWnd, hDC);

	m_bInvalidateBoard = true;	
}

void GBoardDrawing::setdrawskeleton(bool bDrawSkeleton)
{
	if (m_bDrawSkeleton == bDrawSkeleton)
		return;

	m_bDrawSkeleton = bDrawSkeleton;
	m_bInvalidateBoard = true;	
}									

int GBoardDrawing::getdrawwidth() 
{
	return m_pBoard->getx()*m_nWidth + m_nWidth/2;
};

int GBoardDrawing::getdrawheight() 
{
	return m_pBoard->gety()*m_nWidth + m_nWidth/2;
};

bool GBoardDrawing::getdraw3d() 
{
	return tetgame.m_RegData.Settings.Direct.b3D;
}			 

void GBoardDrawing::DrawBoard(HDC hDC, HDC hBackDC, int nBX, int nBY)
{	
	HDC hBDC = CreateCompatibleDC(hDC);
	bool bDraw3d = getdraw3d();

	int i,j;
	int nsX = m_pBoard->getx();
	int nsY = m_pBoard->gety();
	int k = m_nWidth*3/2, m=m_nWidth*2, o=m_nWidth/2;

	SelectObject(hBDC, m_hBlockBitmaps[0]);
	if (m_bDrawSkeleton)
	{
		for (j=0; j < nsY; j++)
			for (i=0; i < nsX; i++)
				BitBlt(m_hPageDC, i*m_nWidth+o, j*m_nWidth, m, m, hBDC, 0, o, SRCCOPY);
	
		RECT rt = {0, 0, o, nsY*m_nWidth+o};
		FillBackground(m_hPageDC, &rt, hBackDC, m_nX-nBX, m_nY-nBY);
		RECT rt2 = {0, nsY*m_nWidth, nsX*m_nWidth+o, nsY*m_nWidth+o};
		FillBackground(m_hPageDC, &rt2, hBackDC, m_nX-nBX, nsY*m_nWidth+m_nY-nBY);

		if (bDraw3d)
		{
			POINT p[4] = {{o,0},{0,o},{m_nWidth,o},{k,0}}; 
			HRGN hRgn = CreatePolygonRgn(p, 4, WINDING);
			int nbltX, nbltY, pnbltX=0, pnbltY=0;

			for (j=0; j < nsX; j++)
			{
				nbltX = j*m_nWidth;
				nbltY = nsY*m_nWidth;

				OffsetRgn(hRgn, nbltX-pnbltX, nbltY-pnbltY);
				SelectClipRgn(m_hPageDC, hRgn);

				BitBlt(m_hPageDC, nbltX, nbltY, k, k,
				   hBDC, 0, 0, SRCCOPY);
				pnbltX = nbltX;
				pnbltY = nbltY;
			}

			SelectClipRgn(m_hPageDC, NULL);
			DeleteObject(hRgn);
		}
	}
	else
	{
		RECT rt = {0, 0, nsY*m_nWidth+m_nWidth/2, nsX*m_nWidth+m_nWidth/2};
		FillBackground(m_hPageDC, &rt, hBackDC, m_nX-nBX, m_nY-nBY);
	}

	if (bDraw3d)
	{
		POINT p[6] = {{o,1},{0,o},{0,k},{m_nWidth,k},{k,m_nWidth},{k, 1}}; 
		HRGN hRgn = CreatePolygonRgn(p, 6, WINDING);
		int nbltX, nbltY, pnbltX=0, pnbltY=0;

		for (j=nsY-1; j >= 0; j--)
			for (i=0; i < nsX; i++)
			{
				int nData = m_pBoard->get(i,j);
				if (nData != 0)
				{
					SelectObject(hBDC, m_hBlockBitmaps[nData]);
					nbltX = i*m_nWidth;
					nbltY = j*m_nWidth;

					OffsetRgn(hRgn, nbltX-pnbltX, nbltY-pnbltY);
					SelectClipRgn(m_hPageDC, hRgn);
					BitBlt(m_hPageDC, nbltX, nbltY, k, k,
					   hBDC, 0, 0, SRCCOPY);

					pnbltX = nbltX;
					pnbltY = nbltY;
				}
			}

		SelectClipRgn(m_hPageDC, NULL);
		DeleteObject(hRgn);
	}
	else
		for (j=nsY-1; j >= 0; j--)
			for (i=0; i < nsX; i++)
			{
				int nData = m_pBoard->get(i,j);
				if (nData != 0)
				{
					SelectObject(hBDC, m_hBlockBitmaps[nData]);
					BitBlt(m_hPageDC, i*m_nWidth+o+(bDraw3d ? -o : 0), j*m_nWidth, m_nWidth, m_nWidth, hBDC, 0, o, SRCCOPY);
				}
			}
	DeleteDC(hBDC);

}

void GBoardDrawing::PutText(const char *pText, int nSize)
{
	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));
	lf.lfHeight = nSize;
	strcpy(lf.lfFaceName, "Verdana");

	HFONT hFont = CreateFontIndirect(&lf);
	SelectObject(m_hPageDC, hFont);

	int nStrLen = strlen(pText);

	RECT rect = {0,0,0,0};
	DrawText(m_hPageDC, pText, nStrLen,
			 &rect, DT_CALCRECT);

	rect.right -= rect.left;
	rect.bottom -= rect.top;

	rect.right = (rect.left = (getdrawwidth() - rect.right)/2) + rect.right;
	rect.bottom = (rect.top = (getdrawheight() - rect.bottom)/2) + rect.bottom;

	InflateRect(&rect, 4, 4);

	HBRUSH hBrush = CreateSolidColorBrush(0x777777);

	FillRect(m_hPageDC, &rect, hBrush);
	DrawEdge(m_hPageDC, &rect, EDGE_RAISED, BF_RECT | BF_ADJUST);
	SetTextColor(m_hPageDC, 0x00FFFF);
	SetBkMode(m_hPageDC, TRANSPARENT);
	DrawText(m_hPageDC, pText, nStrLen,
			 &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	DeleteObject(hBrush);
	DeleteObject(hFont);
}

void GBoardDrawing::BltBoard(HDC hDC, HRGN hBackRegion)
{
	RECT rect = {m_nX, m_nY, m_nX+getdrawwidth(), m_nY+getdrawheight()};
	TakeOutRegion(hBackRegion, &rect);
	BitBlt(hDC, m_nX, m_nY, getdrawwidth(), getdrawheight(), m_hPageDC, 0, 0, SRCCOPY);
}

void GBoardDrawing::DrawLineBlink(HDC hDC)
{
	int j=0;		
	int o=(getdraw3d() != 0)*(m_nWidth/2);
	while ((j = m_pBoard->enumcompletelines(j)) != -1)
	{
		RECT rect = {m_nX+m_nWidth/2-o, m_nY + j*m_nWidth+o, 
	      m_nX + m_nWidth * m_pBoard->getx()+m_nWidth/2-o, m_nY + j*m_nWidth + m_nWidth+o};
		FillRect(hDC, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
		j++;
	}
}

void GBoardDrawing::DrawIfNessAndBlt(HDC hDC, HDC hBack, HRGN hBackRegion, 
                                             int nBX, int nBY)
{
	if (checkinvalidation())
		DrawBoard(hDC, hBack, nBX, nBY);
	BltBoard(hDC, hBackRegion);
}	 

bool GBoardDrawing::checkinvalidation()
{
	bool bInvalidateBoard = false;
	if (m_bInvalidateBoard)
		bInvalidateBoard = true;
	else
		if (!(*m_pBoard == m_LastDraw))
		{
			bInvalidateBoard = true;
			m_LastDraw = *m_pBoard;
		}
		else 
		{
			bool b3D = getdraw3d();
			if (b3D != m_b3D)
			{
				bInvalidateBoard = true;
				m_b3D = b3D;
			}	
		}					
	if (bInvalidateBoard)
	{
		m_bInvalidateBoard = false;
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////

GTetBoard::GTetBoard()
          :GBoard(10, 20)
{
}		   

void GTetBoard::put(int nX, int nY, GTetPiece &piece)
{
	if (!piece.valid())
		return;

	int i,j,k;
	for (i=0; i < 4; i++)
		for (j=0; j < 4; j++)
			if (nX + j >= 0  &&  nX + j < m_nX  &&
			    nY + i >= 0  &&  nY + i < m_nY)
				{
					k = piece.get(j,i);
					if (k != 0)
						m_pBoard[m_nX*(nY + i)+nX+j] = k;
				}
}		

bool GTetBoard::checkpos(int nX, int nY, GTetPiece &piece)
{
	if (!piece.valid())
		return false;

	bool bValid = true;
	int i,j;
	for (i=0; i < 4  &&  bValid; i++)
		for (j=0; j < 4  &&  bValid; j++)
			if (piece.get(j,i) != 0)
			{
				if (nX + j >= 0  &&  nX + j < m_nX  &&
					nY + i >= 0  &&  nY + i < m_nY)
				{
					if (m_pBoard[(nY + i)*m_nX + nX + j] != 0)
						bValid = false;
				}
				else
					bValid = false;
			}

	return bValid;
}

bool GTetBoard::checkposoverblocks(int nX, int nY, GTetPiece &piece)
{
	bool bValid = true;
	int i,j;
	for (i=0; i < 4  &&  bValid; i++)
		for (j=0; j < 4  &&  bValid; j++)
			if (nX + j >= 0  &&  nX + j < m_nX  &&
				nY + i >= 0  &&  nY + i < m_nY)
				if (piece.get(j,i) != 0  &&  m_pBoard[m_nX*(nY + i)+nX + j] != 0)
					bValid = false;
	return bValid;
}

GTetBoard& GTetBoard::operator=(const GTetBoard &board)
{
	memcpy(m_pBoard, board.m_pBoard, m_nX*m_nY);
	return *this;
}

bool GTetBoard::operator ==(const GTetBoard &board)
{
	return GBoard::operator ==(board);
}

////////////////////////////////////////////////////////////////////////////////////////////

GTetPiece::GTetPiece()	
          :GBoard(4, 4)
{
	m_nRot = 0;
	m_nForm = -1;
}

int GTetPiece::getrforms(int nForm)
{
	const char anrForms[7] = {1, 2, 4, 4, 2, 2, 4};
	if (nForm >= 0  &&  nForm < 7)
		return anrForms[nForm];
	return 0;
}

void GTetPiece::setform(int nForm, int nrForm)
{
	const char anrForms[7][4][4][4] = 
	  {{{{1,1,0,0}, {1,1,0,0}, {0,0,0,0}, {0,0,0,0}},
	    {{1,1,0,0}, {1,1,0,0}, {0,0,0,0}, {0,0,0,0}},
	    {{1,1,0,0}, {1,1,0,0}, {0,0,0,0}, {0,0,0,0}},
	    {{1,1,0,0}, {1,1,0,0}, {0,0,0,0}, {0,0,0,0}}},
	   {{{0,0,0,0}, {2,2,2,2}, {0,0,0,0}, {0,0,0,0}},
	    {{0,0,2,0}, {0,0,2,0}, {0,0,2,0}, {0,0,2,0}},
	    {{0,0,2,0}, {0,0,2,0}, {0,0,2,0}, {0,0,2,0}},
	    {{0,0,2,0}, {0,0,2,0}, {0,0,2,0}, {0,0,2,0}}},
	   {{{0,0,3,0}, {3,3,3,0}, {0,0,0,0}, {0,0,0,0}},
	    {{3,3,0,0}, {0,3,0,0}, {0,3,0,0}, {0,0,0,0}},
	    {{0,0,0,0}, {3,3,3,0}, {3,0,0,0}, {0,0,0,0}},
	    {{0,3,0,0}, {0,3,0,0}, {0,3,3,0}, {0,0,0,0}}},
	   {{{4,0,0,0}, {4,4,4,0}, {0,0,0,0}, {0,0,0,0}},
	    {{0,4,4,0}, {0,4,0,0}, {0,4,0,0}, {0,0,0,0}},
	    {{0,0,0,0}, {4,4,4,0}, {0,0,4,0}, {0,0,0,0}},
	    {{0,4,0,0}, {0,4,0,0}, {4,4,0,0}, {0,0,0,0}}},
	   {{{0,5,0,0}, {5,5,0,0}, {5,0,0,0}, {0,0,0,0}},
	    {{0,0,0,0}, {5,5,0,0}, {0,5,5,0}, {0,0,0,0}},
	    {{0,0,0,0}, {5,5,0,0}, {0,5,5,0}, {0,0,0,0}},
	    {{0,0,0,0}, {5,5,0,0}, {0,5,5,0}, {0,0,0,0}}},
	   {{{6,0,0,0}, {6,6,0,0}, {0,6,0,0}, {0,0,0,0}},
	    {{0,0,0,0}, {0,6,6,0}, {6,6,0,0}, {0,0,0,0}},
	    {{0,0,0,0}, {0,6,6,0}, {6,6,0,0}, {0,0,0,0}},
	    {{0,0,0,0}, {0,6,6,0}, {6,6,0,0}, {0,0,0,0}}},
	   {{{0,7,0,0}, {0,7,7,0}, {0,7,0,0}, {0,0,0,0}},
	    {{0,7,0,0}, {7,7,7,0}, {0,0,0,0}, {0,0,0,0}},
	    {{0,7,0,0}, {7,7,0,0}, {0,7,0,0}, {0,0,0,0}},
	    {{0,0,0,0}, {7,7,7,0}, {0,7,0,0}, {0,0,0,0}}}};
	if (nrForm < 0)
		nrForm = 0;
	if (nrForm >= 4)
		nrForm = 3;
	if (nForm < 0)
		nForm = 0;
	if (nForm >= 7)
		nForm = 6;
	m_nForm = nForm;
	m_nRot = nrForm;
	memcpy(m_pBoard, anrForms[nForm][nrForm], 16);
}

void GTetPiece::rotate()
{
	if (!(m_nForm >= 0  &&  m_nForm < 7))
		return;
	
	m_nRot++;
	if (m_nRot >= getrforms(m_nForm))
		m_nRot = 0;

	setform(m_nForm, m_nRot);
}

int GTetPiece::sizey()
{
	int nRet = 0,i,j;
	for (i=0; i < 4; i++)
		for (j=0; j < 4; j++)
			if (m_pBoard[i*m_nX+j] != 0)
			{
				nRet++;
				break;
			}
	return nRet;
}

int GTetPiece::startx()
{
	int nRet = 0,i,j;
	for (i=0; i < 4; i++)
		for (j=0; j < 4; j++)
			if (m_pBoard[j*m_nX+i] != 0)
			{
				return i;
				break;
			}
	return 0;
}

int GTetPiece::sizex()
{
	int nRet = 0,i,j;
	for (i=0; i < 4; i++)
		for (j=0; j < 4; j++)
			if (m_pBoard[j*m_nX+i] != 0)
			{
				nRet++;
				break;
			}
	return nRet;
}

void GTetPiece::random()
{
	int nChooseForm = randb(7);
	setform(nChooseForm, randb(getrforms(nChooseForm)));
}				

////////////////////////////////////////////////////////////////////////////////////////////

inline unsigned char crhrBrightnessLim(unsigned char cr, int fBright)
{
	int nRet = (int)(cr + fBright);
	if (nRet > 0xFF)
		nRet = 0xFF;
	if (nRet < 0)
		nRet = 0;
	return nRet;
}

inline COLORREF crBrightness(COLORREF cr, int fBright)
{
	return (((crhrBrightnessLim((unsigned char)(cr & 0xFF), fBright))) | 
	       ((crhrBrightnessLim((unsigned char)((cr >> 8) & 0xFF), fBright)) << 8)	| 
		   ((crhrBrightnessLim((unsigned char)((cr >> 16) & 0xFF), fBright)) << 16)) | 0xFF000000;
}

inline unsigned int crgBrightLim(unsigned char ch, unsigned int fBright)
{
	unsigned int x = (int)(((unsigned int)ch)*fBright)/0x100;
	if (x > 0x100)
		x = 0x100;
	return x;
}

inline COLORREF crBright(COLORREF cr, unsigned int fBright)
{
	return (((crgBrightLim((unsigned char)(cr & 0xFF), fBright))) | 
	       ((crgBrightLim((unsigned char)((cr >> 8) & 0xFF), fBright)) << 8)	| 
		   ((crgBrightLim((unsigned char)((cr >> 16) & 0xFF), fBright)) << 16)) | 0xFF000000;
}

void DrawTetrisianBlock(HDC hDc, int nColor, int nX, int nY, int nsX, COLORREF *cr, int nSize)
{
	if (nSize < nsX * nsX*3/2*3/2)
		return;
	
	memset(cr, 0, 4*nsX*nsX*3/2*3/2);

	const COLORREF crAr[10] = {0xFF444444, 0xFFFF3333, 0xFF3333FF, 0xFFFFAA00, 
	                          0xFFFFFF00, 0xFF00AAFF, 0xFFFF00FF, 0xFF00FF00, 
							  0xFFFFFFFF, 0xFF888888};
	BITMAPINFO bi;
	memset(&bi, 0, sizeof(bi));
	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
	bi.bmiHeader.biWidth = nsX*3/2;
	bi.bmiHeader.biHeight = -nsX*3/2;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = BI_RGB;

	double x,y,d = 4*nsX*nsX/5;
	COLORREF rf = crAr[nColor];
	int j;

	for (int i=0; i < nsX*nsX*3/2*3/2; i++)
		cr[i] = 0;

	for (i=0; i < nsX; i++)
		for (j=0; j < nsX; j++)
		{
			x = nsX/2-j;
			y = nsX/2-i;
			cr[(i+nsX/2)*bi.bmiHeader.biWidth+j] = crBright(rf, (unsigned int)(0x100-0x100*(x*x+y*y)/d));
		}

	for (i=nsX/2-1; i >= 1; i--)
		for (j=0; j < nsX; j++)
			cr[bi.bmiHeader.biWidth*(nsX/2-i)+j+i] = crBrightness(cr[bi.bmiHeader.biWidth*(nsX/2+3)+j], 0x30);	

	int nDark = (nColor == 0) ? -0x0 : -0x60;
	for (i=nsX/2-1; i >= 0; i--)
		for (j=0; j < nsX; j++)
			cr[bi.bmiHeader.biWidth*(nsX/2+j-i)+nsX+i] = crBrightness(cr[bi.bmiHeader.biWidth*(nsX/2+j)+nsX-3], nDark);	

	for (i=0; i < nsX; i++)
	{
		cr[bi.bmiHeader.biWidth*nsX/2+i] = crBrightness(cr[bi.bmiHeader.biWidth*nsX/2+i], 0x20);
		cr[bi.bmiHeader.biWidth*(nsX/2+nsX-1)+i] = crBrightness(cr[bi.bmiHeader.biWidth*(nsX/2+nsX-1)+i], -0x20);
	}

	for (i=0; i < nsX; i++)
	{
		cr[bi.bmiHeader.biWidth*(i+nsX/2)] = crBrightness(cr[bi.bmiHeader.biWidth*(i+nsX/2)], 0x20);
		cr[bi.bmiHeader.biWidth*(i+nsX/2)+nsX-1] = crBrightness(cr[bi.bmiHeader.biWidth*(i+nsX/2)+nsX-1], -0x20);
	}

	SetDIBitsToDevice(hDc, nX, nY, nsX*3/2, nsX*3/2, 0, 0, 0, nsX*3/2, cr, &bi, DIB_RGB_COLORS);
}

static unsigned __int64 nsRand = time(NULL);

int randb(int nR)
{
  return (int)(((nsRand = ((0xFBC45F7131F564 + 1) * nsRand + 1)) / 10000000) % nR);
}

HBRUSH CreateSolidColorBrush(COLORREF cr)
{
	LOGBRUSH lb;
	lb.lbColor = cr;
	lb.lbHatch = 0;
	lb.lbStyle = BS_SOLID;
	return CreateBrushIndirect(&lb);	
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
			            8, 8, 350, 30, hwndDlg, NULL, GetModuleHandle(NULL), NULL);
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
	return DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ENTERNAME), hWnd, OtherDialogProc, (DWORD)&strs);
}
