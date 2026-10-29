/*
**	(c) COPYRIGHT CERN 1994.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTEXTPARSE_H
#define HTEXTPARSE_H

#include "sysdep.h"
#include "HTStream.h"
#include "HTAccess.h"

typedef struct _HTExtParseStruct HTExtParseStruct;
typedef void (*CallClient) PARAMS((struct _HTExtParseStruct *me));
struct _HTExtParseStruct {
	CallClient	call_client;
#if 0
	void            (*call_client)(struct _HTExtParseStruct *eps);
#endif
	int             used;         /* how much of the buffer is being used */
	BOOL            finished;     /* document loaded? */
        int             length;       /* how long the buffer is */
	char *          buffer;       /* buffer to store in until client takes over*/
	char *          content_type;
	HTRequest *	request;      /* the request structure */
};

/*extern HTStream * HTExtParse PARAMS((void (*CallMeArg)()));*/

/* extern void HTExtParse PARAMS((HTExtParseStruct * eps)); */

extern HTStream* HTExtParse PARAMS((
	HTRequest *		request,
	void *			param,
	HTFormat		input_format,
	HTFormat		output_format,
	HTStream *		output_stream));


#endif
