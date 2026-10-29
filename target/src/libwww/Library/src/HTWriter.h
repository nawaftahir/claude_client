/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTWRITE_H
#define HTWRITE_H

#include "HTIOStream.h"
extern HTOutput_new HTWriter_new;

extern BOOL HTWriter_set (HTOutputStream *	me,
			  HTNet *		net,
			  HTChannel *		ch,
			  void *		param,
			  int			mode);
#endif
