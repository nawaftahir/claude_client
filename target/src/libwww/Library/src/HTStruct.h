/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTSTRUCT_H
#define HTSTRUCT_H

#include "HTStream.h"
#include "HTList.h"
typedef struct _HTStructured HTStructured;

typedef struct _HTStructuredClass {

    char * name;

    int (*flush)	(HTStructured * me);

    int (*_free)	(HTStructured *	me);

    int (*abort)	(HTStructured *	me, HTList * errorlist);

    int (*put_character)(HTStructured *	me, char ch);

    int (*put_string)	(HTStructured *	me, const char * str);

    int (*put_block)	(HTStructured * me, const char * str, int len);
    void (*start_element)(HTStructured *me,
			  int		element_number,
			  const BOOL *	attribute_present,
			  const char **	attribute_value);

    void (*end_element)	(HTStructured *	me, int element_number);

    void (*put_entity)	(HTStructured *	me, int entity_number);

    int (*unparsed_begin_element)(HTStructured * me, const char * str, int len);

    int (*unparsed_end_element)(HTStructured * me, const char * str, int len);

    int (*unparsed_entity)(HTStructured * me, const char * str, int len);
		
} HTStructuredClass;

#endif
