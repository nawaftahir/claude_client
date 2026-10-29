/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef SGML_H
#define SGML_H

#include "HTStream.h"
#include "HTStruct.h"
typedef enum _SGMLContent{
    SGML_EMPTY,    	/* no content */
    SGML_LITERAL, 	/* character data. Recognized exact close tag only.
  		    	   Old www server compatibility only! Not SGML */
    SGML_CDATA,    	/* character data. recognize </ only */
    SGML_RCDATA,   	/* replaceable character data. recognize </ and &ref; */
    SGML_MIXED,    	/* elements and parsed character data. recognize all markup */
    SGML_ELEMENT   	/* any data found will be returned as an error*/
} SGMLContent;
typedef struct _HTAttr {
    char *	name;		/* The (constant) name of the attribute */
    				/* Could put type info in here */
} HTAttr;

extern char * HTAttr_name (HTAttr * attr);
typedef struct _HTTag {
    char *      name;			/* The name of the tag */
    HTAttr *	attributes;		/* The list of acceptable attributes */
    int		number_of_attributes;	/* Number of possible attributes */
    SGMLContent contents;		/* End only on end tag @@ */		
} HTTag;

extern char * HTTag_name (HTTag * tag);
extern SGMLContent HTTag_content (HTTag * tag);
extern int HTTag_attributes (HTTag * tag);
extern char * HTTag_attributeName (HTTag * tag, int attribute_number);
#define MAX_ATTRIBUTES 32	     /* Max number of attributes per element */

typedef struct {
    HTTag *		tags;		/* Must be in strcmp order by name */ 
    int			number_of_tags;
    const char **	entity_names;	/* Must be in strcmp order by name */
    int			number_of_entities;
} SGML_dtd;

extern HTTag * SGML_findTag (SGML_dtd * dtd, int element_number);
extern char * SGML_findTagName (SGML_dtd * dtd, int element_number);
extern SGMLContent SGML_findTagContents (SGML_dtd * dtd, int element_number);
extern int SGML_findElementNumber(SGML_dtd *dtd, char *name_element);
extern HTStream * SGML_new (const SGML_dtd * 	dtd,
			    HTStructured *	target);
#endif	/* SGML_H */
