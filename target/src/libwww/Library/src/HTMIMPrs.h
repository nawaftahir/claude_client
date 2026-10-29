/*
**	(c) COPYRIGHT MIT 1996.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTMIMPRS_H
#define HTMIMPRS_H

#include "HTHeader.h"
typedef struct _HTMIMEParseEl HTMIMEParseEl;

struct _HTMIMEParseSet {
    int size;
    HTMIMEParseEl ** parsers;
    HTMIMEParseEl * regexParsers;
};

#define MIMEParseSet_NULL {0, NULL, NULL}
extern HTMIMEParseSet * HTMIMEParseSet_new(int hashSize);
extern int HTMIMEParseSet_deleteAll (HTMIMEParseSet * me);
extern HTMIMEParseEl * HTMIMEParseSet_add (HTMIMEParseSet * me, 
					   const char * token, 
					   BOOL caseSensitive, 
					   HTParserCallback * callback);
extern HTMIMEParseEl * HTMIMEParseSet_addRegex (HTMIMEParseSet * me, 
						const char * token, 
						BOOL caseSensitive, 
						HTParserCallback * callback);
extern int HTMIMEParseSet_delete (HTMIMEParseSet * me, const char * token);
extern int HTMIMEParseSet_dispatch (HTMIMEParseSet * me, HTRequest * request, 
				    char * token, char * value, BOOL * pFound);
#endif /* HTMIMPRS_H */
