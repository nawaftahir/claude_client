/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTREQ_H
#define HTREQ_H

typedef long HTRequestID;
typedef struct _HTRequest HTRequest;

#include "HTEvent.h"
#include "HTList.h"
#include "HTAssoc.h"
#include "HTFormat.h"
#include "HTStream.h"
#include "HTError.h"
#include "HTNet.h"
#include "HTUser.h"
#include "HTResponse.h"
extern HTRequest * HTRequest_new (void);
extern BOOL HTRequest_clear (HTRequest * me);
extern HTRequest * HTRequest_dup (HTRequest * src);
extern HTRequest * HTRequest_dupInternal (HTRequest * src);

extern BOOL HTRequest_setInternal (HTRequest * request, BOOL mode);
extern BOOL HTRequest_internal (HTRequest * request);
extern void HTRequest_delete (HTRequest * request);
extern BOOL HTLoad (HTRequest * request, BOOL recursive);
extern BOOL HTServe(HTRequest * request, BOOL recursive);
extern BOOL HTRequest_kill(HTRequest * request);
extern void HTRequest_setAnchor (HTRequest *request, HTAnchor *anchor);
extern HTParentAnchor * HTRequest_anchor (HTRequest *request);

extern HTChildAnchor * HTRequest_childAnchor (HTRequest * request);
extern BOOL HTRequest_setUserProfile (HTRequest * request, HTUserProfile * up);
extern HTUserProfile * HTRequest_userProfile (HTRequest * request);
extern HTNet * HTRequest_net (HTRequest * request);
extern BOOL HTRequest_setNet (HTRequest * request, HTNet * net);
extern HTResponse * HTRequest_response (HTRequest * request);
extern BOOL HTRequest_setResponse (HTRequest * request, HTResponse * response);
extern void HTRequest_setMethod (HTRequest *request, HTMethod method);
extern HTMethod HTRequest_method (HTRequest *request);
extern HTPriority HTRequest_priority (HTRequest * request);
extern BOOL HTRequest_setPriority (HTRequest * request, HTPriority priority);
extern BOOL HTRequest_setFlush (HTRequest * me, BOOL mode);
extern BOOL HTRequest_flush (HTRequest * me);
extern int HTRequest_forceFlush (HTRequest * request);
extern HTList * HTRequest_error (HTRequest * request);
extern void HTRequest_setError	(HTRequest * request, HTList * list);
extern void HTRequest_deleteAllErrors (HTRequest * request);
extern BOOL HTRequest_addError (HTRequest * 	request,
				HTSeverity	severity,
				BOOL		ignore,
				int		element,
				void *		par,
				unsigned int	length,
				char *		where);

extern BOOL HTRequest_addSystemError (HTRequest * 	request,
				      HTSeverity 	severity,
				      int		errornumber,
				      BOOL		ignore,
				      char *		syscall);
extern BOOL HTRequest_setMaxRetry (int newmax);
extern int  HTRequest_maxRetry (void);

extern int HTRequest_retrys (HTRequest * request);
extern BOOL HTRequest_doRetry (HTRequest *request);
extern BOOL HTRequest_addRetry (HTRequest * request);

extern int HTRequest_AAretrys (HTRequest * request);
extern BOOL HTRequest_addAARetry (HTRequest * request);
extern BOOL HTRequest_setMaxForwards (HTRequest * request, int maxforwards);
extern int HTRequest_maxForwards (HTRequest * request);
extern void HTRequest_setPreemptive (HTRequest *request, BOOL mode);
extern BOOL HTRequest_preemptive (HTRequest *request);
extern void HTRequest_setNegotiation (HTRequest *request, BOOL mode);
extern BOOL HTRequest_negotiation (HTRequest *request);
typedef enum _HTPreconditions {
    HT_NO_MATCH = 0,
    HT_MATCH_THIS,
    HT_MATCH_ANY,
    HT_DONT_MATCH_THIS,
    HT_DONT_MATCH_ANY
} HTPreconditions;

extern void HTRequest_setPreconditions (HTRequest * me, HTPreconditions mode);
extern HTPreconditions HTRequest_preconditions (HTRequest * me);
typedef struct _HTMIMEParseSet HTMIMEParseSet;
extern void HTRequest_setMIMEParseSet (HTRequest *request,
				       HTMIMEParseSet * parseSet, BOOL local);
extern HTMIMEParseSet * HTRequest_MIMEParseSet (HTRequest *request,
					      BOOL * pLocal);
typedef enum _HTGnHd {
    HT_G_CC             = 0x1,
    HT_G_CONNECTION	= 0x2,
    HT_G_DATE		= 0x4,
    HT_G_PRAGMA_NO_CACHE= 0x8,
    HT_G_FORWARDED	= 0x10,
    HT_G_MESSAGE_ID	= 0x20,
    HT_G_MIME		= 0x40,
    HT_G_TRAILER        = 0x80,
    HT_G_TRANSFER       = 0x100,
    HT_G_EXTRA_HEADERS  = 0x200
} HTGnHd;

#define DEFAULT_GENERAL_HEADERS	\
        HT_G_CONNECTION + HT_G_CC + HT_G_TRANSFER + HT_G_TRAILER + \
        HT_G_EXTRA_HEADERS

extern void HTRequest_setGnHd (HTRequest *request, HTGnHd gnhd);
extern void HTRequest_addGnHd (HTRequest *request, HTGnHd gnhd);
extern HTGnHd HTRequest_gnHd (HTRequest *request);
typedef enum _HTRqHd {
    HT_C_ACCEPT_TYPE	= 0x1,
    HT_C_ACCEPT_CHAR	= 0x2,
    HT_C_ACCEPT_ENC	= 0x4,
    HT_C_ACCEPT_TE	= 0x8,
    HT_C_ACCEPT_LAN	= 0x10,
    HT_C_AUTH		= 0x20,             /* Includes proxy authentication */
    HT_C_EXPECT         = 0x40,
    HT_C_FROM		= 0x80,
    HT_C_HOST		= 0x100,
    HT_C_IMS		= 0x200,
    HT_C_IF_MATCH	= 0x400,
    HT_C_IF_MATCH_ANY	= 0x800,
    HT_C_IF_NONE_MATCH	= 0x1000,
    HT_C_IF_NONE_MATCH_ANY=0x2000,
    HT_C_IF_RANGE	= 0x4000,
    HT_C_IF_UNMOD_SINCE	= 0x8000,
    HT_C_MAX_FORWARDS	= 0x10000,
    HT_C_RANGE		= 0x20000,
    HT_C_REFERER	= 0x40000,
    HT_C_USER_AGENT	= 0x80000
} HTRqHd;

#define DEFAULT_REQUEST_HEADERS	\
	HT_C_ACCEPT_TYPE + HT_C_ACCEPT_CHAR + \
	HT_C_ACCEPT_ENC + HT_C_ACCEPT_TE + HT_C_ACCEPT_LAN + HT_C_AUTH + \
        HT_C_EXPECT + HT_C_HOST + HT_C_REFERER + HT_C_USER_AGENT

extern void HTRequest_setRqHd (HTRequest *request, HTRqHd rqhd);
extern void HTRequest_addRqHd (HTRequest *request, HTRqHd rqhd);
extern HTRqHd HTRequest_rqHd (HTRequest *request);
typedef enum _HTRsHd {
    HT_S_AGE		= 0x1,
    HT_S_LOCATION   	= 0x2,
    HT_S_PROXY_AUTH 	= 0x4,
    HT_S_PUBLIC    	= 0x8,
    HT_S_RETRY_AFTER	= 0x10,
    HT_S_SERVER		= 0x20,
    HT_S_VARY		= 0x40,
    HT_S_WARNING	= 0x80,
    HT_S_WWW_AUTH	= 0x100,
    HT_S_TRAILER        = 0x200
} HTRsHd;

#define DEFAULT_RESPONSE_HEADERS HT_S_SERVER

extern void HTRequest_setRsHd (HTRequest * request, HTRsHd rshd);
extern void HTRequest_addRsHd (HTRequest * request, HTRsHd rshd);
extern HTRsHd HTRequest_rsHd (HTRequest * request);
typedef enum _HTEnHd {
    HT_E_ALLOW			= 0x1,
    HT_E_CONTENT_BASE		= 0x2,
    HT_E_CONTENT_ENCODING	= 0x4,
    HT_E_CONTENT_LANGUAGE	= 0x8,
    HT_E_CONTENT_LENGTH		= 0x10,
    HT_E_CONTENT_LOCATION	= 0x20,
    HT_E_CONTENT_MD5		= 0x40,
    HT_E_CONTENT_RANGE		= 0x80,
    HT_E_CTE			= 0x100,	/* Content-Transfer-Encoding */
    HT_E_CONTENT_TYPE		= 0x200,
    HT_E_DERIVED_FROM		= 0x400,
    HT_E_ETAG			= 0x800,
    HT_E_EXPIRES		= 0x1000,
    HT_E_LAST_MODIFIED		= 0x2000,
    HT_E_LINK			= 0x4000,
    HT_E_TITLE			= 0x8000,
    HT_E_URI			= 0x10000,
    HT_E_VERSION		= 0x20000
} HTEnHd;

#define DEFAULT_ENTITY_HEADERS		0xFFFFFFFF		      /* all */

extern void HTRequest_setEnHd (HTRequest *request, HTEnHd enhd);
extern void HTRequest_addEnHd (HTRequest *request, HTEnHd enhd);
extern HTEnHd HTRequest_enHd (HTRequest *request);
extern BOOL HTRequest_addExtraHeader       (HTRequest * request,
                                            char * token, char * value);
extern HTAssocList * HTRequest_extraHeader (HTRequest * request);
extern BOOL HTRequest_deleteExtraHeaderAll (HTRequest * request);
extern void HTRequest_setGenerator (HTRequest *request, HTList *gens,
                                    BOOL override);
extern HTList * HTRequest_generator (HTRequest *request, BOOL *override);
/* TBD */
extern void HTRequest_setConversion (HTRequest *request, HTList *type, BOOL override);
extern HTList * HTRequest_conversion (HTRequest *request);
extern void HTRequest_setEncoding (HTRequest *request, HTList *enc, BOOL override);
extern HTList * HTRequest_encoding (HTRequest *request);
extern void HTRequest_setTransfer (HTRequest *request, HTList *te, BOOL override);
extern HTList * HTRequest_transfer (HTRequest *request);
extern void HTRequest_setLanguage (HTRequest *request, HTList *lang, BOOL override);
extern HTList * HTRequest_language (HTRequest *request);
extern void HTRequest_setCharset (HTRequest *request, HTList *charset, BOOL override);
extern HTList * HTRequest_charset (HTRequest *request);
typedef enum _HTReload {
    HT_CACHE_OK	            = 0x0,	        /* Use any version available */
    HT_CACHE_FLUSH_MEM	    = 0x1,	/* Reload from file cache or network */
    HT_CACHE_VALIDATE	    = 0x2,	             /* Validate cache entry */
    HT_CACHE_END_VALIDATE   = 0x4,                  /* End to end validation */
    HT_CACHE_RANGE_VALIDATE = 0x8,
    HT_CACHE_FLUSH	    = 0x10,                     /* Force full reload */
    HT_CACHE_ERROR          = 0x20         /* An error occurred in the cache */
} HTReload;

extern void HTRequest_setReloadMode (HTRequest *request, HTReload mode);
extern HTReload HTRequest_reloadMode (HTRequest *request);
extern char * HTRequest_defaultPutName (HTRequest * me);
extern BOOL HTRequest_setDefaultPutName (HTRequest * me, char * name);
extern BOOL HTRequest_deleteDefaultPutName (HTRequest * me);
extern BOOL HTRequest_addCacheControl        (HTRequest * request,
                                              char * token, char *value);
extern BOOL HTRequest_deleteCacheControlAll  (HTRequest * request);
extern HTAssocList * HTRequest_cacheControl  (HTRequest * request);
extern time_t HTRequest_date  (HTRequest * request);
extern BOOL HTRequest_setDate (HTRequest * request, time_t date);
extern BOOL HTRequest_addExpect (HTRequest * me,
				 char * token, char * value);
extern BOOL HTRequest_deleteExpect (HTRequest * me);
extern HTAssocList * HTRequest_expect (HTRequest * me);
extern BOOL HTRequest_addRange       (HTRequest * request,
                                      char * unit, char * range);
extern BOOL HTRequest_deleteRangeAll (HTRequest * request);
extern HTAssocList * HTRequest_range (HTRequest * request);
extern BOOL HTRequest_addConnection        (HTRequest * request,
                                            char * token, char * value);
extern BOOL HTRequest_deleteConnection     (HTRequest * request);
extern HTAssocList * HTRequest_connection  (HTRequest * request);
extern BOOL HTRequest_addCredentials       (HTRequest * request,
                                            char * token, char * value);
extern BOOL HTRequest_deleteCredentialsAll (HTRequest * request);
extern HTAssocList * HTRequest_credentials (HTRequest * request);
extern BOOL HTRequest_setRealm (HTRequest * request, char * realm);
extern const char * HTRequest_realm (HTRequest * request);
extern BOOL HTRequest_deleteRealm (HTRequest * me);
extern void HTRequest_setParent (HTRequest *request, HTParentAnchor *parent);
extern HTParentAnchor * HTRequest_parent (HTRequest *request);
extern BOOL HTRequest_addBefore (HTRequest * request, HTNetBefore * filter,
				 const char * tmplate, void * param,
                                 HTFilterOrder order, BOOL override);
extern HTList * HTRequest_before (HTRequest * request, BOOL * override);
extern BOOL HTRequest_deleteBefore (HTRequest * request, HTNetBefore * filter);
extern BOOL HTRequest_deleteBeforeAll (HTRequest * request);
extern BOOL HTRequest_addAfter (HTRequest * request, HTNetAfter * filter,
				const char * tmplate, void * param,
                                int status, HTFilterOrder order,
                                BOOL override);
extern HTList * HTRequest_after (HTRequest * request, BOOL * override);
extern BOOL HTRequest_deleteAfter (HTRequest * request, HTNetAfter * filter);
extern BOOL HTRequest_deleteAfterStatus (HTRequest * request, int status);
extern BOOL HTRequest_deleteAfterAll (HTRequest * request);
typedef int HTPostCallback (HTRequest * request, HTStream * target);

extern void HTRequest_setPostCallback (HTRequest * request, HTPostCallback * cbf);
extern HTPostCallback * HTRequest_postCallback (HTRequest * request);
extern BOOL HTRequest_setEntityAnchor (HTRequest * request, HTParentAnchor * anchor);
extern HTParentAnchor * HTRequest_entityAnchor (HTRequest * request);
extern void HTRequest_setInputStream (HTRequest * request, HTStream * input);
extern HTStream *HTRequest_inputStream (HTRequest * request);
extern BOOL HTRequest_isPostWeb (HTRequest * request);
extern BOOL HTRequest_setSource (HTRequest * request, HTRequest * source);
extern HTRequest * HTRequest_source (HTRequest * request);
extern void HTRequest_setOutputStream (HTRequest *request, HTStream *output);
extern HTStream *HTRequest_outputStream (HTRequest *request);
extern void HTRequest_setOutputFormat (HTRequest *request, HTFormat format);
extern HTFormat HTRequest_outputFormat (HTRequest *request);
extern void HTRequest_setOutputConnected (HTRequest * request, BOOL mode);
extern BOOL HTRequest_outputConnected	 (HTRequest * request);
extern void HTRequest_setDebugStream (HTRequest *request, HTStream *debug);
extern HTStream *HTRequest_debugStream (HTRequest *request);
extern void HTRequest_setDebugFormat (HTRequest *request, HTFormat format);
extern HTFormat HTRequest_debugFormat (HTRequest *request);
typedef int HTRequestCallback (HTRequest * request, void *param);

extern void HTRequest_setCallback (HTRequest *request, HTRequestCallback *cb);
extern HTRequestCallback *HTRequest_callback (HTRequest *request);
extern void HTRequest_setContext (HTRequest *request, void *context);
extern void *HTRequest_context (HTRequest *request);
extern void HTRequest_setFullURI (HTRequest *request, BOOL mode);
extern BOOL HTRequest_fullURI (HTRequest *request);
extern BOOL HTRequest_setProxy    (HTRequest * request, const char * proxy);
extern char * HTRequest_proxy     (HTRequest * request);
extern BOOL HTRequest_deleteProxy (HTRequest * request);
extern long HTRequest_bodyRead (HTRequest * request);
extern long HTRequest_bodyWritten (HTRequest * request);
extern long HTRequest_bytesRead (HTRequest * request);
extern long HTRequest_bytesWritten (HTRequest * request);
#endif /* HTREQ_H */
