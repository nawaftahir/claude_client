/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTLOCAL_H
#define HTLOCAL_H

#include "HTNet.h"
#ifndef NO_UNIX_IO
typedef int HTLocalMode;

#define HT_FB_RDONLY	O_RDONLY
#define HT_FT_RDONLY	HT_FB_RDONLY

#define HT_FB_WRONLY	O_WRONLY|O_CREAT
#define HT_FT_WRONLY	HT_FB_WRONLY

#define HT_FB_RDWR	O_RDWR
#define HT_FT_RDWR	HT_FB_RDWR

#define HT_FB_APPEND	O_APPEND
#define HT_FT_APPEND	HT_FB_APPEND

#else 
typedef const char *HTLocalMode;

#define HT_FB_RDONLY	"rb"
#define HT_FT_RDONLY	"r"

#define HT_FB_WRONLY	"wb"
#define HT_FT_WRONLY	"w"

#define HT_FB_RDWR	"r+b"
#define HT_FT_RDWR	"r+"

#define HT_FB_APPEND	"ab"
#define HT_FT_APPEND	"a"

#endif
extern int HTFileOpen (HTNet * net, char * local, HTLocalMode mode);
extern int HTFileClose (HTNet * net);
#endif   /* HTLOCAL_H */
