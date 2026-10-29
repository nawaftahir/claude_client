/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTTIMER_H
#define HTTIMER_H

#include "wwwsys.h"
#include "HTReq.h"

typedef struct _HTTimer HTTimer;

typedef int HTTimerCallback (HTTimer *, void *, HTEventType type);
extern HTTimer * HTTimer_new (HTTimer *, HTTimerCallback *, 
			      void *, ms_t millis,
                              BOOL relative, BOOL repetitive);
extern BOOL HTTimer_delete (HTTimer * timer);
extern BOOL HTTimer_deleteAll (void);
extern BOOL HTTimer_expireAll (void);
extern int HTTimer_dispatch (HTTimer * timer);
extern int HTTimer_next (ms_t * pSoonest);
extern BOOL HTTimer_refresh(HTTimer * timer, ms_t now);
extern ms_t HTTimer_expiresAbsolute (HTTimer * timer);
#define HTTimer_getTime(t)	HTTimer_expiresRelative(t)
extern ms_t HTTimer_expiresRelative (HTTimer * timer);
extern BOOL HTTimer_hasTimerExpired (HTTimer * timer);
extern HTTimerCallback * HTTimer_callback (HTTimer * timer);
extern BOOL HTTimer_isRelative (HTTimer * timer);
typedef BOOL HTTimerSetCallback (HTTimer * timer);

extern BOOL HTTimer_registerSetTimerCallback (HTTimerSetCallback * cbf);
extern BOOL HTTimer_registerDeleteTimerCallback (HTTimerSetCallback * cbf);
#endif /* HTTIMER_H */
