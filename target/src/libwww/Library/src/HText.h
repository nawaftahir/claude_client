/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTEXT_H
#define HTEXT_H

#include "HTAnchor.h"
#include "HTStream.h"
typedef struct _HText HText;
typedef HText * HText_new (
	HTRequest *		request,
	HTParentAnchor *	anchor,
	HTStream *		output_stream);

typedef BOOL HText_delete (HText * me);
extern BOOL HText_registerCDCallback (HText_new *, HText_delete *);
extern BOOL HText_unregisterCDCallback (void);
typedef enum _HTextStatus {
    HTEXT_BEGIN,
    HTEXT_END,
    HTEXT_ABORT
} HTextStatus;

typedef void HText_build (HText * text, HTextStatus status);
extern BOOL HText_registerBuildCallback (HText_build *);
extern BOOL HText_unregisterBuildCallback (void);
typedef void HText_addText (
	HText * 	text,
	const char * 	buffer,
	int		length);
extern BOOL HText_registerTextCallback (HText_addText *);
extern BOOL HText_unregisterTextCallback (void);
typedef void HText_foundLink (
	HText * 	text,
	int		element_number,
	int		attribute_number,
	HTChildAnchor *	anchor,
	const BOOL *	present,
	const char **	value);
extern BOOL HText_registerLinkCallback (HText_foundLink *);
extern BOOL HText_unregisterLinkCallback (void);
typedef void HText_beginElement (
	HText * 	text,
	int		element_number,
	const BOOL *	present,
	const char **	value);

typedef void HText_endElement (
	HText * 	text,
	int		element_number);
extern BOOL HText_registerElementCallback (HText_beginElement *, HText_endElement *);
extern BOOL HText_unregisterElementCallback (void);
typedef void HText_unparsedBeginElement (
	HText *		HText,
	const char * 	buffer,
	int		length);

typedef void HText_unparsedEndElement (
	HText *		HText,
	const char * 	buffer,
	int		length);
extern BOOL HText_registerUnparsedElementCallback (
	HText_unparsedBeginElement *,
	HText_unparsedEndElement *);
extern BOOL HText_unregisterUnparsedElementCallback (void);
typedef void HText_unparsedEntity (
	HText *		HText,
	const char * 	buffer,
	int		length);
extern BOOL HText_registerUnparsedEntityCallback (HText_unparsedEntity *);
extern BOOL HText_unregisterUnparsedEntityCallback (void);
#endif /* HTEXT_H */
