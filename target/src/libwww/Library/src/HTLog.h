/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTLIBLOG_H
#define HTLIBLOG_H

#include "HTReq.h"
typedef struct _HTLog HTLog;

extern HTLog * HTLog_open (const char * filename, BOOL local, BOOL append);
extern BOOL HTLog_close (HTLog * log);
extern int HTLog_accessCount (HTLog * log);
extern BOOL HTLog_addCLF (HTLog * log, HTRequest * request, int status);
extern BOOL HTLog_addReferer (HTLog * log, HTRequest * request, int status);
extern BOOL HTLog_addLine (HTLog * log, const char * line);
extern BOOL HTLog_addText (HTLog * log, const char * fmt, ...);
#endif
