/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTFWRITE_H
#define HTFWRITE_H

#include "HTStream.h"
#include "HTReq.h"
extern HTStream * HTFWriter_new	(
        HTRequest * request,
	FILE * fp,
	BOOL leave_open);
#endif
