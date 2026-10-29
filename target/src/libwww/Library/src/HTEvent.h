/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTEVENT_H
#define HTEVENT_H
#include "wwwsys.h"
#ifdef IN_EVENT
typedef struct _HTTimer HTTimer;
#endif

typedef enum _HTPriority {
    HT_PRIORITY_INV = -1,
    HT_PRIORITY_OFF = 0,
    HT_PRIORITY_MIN = 1,
    HT_PRIORITY_MAX = 20
} HTPriority; 

#define HTEVENT_INDEX 0x10
typedef enum {
#ifdef WWW_WIN_ASYNC
    HTEvent_READ    = (0x001 | 0 << HTEVENT_INDEX),
    HTEvent_WRITE   = (0x002 | 1 << HTEVENT_INDEX),
    HTEvent_OOB     = (0x004 | 2 << HTEVENT_INDEX),
    HTEvent_ACCEPT  = (0x008 | 3 << HTEVENT_INDEX),
    HTEvent_CONNECT = (0x010 | 4 << HTEVENT_INDEX),
    HTEvent_CLOSE   = (0x020 | 5 << HTEVENT_INDEX),
    HTEvent_TYPES   = 6,	/* winsock has seperate events for all of these */
#define HTEVENT_TYPES	6 /* use in constructing the fake event below */
#else /* WWW_WIN_ASYNC */
    HTEvent_READ    = (0x001 | 0 << HTEVENT_INDEX),
    HTEvent_ACCEPT  = (0x002 | 0 << HTEVENT_INDEX),
    HTEvent_CLOSE   = (0x004 | 0 << HTEVENT_INDEX),
    HTEvent_WRITE   = (0x008 | 1 << HTEVENT_INDEX),
    HTEvent_CONNECT = (0x010 | 1 << HTEVENT_INDEX),
    HTEvent_OOB     = (0x020 | 2 << HTEVENT_INDEX),
    HTEvent_TYPES   = 3,	/* only READ, WRITE, and OOB are real types */
#define HTEVENT_TYPES	3 /* use in constructing the fake event below */
#endif /* !WWW_WIN_ASYNC */
    /*
    **	fake events - these don't correspond to event manager events, but they
    **	are usefull for communicating with the protocol modules
    */
    HTEvent_TIMEOUT = (0x040 | HTEVENT_TYPES << HTEVENT_INDEX),
    HTEvent_BEGIN   = (0x000 | HTEVENT_TYPES << HTEVENT_INDEX),
    HTEvent_END     = (0x080 | HTEVENT_TYPES << HTEVENT_INDEX),
    HTEvent_FLUSH   = (0x100 | HTEVENT_TYPES << HTEVENT_INDEX),
    HTEvent_RESET   = (0x200 | HTEVENT_TYPES << HTEVENT_INDEX),
    HTEvent_ALL     = 0xFFFF
} HTEventType;

#define HTEvent_BITS(type) (type & 0xFFFF)
#define HTEvent_INDEX(type) (type >> HTEVENT_INDEX)

#define HT_EVENT_INITIALIZER \
    {HTEvent_READ, "HTEvent_READ"}, \
    {HTEvent_ACCEPT, "HTEvent_ACCEPT"}, \
    {HTEvent_CLOSE, "HTEvent_CLOSE"}, \
    {HTEvent_WRITE, "HTEvent_WRITE"}, \
    {HTEvent_CONNECT, "HTEvent_CONNECT"}, \
    {HTEvent_OOB, "HTEvent_OOB"}, \
    {HTEvent_TIMEOUT, "HTEvent_TIMEOUT"}, \
    {HTEvent_BEGIN, "HTEvent_BEGIN"}, \
    {HTEvent_END, "HTEvent_END"}, \
    {HTEvent_FLUSH, "HTEvent_FLUSH"}, \
    {HTEvent_RESET, "HTEvent_RESET"}

extern char * HTEvent_type2str(HTEventType type);
typedef int HTEventCallback (SOCKET, void *, HTEventType);
typedef struct _HTEvent HTEvent;

/* Avoid circular include for HTReq->HTNet->HTHost: HTEvent blah */
#include "HTReq.h"
extern int HTEvent_register	(SOCKET, HTEventType, HTEvent *);
extern int HTEvent_unregister	(SOCKET, HTEventType);
typedef int HTEvent_registerCallback(SOCKET, HTEventType, HTEvent *);
typedef int HTEvent_unregisterCallback(SOCKET, HTEventType);

extern void HTEvent_setRegisterCallback(HTEvent_registerCallback *);
extern void HTEvent_setUnregisterCallback(HTEvent_unregisterCallback *);
extern BOOL HTEvent_isCallbacksRegistered(void);
extern HTEvent * HTEvent_new (HTEventCallback * cbf, void * context,
			      HTPriority pritority, int timeoutInMillis);
extern BOOL HTEvent_delete (HTEvent * event);
extern BOOL HTEvent_setParam(HTEvent * event, void * param);
extern BOOL HTEvent_setPriority(HTEvent * event, HTPriority priority);
extern BOOL HTEvent_setTimeout(HTEvent * event, int timeoutInMillis);
extern BOOL HTEvent_setCallback(HTEvent * event, HTEventCallback * cbf);
struct _HTEvent {
    HTPriority		priority;	 /* Priority of this request (event) */
    int                 millis;              /* Timeout in ms for this event */
#ifdef IN_EVENT
    HTTimer *		timer;
#endif
    HTEventCallback *	cbf;			   /* Protocol state machine */
    void *		param;		       /* HTEvent_register parameter */
    HTRequest *		request;
};
#endif /* HTEVENT_H */
