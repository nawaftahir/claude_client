/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTREQMAN_H
#define HTREQMAN_H

#include "HTReq.h"
#include "HTList.h"
#include "HTFormat.h"
#include "HTAnchor.h"
#include "HTMethod.h"
#include "HTAABrow.h"
#include "HTStream.h"
#include "HTNet.h"
#include "HTMIMPrs.h"
struct _HTRequest {

    BOOL		internal;      /* Does the app knows about this one? */

    time_t		date;      /* Time stamp when the request was issued */

    HTMethod		method;

    BOOL                flush;                /* Should we flush immediately */

    HTPriority		priority;		/* Priority for this request */
    HTUserProfile *	userprofile;
    HTNet *		net;		    /* Information about socket etc. */
    HTResponse *        response;
    HTList *		error_stack;		           /* List of errors */
    int			retrys;       	      /* Number of automatic reloads */
    int                 max_forwards;
    int			AAretrys;      	      /* Number of authentication
                                                 retries */
    BOOL		preemptive;
    BOOL		ContentNegotiation;
    HTPreconditions     preconditions;
    HTGnHd		GenMask;
    HTRsHd		ResponseMask;
    HTRqHd		RequestMask;
    HTEnHd		EntityMask;
    HTMIMEParseSet *	parseSet;
    BOOL		pars_local;
    HTList *		conversions;
    BOOL		conv_local;

    HTList *		encodings;
    BOOL		enc_local;

    HTList *		tes;
    BOOL		te_local;

    HTList *		languages;
    BOOL		lang_local;

    HTList *		charsets;
    BOOL		char_local;

    HTList *		befores;
    BOOL		befores_local;

    HTList *		afters;
    BOOL		afters_local;
    char * 		proxy;
    BOOL                full_uri;
    HTReload		reload;
    HTAssocList *       cache_control;
   char *               default_put_name;
    HTAssocList *       byte_ranges;
    HTAssocList *       connection;
    HTAssocList *       expect;
    char *		realm;				    /* Current realm */
    HTAssocList *	credentials;	   /* Credentials received by server */
    HTAssocList	*	extra_headers;
    HTList *		generators;
    BOOL		gens_local;
    HTAssocList *	mandatory;
    HTAssocList *	optional;
    HTParentAnchor *	anchor;	       /* The Client anchor for this request */

    HTChildAnchor *	childAnchor;	    /* For element within the object */
    HTParentAnchor *	parentAnchor;			/* For referer field */
    HTStream *		output_stream; 
    HTStream *		orig_output_stream; 
    HTFormat		output_format;
    BOOL		connected;

    HTStream *		debug_stream;
    HTStream *		orig_debug_stream;
    HTFormat		debug_format;
    HTStream *		input_stream; 
    HTFormat		input_format;
    HTPostCallback *	PostCallback;
    HTRequestCallback *	callback;
    void *		context;
    HTRequest *		source;		     /* Source for request or itself */
    HTParentAnchor *	source_anchor;		  /* Source anchor or itself */

    HTRequest *		mainDestination;	     /* For the typical case */
    HTList *		destinations;		 /* List of related requests */
    int			destRequests;	   /* Number of destination requests */
    int			destStreams;	    /* Number of destination streams */
};
extern BOOL HTRequest_addDestination (HTRequest * src, HTRequest * dest);
extern BOOL HTRequest_removeDestination	(HTRequest * dest);
extern BOOL HTRequest_destinationsReady (HTRequest * me);

extern BOOL HTRequest_linkDestination (HTRequest * dest);
extern BOOL HTRequest_unlinkDestination (HTRequest * dest);

extern BOOL HTRequest_removePostWeb (HTRequest * me);
extern BOOL HTRequest_killPostWeb (HTRequest * me);

#define	HTRequest_mainDestination(me) \
	((me) && (me)->source ? (me)->source->mainDestination : NULL)
#define HTRequest_isDestination(me) \
	((me) && (me)->source && (me) != (me)->source)
#define HTRequest_isMainDestination(me) \
	((me) && (me)->source && \
	(me) == (me)->source->mainDestination)
#define HTRequest_isSource(me) \
	((me) && (me)->source && (me) == (me)->source)
#endif /* HTREQMAN_H */
