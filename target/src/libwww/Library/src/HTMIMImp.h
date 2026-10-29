/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTMIMIMP_H
#define HTMIMIMP_H

#include "HTReq.h"
#include "HTHeader.h"
extern HTParserCallback 
	HTMIME_accept, 
	HTMIME_acceptCharset, 
	HTMIME_acceptEncoding, 
	HTMIME_acceptLanguage, 
	HTMIME_acceptRanges, 
	HTMIME_authenticate,
	HTMIME_authenticationInfo,
	HTMIME_authorization,
	HTMIME_cacheControl,
	HTMIME_connection, 
	HTMIME_contentEncoding, 
	HTMIME_contentLength,
	HTMIME_contentRange,
	HTMIME_contentTransferEncoding, 
	HTMIME_contentType,
	HTMIME_keepAlive,
	HTMIME_link,
	HTMIME_location,
	HTMIME_maxForwards, 
	HTMIME_messageDigest,
	HTMIME_pragma,
	HTMIME_protocol,
	HTMIME_protocolInfo,
	HTMIME_protocolRequest,
	HTMIME_proxyAuthorization,
	HTMIME_proxyAuthenticationInfo,
	HTMIME_public,
	HTMIME_range,
	HTMIME_referer,
	HTMIME_retryAfter, 
	HTMIME_server, 
        HTMIME_trailer,
	HTMIME_transferEncoding,
	HTMIME_upgrade,
	HTMIME_userAgent,
	HTMIME_vary,
	HTMIME_via,
	HTMIME_warning;
#endif
