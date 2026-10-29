/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTTPGEN_H
#define HTTPGEN_H

#include "HTStream.h"
#include "HTReq.h"
extern HTStream * HTTPGen_new  (HTRequest * request, HTStream * target,
				BOOL endHeader, int version);
#endif
