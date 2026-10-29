/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTNET_H
#define HTNET_H
typedef struct _HTNet HTNet;

#include "HTEvent.h"
#include "HTReq.h"
#include "HTResponse.h"
#include "HTTrans.h"
#include "HTHost.h"
#include "HTProt.h"
#include "HTChannl.h"
#include "HTDNS.h"
typedef enum _HTFilterOrder {
    HT_FILTER_FIRST	= 0x0,		/*     0 */
    HT_FILTER_EARLY	= 0x3FFF,	/* 16383 */
    HT_FILTER_MIDDLE	= 0x7FFF,	/* 32767 */
    HT_FILTER_LATE	= 0xBFFE,	/* 49150 */
    HT_FILTER_LAST	= 0xFFFF	/* 65535 */
} HTFilterOrder;
typedef int HTNetBefore (HTRequest * request, void * param, int mode);
extern BOOL HTNetCall_addBefore (HTList * list, HTNetBefore * before,
				 const char * tmplate, void * param,
                                 HTFilterOrder order);
extern BOOL HTNetCall_deleteBefore (HTList * list, HTNetBefore * before);
extern BOOL HTNetCall_deleteBeforeAll (HTList * list);
extern int HTNetCall_executeBefore (HTList * list, HTRequest * request);
typedef int HTNetAfter (HTRequest * request, HTResponse * response,
                        void * param, int status);
extern BOOL HTNetCall_addAfter (HTList * list, HTNetAfter * after,
				const char * tmplate, void * param,
				int status, HTFilterOrder order);
extern BOOL HTNetCall_deleteAfter (HTList * list, HTNetAfter * after);
extern BOOL HTNetCall_deleteAfterStatus (HTList * list, int status);
extern BOOL HTNetCall_deleteAfterAll (HTList * list);
extern int HTNetCall_executeAfter (HTList * list, HTRequest * request,
				   int status);
extern BOOL HTNet_setBefore (HTList * list);

extern HTList * HTNet_before (void);

extern BOOL HTNet_addBefore (HTNetBefore * before, const char * tmplate,
			     void * param, HTFilterOrder order);

extern BOOL HTNet_deleteBefore (HTNetBefore * before);
extern int HTNet_executeBeforeAll (HTRequest * request);
extern BOOL HTNet_setAfter (HTList * list);

extern HTList * HTNet_after (void);

extern BOOL HTNet_addAfter (HTNetAfter * after, const char * tmplate,
			    void * param, int status,
                            HTFilterOrder order);

extern BOOL HTNet_deleteAfter (HTNetAfter * after);

extern BOOL HTNet_deleteAfterStatus (int status);
extern int HTNet_executeAfterAll (HTRequest * request, int status);
extern BOOL HTNet_setMaxSocket (int newmax);
extern int  HTNet_maxSocket (void);
extern void HTNet_increaseSocket (void);
extern void HTNet_decreaseSocket (void);

extern int HTNet_availableSockets (void);
extern void HTNet_increasePersistentSocket (void);
extern void HTNet_decreasePersistentSocket (void);

extern int HTNet_availablePersistentSockets (void);
extern BOOL HTNet_isIdle (void);
extern HTList *HTNet_activeQueue (void);
extern BOOL HTNet_idle (void);
extern BOOL HTNet_idle (void);
extern BOOL HTNet_isEmpty (void);
extern int HTNet_count (void);
extern HTList *HTNet_pendingQueue (void);
extern BOOL HTNet_newClient (HTRequest * request);
extern BOOL HTNet_newServer (HTRequest * request);
extern HTNet * HTNet_new (HTHost * host);
extern HTNet * HTNet_dup (HTNet * src);
extern BOOL HTNet_deleteDup (HTNet * dup);
extern BOOL HTNet_start (HTNet * net);
extern BOOL HTNet_execute (HTNet * net, HTEventType type);

extern HTEvent * HTNet_event (HTNet * net);
extern BOOL HTNet_setEventParam (HTNet * net, void * eventParam);
extern void * HTNet_eventParam (HTNet * net);
extern BOOL HTNet_setEventCallback(HTNet * net, HTEventCallback * cbf);
extern HTEventCallback * HTNet_eventCallback(HTNet * net);
extern BOOL HTNet_delete (HTNet * me, int status);
extern BOOL HTNet_deleteAll (void);
extern BOOL HTNet_wait (HTNet *net);
extern HTPriority HTNet_priority (HTNet * net);
extern BOOL HTNet_setPriority (HTNet * net, HTPriority priority);
extern BOOL HTNet_persistent (HTNet * net);
extern BOOL HTNet_setPersistent (HTNet *           net,
                                 BOOL              persistent,
                                 HTTransportMode   mode);
extern BOOL HTNet_killPipe (HTNet * net);
extern BOOL HTNet_kill (HTNet * me);
extern BOOL HTNet_killAll (void);
#if 0
extern HTInputStream * HTNet_getInput (HTNet * net, HTStream * target,
				       void * param, int mode);
#endif
extern HTOutputStream * HTNet_getOutput (HTNet * me, void * param, int mode);
extern BOOL HTNet_setContext (HTNet * net, void * context);
extern void * HTNet_context (HTNet * net);
extern BOOL HTNet_setSocket (HTNet * net, SOCKET sockfd);
extern SOCKET HTNet_socket (HTNet * net);
extern BOOL HTNet_preemptive (HTNet * net);
extern BOOL HTNet_setRequest (HTNet * net, HTRequest * request);
extern HTRequest * HTNet_request (HTNet * net);
extern BOOL HTNet_setProtocol (HTNet * net, HTProtocol * protocol);
extern HTProtocol * HTNet_protocol (HTNet * net);
extern BOOL HTNet_setTransport (HTNet * net, HTTransport * tp);
extern HTTransport * HTNet_transport (HTNet * net);
extern BOOL HTNet_setChannel (HTNet * net, HTChannel * channel);
extern HTChannel * HTNet_channel (HTNet * net);
extern BOOL HTNet_setHost (HTNet * net, HTHost * host);
extern HTHost * HTNet_host (HTNet * net);
extern BOOL HTNet_setDns (HTNet * net, HTdns * dns);
extern HTdns * HTNet_dns (HTNet * net);
extern HTStream * HTNet_readStream(HTNet * net);
extern BOOL HTNet_setReadStream (HTNet * net, HTStream * stream);
extern BOOL HTNet_setRawBytesCount (HTNet * net, BOOL mode);
extern BOOL HTNet_rawBytesCount (HTNet * net);
#endif /* HTNET_H */
