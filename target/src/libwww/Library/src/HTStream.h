/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTSTREAM_H
#define HTSTREAM_H

#include "HTList.h"

typedef struct _HTStream HTStream;

typedef struct _HTStreamClass {

    char * name;
    int (*flush)	(HTStream * me);
    int (*_free)	(HTStream * me);
    int (*abort)	(HTStream * me, HTList * errorlist);
    int (*put_character)(HTStream * me, char ch);
				
    int (*put_string)	(HTStream * me, const char * str);

    int (*put_block)	(HTStream * me, const char * str, int len);
} HTStreamClass;
extern HTStream * HTBlackHole (void);
extern HTStream * HTErrorStream (void);
#endif /* HTSTREAM_H */
