/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTERROR_H
#define HTERROR_H

typedef struct _HTError HTError;

typedef enum _HTSeverity {
    ERR_UNKNOWN		  = 0x0,
    ERR_FATAL		  = 0x1,
    ERR_NON_FATAL	  = 0x2,
    ERR_WARN	 	  = 0x4,
    ERR_INFO	 	  = 0x8
} HTSeverity;

#include "HTReq.h"
typedef struct _HTErrorMessage {
    int  	code;           	/* Error number */
    char *	msg;          		/* Short explanation */
    char *	url;			/* Explaning URL */
} HTErrorMessage;
typedef enum _HTErrorElement {
	HTERR_CONTINUE = 0,					/* 100 */
	HTERR_SWITCHING,					/* 101 */
	HTERR_OK,						/* 200 */
	HTERR_CREATED,						/* 201 */
	HTERR_ACCEPTED,						/* 202 */
	HTERR_NON_AUTHORITATIVE,				/* 203 */
	HTERR_NO_CONTENT,					/* 204 */
	HTERR_RESET,						/* 205 */
	HTERR_PARTIAL,						/* 206 */
	HTERR_PARTIAL_OK,					/* 207 */
	HTERR_MULTIPLE,						/* 300 */
	HTERR_MOVED,						/* 301 */
	HTERR_FOUND,						/* 302 */
	HTERR_METHOD,						/* 303 */
	HTERR_NOT_MODIFIED,					/* 304 */
	HTERR_USE_PROXY,					/* 305 */
	HTERR_PROXY_REDIRECT,					/* 306 */
	HTERR_TEMP_REDIRECT,					/* 307 */
	HTERR_BAD_REQUEST,					/* 400 */
	HTERR_UNAUTHORIZED,					/* 401 */
	HTERR_PAYMENT_REQUIRED,					/* 402 */
	HTERR_FORBIDDEN,					/* 403 */
	HTERR_NOT_FOUND,					/* 404 */
	HTERR_NOT_ALLOWED,					/* 405 */
	HTERR_NONE_ACCEPTABLE,					/* 406 */
	HTERR_PROXY_UNAUTHORIZED,				/* 407 */
	HTERR_TIMEOUT,						/* 408 */
	HTERR_CONFLICT,						/* 409 */
	HTERR_GONE,						/* 410 */
	HTERR_LENGTH_REQUIRED,					/* 411 */
	HTERR_PRECON_FAILED,					/* 412 */
	HTERR_TOO_BIG,						/* 413 */
	HTERR_URI_TOO_BIG,					/* 414 */
	HTERR_UNSUPPORTED,					/* 415 */
	HTERR_BAD_RANGE,					/* 416 */
	HTERR_EXPECTATION_FAILED,				/* 417 */
	HTERR_REAUTH,				                /* 418 */
	HTERR_PROXY_REAUTH,				        /* 419 */
	HTERR_INTERNAL,						/* 500 */
	HTERR_NOT_IMPLEMENTED,					/* 501 */
	HTERR_BAD_GATE,						/* 502 */
	HTERR_DOWN,						/* 503 */
	HTERR_GATE_TIMEOUT,					/* 504 */
	HTERR_BAD_VERSION,					/* 505 */
	HTERR_NO_PARTIAL_UPDATE,				/* 506 */

	/* Cache warnings */
	HTERR_STALE,						/* 10 */
	HTERR_REVALIDATION_FAILED,				/* 11 */
	HTERR_DISCONNECTED_CACHE,				/* 12 */
	HTERR_HEURISTIC_EXPIRATION,				/* 13 */
	HTERR_TRANSFORMED,					/* 14 */
	HTERR_CACHE,						/* 99 */

	/* Put all non-HTTP status codes after this */
	HTERR_NO_REMOTE_HOST,
	HTERR_NO_HOST,
	HTERR_NO_FILE,
	HTERR_FTP_SERVER,
	HTERR_FTP_NO_RESPONSE,
        HTERR_FTP_LOGIN_FAILURE,
	HTERR_TIME_OUT,
	HTERR_GOPHER_SERVER,
	HTERR_INTERRUPTED,
	HTERR_CON_INTR,
	HTERR_CSO_SERVER,
	HTERR_HTTP09,
	HTERR_BAD_REPLY,
	HTERR_UNKNOWN_AA,
	HTERR_NEWS_SERVER,
	HTERR_FILE_TO_FTP,
	HTERR_AUTO_REDIRECT,
	HTERR_MAX_REDIRECT,
	HTERR_EOF,
	HTERR_WAIS_OVERFLOW,
	HTERR_WAIS_MODULE,
	HTERR_WAIS_NO_CONNECT,
	HTERR_SYSTEM,
	HTERR_CLASS,
	HTERR_ACCESS,
	HTERR_LOGIN,
        HTERR_CACHE_EXPIRED,
        HTERR_NO_AUTO_RULES,
        HTERR_NO_AUTO_PROXY,
	HTERR_ELEMENTS		            /* This MUST be the last element */
} HTErrorElement;
typedef enum _HTErrorShow {
    HT_ERR_SHOW_FATAL     = 0x1,
    HT_ERR_SHOW_NON_FATAL = 0x3,
    HT_ERR_SHOW_WARNING   = 0x7,
    HT_ERR_SHOW_INFO 	  = 0xF,
    HT_ERR_SHOW_PARS	  = 0x10,
    HT_ERR_SHOW_LOCATION  = 0x20,
    HT_ERR_SHOW_IGNORE    = 0x40,
    HT_ERR_SHOW_FIRST     = 0x80,
    HT_ERR_SHOW_LINKS     = 0x100,
    HT_ERR_SHOW_DEFAULT	  = 0x13,
    HT_ERR_SHOW_DETAILED  = 0x1F,
    HT_ERR_SHOW_DEBUG	  = 0x7F
} HTErrorShow;

extern HTErrorShow HTError_show (void);
extern BOOL HTError_setShow (HTErrorShow mask);
extern BOOL HTError_add (HTList * 	list,
			 HTSeverity	severity,
			 BOOL		ignore,
			 int		element,
			 void *		par,
			 unsigned int	length,
			 char *		where);
extern BOOL HTError_addSystem (HTList *		list,
			       HTSeverity 	severity,
			       int		errornumber,
			       BOOL		ignore,
			       char *		syscall);
extern BOOL HTError_deleteAll (HTList * list);
extern BOOL HTError_deleteLast (HTList * list);
extern BOOL HTError_doShow (HTError * info);
extern BOOL HTError_ignoreLast	(HTList * list);
extern BOOL HTError_setIgnore	(HTError * info);
extern int HTError_index		(HTError * info);
extern HTSeverity HTError_severity	(HTError * info);
extern BOOL HTError_hasSeverity 	(HTList * list, HTSeverity severity);
extern void * HTError_parameter		(HTError * info, int * length);
extern const char * HTError_location	(HTError * info);
#endif /* HTERROR_H */
