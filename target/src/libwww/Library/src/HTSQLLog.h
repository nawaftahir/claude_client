/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTSQLLOG_H
#define HTSQLLOG_H

#include "HTReq.h"
typedef struct _HTSQLLog HTSQLLog;

typedef enum _HTSQLLogFlags {
    HTSQLLOG_CLEAR_URIS_TABLE 		= 0x1,
    HTSQLLOG_CLEAR_LINKS_TABLE		= 0x2,
    HTSQLLOG_CLEAR_REQUESTS_TABLE	= 0x4,
    HTSQLLOG_CLEAR_RESOURCES_TABLE	= 0x8,
    HTSQLLOG_DROP_URIS_TABLE 		= 0x10,
    HTSQLLOG_DROP_LINKS_TABLE		= 0x20,
    HTSQLLOG_DROP_REQUESTS_TABLE	= 0x40,
    HTSQLLOG_DROP_RESOURCES_TABLE	= 0x80
} HTSQLLogFlags; 

extern HTSQLLog * HTSQLLog_open (const char * 	host,
				 const char * 	user,
				 const char * 	pw,
				 const char * 	db,
				 HTSQLLogFlags 	flags);
extern BOOL HTSQLLog_close (HTSQLLog * me);
extern BOOL HTSQLLog_addEntry (HTSQLLog * me, HTRequest * request, int status);
extern BOOL HTSQLLog_addLinkRelationship (HTSQLLog * me,
					  const char * src_uri,
					  const char * dst_uri,
					  const char * link_type,
                                          const char * comment);
extern BOOL HTSQLLog_makeRelativeTo (HTSQLLog * me, const char * relative);
extern int HTSQLLog_accessCount (HTSQLLog * me);
#endif
