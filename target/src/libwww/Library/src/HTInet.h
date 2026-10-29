/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTINET_H
#define HTINET_H
#include "HTReq.h"
#include "HTHstMan.h"
extern char * HTErrnoString	(int errnum);
extern int HTInetStatus		(int errnum, char * where);
extern unsigned int HTCardinal (int *		pstatus,
				char **		pp,
				unsigned int	max_value);
extern const char * HTInetString (struct sockaddr_in * sin);
extern int HTParseInet (HTHost * host, char * hostname, HTRequest * request);
extern time_t HTGetTimeZoneOffset (void);
extern ms_t HTGetTimeInMillis (void);
extern char * HTGetHostName (void);
#ifndef HT_DEFAULT_LOGIN
#define HT_DEFAULT_LOGIN	"libwww"
#endif

extern char * HTGetMailAddress (void);
extern char * HTGetNewsServer (void);
extern char * HTGetTmpFileName (const char * dir);
#ifdef WWWLIB_SIG
extern void HTSetSignal (void);
#endif

#endif   /* HTINET_H */
