////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "buildnmb.h"
#include "TetRegData.h"

////////////////////////////////////////////////////////////////////////////////////////////


GTetRegData::GTetRegData()
{
	for (int i=0; i < 10; i++)
		HallOfFame[i].bUseEntry = false;
	memset(&Settings.Direct, 0, sizeof(Settings.Direct));
	Settings.Direct.bAutomaticallyStartsNewGame = 1;
	Settings.Direct.b3D = 1;
	Settings.Direct.nBuild = BUILDNUMBER;
	Settings.strDefaultUsername = "Anonymous";
	datasettings(false);
}

GTetRegData::~GTetRegData()
{
	datasettings(true);
}							 

GString RegQueryValueString(HKEY hKey, LPCTSTR lpValueName)
{
	char ch[1024];
	ch[0] = 0;
	DWORD dwType = 0, dwRead=1024;
	RegQueryValueEx(hKey, lpValueName, 0, &dwType, (BYTE *)ch, &dwRead);
	if (dwType != REG_SZ)
		ch[0] = 0;		
	return ch;
}

void RegSetValueString(HKEY hKey, LPCTSTR lpValueName, const char *pStr)
{
	if (pStr == NULL)
		RegSetValueEx(hKey, lpValueName, 0, REG_SZ, NULL, 0);
	else
		RegSetValueEx(hKey, lpValueName, 0, REG_SZ, (BYTE *)pStr, strlen(pStr) + 1);
}

void GTetRegData::datasettings(bool bSave)
{
	HKEY hKey = NULL;
	char *path = "Software\\CallistaSoft\\Tetrisian";
	RegCreateKeyEx(HKEY_CURRENT_USER, path, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	if (!hKey)
		return;	   

	path = "Settings";
	DWORD dwType = 0, dwRead=0;
	
	if (bSave)
	{
		Settings.Direct.nBuild = BUILDNUMBER;
		RegSetValueEx(hKey, path, 0, REG_BINARY, (CONST BYTE *)&Settings.Direct, sizeof(Settings.Direct));
	}
	else
	{
		Settings.Direct.nBuild = 0;
		RegQueryValueEx(hKey, path, 0, &dwType, (BYTE *)&Settings.Direct, &(dwRead = sizeof(Settings.Direct)));
	}

	path = "DefaultUsername";

	if (bSave)
		RegSetValueString(hKey, path, Settings.strDefaultUsername);
	else
		if (Settings.Direct.nBuild > 665)
			Settings.strDefaultUsername = RegQueryValueString(hKey, path);	

	RegCloseKey(hKey);
	if (!bSave)
	{
		if (Settings.Direct.nBuild <= 655)
		{
			Settings.Direct.b3D = 1;
			Settings.Direct.nBuild = BUILDNUMBER;
		}			
	}
}

void GTetRegData::datahalloffame(bool bSave)
{
	HKEY hKey = NULL;
	char *path = "Software\\CallistaSoft\\Tetrisian";
	RegCreateKeyEx(HKEY_CURRENT_USER, path, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	if (!hKey)
		return;

	path = "HallOfFame";
	char *pKey = "Genesis";

	GMemoryStream memfile;
	int i;
	if (bSave)
	{
		memfile << (int)1;
		for (i=0; i < 10; i++)
		{
			memfile << HallOfFame[i].bUseEntry;
			memfile << HallOfFame[i].strName;
			memfile << HallOfFame[i].nScore;
			memfile << HallOfFame[i].nLines;
			memfile << HallOfFame[i].nElapsed;
		}

		memfile << (int)1;
		memfile.SetPos(0);

		GMemoryStream encmemfile;
		memfile.Encrypt(encmemfile, memfile.GetSize(), pKey, false);
		encmemfile.SetPos(0);

		int nSize = encmemfile.GetSize();
		void *pData = malloc(nSize+sizeof(int));
		(*(int *)pData) = nSize;
		encmemfile.Read((char *)((int *)pData + 1), nSize);

		RegSetValueEx(hKey, path, 0, REG_BINARY, (CONST BYTE *)pData, nSize+sizeof(int));
		free(pData);
	}
	else
	{
		for (i=0; i < 10; i++)
			HallOfFame[i].bUseEntry = false;

 		DWORD dwSize=0, dwType=REG_SZ;
		RegQueryValueEx(hKey, path, 0, &dwType, NULL, &dwSize);
		if (dwSize > sizeof(int)  &&  dwType==REG_BINARY)
		{				
			void *pData = malloc(dwSize);
			RegQueryValueEx(hKey, path, 0, &dwType, (BYTE *)pData, &dwSize);
			if (*(unsigned int *)pData == dwSize - sizeof(int))
			{
				memfile.Write((char *)((int *)pData + 1), dwSize - sizeof(int));
				
				memfile.SetPos(0);
				GMemoryStream decmemfile;
				memfile.Encrypt(decmemfile, memfile.GetSize(), pKey, true);				

				decmemfile.SetPos(dwSize-2*sizeof(int));
				int nValidate = 0;
				decmemfile >> nValidate; 
				if (nValidate == 1)
				{
					decmemfile.SetPos(0);
					decmemfile >> (int)nValidate;
					if (nValidate == 1)
					{
						for (i=0; i < 10; i++)
						{
							decmemfile >> HallOfFame[i].bUseEntry;
 							decmemfile >> HallOfFame[i].strName;
							decmemfile >> HallOfFame[i].nScore;
							decmemfile >> HallOfFame[i].nLines;
							decmemfile >> HallOfFame[i].nElapsed;
						}
					}
				}
			}
			free(pData);
		}
	}

	RegCloseKey(hKey);
}


////////////////////////////////////////////////////////////////////////////////////////////


