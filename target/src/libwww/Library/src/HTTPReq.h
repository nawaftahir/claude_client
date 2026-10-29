/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTTPREQ_H
#define HTTPREQ_H

#include "HTStream.h"
#include "HTReq.h"
extern HTStream * HTTPRequest_new (HTRequest * request, HTStream * target,
				   BOOL endHeader, int version);
#endif
