/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTNEWS_H
#define HTNEWS_H
#include "HTProt.h"
#ifndef NEWS_PORT
#define NEWS_PORT		119			       /* See rfc977 */
#endif

#define MAX_NEWS_LINE		4096

extern HTProtCallback HTLoadNews;
extern BOOL HTNews_setMaxArticles (int new_max);
extern int HTNews_maxArticles (void);
#endif /* HTNEWS_H */
