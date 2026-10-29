/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTPARSE_H
#define HTPARSE_H

#include "HTEscape.h"
#define PARSE_ACCESS		16	/* Access scheme, e.g. "HTTP" */
#define PARSE_HOST		 8	/* Host name, e.g. "www.w3.org" */
#define PARSE_PATH		 4	/* URL Path, e.g. "pub/WWW/TheProject.html" */

#define PARSE_VIEW               2      /* Fragment identifier, e.g. "news" */
#define PARSE_FRAGMENT           PARSE_VIEW
#define PARSE_ANCHOR		 PARSE_VIEW

#define PARSE_PUNCTUATION	 1	/* Include delimiters, e.g, "/" and ":" */
#define PARSE_ALL		31
extern char * HTParse  (const char * aName, const char * relatedName,
			int wanted);
extern char * HTRelative (const char * aName, const char *relatedName);
extern BOOL HTURL_isAbsolute (const char * url);
extern char *HTSimplify (char **filename);
extern BOOL HTCleanTelnetString (char * str);
#endif	/* HTPARSE_H */
