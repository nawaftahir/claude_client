/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTCHANNL_H
#define HTCHANNL_H

typedef struct _HTChannel HTChannel;

#include "HTHost.h"
#include "HTIOStream.h"
extern HTChannel * HTChannel_new (SOCKET sockfd, FILE * fp, BOOL active);
extern BOOL HTChannel_delete (HTChannel * channel, int status);
extern BOOL HTChannel_deleteAll (void);
extern BOOL HTChannel_safeDeleteAll (void);
extern HTChannel * HTChannel_find (SOCKET sockfd);
extern SOCKET HTChannel_socket	(HTChannel * channel);
extern BOOL HTChannel_setSocket	(HTChannel * channel, SOCKET socket);

extern FILE * HTChannel_file	(HTChannel * channel);
extern BOOL HTChannel_setFile   (HTChannel * channel, FILE * fp);
extern BOOL HTChannel_setHost (HTChannel * ch, HTHost * host);
extern HTHost * HTChannel_host (HTChannel * ch);
extern void HTChannel_upSemaphore   (HTChannel * channel);
extern void HTChannel_downSemaphore (HTChannel * channel);
extern void HTChannel_setSemaphore  (HTChannel * channel, int semaphore);
extern BOOL HTChannel_setInput (HTChannel * ch, HTInputStream * input);
extern HTInputStream * HTChannel_input (HTChannel * ch);
extern BOOL HTChannel_deleteInput (HTChannel * channel, int status);

extern BOOL HTChannel_setOutput (HTChannel * ch, HTOutputStream * output);
extern HTOutputStream * HTChannel_output (HTChannel * ch);
extern BOOL HTChannel_deleteOutput (HTChannel * channel, int status);

extern HTInputStream * HTChannel_getChannelIStream (HTChannel * ch);
extern HTOutputStream * HTChannel_getChannelOStream (HTChannel * ch);
#endif /* HTCHANNL */
