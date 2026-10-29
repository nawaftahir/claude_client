/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTIOSTREAM_H
#define HTIOSTREAM_H

typedef struct _HTInputStream HTInputStream;
typedef struct _HTOutputStream HTOutputStream;

#include "HTList.h"
#include "HTStream.h"
#include "HTChannl.h"
typedef struct _HTInputStreamClass {

    char * name;
    int (*flush)	(HTInputStream * me);
    int (*_free)	(HTInputStream * me);
    int (*abort)	(HTInputStream * me, HTList * errorlist);
    int (*read)		(HTInputStream * me);
    int (*close)	(HTInputStream * me);
    int (*consumed)	(HTInputStream * me, size_t bytes);
} HTInputStreamClass;
typedef struct _HTOutputStreamClass {

    char * name;

    int (*flush)	(HTOutputStream * me);

    int (*_free)	(HTOutputStream * me);

    int (*abort)	(HTOutputStream * me, HTList * errorlist);

    int (*put_character)(HTOutputStream * me, char ch);

    int (*put_string)	(HTOutputStream * me, const char * str);

    int (*put_block)	(HTOutputStream * me, const char * str, int len);
    int (*close)	(HTOutputStream * me);
} HTOutputStreamClass;
typedef HTInputStream * HTInput_new	(HTHost *	host,
					 HTChannel *	ch,
					 void *		param,
					 int		mode);
typedef HTOutputStream * HTOutput_new	(HTHost *	host,
					 HTChannel *	ch,
					 void *		param,
					 int		mode);
typedef HTOutputStream * HTOutputConverter_new(
	HTHost *		host,
	HTChannel *		ch,
	void *			param,
	int			mode,
	HTOutputStream *	target);
#endif /* HTIOSTREAM_H */
