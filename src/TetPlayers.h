////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __TETPLAYERS_H__
#define __TETPLAYERS_H__

////////////////////////////////////////////////////////////////////////////////////////////

#pragma pack( push, before_struct)
#pragma pack(1)

struct GTetMessage {
	unsigned char nType; 
	union {
		struct {
			double fTime;
			unsigned char nType; 
			union {
				struct {
					unsigned char nForm;
					unsigned char nRot;
				} event_new_next_piece; 
				// nType = 1;
			} types;
			// Rotate: nType = 2;
			// Left: nType = 3;
			// Right: nType = 4;
			// Down: nType = 5;
			// Super Down: nType = 6;
		} game; // nType == 1;
	} types;
	int getsize() const;
};

#pragma pack( pop, before_struct)

class GTetPlayer : public GObj
{
public:
	GTetPlayer();
	~GTetPlayer();
								
	void setgame(class GTetGame *pGame);
	void setwindow(GWindow *pWindow);
	GWindow *getparentwindow();
	void invalidate();
	void DrawBoard(HDC hDc, HRGN hBackRegion, HDC hBackDC, int nBX, int nBY);

	void initgame();
	void initnogame();
	void endgame();
	bool wasgame();
	void setwasntgame();
	virtual void process(double fTime);

	void user_down();
	void user_left();
	void user_right();
	void user_rotate();
	void user_sdown();
	virtual int user_pause();
	void event_new_next_piece(int nForm, int nRot);	
	virtual void onstate(int nState){};

	void laypiece();
	int eliminatelines();
	int getstate() {return m_nState;};
	int getscore() {return m_nScore;};
	int getlines() {return m_nLines;};
	int getgametime() {return (int)m_timeGame;};
	GString getgametimestr();

	void setmsg(char *pMsg, int nSize=17);
	GPoint calcwindowsize();
	virtual bool getdockdefault() {return true;};
	virtual GString getplayername() { return "Unknown";};

private:
	class GTetPlayerWindow *m_pWindow;
	GWindow *m_pParentWindow;

	GTetBoard m_Board, m_SaveBoard;
	GTetPiece m_Piece;
	GTetPiece m_NextPiece;

	bool m_bInvalidate;
	int m_nState, m_nSaveState;

	int m_nPieceX, m_nPieceY;

	int m_nScore, m_nLines;
	bool m_bGame;

	double m_timeGame;
	double m_speedDrop, m_timeCapasitor;

	bool m_bBlinkMode;
	double m_timeBlinkLeft;
	double m_speedBlink;

	char *m_pMsg;
	int m_nMsgSize;

	void ssendmsg(const GTetMessage &msg);

protected:
	GTetGame *m_pGame;
	double m_timeSync;

	virtual void ongameover() {}; 
	virtual void addsynctime(double fAdd) {m_timeSync += fAdd;}
};

////////////////////////////////////////////////////////////////////////////////////////////

class GTetLocalPlayer : public GTetPlayer
{
public:
	void process(double fTime);
	void onstate(int nState);
	int user_pause();
	GString getplayername();

protected:
	void ongameover(); 
};

////////////////////////////////////////////////////////////////////////////////////////////

class GTetRemotePlayer : public GTetPlayer
{
protected:
	void process(double fTime);
	void addsynctime(double fAdd);
};

////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////
