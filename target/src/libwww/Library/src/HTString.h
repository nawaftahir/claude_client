/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTSTRING_H
#define HTSTRING_H
#define StrAllocCopy(dest, src) HTSACopy (&(dest), src)
#define StrAllocCat(dest, src)  HTSACat  (&(dest), src)

extern char * HTSACopy (char **dest, const char *src);
extern char * HTSACat  (char **dest, const char *src);
extern char * StrAllocMCopy (char ** dest, ...);
extern char * StrAllocMCat (char ** dest, ...);
extern int strcasecomp  (const char *a, const char *b);
extern int strncasecomp (const char *a, const char *b, int n);
extern int tailcomp(const char * s1, const char * s2);
extern int tailcasecomp(const char * s1, const char * s2);
extern char * HTStrMatch	(const char * tmpl, const char * name);
extern char * HTStrCaseMatch	(const char * tmpl, const char * name);
extern char * HTStrCaseStr (char * s1, char * s2);
extern char * HTStrip (char * s);
#endif /* !HTSTRING_H */
