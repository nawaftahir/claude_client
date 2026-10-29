/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTFORMAT_H
#define HTFORMAT_H

#include "HTUtils.h"
#include "HTStream.h"
#include "HTAtom.h"
#include "HTList.h"
#include "HTAnchor.h"
#include "HTReq.h"
#define WWW_INTERNAL	HTAtom_for("www/*")          /* All internal formats */
#define WWW_RAW		HTAtom_for("www/void")   /* Raw output from Protocol */
#define WWW_SOURCE	HTAtom_for("*/*")
#define WWW_PRESENT	HTAtom_for("www/present")
#define WWW_DEBUG	HTAtom_for("www/debug")
#define WWW_UNKNOWN     HTAtom_for("www/unknown")
#define WWW_CACHE         HTAtom_for("www/cache")
#define WWW_CACHE_APPEND  HTAtom_for("www/cache-append")
#define WWW_HTML 	HTAtom_for("text/html")
#define WWW_PLAINTEXT 	HTAtom_for("text/plain")
#define WWW_FORM	HTAtom_for("application/x-www-form-urlencoded")

#define WWW_MIME	HTAtom_for("message/rfc822")
#define WWW_MIME_HEAD	HTAtom_for("message/x-rfc822-head")
#define WWW_MIME_FOOT	HTAtom_for("message/x-rfc822-foot")
#define WWW_MIME_PART   HTAtom_for("message/x-rfc822-partial")
#define WWW_MIME_CONT   HTAtom_for("message/x-rfc822-cont")
#define WWW_MIME_UPGRADE	HTAtom_for("message/x-rfc822-upgrade")

#define WWW_MIME_COPYHEADERS HTAtom_for("www/x-rfc822-headers")

#define WWW_AUDIO       HTAtom_for("audio/basic")

#define WWW_VIDEO 	HTAtom_for("video/mpeg")

#define WWW_GIF 	HTAtom_for("image/gif")
#define WWW_JPEG 	HTAtom_for("image/jpeg")
#define WWW_TIFF 	HTAtom_for("image/tiff")
#define WWW_PNG 	HTAtom_for("image/png")

#define WWW_BINARY 	HTAtom_for("application/octet-stream")
#define WWW_POSTSCRIPT 	HTAtom_for("application/postscript")
#define WWW_RICHTEXT 	HTAtom_for("application/rtf")
#define WWW_GOPHER_MENU HTAtom_for("text/x-gopher")
#define WWW_CSO_SEARCH	HTAtom_for("text/x-cso")

#define WWW_FTP_LNST	HTAtom_for("text/x-ftp-lnst")
#define WWW_FTP_LIST	HTAtom_for("text/x-ftp-list")

#define WWW_NNTP_LIST   HTAtom_for("text/x-nntp-list")
#define WWW_NNTP_OVER	HTAtom_for("text/x-nntp-over")
#define WWW_NNTP_HEAD	HTAtom_for("text/x-nntp-head")

#define WWW_HTTP	HTAtom_for("text/x-http")
#define WWW_RULES	HTAtom_for("application/x-www-rules")
typedef HTStream * HTConverter	(HTRequest *	request,
				 void *		param,
				 HTFormat	input_format,
				 HTFormat	output_format,
				 HTStream *	output_stream);

extern void HTConversion_add   (HTList *	conversions,
				const char * 	rep_in,
				const char * 	rep_out,
				HTConverter *	converter,
				double		quality,
				double		secs, 
				double		secs_per_byte);

extern void HTConversion_deleteAll	(HTList * list);
extern void HTPresentation_setConverter (HTConverter * pconv);
extern HTConverter * HTPresentation_converter (void);
extern void HTPresentation_add (HTList *	conversions,
				const char * 	representation,
				const char * 	command,
				const char * 	test_command,
				double		quality,
				double		secs, 
				double		secs_per_byte);

extern void HTPresentation_deleteAll	(HTList * list);
extern HTConverter HTThroughLine;
extern HTConverter HTBlackHoleConverter;
extern HTConverter HTSaveConverter;
typedef HTStream * HTCoder	(HTRequest *	request,
				 void *		param,
				 HTEncoding	coding,
				 HTStream *	target);
extern HTCoder HTIdentityCoding;
typedef struct _HTCoding HTCoding;
#define WWW_CODING_7BIT		HTAtom_for("7bit")
#define WWW_CODING_8BIT		HTAtom_for("8bit")
#define WWW_CODING_BINARY	HTAtom_for("binary")
#define WWW_CODING_IDENTITY     HTAtom_for("identity")

#define WWW_CODING_BASE64	HTAtom_for("base64")
#define WWW_CODING_MACBINHEX	HTAtom_for("macbinhex")
#define WWW_CODING_CHUNKED	HTAtom_for("chunked")

#define WWW_CODING_COMPRESS	HTAtom_for("compress")
#define WWW_CODING_GZIP	        HTAtom_for("gzip")
#define WWW_CODING_DEFLATE      HTAtom_for("deflate")
extern BOOL HTCoding_add (HTList * 	list,
			 const char *	encoding,
			 HTCoder *	encoder,
			 HTCoder *	decoder,
			 double		quality);

extern void HTCoding_deleteAll (HTList * list);

extern const char * HTCoding_name (HTCoding * me);

extern double HTCoding_quality (HTCoding * me);
extern void HTCharset_add (HTList * list, const char * charset, double quality);
typedef struct _HTAcceptNode {
    HTAtom *	atom;
    double	quality;
} HTAcceptNode;
extern void HTCharset_deleteAll	(HTList * list);
extern void HTLanguage_add (HTList * list, const char * lang, double quality);
extern void HTLanguage_deleteAll (HTList * list);
extern void HTFormat_setConversion	(HTList * list);
extern HTList * HTFormat_conversion	(void);

extern void HTFormat_addConversion (const char *	input_format,
				    const char *	output_format,
				    HTConverter *	converter,
				    double		quality,
				    double		secs, 
				    double		secs_per_byte);
extern void HTFormat_setContentCoding	(HTList * list);
extern HTList * HTFormat_contentCoding	(void);

extern BOOL HTFormat_addCoding ( char *		encoding,
				 HTCoder *	encoder,
				 HTCoder *	decoder,
				 double		quality);
#define HTFormat_isUnityContent(me) \
	((me)==NULL || \
	(me)==WWW_CODING_BINARY || (me)==WWW_CODING_IDENTITY || \
        (me)==WWW_CODING_7BIT || (me)==WWW_CODING_8BIT)
extern void HTFormat_setTransferCoding	(HTList * list);
extern HTList * HTFormat_transferCoding	(void);

extern BOOL HTFormat_addTransferCoding ( char *		encoding,
					 HTCoder *	encoder,
					 HTCoder *	decoder,
					 double		quality);
#define HTFormat_isUnityTransfer(me) \
	((me)==NULL || \
	 (me)==WWW_CODING_BINARY || (me)==WWW_CODING_IDENTITY || \
         (me)==WWW_CODING_7BIT || (me)==WWW_CODING_8BIT)
extern void HTFormat_setLanguage	(HTList * list);
extern HTList * HTFormat_language	(void);
extern void HTFormat_setCharset		(HTList * list);
extern HTList * HTFormat_charset	(void);
extern void HTFormat_deleteAll (void);
extern HTStream * HTStreamStack (HTFormat	rep_in,
				 HTFormat	rep_out,
				 HTStream *	output_stream,
				 HTRequest *	request,
				 BOOL		guess);
extern double HTStackValue	(HTList *	conversions,
				 HTFormat	format_in,
				 HTFormat	format_out,
				 double		initial_value,
				 long int	length);
extern HTStream * HTContentCodingStack (HTEncoding	coding,
					HTStream *	target,
					HTRequest *	request,
					void *		param,
					BOOL		encoding);
extern HTStream * HTContentEncodingStack (HTList *	encodings,
					  HTStream *	target,
					  HTRequest *	request,
					  void *	param);
extern HTStream * HTContentDecodingStack (HTList *	encodings,
					  HTStream *	target,
					  HTRequest *	request,
					  void *	param);
extern HTStream * HTTransferCodingStack (HTEncoding	coding,
					 HTStream *	target,
					 HTRequest *	request,
					 void *		param,
					 BOOL		encoding);
extern HTStream * HTTransferEncodingStack (HTList *	encodings,
					   HTStream *	target,
					   HTRequest *	request,
					   void *	param);
extern HTStream * HTTransferDecodingStack (HTList *	encodings,
					  HTStream *	target,
					  HTRequest *	request,
					  void *	param);
extern HTStream * HTContentTransferCodingStack (HTEncoding	encoding,
					        HTStream *	target,
					        HTRequest *	request,
					        void *		param,
					        BOOL		encode);
typedef struct _HTPresentation {
    HTFormat	rep;			     /* representation name atomized */
    HTFormat	rep_out;			 /* resulting representation */
    HTConverter *converter;	      /* The routine to gen the stream stack */
    char *	command;			       /* MIME-format string */
    char *	test_command;			       /* MIME-format string */
    double	quality;		     /* Between 0 (bad) and 1 (good) */
    double	secs;
    double	secs_per_byte;
} HTPresentation;
#endif /* HTFORMAT */
