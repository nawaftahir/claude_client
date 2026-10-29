/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTINIT_H
#define HTINIT_H
#include "WWWLib.h"
#include "WWWApp.h"
#include "WWWTrans.h"

extern void HTTransportInit (void);
#include "WWWHTTP.h"
#include "WWWFile.h"
#include "WWWFTP.h"
#include "WWWGophe.h"
#include "WWWTelnt.h"
#include "WWWNews.h"

#ifdef HT_DIRECT_WAIS
#include "WWWWAIS.h"
#endif

#ifndef FTP_PORT
#define FTP_PORT	21
#endif
#ifndef NEWS_PORT
#define NEWS_PORT	119
#endif
#ifndef GOPHER_PORT
#define GOPHER_PORT	70
#endif
#ifndef WAIS_PORT
#define WAIS_PORT	666
#endif
#ifndef HTTP_PORT
#define HTTP_PORT	80
#endif

#define HTAccessInit	HTProtocolInit

extern void HTProtocolInit (void);

extern void HTProtocolPreemptiveInit (void);
#include "WWWMIME.h"
#include "WWWApp.h"

extern void HTMIMEInit (void);
#include "WWWApp.h"
#include "WWWMIME.h"
#include "WWWHTML.h"
#include "WWWStream.h"
#include "WWWDir.h"
#include "WWWCache.h"

#ifdef HT_EXPAT
#include "WWWXML.h"
#endif

extern void HTConverterInit	(HTList * conversions);
extern void HTPresenterInit	(HTList * conversions);
extern void HTFormatInit	(HTList * conversions);
#include "WWWHTTP.h"

extern void HTTransferEncoderInit	(HTList * encodings);
#include "WWWZip.h"

extern void HTContentEncoderInit	(HTList * encodings);
#include "WWWApp.h"

extern void HTBeforeInit (void);
extern void HTAfterInit (void);
extern void HTNetInit (void);
extern void HTAAInit (void);
#include "WWWApp.h"

extern void HTAlertInit (void);
#include "WWWFile.h"

extern void HTIconInit (const char * url_prefix);
#endif
