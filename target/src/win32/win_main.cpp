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

	while ( eventTail < eventHead )
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
	ev.evType = SE_NONE;
	ev.evValue = 0;
	ev.evValue2 = 0;
	ev.evPtrLength = 0;
	ev.evPtr = NULL;
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
