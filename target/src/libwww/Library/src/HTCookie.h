/*
**	(c) COPYRIGHT MIT 1999.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTCOOKIE_H
#define HTCOOKIE_H
#include "WWWLib.h"
extern BOOL HTCookie_init (void);
extern BOOL HTCookie_terminate (void);
typedef struct _HTCookie HTCookie;
extern BOOL HTCookie_setName (HTCookie * me, const char * name);
extern char * HTCookie_name (HTCookie * me);
extern BOOL HTCookie_setValue (HTCookie * me, const char * value);
extern char * HTCookie_value (HTCookie * me);
extern BOOL HTCookie_setDomain (HTCookie * me, const char * domain);
extern char * HTCookie_domain (HTCookie * me);
extern BOOL HTCookie_setPath (HTCookie * me, const char * path);
extern char * HTCookie_path (HTCookie * me);
extern time_t HTCookie_setExpiration (HTCookie * me, time_t expiration);
extern time_t HTCookie_expiration (HTCookie * me);
extern time_t HTCookie_setSecure (HTCookie * me, BOOL secure);
extern BOOL HTCookie_isSecure (HTCookie * me);
typedef BOOL HTCookieSetCallback (HTRequest * request, HTCookie * cookie, void * param);
typedef HTAssocList * HTCookieFindCallback (HTRequest * request, void * param);

extern BOOL HTCookie_setCallbacks (HTCookieSetCallback *	setCookie,
				   void * 			setCookieContext,
				   HTCookieFindCallback *	findCookie,
				   void * 			findCookieContext);
extern BOOL HTCookie_deleteCallbacks (void);
typedef enum _HTCookieMode {
    HT_COOKIE_ACCEPT          = 0x1,  /* Accept cookies */
    HT_COOKIE_SEND            = 0x2,  /* Send cookies when fit */
    HT_COOKIE_SAME_HOST       = 0x4,  /* Don't accept cookies for other hosts */
    HT_COOKIE_SAME_DOMAIN     = 0x8,  /* Don't accept cookies for other domains */
    HT_COOKIE_PROMPT          = 0x10  /* Prompt before accepting cookies */
} HTCookieMode;

extern BOOL HTCookie_setCookieMode (HTCookieMode mode);
extern HTCookieMode HTCookie_cookieMode (void);
#endif /* HTCOOKIE_H */
