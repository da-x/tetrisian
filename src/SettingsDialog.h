#ifndef __SETTINGS_DIALOG_H__
#define __SETTINGS_DIALOG_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

extern HWND hSettingsDialogWnd;

BOOL CALLBACK SettingsDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif