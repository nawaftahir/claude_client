/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTTP_H
#define HTTP_H

#include "HTProt.h"
#include "HTStream.h"
typedef enum _HTTPConnectionMode { 
    HTTP_11_PIPELINING     = 0x1,
    HTTP_11_NO_PIPELINING  = 0x2, 
    HTTP_11_MUX            = 0x4,
    HTTP_FORCE_10          = 0x8
} HTTPConnectionMode; 

extern void HTTP_setConnectionMode (HTTPConnectionMode mode);
extern HTTPConnectionMode HTTP_connectionMode (void);
extern BOOL HTTP_setBodyWriteDelay (ms_t first_try, ms_t second_try);
extern void HTTP_bodyWriteDelay (ms_t * first_try, ms_t * second_try);
extern HTProtCallback HTLoadHTTP;
extern HTConverter HTTPStatus_new;

#endif /* HTTP_H */
