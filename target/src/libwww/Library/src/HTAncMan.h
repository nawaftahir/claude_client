/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTANCMAN_H
#define HTANCMAN_H

#include "HTAnchor.h"
#include "HTList.h"
#include "HTAtom.h"
#include "HTMethod.h"
struct _HTAnchor {
  HTLink	mainLink;	/* Main (or default) destination of this */
  HTList *	links;  	/* List of extra links from this, if any */
  HTParentAnchor * parent;	/* Parent of this anchor (self for adults) */
};
struct _HTParentAnchor {
  /* Common part from the generic anchor structure */
  HTLink	mainLink;	/* Main (or default) destination of this */
  HTList *	links;  	/* List of extra links from this, if any */
  HTParentAnchor * parent;	/* Parent of this anchor (self) */

  /* ParentAnchor-specific information */
  HTList **	children;	/* Hash of subanchors of this, if any */
  HTList *	sources;	/* List of anchors pointing to this, if any */
  void *	document;	/* The document within this is an anchor */
  char *	physical;	/* Physical address */
  char * 	address;	/* Absolute address of this node */
  BOOL		isIndex;	/* Acceptance of a keyword search */

  HTAssocList * headers;        /* Unparsed headers */
  BOOL		header_parsed;	/* Are we done parsing? */

  /* We keep a list of variants of this anchor, if any */
  HTList *	variants;

  /* Entity header fields */
  char *	title;
  HTMethod	allow;	        /* Allowed methods (bit-flag) */

  HTFormat	content_type;	/* Content type */
  HTAssocList *	type_parameters;/* Content type parameters (charset etc.) */

  HTAssocList * meta_tags;      /* Set of metatags found in the HTML text */

  char *	content_base;
  HTList *	content_encoding;
  HTList *	content_language;
  long int	content_length;
  char *	content_location;
  char *	content_md5;

  HTEncoding	cte;	        /* Content-Transfer-Encoding */

  time_t	date;		/* When was the request issued */  
  time_t	expires;	/* When does the copy expire */
  time_t	last_modified;	/* When was this last modified */
  time_t	age;	        /* Cache estimate of age */
  char * 	etag;		/* entity tag */

  char *	derived_from;	/* Opaque string */
  char *	version;	/* Opaque string */
};
struct _HTChildAnchor {
  /* Common part from the generic anchor structure */
  HTLink	mainLink;	/* Main (or default) destination of this */
  HTList *	links;  	/* List of extra links from this, if any */
  HTParentAnchor * parent;	/* Parent of this anchor */

  /* ChildAnchor-specific information */
  char * 	tag;		/* Address of this anchor relative to parent */
};
#endif /* HTANCMAN_H */
