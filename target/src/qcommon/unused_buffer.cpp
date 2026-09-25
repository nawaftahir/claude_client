#include <string.h>

// Nothing in the game calls this file's functions; original names unknown.

static char unusedBuffer[56];
static int unusedCount;

void UnusedBuffer_Stub(void)
{
}

void UnusedBuffer_Get(char **buffer, int *count)
{
	*buffer = unusedBuffer;
	*count = unusedCount;
}

void UnusedBuffer_Clear(void)
{
	memset(unusedBuffer, 0, sizeof(unusedBuffer));
	unusedCount = 0;
}
