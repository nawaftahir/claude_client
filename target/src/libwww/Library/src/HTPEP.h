/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTPEP_H
#define HTPEP_H
#include "HTList.h"
#include "HTReq.h"
#include "HTUTree.h"

typedef struct _HTPEPModule HTPEPModule;
extern HTPEPModule * HTPEP_newModule(const char *	protocol,
				     HTNetBefore *	before,
			             HTNetAfter *	after,
				     HTUTree_gc *	gc);
extern HTPEPModule * HTPEP_findModule (const char * protocol);
extern BOOL HTPEP_deleteModule (const char * protocol);
extern BOOL HTPEP_deleteAllModules (void);
extern HTPEPModule * HTPEP_findModule (const char * name);
extern HTNetBefore HTPEP_beforeFilter;
extern HTNetAfter  HTPEP_afterFilter;
#endif	/* NOT HTPEP_H */
