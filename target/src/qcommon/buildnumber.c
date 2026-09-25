// build identification; original file name unknown
#include <stdio.h>

#define BUILD_NUMBER 696
#define BUILD_DATE   "Oct 24 2005"
#define BUILD_TIME   "17:45:05"

// Original name unknown; size from the layout.
char buildInfo[1024];

const char *getBuildNumber( void )
{
	sprintf(buildInfo, "%d %s %s", BUILD_NUMBER, BUILD_DATE, BUILD_TIME);
	return buildInfo;
}
