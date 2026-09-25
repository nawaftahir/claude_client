#include "../qcommon/qcommon.h"

static char sys_processSemaphoreFile[32];

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
