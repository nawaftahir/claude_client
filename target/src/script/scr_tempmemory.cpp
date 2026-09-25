#include "../qcommon/qcommon.h"
#include "script_public.h"

int currentPos;

/*
==============
TempMemoryReset
==============
*/
void TempMemoryReset()
{
	currentPos = 0;
}

/*
==============
TempMalloc
==============
*/
char *TempMalloc( int len )
{
	char *buf;
	int newPos;

	newPos = currentPos + len;
	buf = (char *)Hunk_ReallocateTempMemoryInternal( newPos ) + currentPos;
	currentPos = newPos;
	return buf;
}

/*
==============
TempMallocAlign
==============
*/
char *TempMallocAlign( int len )
{
	return TempMalloc( len );
}

/*
==============
TempMallocAlignStrict
==============
*/
char *TempMallocAlignStrict( int len )
{
	return TempMalloc( len );
}

/*
==============
TempMemorySetPos
==============
*/
void TempMemorySetPos( char *pos )
{
	char *end;

	end = TempMalloc( 0 );
	currentPos -= end - pos;
	Hunk_ReallocateTempMemoryInternal( currentPos );
}
