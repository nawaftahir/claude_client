/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTBTTEE_H
#define HTBTREE_H

#include "HTArray.h"
typedef struct _HTBTree HTBTree;

typedef struct _HTBTree_element HTBTElement;
extern HTBTree * HTBTree_new (HTComparer * comp);
extern void HTBTree_free (HTBTree * tree);
extern void HTBTreeAndObject_free (HTBTree * tree);
extern void HTBTree_add (HTBTree* tree, void * object);
extern void * HTBTree_object (HTBTElement * element);
extern HTBTElement * HTBTree_next (HTBTree * tree, HTBTElement * element);
#endif
