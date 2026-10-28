// build identification; original file name unknown
#include <stdio.h>

#define BUILD_NUMBER "crc6_v1"
#define BUILD_TIME   "Thu Oct 06 2005 07:13:42PM"

// Mac STABS buf[128]; retail .data ends 128 bytes after this buffer.
char buildInfo[128];

const char *getBuildNumber( void )
{
	sprintf(buildInfo, "%s %s", BUILD_NUMBER, BUILD_TIME);
	return buildInfo;
}
