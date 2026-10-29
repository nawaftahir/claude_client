/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTXPARSE_H
#define HTXPARSE_H

#include "HTStream.h"
#include "HTReader.h"
#include "HTReq.h"

typedef struct _HTXParseStruct HTXParseStruct;

typedef void CallClient (HTXParseStruct * me);

struct _HTXParseStruct {
	CallClient	*call_client;
	int             used;         /* how much of the buffer is being used*/
	BOOL            finished;     /* document loaded? */
        int             length;       /* how long the buffer is */
	char *          buffer;       /* storage in until client takes over */
	char *          content_type;
	HTRequest *	request;      /* the request structure */
};

extern HTConverter HTXParse;

#endif
