/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTMIME_H
#define HTMIME_H

#include "HTStream.h"
#include "HTFormat.h"
extern void HTMIME_setSaveStream (HTConverter * save_stream);
extern HTConverter * HTMIME_saveStream (void);
extern HTConverter HTMIMEConvert;
extern HTConverter HTMIMEHeader;
extern HTConverter HTMIMEFooter;
extern HTConverter HTMIMEContinue;
extern HTConverter HTMIMEUpgrade;
extern HTConverter HTMIMEPartial;
#ifndef NO_CACHE
extern HTConverter HTCacheCopyHeaders;
#endif
#endif
