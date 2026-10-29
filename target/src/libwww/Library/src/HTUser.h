/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTUSER_H
#define HTUSER_H

typedef struct _HTUserProfile HTUserProfile;
extern HTUserProfile * HTUserProfile_new (const char * name, void * context);
extern BOOL HTUserProfile_localize (HTUserProfile * up);
extern BOOL HTUserProfile_delete (HTUserProfile * up);
extern char * HTUserProfile_fqdn (HTUserProfile * up);
extern BOOL HTUserProfile_setFqdn (HTUserProfile * up, const char * fqdn);
extern char * HTUserProfile_email (HTUserProfile * up);
extern BOOL HTUserProfile_setEmail (HTUserProfile * up, const char * email);
extern char * HTUserProfile_news (HTUserProfile * host);
extern BOOL HTUserProfile_setNews (HTUserProfile * host, const char * news);
extern char * HTUserProfile_tmp (HTUserProfile * host);
extern BOOL HTUserProfile_setTmp (HTUserProfile * host, const char * tmp);
extern time_t HTUserProfile_timezone (HTUserProfile * up);
extern BOOL HTUserProfile_setTimezone (HTUserProfile * up, time_t timezone);
extern void * HTUserProfile_context (HTUserProfile * up);
extern BOOL HTUserProfile_setContext (HTUserProfile * up, void * context);
#endif /* HTUser_H */
