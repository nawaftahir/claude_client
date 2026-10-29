/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTRESPONSE_H
#define HTRESPONSE_H

typedef struct _HTResponse HTResponse;

#include "HTEvent.h"
#include "HTList.h"
#include "HTAssoc.h"
#include "HTFormat.h"
#include "HTUser.h"
extern HTResponse * HTResponse_new (void);
extern BOOL HTResponse_delete (HTResponse * response);
extern HTAnchor * HTResponse_redirection (HTResponse * response);
extern BOOL HTResponse_setRedirection (HTResponse * response, HTAnchor * anchor);
extern time_t HTResponse_retryTime (HTResponse * response);
extern BOOL HTResponse_setRetryTime (HTResponse * response, time_t retry);
extern BOOL HTResponse_addChallenge (HTResponse * response, char * token, char * value);

extern BOOL HTResponse_deleteChallengeAll (HTResponse * response);

extern HTAssocList * HTResponse_challenge (HTResponse * response);
extern BOOL HTResponse_setRealm (HTResponse * response, char * realm);
extern const char * HTResponse_realm (HTResponse * response);
extern BOOL HTResponse_setScheme (HTResponse * response, char * scheme);
extern const char * HTResponse_scheme (HTResponse * response);
extern BOOL HTResponse_addConnection       (HTResponse * response,
                                            char * token, char * value);
extern BOOL HTResponse_deleteConnectionAll (HTResponse * response);
extern HTAssocList * HTResponse_connection (HTResponse * response);
extern BOOL HTResponse_addProtocol       (HTResponse * response,
                                          char * token, char * value);
extern BOOL HTResponse_deleteProtocolAll (HTResponse * response);
extern HTAssocList * HTResponse_protocol (HTResponse * response);
extern BOOL HTResponse_addProtocolInfo       (HTResponse * response,
                                              char * token, char * value);
extern BOOL HTResponse_deleteProtocolInfoAll (HTResponse * response);
extern HTAssocList * HTResponse_protocolInfo (HTResponse * response);
extern BOOL HTResponse_addProtocolRequest       (HTResponse * response,
                                                 char * token, char * value);
extern BOOL HTResponse_deleteProtocolRequestAll (HTResponse * response);
extern HTAssocList * HTResponse_protocolRequest (HTResponse * response);
extern BOOL HTResponse_addCacheControl       (HTResponse * response,
                                              char * token, char *value);
extern BOOL HTResponse_deleteCacheControlAll (HTResponse * response);
extern HTAssocList * HTResponse_cacheControl (HTResponse * response);
typedef enum _HTCachable {
    HT_NO_CACHE            = 0,
    HT_CACHE_ALL           = 1,
    HT_CACHE_ETAG          = 2,
    HT_CACHE_NOT_MODIFIED  = 3
} HTCachable; 

extern HTCachable HTResponse_isCachable  (HTResponse * me);
extern BOOL HTResponse_setCachable (HTResponse * me, HTCachable mode);
extern char * HTResponse_etag (HTResponse * me);
extern BOOL HTResponse_isCached (HTResponse * me, BOOL mode);
extern time_t HTResponse_maxAge              (HTResponse * response);
extern BOOL   HTResponse_mustRevalidate      (HTResponse * response);
extern char * HTResponse_noCache             (HTResponse * response);
extern BOOL HTResponse_addRange       (HTResponse * response,
                                       char * unit, char * range);
extern BOOL HTResponse_deleteRangeAll (HTResponse * response);
extern HTAssocList * HTResponse_range (HTResponse * response);
extern long int HTResponse_length (HTResponse * response);
extern void HTResponse_setLength  (HTResponse * response, long int length);
extern void HTResponse_addLength  (HTResponse * response, long int deltalength);
extern HTFormat HTResponse_format (HTResponse * response);
extern void HTResponse_setFormat  (HTResponse * response, HTFormat format);
extern HTAssocList * HTResponse_formatParam (HTResponse * response);
extern BOOL HTResponse_addFormatParam  (HTResponse * response,
					const char * name, const char * value);
extern HTCharset HTResponse_charset (HTResponse * response);
extern BOOL HTResponse_setCharset   (HTResponse * response, HTCharset charset);
extern HTLevel HTResponse_level (HTResponse * response);
extern BOOL HTResponse_setLevel (HTResponse * response, HTLevel level);
extern HTList * HTResponse_encoding (HTResponse * response);
extern BOOL HTResponse_addEncoding  (HTResponse * response, HTEncoding enc);
extern HTList * HTResponse_transfer (HTResponse * response);
extern BOOL HTResponse_addTransfer  (HTResponse * response, HTEncoding te);
extern HTEncoding HTResponse_contentTransferEncoding (HTResponse * response);
extern BOOL HTResponse_setContentTransferEncoding (HTResponse * response,
                                                   HTEncoding cte);
extern BOOL HTResponse_addVariant (HTResponse * me, char * token, char * value);
extern BOOL HTResponse_deleteVariantAll (HTResponse * me);
extern HTAssocList * HTResponse_variant (HTResponse * me);
extern BOOL HTResponse_addTrailer (HTResponse * me, char * token, char * value);
extern BOOL HTResponse_deleteTrailerAll (HTResponse * me);
extern HTAssocList * HTResponse_trailer (HTResponse * me);
extern BOOL HTResponse_addHeader       (HTResponse * response,
                                        char * token, char * value);
extern BOOL HTResponse_deleteHeaderAll (HTResponse * response);
extern HTAssocList * HTResponse_header (HTResponse * response);

extern HTAssocList * HTResponse_handOverHeader (HTResponse * me);
extern char * HTResponse_reason (HTResponse * me);
extern BOOL HTResponse_setReason (HTResponse * me, char * reason);
#endif /* HTRESPONSE_H */
