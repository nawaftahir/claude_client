/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTNDIR_H
#define HTNDIR_H
#include "HTReq.h"
typedef enum _HTNewsDirKey {
    HT_NDK_NONE		= 0,			/* Unsorted */
    HT_NDK_INDEX  	= 1,			/* Sort by Message Number */
    HT_NDK_DATE   	= 2,			/* Sort by date */
    HT_NDK_SUBJECT	= 3,			/* Sort by Subject */
    HT_NDK_FROM		= 4,			/* Sort by Sender */
    HT_NDK_GROUP	= 5,			/* Sort group listing */
    HT_NDK_REFTHREAD    = 6  /* Sort messages by date and thread Added by MP */
} HTNewsDirKey;
typedef struct _HTNewsDir HTNewsDir;
extern HTNewsDir * HTNewsDir_new (HTRequest * request, const char * title,
				  HTNewsDirKey key, BOOL cache);
typedef struct _HTNewsNode HTNewsNode;
extern HTNewsNode * HTNewsDir_addElement (HTNewsDir * dir, int index,
                                          char * subject,
				          char * from, time_t date,
                                          char * name,
				          int refs, HTList * refNames);
extern HTNewsNode * HTNewsDir_addGroupElement (HTNewsDir * dir, char * group,
                                               BOOL tmplate);
extern BOOL HTNewsDir_belongsToSet (HTNewsDir* dir, char* group);
extern BOOL HTNewsDir_free (HTNewsDir * dir);
#endif /* HTNDIR */
