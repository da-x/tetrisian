////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __TETBOARD_H__
#define __TETBOARD_H__

////////////////////////////////////////////////////////////////////////////////////////////

class GBoard : public GObj
{
public:	
	GBoard(int nX, int nY);
	GBoard(const GBoard &board);
	~GBoard();

	void initinstance();
	void makeempty(int nVal=0);
	void setsize(int nX, int nY);
	void initdrawingtools(int nWidth);
	int checkcompletelines();
	void removecompletelines();
	int enumcompletelines(int nNext);
	GBoard& operator =(const GBoard &board);
	bool operator ==(const GBoard &board);
	int getx() {return m_nX;};
	int gety() {return m_nY;};
	void set(int nX, int nY, int nVal);
	int get(int nX, int nY);

protected:
	int m_nX, m_nY;
	char *m_pBoard;

	class GBoardDrawing *m_pDrawingObject;

public:
	class GBoardDrawing *getdrawingobject();	
};

///////////////////////////////////////////////////////////////////////////////////////////

class GBoardDrawing : public GObj
{
public:
	GBoardDrawing(GBoard *pBoard, int nWidth);
	~GBoardDrawing();

	void setposition(int nX, int nY);
	GPoint getpos() {return GPoint(m_nX, m_nY);};
	void setwidth(int nWidth);
	void setdrawskeleton(bool m_bDrawSkeleton);
	int getdrawwidth();
	int getdrawheight();
	int getwidth() {return m_nWidth;};
	bool getdraw3d();

	void DrawBoard(HDC hDC, HDC hBack, int nBX, int nBY);
	void PutText(const char *pText, int nSize=17);
	void BltBoard(HDC hDC, HRGN hBackRegion);
	void DrawLineBlink(HDC hDC);
	void DrawIfNessAndBlt(HDC hDC, HDC hBack, HRGN hBackRegion, int nBX, int nBY);

private:
	GBoard *m_pBoard;
	GBoard m_LastDraw;

	int m_nWidth, m_nX, m_nY;
	HBITMAP m_hPageBitmap;
	HDC m_hPageDC;
	bool m_bDrawSkeleton;
	bool m_bInvalidateBoard;
	bool m_b3D;
	
	COLORREF *m_crbuffer;
	HBITMAP m_hBlockBitmaps[10]; 

	bool checkinvalidation();
};

///////////////////////////////////////////////////////////////////////////////////////////

class GTetPiece;

class GTetBoard : public GBoard
{
public:
	GTetBoard();
	void put(int nX, int nY, GTetPiece &piece);
	bool checkpos(int nX, int nY, GTetPiece &piece);
	bool checkposoverblocks(int nX, int nY, GTetPiece &piece);
	GTetBoard &operator =(const GTetBoard &board);
	bool operator ==(const GTetBoard &board);
	bool operator !=(const GTetBoard &board)
		{return !operator==(board);};

//private:
	//friend GTetGame;
};

////////////////////////////////////////////////////////////////////////////////////////////

class GTetPiece : public GBoard
{
public:	
	GTetPiece();
	int getrforms(int nForm);
	void setform(int nForm, int nrForm=0);
	void rotate();
	int sizex();
	int startx();
	int sizey();
	void random();
	int getform(){return m_nForm;};
	int getrot(){return m_nRot;};
	int valid() {return m_nForm >= 0  &&  m_nForm < 7;};

private:
	int m_nRot, m_nForm;	
};

////////////////////////////////////////////////////////////////////////////////////////////

void DrawTetrisianBlock(HDC hDc, int nColor, int nX, int nY, int nsX, COLORREF *cr, int nSize);
HBRUSH CreateSolidColorBrush(COLORREF cr);
void TakeOutRegion(const HRGN hBackground, const RECT *rect);
void FillBackground(HDC hDC, CONST RECT *rect, HDC hBackDC, int nX, int nY);
void AddTextBetweenRegions(HDC hDC, HRGN hBackgroundRegion, const char *pStr,
                           int nX, int nY, UINT uFormat, HDC hBackground);
int randb(int nR);
const char *AddOrdinal(int n);
int ShowOtherDialog(HWND hWnd, char *title, char *text);

////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////
