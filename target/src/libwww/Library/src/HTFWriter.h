/*
**	(c) COPYRIGHT CERN 1994.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTFWRITE_H
#define HTFWRITE_H

#include "sysdep.h"
#include "HTStream.h"
#include "HTFormat.h"

#ifdef SHORT_NAMES
#define	HTFWriter_new	HTFWnew
#endif
extern HTStream * HTFWriter_new PARAMS((FILE *	fp,
					     BOOL	leave_open));
#ifndef NO_EXTERN_TYPEDEF_FUNC
extern HTConverter HTSaveAndExecute, HTSaveLocally, HTSaveAndCallBack,
       HTThroughLine;
#endif
extern HTStream * HTBlackHole NOPARAMS;
extern char *HTFWriter_filename PARAMS((char * 		path,
					char *		url,
				      	 CONST char * 	suffix,
				      	 unsigned int	limit,
	                                 BOOL           flat));
typedef struct _HTCacheItem {
    HTParentAnchor *	anchor;
    HTFormat		format;		/* May have many formats per anchor */
    char *		filename;
    time_t		load_time;
    time_t		load_delay;
    int			reference_count;
} HTCacheItem;
extern HTList * HTCache;	/* made up of HTCacheItem items */
extern int	HTCacheLimit;
extern void HTCacheClear PARAMS((HTList * cache));
extern void HTCacheDeleteAll NOPARAMS;
#ifndef NO_EXTERN_TYPEDEF_FUNC
extern HTConverter HTCacheWriter;
#endif

#endif
