/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTWWWSTR_H
#define HTWWWSTR_H

#include "HTUser.h"
#include "HTAtom.h"
extern char * HTNextField (char** pstr);
extern char * HTNextPair (char ** pstr);
extern char * HTNextParam (char ** pstr);
extern char * HTNextLWSToken (char ** pstr);
extern char * HTNextSegment (char ** pstr);
extern char * HTNextElement (char ** pstr);
extern char * HTNextSExp (char ** exp, char ** param);
typedef enum _HTEOLState {
    EOL_ERR = -1,
    EOL_BEGIN = 0,
    EOL_FCR,
    EOL_FLF,
    EOL_DOT,
    EOL_SCR,
    EOL_SLF,
    /* intermediate states */
    EOL_END,
    EOL_FOLD,
    EOL_LINE
} HTEOLState;
extern const char * HTDateTimeStr (time_t *calendar, BOOL local);
extern BOOL HTDateDirStr (time_t * time, char * str, int len);
extern time_t HTParseTime (const char * str, HTUserProfile * up, BOOL expand);
extern const char * HTMessageIdStr (HTUserProfile * up);
extern BOOL HTMIMEMatch (HTAtom * tmplate, HTAtom * actual);
extern void HTNumToStr (unsigned long n, char *str, int len);
extern char * HTWWWToLocal (const char * url, const char * base,
			    HTUserProfile * up);
extern char * HTLocalToWWW (const char * local, const char * access);
#endif
