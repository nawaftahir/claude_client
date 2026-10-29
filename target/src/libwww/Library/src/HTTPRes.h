/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTTPRES_H
#define HTTPRES_H

#include "HTStream.h"
#include "HTReq.h"
extern HTStream * HTTPResponse_new (HTRequest *	request, HTStream * target,
				    BOOL endHeader, int version);
#endif
