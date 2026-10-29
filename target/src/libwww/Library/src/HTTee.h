/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef _HTTEE_H
#define _HTTEE_H

#include "HTStream.h"
#include "HTArray.h"

extern HTStream * HTTee (HTStream * s1, HTStream * s2, HTComparer * resolver);

#endif /* HTTEE_H */
