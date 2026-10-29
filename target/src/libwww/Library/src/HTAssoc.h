/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTASSOC_H
#define HTASSOC_H

#include "HTList.h"

typedef HTList HTAssocList;

typedef struct {
    char * name;
    char * value;
} HTAssoc;
extern HTAssocList * HTAssocList_new (void);
extern BOOL 	     HTAssocList_delete (HTAssocList * alist);
extern BOOL HTAssocList_addObject (
	HTAssocList *	alist,
	const char *	name,
	const char *	value);

extern BOOL HTAssocList_replaceObject (
	HTAssocList *	list,
	const char * 	name,
	const char *	value);
extern BOOL HTAssocList_removeObject (
	HTAssocList * 	list,
	const char * 	name);
extern char * HTAssocList_findObject (
	HTAssocList *	alist,
	const char *	name);
extern char * HTAssocList_findObjectExact (
	HTAssocList *	alist,
	const char *	name);
extern char * HTAssocList_findObjectCaseSensitive (
	HTAssocList *	list,
	const char *	name);
extern char * HTAssocList_findObjectCaseSensitiveExact (
	HTAssocList *	list,
	const char * 	name);
#define HTAssoc_name(me)	((me) ? (me)->name : NULL)
#define HTAssoc_value(me)	((me) ? (me)->value : NULL)
#define	HTAssocList_nextObject(me) \
	((me) && ((me) = (me)->next) ? (me)->object : NULL)
#endif /* not HTASSOC_H */
