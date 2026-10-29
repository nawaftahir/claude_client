/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTTCP_H
#define HTTCP_H
#include "HTReq.h"
#include "HTNet.h"
extern int HTDoConnect (HTNet * net);
extern int HTDoAccept (HTNet * listen, HTNet * accept);
extern int HTDoListen (HTNet * net, HTNet * accept, int backlog);
extern int HTDoClose (HTNet * net);
#endif   /* HTTCP_H */
