/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTUTREE_H
#define HTUTREE_H
#include "HTReq.h"
typedef struct _HTUTree     HTUTree;
typedef struct _HTURealm     HTURealm;
typedef struct _HTUTemplate HTUTemplate;
typedef int HTUTree_gc		(void * context);

extern HTUTree * HTUTree_new (const char * 		root,
			      const char * 		host,
			      int 			port,
			      HTUTree_gc *	 	gc);
extern BOOL HTUTree_delete (const char * 	root,
			    const char * 	host,
			    int			port);
extern BOOL HTUTree_deleteAll (void);
extern HTUTree * HTUTree_find (const char *	root,
			       const char *	host,
			       int		port);
extern void * HTUTree_findNode (HTUTree * tree, 
                                const char * realm, const char * path); 
extern BOOL HTUTree_addNode (HTUTree * tree, 
                             const char * realm, const char * path, 
                             void * context);
extern BOOL HTUTree_replaceNode (HTUTree * tree, 
                                 const char * realm, const char * path, 
                                 void * context);
extern BOOL HTUTree_deleteNode (HTUTree * tree, 
                                const char * realm, const char * path);
#endif /* HTUTREE_H */
