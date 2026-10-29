/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTMULPAR_H
#define HTMULPAR_H

#include "HTStream.h"
#include "HTList.h"
typedef struct _HTMultipart HTMultipart;

typedef struct _HTMultipartClass {

    char * name;

    int (*flush)	(HTMultipart *	me);

    int (*_free)	(HTMultipart *	me);

    int (*abort)	(HTMultipart *	me, HTList * errorlist);

    int (*put_character)(HTMultipart *	me, char ch);

    int (*put_string)	(HTMultipart *	me, const char * str);

    int (*put_block)	(HTMultipart *	me, const char * str, int len);
    int (*begin_part)	(HTMultipart *	me, const char * boundary);

    int (*end_part)	(HTMultipart *	me);

    int (*preamble)	(HTMultipart *	me, const char * pre);

    int (*epilogue)	(HTMultipart *	me, const char * epi);
		
} HTMultipartClass;

#endif
