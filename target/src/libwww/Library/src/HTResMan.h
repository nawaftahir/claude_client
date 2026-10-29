/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTRESMAN_H
#define HTRESMAN_H

#include "HTResponse.h"
#include "HTList.h"
#include "HTFormat.h"
#include "HTAnchor.h"
#include "HTMethod.h"
#include "HTAABrow.h"
#include "HTStream.h"
#include "HTNet.h"
#include "HTMIMPrs.h"
struct _HTResponse {

    int                 hash;
    HTAnchor *		redirectionAnchor;		  /* Redirection URL */
    time_t		retry_after;		 /* Absolut time for a retry */
    char *		realm;				    /* Current realm */
    char *		scheme;				   /* Current scheme */

    HTAssocList *	challenge;         /* Challenge received in response */
    HTAssocList *       connection;
    HTAssocList *	protocol;
    HTAssocList *	protocol_info;
    HTAssocList *	protocol_request;
    HTCachable          cachable;
    BOOL                cached;             /* If anchor has inherited lists */
    HTAssocList *       cache_control;
    HTAssocList *       byte_ranges;
    HTAssocList *       variants;
    long int	       content_length;
    HTFormat           content_type;
    HTAssocList *      type_parameters;    /* Type parameters (charset etc.) */
    HTList *	       content_encoding;
    HTList *          transfer_encoding;
    HTEncoding         cte;
    HTAssocList *      trailer;
    HTAssocList *      headers;
    char *             reason;             /* JK: HTTP reason string */
}; /* End of definition of HTResponse */
#endif /* HTRESMAN_H */
