/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTICONS_H
#define HTICONS_H

#include "WWWLib.h"
typedef struct _HTIconNode HTIconNode;
extern BOOL HTIcon_add (const char * url, const char * prefix,
				char * alt, char * type_templ);
extern BOOL HTIcon_addUnknown (const char * url, const char * prefix,
				char * alt);
extern BOOL HTIcon_addBlank (const char * url, const char * prefix,
				char * alt);
extern BOOL HTIcon_addParent (const char * url, const char * prefix,
				char * alt);
extern BOOL HTIcon_addDir (const char * url, const char * prefix,
				char * alt);
typedef  enum _HTFileMode {
    HT_IS_FILE,				/* Normal file */
    HT_IS_DIR,				/* Directory */
    HT_IS_BLANK,			/* Blank Icon */
    HT_IS_PARENT			/* Parent Directory */
} HTFileMode;


extern HTIconNode * HTIcon_find (HTFileMode	mode,
				 HTFormat	content_type,
				 HTEncoding	content_encoding);
extern char * HTIcon_url (HTIconNode * node);
extern char * HTIcon_alternative (HTIconNode * node, BOOL brackets);
extern void HTIcon_deleteAll (void);
#endif /* HTICONS */
