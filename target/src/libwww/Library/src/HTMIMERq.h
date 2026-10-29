/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTMIMERQ_H
#define HTMIMERQ_H

#include "HTStream.h"
#include "HTReq.h"
extern HTStream * HTMIMERequest_new    (HTRequest * request, HTStream * target,
					BOOL endHeader);

#endif
