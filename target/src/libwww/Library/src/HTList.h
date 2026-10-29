/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTLIST_H
#define HTLIST_H

#include "HTArray.h"

typedef struct _HTList HTList;

struct _HTList {
  void * object;
  HTList * next;
};
extern HTList *	HTList_new	(void);
extern BOOL	HTList_delete	(HTList *me);
extern BOOL HTList_addObject (HTList *me, void *newObject);
extern BOOL HTList_appendObject (HTList * me, void * newObject);
extern HTList * HTList_addList (HTList * me, void * newObject);
extern HTList * HTList_appendList (HTList * me, void * newObject);
extern BOOL	HTList_removeObject		(HTList * me, void * oldObject);
extern BOOL	HTList_quickRemoveElement	(HTList * me, HTList * last);
extern BOOL	HTList_removeObjectAll		(HTList * me, void * oldObject);

extern void *	HTList_removeLastObject		(HTList * me);
extern void *	HTList_removeFirstObject	(HTList * me);
#define 	HTList_isEmpty(me)		(me ? me->next == NULL : YES)
extern int	HTList_count			(HTList *me);
extern int	HTList_indexOf	 (HTList * me, void * object);
extern int	HTList_indexOfElement (HTList * me, HTList * element);
extern void *	HTList_objectAt	 (HTList * me, int position);
extern HTList * HTList_elementOf (HTList * me, void * object, HTList ** pLast);
#define 	HTList_objectOf(me)		(me ? me->object: NULL)
#define		HTList_lastObject(me) \
		((me) && (me)->next ? (me)->next->object : NULL)
extern void * HTList_firstObject  (HTList * me);
#define		HTList_nextObject(me) \
		((me) && ((me) = (me)->next) ? (me)->object : NULL)
extern BOOL HTList_insertionSort(HTList * list, HTComparer * comp);
#define HTList_free(x)  HT_FREE(x)

#endif /* HTLIST_H */
