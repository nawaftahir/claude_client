/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTAAUTIL_H
#define HTAAUTIL_H
#include "HTReq.h"
#include "HTNet.h"
#include "HTUTree.h"
typedef struct _HTAAModule HTAAModule;

extern HTAAModule * HTAA_newModule (const char *		scheme,
				    HTNetBefore *		before,
				    HTNetAfter *		after,
				    HTNetAfter *                update,
				    HTUTree_gc *		gc);
extern HTAAModule * HTAA_findModule (const char * scheme);
extern BOOL HTAA_deleteModule (const char * scheme);
extern BOOL HTAA_deleteAllModules (void);
extern void * HTAA_updateNode (BOOL proxy,
                               char const * scheme,
			       const char * realm, const char * url,
			       void * context);
extern BOOL HTAA_deleteNode (BOOL proxy_access, char const * scheme,
                             const char * realm, const char * url);
HTNetBefore HTAA_beforeFilter;
HTNetAfter HTAA_afterFilter;
HTNetAfter HTAA_updateFilter;
HTNetBefore HTAA_proxyBeforeFilter;
#endif	/* NOT HTAAUTIL_H */
