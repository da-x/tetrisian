////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "TetRegData.h"
#include "TetBoard.h"
#include "TetPlayers.h"
#include "TetGameInstance.h"
#include "TetMainWindow.h"

GTetGame tetgame;

////////////////////////////////////////////////////////////////////////////////////////////

GTetGame::GTetGame()
{
	m_playLocal.setgame(this);
	m_iTime.QuadPart = 0;
	
	m_bDownKeyPressed = false;
	m_nKey = 0;
	m_nGameMode = 0;

	if (!m_RegData.Settings.Direct.bAutomaticallyStartsNewGame)
	{
		if (m_nGameMode == 0)
			m_playLocal.initnogame();
	}
}

GTetGame::~GTetGame()
{
}

GTetGame::DrawBoard(HDC hDc, HRGN hBackRegion, HDC hBackDC, int nBX, int nBY)
{
	m_playLocal.DrawBoard(hDc, hBackRegion, hBackDC, nBX, nBY);
	return 0;
}

void GTetGame::setwindow(GTetMainWindow *pWindow)
{
	m_pWindow = pWindow;
	m_playLocal.setwindow((GWindow *)m_pWindow);
};

void GTetGame::invalidate()
{
	m_pWindow->apiInvalidate();
}

void GTetGame::process()
{
	LARGE_INTEGER counter, freq, diff;
	QueryPerformanceCounter(&counter);
	QueryPerformanceFrequency(&freq);

	if (m_iTime.QuadPart == 0)
		m_iTime.QuadPart = counter.QuadPart;		
	diff.QuadPart = counter.QuadPart - m_iTime.QuadPart;
	m_iTime.QuadPart = counter.QuadPart;

	double fTime = ((double)diff.QuadPart) / freq.QuadPart;
	
	/*
	char ch[100];
	SYSTEMTIME st;
	GetLocalTime(&st);
	int nMilli = ((int)st.wMilliseconds) + 
	             ((int)st.wSecond)*1000+
				 ((int)st.wMinute)*60000+
				 ((int)st.wHour)*3600000;

	wsprintf(ch, "%d\n", nMilli);
	OutputDebugString(ch);
	*/

	if (m_nKey != 0)
	{
		m_timeKey += fTime;
		while (m_timeKey > m_speedKey)
		{
			m_timeKey -= m_speedKey;
			if (m_speedKey == 0)
			{
				switch (m_nKey)
				{
					case VK_UP:	
						m_playLocal.user_rotate();
						break;
					case VK_SPACE:	
						m_playLocal.user_sdown();
						break;
					case 0x50: // P
					{			
						m_playLocal.user_pause();
						break;					
					}
				}
				m_speedKey = 0.01;
			}
			else
			{
				if (m_pKeyFriction)
				{
					m_speedKey = 0.2;
					m_pKeyFriction = false;
				}
				else
					m_speedKey = 0.05;
				switch (m_nKey)
				{
					case VK_LEFT:
						m_playLocal.user_left();
						break;
					case VK_RIGHT:
						m_playLocal.user_right();
						break;
				}
			}
		}
	}

	if (m_bDownKeyPressed)
	{
		m_timeDownKey += fTime;
		
		while (m_timeDownKey > m_speedDownKey)
		{
			if (m_speedDownKey == 0)
				m_speedDownKey = 0.2;

			m_timeDownKey -= m_speedDownKey;
			m_playLocal.user_down();
			if (m_speedDownKey == 0.2)
				m_speedDownKey = 0.03;
		}
	}

	m_playLocal.process(fTime);
}

void GTetGame::keydown(DWORD nKey)
{
	if (m_nKey != nKey)
	{
		if (nKey == VK_DOWN)
		{
			m_bDownKeyPressed = true;
			m_timeDownKey = 0;
			m_speedDownKey = 0;
		}

		m_nKey = nKey;

		m_speedKey = 0;
		m_timeKey = 0;
		m_pKeyFriction = true;
	}
}

void GTetGame::keyup(DWORD nKey)
{
	if (nKey == VK_DOWN)
		m_bDownKeyPressed = false;
	if (m_nKey == nKey)
		m_nKey = 0;
}

void GTetGame::halloffamecheck()
{
	int nGameScore = m_playLocal.getscore();
	if (nGameScore > 0)
	{
		m_RegData.datahalloffame(false);
		int k=-1;
		for (int i=0; i < 10; i++)
		{
			int nScore = m_RegData.HallOfFame[i].bUseEntry ? m_RegData.HallOfFame[i].nScore : 0;
			if (nScore < nGameScore)
			{
				for (int j=9; i < j; j--)
					m_RegData.HallOfFame[j] = m_RegData.HallOfFame[j-1];			
				k = i;
				break;
			}
		}
		if (k != -1)
		{
			char ch[128], rettext[128];
			wsprintf(ch, "Congratulations! you have entered the Hall Of Fame at the %d%s place!\n"
				"Enter your name:", k+1, AddOrdinal(k+1));
			strcpy(rettext, m_RegData.Settings.strDefaultUsername);
			ShowOtherDialog(*m_pWindow, ch, rettext);
			if (rettext[0] == 0)
				strcpy(rettext, "Anonymous");
			m_RegData.HallOfFame[k].nScore = nGameScore;
			m_RegData.HallOfFame[k].strName = rettext;
			m_RegData.HallOfFame[k].nLines = m_playLocal.getlines();
			m_RegData.HallOfFame[k].nElapsed = m_playLocal.getgametime();
			m_RegData.HallOfFame[k].bUseEntry = true;
			m_RegData.datahalloffame(true);
		}
	}
}

void GTetGame::notifymsg(GTetPlayer *pPlayer, const GTetMessage &msg)
{
}	  

bool GTetGame::isgameplaying()
{
	if (m_playLocal.getstate() != 100)
		return true;
	return false;		
}

void GTetGame::setgamemode(int nGameMode)
{
	if (m_nGameMode != nGameMode)
	{
		bool bSwitch = true;
		if (isgameplaying())
		{
			switch (MessageBox(*m_pWindow, "Current game must end in order to switch game mode.\n"
			                   "Do you want to end the current game?\n", "Warning", MB_YESNO | MB_ICONWARNING))
			{
				case IDYES:
					user_endgame();
					break;

				case IDNO:
					bSwitch = false;
					break;
			} 
		}
		if (bSwitch)
		{
			SendMessage(*m_pWindow, WM_INITMENU, 0, 0);	
			m_nGameMode = nGameMode;
		}
	}
}

void GTetGame::user_startgame()
{
	if (m_nGameMode == 0)
	{
		if (isgameplaying())
			user_endgame();
		m_playLocal.initgame();		
	}
}

void GTetGame::user_endgame()
{
	if (m_nGameMode == 0)
	{
		if (m_playLocal.wasgame())
		{
			m_playLocal.setwasntgame();
			if (m_playLocal.getstate() < 100)
				m_playLocal.endgame();
			halloffamecheck();
		}
	}
	SendMessage(*m_pWindow, WM_INITMENU, 0, 0);
};

////////////////////////////////////////////////////////////////////////////////////////////
