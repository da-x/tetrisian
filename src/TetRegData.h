////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __TETREGDATA_H__
#define __TETREGDATA_H__

////////////////////////////////////////////////////////////////////////////////////////////

class GTetRegData : public GObj
{
public:
	struct GSettings
	{					
		struct GDirect
		{	 		  		
			unsigned bAutomaticallyStartsNewGame:1;		
			unsigned b3D:1;		
			int nBuild;
		} Direct;
		GString strDefaultUsername;
	} Settings;	
	
	struct GHallOfFame 
	{
		bool bUseEntry;
		GString strName;
		int nScore;
		int nElapsed;
		int nLines;
	} HallOfFame[10];

	GTetRegData();
	~GTetRegData();

	void datasettings(bool bSave);
	void datahalloffame(bool bSave);
};


////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////

