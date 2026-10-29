/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTHOST_H
#define HTHOST_H

typedef struct _HTHost HTHost;
#define HOST_HASH_SIZE		HT_M_HASH_SIZE

#include "HTChannl.h"
#include "HTReq.h"
#include "HTEvent.h"
#include "HTProt.h"
#include "HTTimer.h"
extern HTHost * HTHost_new (char * host, u_short u_port);
extern HTHost * HTHost_newWParse(HTRequest * request, char * url, u_short u_port);
extern int HTHost_hash (HTHost * host);
extern HTHost * HTHost_find (char * host);
extern void HTHost_deleteAll (void);
extern BOOL HTHost_isIdle (HTHost * host);
extern char * HTHost_name	(HTHost * host);
extern char * HTHost_class	(HTHost * host);
extern void HTHost_setClass	(HTHost * host, char * s_class);

extern int  HTHost_version	(HTHost * host);
extern void HTHost_setVersion	(HTHost * host, int version);
extern HTMethod HTHost_publicMethods 	(HTHost * me);
extern void HTHost_setPublicMethods 	(HTHost * me, HTMethod methodset);
extern void HTHost_appendPublicMethods	(HTHost * me, HTMethod methodset);
extern char * HTHost_server	(HTHost * host);
extern BOOL HTHost_setServer	(HTHost * host, const char * server);
extern char * HTHost_userAgent 	(HTHost * host);
extern BOOL HTHost_setUserAgent	(HTHost * host, const char * userAgent);
extern char * HTHost_rangeUnits  (HTHost * host);
extern BOOL HTHost_setRangeUnits (HTHost * host, const char * units);
extern BOOL HTHost_isRangeUnitAcceptable (HTHost * host, const char * unit);
extern void HTHost_setContext (HTHost * me, void * context);
extern void * HTHost_context  (HTHost * me);
extern int  HTHost_addNet    (HTHost * host, HTNet * net);
extern BOOL HTHost_deleteNet (HTHost * host, HTNet * net, int status);

extern HTList * HTHost_net   (HTHost * host);
extern int HTHost_connect (HTHost * host, HTNet * net, char * url);

extern int HTHost_accept  (HTHost * host, HTNet * net, char * url);

extern int HTHost_listen  (HTHost * host, HTNet * net, char * url);
extern BOOL HTHost_setCloseNotification (HTHost * host, BOOL mode);
extern BOOL HTHost_closeNotification (HTHost * host);
extern BOOL HTHost_setChannel (HTHost * host, HTChannel * channel);
extern HTChannel * HTHost_channel (HTHost * host);
extern BOOL HTHost_clearChannel (HTHost * host, int status);
extern HTTransportMode HTHost_mode (HTHost * host, BOOL * active);
extern BOOL HTHost_setMode (HTHost * host, HTTransportMode mode);
extern HTNet * HTHost_nextPendingNet (HTHost * host);
extern HTHost * HTHost_nextPendingHost (void);
extern BOOL HTHost_launchPending (HTHost * host);
extern void HTHost_enable_PendingReqLaunch (void);
extern void HTHost_disable_PendingReqLaunch (void);
extern BOOL HTHost_setPersistent (HTHost * host, BOOL persistent,
                                  HTTransportMode mode);
extern BOOL HTHost_isPersistent (HTHost * host);
extern BOOL HTHost_setActiveTimeout (ms_t timeout);
extern ms_t HTHost_activeTimeout (void);
extern time_t HTHost_persistTimeout (void);
extern BOOL HTHost_setPersistTimeout (time_t timeout);
extern void HTHost_setPersistExpires (HTHost * host, time_t expires);
extern time_t HTHost_persistExpires (HTHost * host);
extern void HTHost_setReqsPerConnection (HTHost * host, int reqs);
extern int HTHost_reqsPerConnection (HTHost * host);
extern void HTHost_setReqsMade (HTHost * host, int reqs);
extern int HTHost_reqsMade (HTHost * host);
extern HTNet * HTHost_firstNet     (HTHost * host);
extern HTNet * HTHost_getReadNet  (HTHost * host);
extern HTNet * HTHost_getWriteNet (HTHost * host);
extern HTInputStream * HTHost_getInput (HTHost * host, HTTransport * transport,
				        void * param, int mode);

extern HTOutputStream * HTHost_getOutput (HTHost * host, HTTransport * tp,
					  void * param, int mode);
extern int HTHost_read(HTHost * host, HTNet * net);

extern BOOL HTHost_setConsumed(HTHost * host, size_t bytes);
extern BOOL HTHost_setRemainingRead(HTHost * host, size_t remainaing);
extern size_t HTHost_remainingRead (HTHost * host);
extern BOOL HTHost_setMaxPipelinedRequests (int max);
extern int HTHost_maxPipelinedRequests (void);
extern int HTHost_numberOfOutstandingNetObjects (HTHost * host);
extern int HTHost_numberOfPendingNetObjects (HTHost * host);
extern BOOL HTHost_recoverPipe (HTHost * host);
extern BOOL HTHost_doRecover (HTHost * host);
extern BOOL HTHost_killPipe (HTHost * host);
extern int HTHost_register(HTHost * host, HTNet * net, HTEventType type);
extern int HTHost_unregister(HTHost * host, HTNet * net, HTEventType type);
extern int HTHost_tickleFirstNet(HTHost * host, HTEventType type);

extern SockA * HTHost_getSockAddr(HTHost * host);
extern int HTHost_eventTimeout (void);
extern void HTHost_setEventTimeout (int millis);
extern BOOL HTHost_setWriteDelay (HTHost * host, ms_t delay);
extern ms_t HTHost_writeDelay (HTHost * host);
extern int HTHost_findWriteDelay(HTHost * host, ms_t lastFlushTime, int buffSize);
extern int HTHost_forceFlush(HTHost * host);
extern BOOL HTHost_setDefaultWriteDelay (ms_t delay);
extern ms_t HTHost_defaultWriteDelay (void);
extern BOOL HTHost_setHome (HTHost * host, int home);
extern int HTHost_home (HTHost * host);

extern BOOL HTHost_setRetry (HTHost * host, int retry);
extern int HTHost_retry (HTHost * host);
extern BOOL HTHost_decreaseRetry (HTHost * host);
typedef int HTHost_ActivateRequestCallback (HTRequest * request);
extern void HTHost_setActivateRequestCallback
		(HTHost_ActivateRequestCallback * cbf);
#endif /* HTHOST_H */
