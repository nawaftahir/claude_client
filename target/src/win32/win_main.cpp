#include "../qcommon/qcommon.h"
#include <float.h>
#include "win_local.h"

static char sys_processSemaphoreFile[32];

void Key_Shutdown( void );
void Sys_DestroyConsole( void );
void Win_ShutdownLocalization( void );
void RefreshQuitOnErrorCondition( void );
void Con_Shutdown( void );
void Com_ShutdownEvents( void );

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
========================================================================

RENDERER DLL

========================================================================
*/

struct refexport_t;
struct refimport_t;
typedef refexport_t *(*GetRefAPI_t)( int apiVersion, refimport_t *rimp );

const char *Dvar_EnumToString( const dvar_t *dvar );

static const char *gfx_driverNames[] = { "d3d", NULL };
HMODULE hGfxDll;

/*
==================
Sys_LoadRenderer
==================
*/
GetRefAPI_t Sys_LoadRenderer( void )
{
	const dvar_t *gfx_driver;
	const char *dllName;
	GetRefAPI_t getRefAPI;

	gfx_driver = Dvar_RegisterEnum( "gfx_driver", gfx_driverNames, 0, 0x1020 );
	dllName = va( "gfx_%s_mp_x86_s.dll", Dvar_EnumToString( gfx_driver ) );
	hGfxDll = LoadLibraryA( dllName );
	if ( !hGfxDll )
		Com_Error( ERR_FATAL, va( "Failed to load gfx driver %s.", dllName ) );
	getRefAPI = (GetRefAPI_t)GetProcAddress( hGfxDll, "GetRefAPI" );
	if ( !getRefAPI )
	{
		FreeLibrary( hGfxDll );
		hGfxDll = NULL;
		Com_Error( ERR_FATAL, va( "Failed to find \"GetRefAPI\" in gfx driver %s.", dllName ) );
	}
	return getRefAPI;
}

/*
==================
Sys_UnloadRenderer
==================
*/
void Sys_UnloadRenderer( void )
{
	if ( hGfxDll )
	{
		FreeLibrary( hGfxDll );
		hGfxDll = NULL;
	}
}

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
