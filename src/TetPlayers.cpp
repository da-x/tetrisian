////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "TetRegData.h"
#include "TetBoard.h"
#include "TetPlayers.h"
#include "TetGameInstance.h"
#include "TetMainWindow.h"
#include "TetPlayerWindow.h"

////////////////////////////////////////////////////////////////////////////////////////////

int GTetMessage::getsize() const
{
	int nSize = sizeof(nType);
	switch (nType)
	{
		case 1:
			nSize += sizeof(types.game.fTime) + sizeof(types.game.nType);
			switch (types.game.nType)
			{
				case 1:
					nSize += sizeof(types.game.types.event_new_next_piece);
					break;
			}	
			break;
	}	
	return nSize;
}

////////////////////////////////////////////////////////////////////////////////////////////

/*

Game states:
	
	0 - Waiting for a new piece to come in. 
	1 - Waiting for another new piece to come in. 
	2 - A new piece has arrived.
	3 - The piece is falling.

*/

GTetPlayer::GTetPlayer()
{
	m_bInvalidate = false;
	m_bGame = false;
	m_pMsg = NULL;
	m_pGame = NULL;
	m_pParentWindow = NULL;
	m_speedDrop = 0.6;
	m_pWindow = new GTetPlayerWindow(); 
	m_Board.initdrawingtools(16);
	m_Board.getdrawingobject()->setposition(20, 40);
	m_Board.getdrawingobject()->setdrawskeleton(true);
	m_Board.makeempty();
	m_NextPiece.initdrawingtools(14);
	GPoint pnt = m_Board.getdrawingobject()->getpos();
	m_NextPiece.getdrawingobject()->setposition(
	 pnt.GetX() + m_Board.getdrawingobject()->getdrawwidth()+20, pnt.GetY() + 30);
	m_NextPiece.getdrawingobject()->setdrawskeleton(false);
	m_NextPiece.makeempty();
	m_Piece.makeempty();

	initgame();	 		
}

GTetPlayer::~GTetPlayer()
{
	delete m_pWindow;
}

void GTetPlayer::setgame(GTetGame *pGame)
{
	m_pGame = pGame;
}

void GTetPlayer::setwindow(GWindow *pWindow)
{
	m_pParentWindow = pWindow;
	m_pWindow->Create(this);
}

GWindow *GTetPlayer::getparentwindow()
{
	return m_pParentWindow;
}

void GTetPlayer::invalidate() 
{
	if (!m_bInvalidate)
	{
		m_pWindow->apiInvalidate();
		m_bInvalidate = true;
	}
}

void GTetPlayer::DrawBoard(HDC hDc, HRGN hBackRegion, HDC hBackDC, int nBX, int nBY)
{
	m_bInvalidate = false;
	GTetBoard saveboard = m_Board; 
	bool b3D = m_Board.getdrawingobject()->getdraw3d();

	if (m_nState == 3)
		m_Board.put(m_nPieceX, m_nPieceY, m_Piece);

	m_NextPiece.getdrawingobject()->DrawIfNessAndBlt(hDc, hBackDC, hBackRegion, nBX, nBY); 
	m_Board.getdrawingobject()->DrawIfNessAndBlt(hDc, hBackDC, hBackRegion, nBX, nBY); 

	GBoardDrawing *dt = m_Board.getdrawingobject();

	if (m_pMsg)
		dt->PutText(m_pMsg, m_nMsgSize);

	dt->BltBoard(hDc, hBackRegion);

	if (m_nState == 4  &&  m_bBlinkMode)
		dt->DrawLineBlink(hDc);

	char ch[100];

	GString strUser("User: ");
	strUser += getplayername();

	AddTextBetweenRegions(hDc, hBackRegion, strUser, 
	   dt->getpos().GetX()+dt->getwidth(), dt->getpos().GetY()-25, 0, hBackDC);
	
	AddTextBetweenRegions(hDc, hBackRegion, "Next Piece: ", 
	   dt->getpos().GetX()+dt->getdrawwidth()+15, dt->getpos().GetY(), 0, hBackDC);
			
	wsprintf(ch, "Score:\n  %d\n\nTime:\n  %s\n", m_nScore, (char *)getgametimestr());
	AddTextBetweenRegions(hDc, hBackRegion, ch, 
		dt->getpos().GetX()+dt->getdrawwidth()+15, dt->getpos().GetY()+130, 0, hBackDC);

	m_Board = saveboard;
}

void GTetPlayer::initgame()
{
	setmsg(NULL);
	m_bGame = true;
	m_Board.makeempty();
	m_timeSync = 0;
	m_nState = 0;
	m_nPieceX = 0;
	m_nPieceY = 0;
	m_nScore = 0;
	m_nLines = 0;
	m_timeGame = 0;
	m_timeCapasitor = 0;
}

void GTetPlayer::initnogame()
{		
	m_nState = 100;
	m_Board.makeempty();
	m_NextPiece.makeempty();
	setmsg("Start a new game");
}

void GTetPlayer::endgame()
{
	m_nState = 100;
	if (m_nState < 4)
		m_Board.put(m_nPieceX, m_nPieceY, m_Piece);
	setmsg("Game Ended!");
}

bool GTetPlayer::wasgame()
{
	return m_bGame;
}

void GTetPlayer::setwasntgame()
{
	m_bGame = false;
}

void GTetPlayer::process(double fTime)
{
	double fTimePassed = fTime;	

	if (m_nState == 2  ||  m_nState == 3)
		m_timeGame += fTimePassed;

	double fSaveTimePassed = fTimePassed;
	double fSmallestTimeUnit = 0.0001;
	int nUnits = (int)(fTimePassed / fSmallestTimeUnit);

	for (int i=0; i <= nUnits; i++)
	{
		fTimePassed = (i == nUnits) ? (fSaveTimePassed - i*fSmallestTimeUnit) 
		              : fSmallestTimeUnit;
		addsynctime(fTimePassed);

		int nPrevState = m_nState;
		onstate(m_nState);
		switch (m_nState)
		{
			case 2:
			{
				m_nState = 3;
				int nBoardX = m_Board.getx();
				int nBoardY = m_Board.gety();
				m_nPieceX = (nBoardX-m_Piece.sizex())/2-m_Piece.startx();
				m_nPieceY = -4;
				while (m_nPieceY < nBoardY)
				{
					if (!m_Board.checkposoverblocks(m_nPieceX, m_nPieceY, m_Piece))
					{
						m_Board.put(m_nPieceX, m_nPieceY, m_Piece);
						m_nState = 100;
						setmsg("Game Over!",24);
						ongameover();
						break;
					}
					if (m_Board.checkpos(m_nPieceX, m_nPieceY, m_Piece))
						break;
					else
						m_nPieceY++;
				}
				invalidate();
				break;
			}
			case 3:
			{
				m_timeCapasitor += fTimePassed;
				while (m_timeCapasitor > m_speedDrop)
				{
					m_timeCapasitor -= m_speedDrop;

					if (m_Board.checkpos(m_nPieceX, m_nPieceY+1, m_Piece))
					m_nPieceY++;
					else
						laypiece();
					invalidate();

					if (nPrevState != m_nState)
						break;
				}
				break;				 			
			}
			case 4:
			{
				m_timeCapasitor += fTimePassed;
				while (m_timeCapasitor > m_speedBlink)
				{
					m_timeCapasitor -= m_speedBlink;

					m_bBlinkMode = !m_bBlinkMode;
					invalidate();
					
					if (m_timeBlinkLeft > m_speedBlink)
						m_timeBlinkLeft -= m_speedBlink;
					else
					{
						m_nState = 1;
						eliminatelines();
					}
				}
				break;		
			}
			default:
			{
				addsynctime(fTimePassed);
				fTimePassed = 0;
				break;
			}
		}
	}
}

void GTetPlayer::user_down()
{
	if (m_nState == 3)
	{
		GTetMessage tm;
		tm.nType = 1;
		tm.types.game.nType = 5;
		tm.types.game.fTime = m_timeSync;
		ssendmsg(tm);

		m_timeCapasitor += m_speedDrop - fmod(m_timeCapasitor, m_speedDrop);
	}
}

void GTetPlayer::user_left()
{
	if (m_nState == 3)
		if (m_Board.checkpos(m_nPieceX-1, m_nPieceY, m_Piece))
		{
			GTetMessage tm;
			tm.nType = 1;
			tm.types.game.nType = 3;
			tm.types.game.fTime = m_timeSync;
			ssendmsg(tm);

			m_nPieceX--;
			invalidate();
		}
}

void GTetPlayer::user_right()
{
	if (m_nState == 3)
		if (m_Board.checkpos(m_nPieceX+1, m_nPieceY, m_Piece))
		{
			GTetMessage tm;
			tm.nType = 1;
			tm.types.game.nType = 4;
			tm.types.game.fTime = m_timeSync;
			ssendmsg(tm);

			m_nPieceX++;
			invalidate();
		}
}

void GTetPlayer::user_sdown()
{
	if (m_nState == 3)
	{
		GTetMessage tm;
		tm.nType = 1;
		tm.types.game.nType = 6;
		tm.types.game.fTime = m_timeSync;
		ssendmsg(tm);

		while (m_Board.checkpos(m_nPieceX, m_nPieceY+1, m_Piece))
			m_nPieceY++;
		invalidate();
		laypiece();
	}
}

void GTetPlayer::user_rotate()
{
	if (m_nState == 3)
	{
		GTetMessage tm;
		tm.nType = 1;
		tm.types.game.nType = 2;
		tm.types.game.fTime = m_timeSync;
		ssendmsg(tm);

		GTetPiece rotpiece = m_Piece;
		rotpiece.rotate();

		if (m_Board.checkpos(m_nPieceX, m_nPieceY, rotpiece))
		{
			m_Piece = rotpiece;					
			invalidate();
		}
	}
}

int GTetPlayer::user_pause()
{
	int nRet = 2;
	if (m_nState == 99)
	{
		m_Board = m_SaveBoard;
		m_nState = m_nSaveState;
		setmsg(NULL);
		nRet = 0;		
	}
	else
	{	
		if (m_nState >= 2  &&  m_nState <= 4)
		{
			m_nSaveState = m_nState;
			m_nState = 99;
			m_SaveBoard = m_Board;
			m_Board.makeempty(9);
			setmsg("Paused");
			nRet = 1;
		}
	}
	invalidate();
	return nRet;
}	 

void GTetPlayer::event_new_next_piece(int nForm, int nRot)
{
	if (m_nState == 0  ||  m_nState == 1)
	{
		GTetMessage tm;
		tm.nType = 1;
		tm.types.game.nType = 1;
		tm.types.game.fTime = m_timeSync;
		tm.types.game.types.event_new_next_piece.nForm = nForm;
		tm.types.game.types.event_new_next_piece.nRot = nRot;
		ssendmsg(tm);

		m_Piece = m_NextPiece;
		m_NextPiece.setform(nForm, nRot);
		m_nState++;
	}
}	

void GTetPlayer::laypiece()
{
	m_nScore += 1;
	invalidate();
	m_Board.put(m_nPieceX, m_nPieceY, m_Piece);

	if (m_Board.checkcompletelines() > 0)
	{
		m_nState = 4;
		m_timeBlinkLeft = 0.5;
		m_speedBlink = 0.03;
		m_bBlinkMode = true;
		m_timeCapasitor = m_speedBlink;
	}
	else
		m_nState = 1;
}

int GTetPlayer::eliminatelines()
{
	int nPoints[5] = {0, 10, 30, 70, 110};			 				;
	int nComplete = m_Board.checkcompletelines();
	m_nLines += nComplete;
	m_nScore += nPoints[nComplete];

	m_Board.removecompletelines();
	return nComplete;
}

GString GTetPlayer::getgametimestr()
{
	char ch[100];

	wsprintf(ch, "%02d:%02d:%02d", (int)m_timeGame/3600, 
	  ((int)m_timeGame % 3600)/60, (int)m_timeGame % 60);

	return ch;
}


void GTetPlayer::setmsg(char *pMsg, int nSize)
{
	m_nMsgSize = nSize;
	m_pMsg = pMsg;
	invalidate();
}

void GTetPlayer::ssendmsg(const GTetMessage &msg)
{
	if (m_pGame)
		m_pGame->notifymsg(this, msg);
}

GPoint GTetPlayer::calcwindowsize()
{
	GPoint pntRet = m_Board.getdrawingobject()->getpos();
	pntRet.SetX(pntRet.GetX() + m_Board.getdrawingobject()->getdrawwidth());
	pntRet.SetY(pntRet.GetY() + m_Board.getdrawingobject()->getdrawheight());

	pntRet.SetX(pntRet.GetX() + 20 + m_NextPiece.getdrawingobject()->getdrawwidth());

	pntRet.SetX(pntRet.GetX() + 20);
	pntRet.SetY(pntRet.GetY() + 20);
	return pntRet;
}

 
////////////////////////////////////////////////////////////////////////////////////////////

void GTetLocalPlayer::process(double fTime)
{
	if (getstate() == 0  || getstate() == 1)
	{			
		onstate(getstate());
		GTetPiece tp;
		tp.random();
		event_new_next_piece(tp.getform(), tp.getrot());
	}

	GTetPlayer::process(fTime);	
}				    

void GTetLocalPlayer::onstate(int nState)
{
	if (nState == 1  &&  m_pGame)
		m_pGame->keyblank();
}

int GTetLocalPlayer::user_pause()
{
	int nRet = GTetPlayer::user_pause();

	char *ch = ((nRet != 0) ? "Resum&e" : "Paus&e");
	HMENU hMenu = GetMenu(*getparentwindow());
	MENUITEMINFO mii;
	memset(&mii, 0, sizeof(mii));
	mii.cbSize = sizeof(mii);
	mii.fMask = MIIM_DATA | MIIM_TYPE;
	mii.fType = MFT_STRING;
	mii.dwTypeData = ch;
	mii.cch = strlen(ch);
	SetMenuItemInfo(hMenu, ID_PAUSE, FALSE, &mii);
	DrawMenuBar(*getparentwindow());

	return nRet;
}

void GTetLocalPlayer::ongameover()
{
	if (m_pGame)
	{
		m_pGame->halloffamecheck();
		setwasntgame();
		SendMessage(*getparentwindow(), WM_INITMENU, 0, 0);
	}
}					  

GString GTetLocalPlayer::getplayername()
{
	return m_pGame->m_RegData.Settings.strDefaultUsername; 
}

////////////////////////////////////////////////////////////////////////////////////////////

void GTetRemotePlayer::process(double fTime)
{
	process(fTime);
}

void GTetRemotePlayer::addsynctime(double fAdd)
{
	GTetPlayer::addsynctime(fAdd); 
}

////////////////////////////////////////////////////////////////////////////////////////////
