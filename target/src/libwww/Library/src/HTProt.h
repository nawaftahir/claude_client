/*
**        (c) COPYRIGHT MIT 1995.
**        Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTPROT_H
#define HTPROT_H

typedef struct _HTProtocol HTProtocol;
typedef u_short HTProtocolId;

#include "HTReq.h"
#include "HTAnchor.h"
#include "HTEvent.h"
#include "HTTrans.h"
typedef int HTProtCallback (SOCKET, HTRequest *);

extern BOOL HTProtocol_add (const char *               name,
                            const char *        transport,
                            HTProtocolId        port,
                            BOOL                preemptive,
                            HTProtCallback *        client,
                            HTProtCallback *        server);
extern BOOL HTProtocol_delete (const char * name);
extern BOOL HTProtocol_deleteAll (void);
extern HTProtocol * HTProtocol_find (HTRequest * request, const char * access);
extern HTProtCallback * HTProtocol_client (HTProtocol * protocol);
extern HTProtCallback * HTProtocol_server (HTProtocol * protocol);
extern HTProtocolId HTProtocol_id (HTProtocol * protocol);
extern const char * HTProtocol_name (HTProtocol * protocol);
extern BOOL HTProtocol_preemptive (HTProtocol * protocol);
extern BOOL HTProtocol_setTransport (HTProtocol * protoccol,
                                     const char * transport);
extern const char * HTProtocol_transport (HTProtocol * protocol);
#endif /* HTPROT_H */
