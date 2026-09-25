// spike: stand-in for client code that passes these by pointer (e.g. to the renderer)
#include "../qcommon/qcommon.h"
#include "../server/server.h"
void G_LogPrintf( const char *fmt, ... );
void *spike_addr_taken[] = {
	(void *)va, (void *)Com_sprintf, (void *)Com_DPrintf, (void *)Com_Error, (void *)Com_Printf,
	(void *)FS_Printf, (void *)G_LogPrintf, (void *)SV_SendServerCommand, (void *)Sys_Error,
};
