#ifndef __TETRIS_H__
#define __TETRIS_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////


/*
class DTetGame : public GObj
{
public:
	DTetRegData m_RegData;

	DTetGame();
	~DTetGame();
	DrawBoard(HDC hDc, HRGN hBackRegion, HDC hBackDC, int nBX, int nBY);
	void setwindow(HWND hWnd) {m_hWnd = hWnd;};
	void invalidate() {InvalidateRect(m_hWnd, NULL, TRUE);};
	void process();
	void keydown(DWORD nKey);
	void keyup(DWORD nKey);
	void user_left();
	void user_right();
	void laypiece();
	void newgame();
	bool pausetoggle();
	void set_no_game_state();
	void halloffamecheck();

	int getstate() const {return m_nState;};
	const char *getcurtaindropmsg() const {return m_pCurtainDropMsg;};
	int getscore() const {return m_nScore;};
	const char *gettime() const;

	void SetCurtainState(char *pMsg, double iCurtainDropSpeed = 0.04, 
	                     int nCurtainColor=8, int nTextSize=25);

private:
	DTetBoard m_Board;
	DTetPiece m_NextPiece;

	DTetBoard m_BoardSave;
	DTetPiece m_Piece;

	int m_nScore, m_nLines;

	int m_nState, m_nCurtain, m_nPrevState;
	int m_nCurtainColor, m_nCurtainTextSize;
	char *m_pCurtainDropMsg;

	int m_nPieceX, m_nPieceY;

	HWND m_hWnd;

	LARGE_INTEGER m_iTime; 

	double m_accTime, m_iCurtainDropSpeed,
	       m_iDropSpeed;

	bool m_bBlinkMode;
	double m_iBlinkDurationLeft;
	double m_iBlinkSpeed;

	double m_iGameTime;
	
	DWORD m_nKey;		  						   
	bool m_bDownKeyPressed;
	double m_iDelayKey;
	double m_accKeyTime;
	double m_iKeyRepeat;
};
*/

////////////////////////////////////////////////////////////////////////////////////////////

void DrawTetrisianBlock(HDC hDc, int nColor, int nX, int nY, int nsX, COLORREF *cr, int nSize);
int randb(int nR);
HBRUSH CreateSolidColorBrush(COLORREF cr);

///////////////////////////////////////////////////////////////////////////////////////////

#endif