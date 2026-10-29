/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTHEADER_H
#define HTHEADER_H

#include "HTResponse.h"
#include "HTStream.h"
typedef int HTParserCallback (HTRequest * request, HTResponse * response,
                              char * token, char * value);
extern BOOL HTGenerator_add (HTList * gens, HTPostCallback * callback);
extern BOOL HTGenerator_delete (HTList * gens, HTPostCallback * callback);
extern BOOL HTGenerator_deleteAll (HTList * gens);
#define MIME_HASH_SIZE  HT_L_HASH_SIZE

extern void HTHeader_setMIMEParseSet (HTMIMEParseSet * list);
extern HTMIMEParseSet * HTHeader_MIMEParseSet (void);
extern BOOL HTHeader_addParser (const char * token, BOOL case_sensitive,
				HTParserCallback * callback);
extern BOOL HTHeader_addRegexParser (const char * token, BOOL case_sensitive,
				HTParserCallback * callback);
extern BOOL HTHeader_deleteParser (const char * token);
extern void HTHeader_setGenerator (HTList * list);
extern BOOL HTHeader_addGenerator (HTPostCallback * callback);
extern BOOL HTHeader_deleteGenerator (HTPostCallback * callback);
extern HTList * HTHeader_generator (void);
extern void HTHeader_deleteAll (void);
#endif /* HTHEADER_H */
