////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __TETGAMEINSTANCE_H__
#define __TETGAMEINSTANCE_H__

////////////////////////////////////////////////////////////////////////////////////////////

class GTetGame : public GObj
{
public:
	GTetRegData m_RegData;
	GTetLocalPlayer m_playLocal;

	GTetGame();
	~GTetGame();
	DrawBoard(HDC hDc, HRGN hBackRegion, HDC hBackDC, int nBX, int nBY);
	void setwindow(class GTetMainWindow *pWindow);
	void invalidate();
	void process();
	void keydown(DWORD nKey);
	void keyup(DWORD nKey);
	void keyblank() {m_bDownKeyPressed=false;}; 
	void halloffamecheck();
	void notifymsg(GTetPlayer *pPlayer, const GTetMessage &msg);
	bool isgameplaying();
	int getgamemode() {return m_nGameMode;};
	void setgamemode(int nGameMode);
	void user_startgame();
	void user_endgame();

private:
	class GTetMainWindow *m_pWindow;

	LARGE_INTEGER m_iTime; 

	DWORD m_nKey;		  						   
	bool m_bDownKeyPressed, m_pKeyFriction;
	double m_timeDelayKey;
	double m_timeKey, m_timeDownKey;
	double m_speedKey, m_speedDownKey;

	int m_nGameMode;
};

extern GTetGame tetgame;

////////////////////////////////////////////////////////////////////////////////////////////


#endif