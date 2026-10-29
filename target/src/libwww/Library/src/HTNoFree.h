/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef _HTNOFREE_H
#define _HTNOFREE_H

#include "HTStream.h"

extern HTStream * HTNoFreeStream_new (HTStream * target);

extern int HTNoFreeStream_delete (HTStream * me);

#endif /* HTNOFREE_H */
