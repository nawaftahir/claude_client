/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTATOM_H
#define HTATOM_H

#include "HTList.h"

typedef struct _HTAtom HTAtom;
struct _HTAtom {
	HTAtom *	next;
	char *		name;
}; /* struct _HTAtom */
extern HTAtom * HTAtom_for	(const char * string);
extern HTAtom * HTAtom_caseFor	(const char * string);
#define HTAtom_name(a) ((a) ? (a)->name : NULL)
extern HTList * HTAtom_templateMatches (const char * templ);
extern void HTAtom_deleteAll (void);

#endif
