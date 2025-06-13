#define _CRT_SECURE_NO_WARNINGS
#include<Windows.h>
#include<CommCtrl.h>
#include<cstdio>
#include<iostream>
#include"resource.h"

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, (DLGPROC) DlgProc, 0);
	return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		HWND hPrefix = GetDlgItem(hwnd, IDC_SPIN_PREFIX);
		SendMessage(hPrefix, UDM_SETRANGE, 0, MAKELPARAM(30, 1));
		//https://learn.microsoft.com/en-us/windows/win32/controls/udm-setrange
		SetFocus(GetDlgItem(hwnd, IDC_IPADDRESS));
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
	}	
		break;
	case WM_COMMAND:
	{
		HWND hIPaddress = GetDlgItem(hwnd, IDC_IPADDRESS);
		HWND hIPmask = GetDlgItem(hwnd, IDC_IPMASK);
		HWND hEditPrefix = GetDlgItem(hwnd, IDC_EDIT_PREFIX);
		DWORD dwIPaddress = 0;
		DWORD dwIPmask = 0;
		switch (LOWORD(wParam))
		{
		case IDC_IPADDRESS:
		{
			SendMessage(hIPaddress, IPM_GETADDRESS, 0, (LPARAM)&dwIPaddress);
			if (FIRST_IPADDRESS(dwIPaddress) < 128)SendMessage(hIPmask, IPM_SETADDRESS, 0, 0xFF000000);
			else if (FIRST_IPADDRESS(dwIPaddress) < 192)SendMessage(hIPmask, IPM_SETADDRESS, 0, 0xFFFF0000);
			else if (FIRST_IPADDRESS(dwIPaddress) < 224)SendMessage(hIPmask, IPM_SETADDRESS, 0, 0xFFFFFF00);
		}
		break;
		case IDC_IPMASK:
		{
			SendMessage(hIPmask, IPM_GETADDRESS, 0, (LPARAM)&dwIPmask);
			DWORD dwIPprefix = 0;
			for (DWORD iMask = dwIPmask; iMask & 0x80000000; dwIPprefix++)iMask <<= 1;
			CHAR sz_prefix[3];
			sprintf(sz_prefix, "%i", dwIPprefix);
			SendMessage(hEditPrefix, WM_SETTEXT, 0, (LPARAM)sz_prefix);
		}
		break;
		
		case IDOK:
			break;
		case IDCANCEL:
			EndDialog(hwnd, 0);
			break;
		}
	}
		break;
	case WM_NOTIFY:
	{
		HWND hEditPrefix = GetDlgItem(hwnd, IDC_EDIT_PREFIX);
		HWND hIPmask = GetDlgItem(hwnd, IDC_IPMASK);
		//switch (((NMHDR*)lParam) -> idFrom)
		switch(LOWORD(wParam))
		{
		case IDC_SPIN_PREFIX:
		{

			std::cout << "WM_NOTIFY:IDC_SPIN_PREFIX:" << std::endl;
			
			DWORD dwPrefix = ((NMUPDOWN*)lParam)->iPos;
			INT iDelta = ((NMUPDOWN*)lParam)->iDelta;
			dwPrefix += iDelta;
			//https://learn.microsoft.com/en-us/windows/win32/controls/udn-deltapos
			std::cout << dwPrefix << std::endl;
			std::cout << iDelta << std::endl;
			DWORD dwIPmask = ~(0xFFFFFFFF >> dwPrefix);
			//system("Pause");
			SendMessage(hIPmask, IPM_SETADDRESS, 0, dwIPmask);

		}
		break;
		}
	}
		break;
	case WM_CLOSE:
		FreeConsole();
		EndDialog (hwnd, 0);
	}
	return FALSE;
}