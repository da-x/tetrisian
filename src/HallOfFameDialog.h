#ifndef __HALL_OF_FAME_DIALOG_H__
#define __HALL_OF_FAME_DIALOG_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

extern HWND hHallOfFameDialogWnd;

BOOL CALLBACK HallOfFameDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif