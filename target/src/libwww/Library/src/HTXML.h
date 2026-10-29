/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTXML_H
#define HTXML_H

#include "HTFormat.h"
#include "HTStream.h"
#ifdef HT_STRUCT_XML_STREAM
#include "HTStruct.h"
#include "SGML.h"
#endif /* HT_STRUCT_XML_STREAM */

#include <xmlparse.h>
extern HTConverter HTXML_new;
typedef void HTXMLCallback_new (
	HTStream *		me,
	HTRequest *		request,
	HTFormat 		target_format,
	HTStream *		target_stream,
	XML_Parser              xmlparser,
	void *                  context);
extern BOOL HTXMLCallback_registerNew (HTXMLCallback_new *, void * context);
#endif
