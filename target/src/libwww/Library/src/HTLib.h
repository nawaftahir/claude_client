/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTLIB_H
#define HTLIB_H

#include "HTUser.h"
extern BOOL HTLibInit (const char * AppName, const char * AppVersion);
extern BOOL HTLibTerminate (void);
extern const char * HTLib_name (void);
extern const char * HTLib_version (void);
extern BOOL HTLib_isInitialized (void);
extern const char * HTLib_appName (void);
extern BOOL HTLib_setAppName (const char * name);
extern const char * HTLib_appVersion (void);
extern BOOL HTLib_setAppVersion (const char * version);
extern BOOL HTLib_secure (void);
extern void HTLib_setSecure (BOOL mode);
extern HTUserProfile * HTLib_userProfile (void);
extern BOOL HTLib_setUserProfile (HTUserProfile * up);
#endif /* HTLIB_H */
