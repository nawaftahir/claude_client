/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTMUXCHANNEL_H
#define HTMUXCHANNEL_H

typedef struct _HTMuxChannel     HTMuxChannel;
typedef struct _HTMuxSession     HTMuxSession;
typedef struct _HTMuxProtocol    HTMuxProtocol;

typedef unsigned char            HTMuxSessionId;

#include "HTMuxHeader.h"
#include "HTChannl.h"
#include "HTHost.h"

#define INVSID		0		/* Invalid session id */
#define INVPID          0               /* Invalid protocol id */
extern HTMuxChannel * HTMuxChannel_new (HTHost * host);

extern HTMuxChannel * HTMuxChannel_find (HTHost * host);

extern BOOL HTMuxChannel_delete (HTMuxChannel * me);

extern BOOL HTMuxChannel_deleteAll (void);

extern HTNet * HTMuxChannel_net (HTMuxChannel * me);

extern HTMuxSession * HTMuxChannel_findSession (HTMuxChannel * me, HTMuxSessionId sid);

extern HTMuxSession * HTMuxChannel_findSessionFromNet (HTMuxChannel * me,
						       HTNet * net);

extern HTHost * HTMuxChannel_host (HTMuxChannel * muxch);

extern int HTMuxChannel_sendControl (HTMuxChannel * muxch, HTMuxSessionId sid,
				     HTMuxHeader opcode, int value,
				     const void * param, int param_size);
typedef enum _HTMuxClose {
    MUX_S_END_READ     = 0x1,
    MUX_S_END_WRITE    = 0x2,
    MUX_S_END          = 0x3
} HTMuxClose;

extern HTMuxSessionId HTMuxSession_accept (HTMuxChannel * muxch, HTNet * net,
					   HTProtocolId pid);

extern HTMuxSessionId HTMuxSession_connect (HTMuxChannel * muxch, HTNet * net,
					    HTProtocolId pid);

extern int HTMuxSession_close (HTMuxChannel * muxch, HTMuxSessionId sid);

extern HTMuxSession * HTMuxSession_register (HTMuxChannel * muxch,
				             HTMuxSessionId sid,
                                             HTProtocolId pid);

extern HTMuxSessionId HTMuxSession_id (HTMuxSession * session);

extern HTProtocolId HTMuxSession_pid (HTMuxSession * session);

extern HTNet * HTMuxSession_net (HTMuxSession * session);

extern BOOL HTMuxSession_setClose (HTMuxChannel * muxch,
				   HTMuxSession * session, HTMuxClose close);

extern int  HTMuxSession_credit (HTMuxSession * session);

extern BOOL HTMuxSession_setCredit (HTMuxChannel * muxch,
				    HTMuxSessionId sid, int credit);

extern int  HTMuxSession_fragment (HTMuxSession * session);

extern BOOL HTMuxSession_setFragment (HTMuxChannel * muxch,
				      HTMuxSessionId sid, int fragment);

extern int HTMuxSession_disposeData (HTMuxSession * me,
				     const char * buf, int len);
extern BOOL HTMuxProtocol_add (HTMuxChannel * muxch,
			       HTProtocolId pid, const char * protocol);

extern BOOL HTMuxProtocol_delete (HTMuxChannel * muxch, HTProtocolId pid);
#endif
