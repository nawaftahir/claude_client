#include "../qcommon/qcommon.h"
#include <float.h>
#include <direct.h>
#include <tlhelp32.h>
#include "win_local.h"

static char sys_processSemaphoreFile[32];
static char sys_cmdline[1024];

extern qboolean com_errorEntered;

enum language_t;

void Key_Shutdown( void );
void Sys_DestroyConsole( void );
language_t Win_InitLocalization( void );
void Win_ShutdownLocalization( void );
void RefreshQuitOnErrorCondition( void );
void Con_Shutdown( void );
void Com_ShutdownEvents( void );
void Com_ForceSafeMode( void );
void Sys_CreateConsole( HINSTANCE hInstance );
void Conbuf_AppendText( const char *pMsg );
void Sys_SetErrorText( const char *buf );
void Sys_CreateSplashWindow( void );
void Sys_DestroySplashWindow( void );
void Sys_ShowSplashWindow( void );
void FixWindowsDesktop( void );
void CL_ShutdownHunkUsers( void );
void CL_ShutdownRef( void );
LONG WINAPI MainWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

/*
==================
Sys_GetSemaphoreFileName
==================
*/
void Sys_GetSemaphoreFileName( void )
{
	char modulePath[MAX_PATH];
	char *moduleName;
	char *i;

	GetModuleFileNameA( NULL, modulePath, MAX_PATH );
	modulePath[MAX_PATH - 1] = 0;
	moduleName = modulePath;

	for ( i = modulePath; *i; i++ )
	{
		if ( *i == '\\' || *i == ':' )
			moduleName = i + 1;
		else if ( *i == '.' )
			*i = 0;
	}

	sprintf( sys_processSemaphoreFile, "__%s", moduleName );
}

/*
==================
Sys_NormalExit
==================
*/
void Sys_NormalExit( void )
{
	DeleteFileA( sys_processSemaphoreFile );
}

/*
========================================================================

HARDWARE INFO

========================================================================
*/

typedef struct
{
	double cpuGHz;
	int sysMB;
	bool SSE;
	char gpuDescription[512];
} SysInfo;

static SysInfo sys_info;

static dvar_t *sys_cpuGHz;
static dvar_t *sys_sysMB;
static dvar_t *sys_gpu;
static dvar_t *sys_configSum;

const char *Win_LocalizeRef( const char *ref );
double Sys_BenchmarkGHz( void );
int Sys_SystemMemoryMB( void );
void Sys_DetectVideoCard( char *description );
bool Sys_SupportsSSE( void );

/*
==================
Sys_ShouldUpdateForConfigChange
==================
*/
bool Sys_ShouldUpdateForConfigChange( void )
{
	if ( MessageBoxA( NULL, Win_LocalizeRef( "WIN_CONFIGURE_UPDATED_BODY" ), Win_LocalizeRef( "WIN_CONFIGURE_UPDATED_TITLE" ), MB_YESNO | MB_ICONINFORMATION ) == IDYES )
		return true;

	return false;
}

/*
==================
Sys_RegisterInfoDvars
==================
*/
void Sys_RegisterInfoDvars( void )
{
	sys_cpuGHz = Dvar_RegisterFloat( "sys_cpuGHz", 0.0f, -FLT_MAX, FLT_MAX, DVAR_ARCHIVE | DVAR_INIT | DVAR_CHANGEABLE_RESET );
	sys_sysMB = Dvar_RegisterInt( "sys_sysMB", 0, INT_MIN, INT_MAX, DVAR_ARCHIVE | DVAR_INIT | DVAR_CHANGEABLE_RESET );
	sys_gpu = Dvar_RegisterString( "sys_gpu", "", DVAR_ARCHIVE | DVAR_INIT | DVAR_CHANGEABLE_RESET );
	sys_configSum = Dvar_RegisterInt( "sys_configSum", 0, INT_MIN, INT_MAX, DVAR_ARCHIVE | DVAR_INIT | DVAR_CHANGEABLE_RESET );
}

/*
==================
Sys_HasConfigureChecksumChanged
==================
*/
bool Sys_HasConfigureChecksumChanged( int checksum )
{
	bool changed;

	Sys_RegisterInfoDvars();

	changed = false;
	if ( sys_configSum->current.integer && sys_configSum->current.integer != checksum )
		changed = Sys_ShouldUpdateForConfigChange();

	if ( !sys_configSum->current.integer || sys_configSum->current.integer != checksum )
		Dvar_SetInt( sys_configSum, checksum );

	return changed;
}

/*
==================
Sys_FindInfo
==================
*/
void Sys_FindInfo( void )
{
	dvar_t *sys_SSE;

	sys_info.cpuGHz = Sys_BenchmarkGHz();
	sys_info.sysMB = Sys_SystemMemoryMB();
	Sys_DetectVideoCard( sys_info.gpuDescription );
	sys_info.SSE = Sys_SupportsSSE();

	sys_SSE = Dvar_RegisterBool( "sys_SSE", false, DVAR_CHANGEABLE_RESET );
	Dvar_SetBool( sys_SSE, sys_info.SSE );

	_controlfp( _PC_24, _MCW_PC );
	_controlfp( _MCW_EM, _MCW_EM );
}

/*
==================
Sys_GetInfo

Hands qcommon a copy of the last-measured hardware info (see Sys_FindInfo),
for Com_SetRecommended/Com_CheckSetRecommended.
==================
*/
void Sys_GetInfo( SysInfo *info )
{
	*info = sys_info;
}

void Sys_ArchiveInfo( int checksum );

/*
==================
Sys_ShouldUpdateForInfoChange
==================
*/
bool Sys_ShouldUpdateForInfoChange( void )
{
	Sys_ArchiveInfo( 0 );
	if ( MessageBoxA( NULL, Win_LocalizeRef( "WIN_COMPUTER_CHANGE_BODY" ), Win_LocalizeRef( "WIN_COMPUTER_CHANGE_TITLE" ), MB_YESNO | MB_ICONINFORMATION ) == IDYES )
		return true;

	return false;
}

/*
==================
Sys_HasInfoChanged
==================
*/
bool Sys_HasInfoChanged( void )
{
	Sys_RegisterInfoDvars();

	if ( sys_cpuGHz->current.decimal > sys_info.cpuGHz * 1.1f
		|| sys_cpuGHz->current.decimal < sys_info.cpuGHz * 0.9f )
	{
		return Sys_ShouldUpdateForInfoChange();
	}

	if ( sys_sysMB->current.integer > sys_info.sysMB + 32
		|| sys_sysMB->current.integer < sys_info.sysMB - 32
		|| strcmp( sys_gpu->current.string, sys_info.gpuDescription ) )
	{
		return Sys_ShouldUpdateForInfoChange();
	}

	return false;
}

/*
==================
Sys_ArchiveInfo
==================
*/
void Sys_ArchiveInfo( int checksum )
{
	Sys_RegisterInfoDvars();
	Dvar_SetFloat( sys_cpuGHz, sys_info.cpuGHz );
	Dvar_SetInt( sys_sysMB, sys_info.sysMB );
	Dvar_SetString( sys_gpu, sys_info.gpuDescription );
	Dvar_SetInt( sys_configSum, checksum );
}

WinVars_t g_wv;

static void Sys_ShutdownEvents( void );

/*
==================
Sys_Quit
==================
*/
void Sys_Quit( void )
{
	timeEndPeriod( 1 );
	IN_Shutdown();
	Key_Shutdown();
	Sys_DestroyConsole();
	Sys_NormalExit();
	Win_ShutdownLocalization();
	RefreshQuitOnErrorCondition();
	Dvar_Shutdown();
	Cmd_Shutdown();
	Con_Shutdown();
	Com_ShutdownEvents();
	Sys_ShutdownEvents();

	exit( 0 );
}

/*
========================================================================

EVENT LOOP

========================================================================
*/

#define MAX_QUED_EVENTS 256
#define MASK_QUED_EVENTS ( MAX_QUED_EVENTS - 1 )

static sysEvent_t eventQue[MAX_QUED_EVENTS];
static int eventHead, eventTail;
static byte sys_packetReceived[MAX_MSGLEN];

char *Sys_ConsoleInput( void );

/*
================
Sys_QueEvent

A time of 0 will get the current time
Ptr should either be null, or point to a block of data that can
be freed by the game later.
================
*/
void Sys_QueEvent( int time, sysEventType_t type, int value, int value2, int ptrLength, void *ptr )
{
	sysEvent_t *ev;

	ev = &eventQue[eventHead & MASK_QUED_EVENTS];

	if ( eventHead - eventTail >= MAX_QUED_EVENTS )
	{
		Com_Printf( "Sys_QueEvent: overflow\n" );
		// we are discarding an event, but don't leak memory
		if ( ev->evPtr )
		{
			Z_Free( ev->evPtr );
		}
		eventTail++;
	}

	eventHead++;

	if ( time == 0 )
	{
		time = Sys_Milliseconds();
	}

	ev->evTime = time;
	ev->evType = type;
	ev->evValue = value;
	ev->evValue2 = value2;
	ev->evPtrLength = ptrLength;
	ev->evPtr = ptr;
}

/*
================
Sys_ShutdownEvents
================
*/
static void Sys_ShutdownEvents( void )
{
	sysEvent_t *ev;

	while ( eventHead > eventTail )
	{
		ev = &eventQue[eventTail++ & MASK_QUED_EVENTS];

		if ( ev->evPtr )
		{
			Z_Free( ev->evPtr );
		}
	}
}

/*
================
Sys_GetEvent
================
*/
sysEvent_t Sys_GetEvent( void )
{
	MSG msg;
	sysEvent_t ev;
	char *s;
	msg_t netmsg;
	netadr_t adr;

	// return if we have data
	if ( eventHead > eventTail )
	{
		eventTail++;
		return eventQue[( eventTail - 1 ) & MASK_QUED_EVENTS];
	}

	// pump the message loop
	while ( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
	{
		if ( !GetMessage( &msg, NULL, 0, 0 ) )
		{
			_controlfp( _PC_24, _MCW_PC );
			_controlfp( _MCW_EM, _MCW_EM );
			Com_Quit_f();
		}

		// save the msg time, because wndprocs don't have access to the timestamp
		g_wv.sysMsgTime = msg.time;

		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	_controlfp( _PC_24, _MCW_PC );
	_controlfp( _MCW_EM, _MCW_EM );

	// check for console commands
	s = Sys_ConsoleInput();
	if ( s )
	{
		char *b;
		int len;

		len = strlen( s ) + 1;
		b = (char *)Z_Malloc( len );
		Q_strncpyz( b, s, len - 1 );
		Sys_QueEvent( 0, SE_CONSOLE, 0, 0, len, b );
	}

	// check for network packets
	MSG_Init( &netmsg, sys_packetReceived, sizeof( sys_packetReceived ) );
	if ( Sys_GetPacket( &adr, &netmsg ) )
	{
		netadr_t *buf;
		int len;

		// copy out to a seperate buffer for qeueing
		len = sizeof( netadr_t ) + netmsg.cursize - netmsg.readcount;
		buf = (netadr_t *)Z_Malloc( len );
		*buf = adr;
		memcpy( buf + 1, &netmsg.data[netmsg.readcount], netmsg.cursize - netmsg.readcount );
		Sys_QueEvent( 0, SE_PACKET, 0, 0, len, buf );
	}

	// return if we have data
	if ( eventHead > eventTail )
	{
		eventTail++;
		return eventQue[( eventTail - 1 ) & MASK_QUED_EVENTS];
	}

	// create an empty event to return
	memset( &ev, 0, sizeof( ev ) );
	ev.evTime = Sys_Milliseconds();

	return ev;
}

/*
=================
Sys_In_Restart_f

Restart the input subsystem
=================
*/
void Sys_In_Restart_f( void )
{
	IN_Shutdown();
	IN_Init();
}

/*
==================
Sys_OutOfMemErrorInternal
==================
*/
void Sys_OutOfMemErrorInternal( const char *filename, int line )
{
	Com_Printf( "Out of memory: filename '%s', line %d\n", filename, line );
	MessageBoxA( NULL, Win_LocalizeRef( "WIN_OUT_OF_MEM_BODY" ), Win_LocalizeRef( "WIN_OUT_OF_MEM_TITLE" ), MB_ICONERROR );
	exit( -1 );
}

/*
==================
Sys_StartProcess
==================
*/
void Sys_StartProcess( const char *exeName, qboolean doexit )
{
	char szPathOrig[MAX_PATH];
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	DWORD error;
	char *msgBuf;

	memset( &si, 0, sizeof( si ) );
	si.cb = sizeof( si );

	GetCurrentDirectoryA( MAX_PATH, szPathOrig );

	if ( !CreateProcessA( NULL, va( "\"%s\\%s\"", szPathOrig, exeName ), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi ) )
	{
		error = GetLastError();
		FormatMessageA( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, error, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), (LPSTR)&msgBuf, 0, NULL );
		Com_Error( ERR_DROP, va( "EXE_ERR_COULDNT_START_PROCESS\x15'%s\\%s'\n%s\n%08x", szPathOrig, exeName, msgBuf, error ) );
	}

	if ( doexit )
		Cbuf_AddText( "quit\n" );
}

// Retail keeps this as a separate call from Sys_Error (0x4627b0); name unknown.
static void Sys_ErrorShutdown( void )
{
	Sys_DestroySplashWindow();
	timeEndPeriod( 1 );
	IN_Shutdown();
	CL_ShutdownHunkUsers();
	CL_ShutdownRef();
}

/*
==================
Sys_Error
==================
*/
void Sys_Error( const char *error, ... )
{
	va_list argptr;
	char string[4096];
	MSG msg;

	com_errorEntered = qtrue;

	va_start( argptr, error );
	vsprintf( string, error, argptr );
	va_end( argptr );

	Sys_ErrorShutdown();
	FixWindowsDesktop();

	Conbuf_AppendText( string );
	Conbuf_AppendText( "\n" );

	Sys_SetErrorText( string );
	MessageBoxA( NULL, string, Win_LocalizeRef( "WIN_ERROR" ), MB_ICONERROR );
	Sys_ShowConsole( 1, qtrue );

	// wait for the user to quit
	while ( GetMessageA( &msg, NULL, 0, 0 ) )
	{
		TranslateMessage( &msg );
		DispatchMessageA( &msg );
	}

	Com_Quit_f();
}

/*
==================
Sys_Print
==================
*/
void Sys_Print( const char *msg )
{
	Conbuf_AppendText( msg );
}

/*
==================
Sys_GetClipboardData
==================
*/
char *Sys_GetClipboardData( void )
{
	char *data;
	char *cliptext;
	HANDLE hClipboardData;

	data = NULL;

	if ( OpenClipboard( NULL ) )
	{
		hClipboardData = GetClipboardData( CF_TEXT );
		if ( hClipboardData )
		{
			cliptext = (char *)GlobalLock( hClipboardData );
			if ( cliptext )
			{
				data = (char *)Z_Malloc( GlobalSize( hClipboardData ) + 1 );
				I_strncpyz( data, cliptext, GlobalSize( hClipboardData ) );
				GlobalUnlock( hClipboardData );

				strtok( data, "\n\r\b" );
			}
		}
		CloseClipboard();
	}

	return data;
}

/*
==================
Sys_LoadingKeepAlive

Keeps power-management messages flowing and the display awake during long loads.
==================
*/
void Sys_LoadingKeepAlive( void )
{
	MSG msg;

	if ( PeekMessageA( &msg, NULL, WM_POWERBROADCAST, WM_POWERBROADCAST, PM_NOREMOVE ) )
	{
		if ( GetMessageA( &msg, NULL, WM_POWERBROADCAST, WM_POWERBROADCAST ) > 0 )
		{
			TranslateMessage( &msg );
			DispatchMessageA( &msg );
		}
	}

	SetThreadExecutionState( ES_DISPLAY_REQUIRED );

	_controlfp( _PC_24, _MCW_PC );
	_controlfp( _MCW_EM, _MCW_EM );
}

/*
=================
Sys_Net_Restart_f
=================
*/
static void Sys_Net_Restart_f( void )
{
	NET_Restart();
}

/*
================
Sys_Init
================
*/
void Sys_Init( void )
{
	OSVERSIONINFOA osversion;

	timeBeginPeriod( 1 );

	Cmd_AddCommand( "in_restart", Sys_In_Restart_f );
	Cmd_AddCommand( "net_restart", Sys_Net_Restart_f );

	osversion.dwOSVersionInfoSize = sizeof( osversion );

	if ( !GetVersionExA( &osversion ) )
		Sys_Error( "Couldn't get OS info" );

	if ( osversion.dwMajorVersion < 4 )
		Sys_Error( "Call of Duty 2 Multiplayer requires Windows version 4 or greater" );

	if ( osversion.dwPlatformId == VER_PLATFORM_WIN32s )
		Sys_Error( "Call of Duty 2 Multiplayer doesn't run on Win32s" );

	legacyHacks->winnt = osversion.dwPlatformId == VER_PLATFORM_WIN32_NT;

	Com_Printf( "Measured CPU speed is %.2lf GHz\n", sys_info.cpuGHz );
	Com_Printf( "System memory is %i MB (capped at 1 GB)\n", sys_info.sysMB );
	Com_Printf( "Video card is \"%s\"\n", sys_info.gpuDescription );

	Com_Printf( "Streaming SIMD Extensions (SSE) %ssupported\n", sys_info.SSE ? "" : "not " );

	Com_Printf( "\n" );

	IN_Init();
}

/*
==================
Sys_NoFreeFilesError
==================
*/
static void Sys_NoFreeFilesError( void )
{
	MessageBoxA( NULL, Win_LocalizeRef( "WIN_DISK_FULL_BODY" ), Win_LocalizeRef( "WIN_DISK_FULL_TITLE" ), MB_ICONERROR );
	exit( -1 );
}

/*
==================
Sys_IsGameProcess
==================
*/
static qboolean Sys_IsGameProcess( DWORD id )
{
	HANDLE process;
	HANDLE snapshot;
	MODULEENTRY32 me;
	char modulePath[MAX_PATH];
	char *moduleName;
	char *i;
	qboolean isGame;

	isGame = qfalse;

	process = OpenProcess( PROCESS_ALL_ACCESS, FALSE, id );
	if ( !process )
		return qfalse;
	CloseHandle( process );

	snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, id );
	if ( snapshot == INVALID_HANDLE_VALUE )
		return qfalse;

	me.dwSize = sizeof( me );

	if ( Module32First( snapshot, &me ) )
	{
		GetModuleFileNameA( NULL, modulePath, MAX_PATH );
		modulePath[MAX_PATH - 1] = 0;
		moduleName = modulePath;

		for ( i = modulePath; *i; i++ )
		{
			if ( *i == '\\' || *i == ':' )
				moduleName = i + 1;
		}

		do
		{
			if ( !stricmp( me.szModule, moduleName ) )
			{
				isGame = qtrue;
				break;
			}
		}
		while ( Module32Next( snapshot, &me ) );
	}

	CloseHandle( snapshot );
	return isGame;
}

/*
==================
Sys_CheckCrashOrRerun

Returns false if another instance is running or the user cancelled.
==================
*/
static qboolean Sys_CheckCrashOrRerun( void )
{
	HANDLE file;
	DWORD procID;
	DWORD id;
	DWORD byteCount;
	int answer;

	if ( !sys_processSemaphoreFile[0] )
		return qtrue;

	procID = GetCurrentProcessId();

	file = CreateFileA( sys_processSemaphoreFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_HIDDEN, NULL );
	if ( file != INVALID_HANDLE_VALUE )
	{
		if ( ReadFile( file, &id, sizeof( id ), &byteCount, NULL ) && byteCount == sizeof( id ) )
		{
			CloseHandle( file );

			if ( procID == id || !Sys_IsGameProcess( id ) )
			{
				answer = MessageBoxA( NULL, Win_LocalizeRef( "WIN_IMPROPER_QUIT_BODY" ), Win_LocalizeRef( "WIN_IMPROPER_QUIT_TITLE" ), MB_YESNOCANCEL | MB_ICONWARNING );
				if ( answer == IDYES )
					Com_ForceSafeMode();
				else if ( answer != IDNO && answer == IDCANCEL )
					return qfalse;
			}
			else
			{
				return qfalse;
			}
		}
		else
		{
			CloseHandle( file );
		}
	}

	file = CreateFileA( sys_processSemaphoreFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN, NULL );
	if ( file == INVALID_HANDLE_VALUE )
		Sys_NoFreeFilesError();

	if ( !WriteFile( file, &procID, sizeof( procID ), &byteCount, NULL ) || byteCount != sizeof( procID ) )
	{
		CloseHandle( file );
		Sys_NoFreeFilesError();
	}

	CloseHandle( file );
	return qtrue;
}

/*
==================
Win_RegisterClass
==================
*/
static void Win_RegisterClass( void )
{
	WNDCLASSEXA wc;

	memset( &wc, 0, sizeof( wc ) );
	wc.cbSize = sizeof( wc );
	wc.lpfnWndProc = MainWndProc;
	wc.hInstance = g_wv.hInstance;
	wc.hIcon = LoadIconA( g_wv.hInstance, MAKEINTRESOURCEA( 1 ) );
	wc.hCursor = LoadCursorA( NULL, IDC_ARROW );
	wc.hbrBackground = CreateSolidBrush( RGB( 0, 0, 0 ) );
	wc.lpszClassName = "CoD2";

	if ( !RegisterClassExA( &wc ) )
		Com_Error( ERR_FATAL, "EXE_ERR_COULDNT_REGISTER_WINDOW" );
}

/*
==================
PrintWorkingDir
==================
*/
static void PrintWorkingDir( void )
{
	char cwd[256];

	_getcwd( cwd, sizeof( cwd ) );
	Com_Printf( "Working directory: %s\n", cwd );
}

/*
==================
WinMain
==================
*/
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	_controlfp( _PC_24, _MCW_PC );
	_controlfp( _MCW_EM, _MCW_EM );

	Sys_InitMainThread();
	Win_InitLocalization();

	if ( strnicmp( lpCmdLine, "allowdupe", 9 ) || lpCmdLine[9] > ' ' )
	{
		Sys_GetSemaphoreFileName();

		if ( !Sys_CheckCrashOrRerun() )
		{
			Win_ShutdownLocalization();
			return 0;
		}
	}

	// should never get a previous instance in Win32
	if ( hPrevInstance )
	{
		Win_ShutdownLocalization();
		return 0;
	}

	Dvar_Init();
	Sys_FindInfo();

	g_wv.hInstance = hInstance;
	I_strncpyz( sys_cmdline, lpCmdLine, sizeof( sys_cmdline ) );

	Sys_CreateConsole( g_wv.hInstance );

	Sys_CreateSplashWindow();
	Sys_ShowSplashWindow();

	Win_RegisterClass();

	SetErrorMode( SEM_FAILCRITICALERRORS );

	// get the initial time base
	Sys_Milliseconds();

	Com_Init( sys_cmdline );

	PrintWorkingDir();

	// hide the early console since we've reached the point where we
	// have a working graphics subsystems
	if ( !com_dedicated->current.integer && !com_viewlog->current.integer )
		Sys_ShowConsole( 0, qfalse );

	SetFocus( g_wv.hWnd );

	// main game loop
	while ( 1 )
	{
		// if not running as a game client, sleep a bit
		if ( g_wv.isMinimized || ( com_dedicated && com_dedicated->current.integer ) )
			Sleep( 5 );

		Com_Frame();
	}

	// never gets here
	return 0;
}
