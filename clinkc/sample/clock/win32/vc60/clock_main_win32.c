/******************************************************************
*
*	CyberX3D for C++
*
*	Copyright (C) Satoshi Konno 1996-2003
*
*	File:	ClockMain.cpp
*
******************************************************************/
#include <winsock2.h>   
#include <windowsx.h>   
#include <commdlg.h>   
//#include "resource.h"

#include "clock.h"
#include "clock_device.h"

static HINSTANCE gInstance;
static HBITMAP clockBitmap;

const char panelFilename[] = "images/clock.bmp";
const char szTitle[] = "CyberLink Clock Device";

LONG WINAPI WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

CgUpnpDevice *clockDev; 

////////////////////////////////////////////////////////// 
//  InitApp
////////////////////////////////////////////////////////// 

void InitApp()
{
	clockBitmap = (HBITMAP)LoadImage(gInstance, panelFilename, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE); 

	clockDev = upnp_clock_device_new();
	cg_upnp_device_start(clockDev);
}

void ExitApp()
{
	cg_upnp_device_stop(clockDev);
	cg_upnp_device_delete(clockDev);
}

void CALLBACK ClockUpdate(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	upnp_clock_device_update(clockDev);
	InvalidateRect(hWnd, NULL, FALSE);
}

////////////////////////////////////////////////////////// 
//  WinMain
////////////////////////////////////////////////////////// 

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	static char szAppName[] = "CyberLink Clock Device";
	WNDCLASS	wc;
	MSG			msg; 
	HWND		hWnd;
	int borderXSize, borderYSize;

	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)WndProc;	  
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= NULL;  
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName		= ""/*MAKEINTRESOURCE(IDR_MENU)*/;  
	wc.lpszClassName	= szAppName; 

	RegisterClass( &wc );
  
	gInstance = hInstance;

	borderXSize = GetSystemMetrics(SM_CXFIXEDFRAME);
	borderYSize = GetSystemMetrics(SM_CXFIXEDFRAME);

	hWnd = CreateWindow(
				szAppName,
				szTitle,
				//WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
				WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
				CW_USEDEFAULT, CW_USEDEFAULT, 
				300 + borderXSize*2, 
				200 + borderYSize *2 + GetSystemMetrics(SM_CYCAPTION),
				NULL,	 
				NULL,	 
				hInstance,
				NULL);

	ShowWindow( hWnd, nCmdShow );
	UpdateWindow( hWnd );		

	SetTimer(hWnd, 1, 1000, ClockUpdate);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage( &msg ); 
		DispatchMessage( &msg );
	}

	return( msg.wParam ); 
}

////////////////////////////////////////////////////////// 
//  OnPaint
////////////////////////////////////////////////////////// 

void OnPaint(HWND hWnd) 
{
	int width = 300;
	int height = 200;
	PAINTSTRUCT ps;
	HDC hdc;
	HDC memDC;
	HBITMAP memBitmap;
	HDC bitmapDC;
	HFONT hFont;
	HFONT oldFont;
	HBRUSH barBrush;
	RECT rect;
	CgSysTime currTime;
	char textBuf[SYSTEM_TIME_BUF_LEN];
	int timeStrX = 60;
	int timeStrY = 80;
	int timeStrWidth = 180;
	int timeStrOffset = 55;
	int DEFAULT_SECOND_BLOCK_HEIGHT = 8;
	int sec;
	int secBarBlockSize = timeStrWidth / 60;
	int secBarBlockY = timeStrY + timeStrOffset;
	int n, x;
	char secStr[8];

	hdc = BeginPaint(hWnd,&ps);

	memDC = CreateCompatibleDC(hdc);
	memBitmap = CreateCompatibleBitmap(hdc, width, height);
	SelectObject(memDC, memBitmap);

	bitmapDC = CreateCompatibleDC(hdc);
	SelectObject(bitmapDC, clockBitmap);
	BitBlt(memDC, 0, 0, width, height, bitmapDC, 0, 0, SRCCOPY);
	DeleteDC(bitmapDC);

	SetBkMode(memDC,TRANSPARENT);
	SetTextColor(memDC,RGB(0, 0, 0));

	currTime = cg_getcurrentsystemtime();

	//// Date String ////

	GetDateString(currTime, textBuf);
	hFont = CreateFont(20, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Lucida Console");
	oldFont = (HFONT)SelectObject(memDC, hFont);
	TextOut(memDC, 45, 45, textBuf, strlen(textBuf));
	SelectObject(memDC, oldFont);
	DeleteObject(hFont);

	//// Time String ////
	
	GetTimeString(currTime, textBuf);
	hFont = CreateFont(60, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Lucida Console");
	oldFont = (HFONT)SelectObject(memDC, hFont);
	TextOut(memDC, timeStrX, timeStrY, textBuf, strlen(textBuf));
	SelectObject(memDC, oldFont);
	DeleteObject(hFont);

	//// Second Bar ////
	
	barBrush = CreateSolidBrush(RGB(0, 0, 0));
	sec = GetSecond(currTime);
	for (n=0; n<sec; n++) {
		x = timeStrX + (secBarBlockSize*n);
		rect.left = x;
		rect.top = secBarBlockY;
		rect.right = x + (secBarBlockSize - 1);
		rect.bottom = secBarBlockY + DEFAULT_SECOND_BLOCK_HEIGHT;
		FillRect(memDC, &rect, barBrush);
	}
	DeleteObject(barBrush);

	if (sec != 0 && (sec % 10) == 0) {
		x = timeStrX + (secBarBlockSize*sec);
		hFont = CreateFont(10, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Lucida Console");
		oldFont = (HFONT)SelectObject(memDC, hFont);
		sprintf(secStr, "%02d", sec);
		TextOut(memDC, x + secBarBlockSize, secBarBlockY, secStr, strlen(secStr));
		SelectObject(memDC, oldFont);
		DeleteObject(hFont);
	}

	BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);

	DeleteDC(memDC);

	EndPaint(hWnd,&ps);
}

////////////////////////////////////////////////////////// 
//  WndProc
////////////////////////////////////////////////////////// 
 
LONG WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg){
	case WM_CREATE: 
		InitApp();
		return 0;

	case WM_SIZE:
	     return 0;

	case WM_PAINT:
		OnPaint(hWnd);
		return 0;

	case WM_ERASEBKGND:
		return 0;

	case WM_COMMAND:
		return 0;

	case WM_LBUTTONDOWN:
		return 0;

	case WM_LBUTTONUP:
		return 0;

	case WM_MOUSEMOVE:
		return 0;

	case WM_DESTROY:
		ExitApp();
		PostQuitMessage( 0 );
		return 0;
	}
	return DefWindowProc( hWnd, msg, wParam, lParam );
}
