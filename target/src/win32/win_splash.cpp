#include "../qcommon/qcommon.h"
#include "win_local.h"

#define SPLASH_CLASS "CoD Splash Screen"
#define SPLASH_WIDTH 320
#define SPLASH_HEIGHT 100

static ALIGNED(16) HWND g_splashWnd;

void Sys_CreateSplashWindow( void );
void Sys_DestroySplashWindow( void );
void Sys_ShowSplashWindow( void );
void Sys_HideSplashWindow( void );

/*
==================
Sys_CreateSplashWindow
==================
*/
void Sys_CreateSplashWindow( void )
{
	WNDCLASSA wc;
	SIZE screenSize;
	HANDLE hbmp;
	HWND hwnd;
	RECT rc;
	SIZE bmpSize;

	memset( &wc, 0, sizeof( wc ) );
	wc.lpfnWndProc = DefWindowProcA;
	wc.hInstance = g_wv.hInstance;
	wc.hIcon = LoadIconA( g_wv.hInstance, MAKEINTRESOURCEA( 1 ) );
	wc.hCursor = LoadCursorA( NULL, IDC_ARROW );
	wc.hbrBackground = (HBRUSH)( COLOR_WINDOW + 1 );
	wc.lpszClassName = SPLASH_CLASS;

	if ( !RegisterClassA( &wc ) )
		return;

	screenSize.cx = GetSystemMetrics( SM_CXFULLSCREEN );
	screenSize.cy = GetSystemMetrics( SM_CYFULLSCREEN );

	hbmp = LoadImageA( NULL, "cod.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
	if ( !hbmp )
		return;

	g_splashWnd = CreateWindowExA( WS_EX_APPWINDOW, SPLASH_CLASS, "Call of Duty 2 Multiplayer", WS_POPUP | WS_BORDER | WS_SYSMENU,
		( screenSize.cx - SPLASH_WIDTH ) / 2, ( screenSize.cy - SPLASH_HEIGHT ) / 2, SPLASH_WIDTH, SPLASH_HEIGHT, NULL, NULL, g_wv.hInstance, NULL );
	if ( !g_splashWnd )
		return;

	hwnd = CreateWindowExA( 0, "Static", NULL, WS_CHILD | WS_VISIBLE | SS_BITMAP, 0, 0, SPLASH_WIDTH, SPLASH_HEIGHT, g_splashWnd, NULL, g_wv.hInstance, NULL );
	if ( !hwnd )
		return;

	SendMessageA( hwnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbmp );
	GetWindowRect( hwnd, &rc );

	bmpSize.cx = rc.right - rc.left + 2;
	bmpSize.cy = rc.bottom - rc.top + 2;

	rc.left = ( screenSize.cx - bmpSize.cx ) / 2;
	rc.right = rc.left + bmpSize.cx;
	rc.top = ( screenSize.cy - bmpSize.cy ) / 2;
	rc.bottom = rc.top + bmpSize.cy;

	AdjustWindowRect( &rc, WS_CHILD | WS_VISIBLE | SS_BITMAP, FALSE );
	SetWindowPos( g_splashWnd, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER );
}

/*
==================
Sys_DestroySplashWindow
==================
*/
void Sys_DestroySplashWindow( void )
{
	if ( g_splashWnd )
	{
		ShowWindow( g_splashWnd, SW_HIDE );
		DestroyWindow( g_splashWnd );
		g_splashWnd = NULL;
	}
}

/*
==================
Sys_ShowSplashWindow
==================
*/
void Sys_ShowSplashWindow( void )
{
	if ( g_splashWnd )
	{
		ShowWindow( g_splashWnd, SW_SHOW );
		UpdateWindow( g_splashWnd );
	}
}

/*
==================
Sys_HideSplashWindow
==================
*/
void Sys_HideSplashWindow( void )
{
	if ( g_splashWnd )
		ShowWindow( g_splashWnd, SW_HIDE );
}

