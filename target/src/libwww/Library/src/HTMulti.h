/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTMULTI_H
#define HTMULTI_H

#include "HTReq.h"
extern void HTAddWelcome (char * welcome_name);
extern char * HTMulti (HTRequest *	req,
		      char *		path,
		      struct stat *	stat_info);
#endif /* HTMULTI_H */
