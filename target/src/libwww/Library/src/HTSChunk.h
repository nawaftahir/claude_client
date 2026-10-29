/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTSCHUNK_H
#define HTSCHUNK_H

#include "HTChunk.h"
#include "HTStream.h"

extern HTStream * HTStreamToChunk (HTRequest * 	request,
				   HTChunk **	chunk,
				   int 		max_size);
#endif /* HTSCHUNK_H */
