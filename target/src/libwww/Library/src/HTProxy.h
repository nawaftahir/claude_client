/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTPROXY_H
#define HTPROXY_H

#include "HTList.h"
extern BOOL HTProxy_add		(const char * access, const char * proxy);
extern BOOL HTProxy_addRegex (const char * regex,
                              const char * proxy,
                              int regex_flags);
extern BOOL HTProxy_deleteAll	(void);
extern BOOL HTNoProxy_add	(const char * host, const char * access,
				 unsigned port);
extern BOOL HTNoProxy_addRegex (const char * regex, int regex_flags);
extern BOOL HTNoProxy_deleteAll	(void);
extern int  HTProxy_NoProxyIsOnlyProxy (void);
extern void HTProxy_setNoProxyIsOnlyProxy (int value);
extern char * HTProxy_find	(const char * url);
extern BOOL HTGateway_add	(const char * access, const char * gate);
extern BOOL HTGateway_deleteAll	(void);
extern char * HTGateway_find	(const char * url);
extern void HTProxy_getEnvVar	(void);
#endif /* HTPROXY_H */
