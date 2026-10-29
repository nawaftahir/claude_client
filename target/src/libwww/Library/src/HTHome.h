/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTHOME_H
#define HTHOME_H
#include "WWWLib.h"
#define LOGICAL_DEFAULT 	"WWW_HOME"    /* Defined to be the home page */

#ifndef PERSONAL_DEFAULT
#define PERSONAL_DEFAULT 	"WWW/default.html"	/* in home directory */
#endif

/* If the home page isn't found, use this file: */
#ifndef LAST_RESORT
#define LAST_RESORT		"http://www.w3.org/"
#endif
/* If one telnets to an access point it will look in this file for home page */
#ifndef REMOTE_POINTER
#define REMOTE_POINTER 		 "/etc/www-remote.url"	    /* can't be file */
#endif

/* and if that fails it will use this. */
#ifndef REMOTE_ADDRESS
#define REMOTE_ADDRESS  	"http://www.w3.org/"	    /* can't be file */
#endif

#ifndef LOCAL_DEFAULT_FILE
#define LOCAL_DEFAULT_FILE 	"/usr/local/lib/WWW/default.html"
#endif
extern HTParentAnchor * HTHomeAnchor (void);
extern HTParentAnchor * HTTmpAnchor (HTUserProfile * up);
#define HTFindRelatedName	HTGetCurrentDirectoryURL
extern char *  HTGetCurrentDirectoryURL (void);
extern BOOL HTParseFormInput (HTAssocList * list, const char * str);
extern int HTSetTraceMessageMask (const char * shortnames);
#endif /* HTHOME_H */
