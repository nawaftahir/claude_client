/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTACCESS_H
#define HTACCESS_H

#include "HTReq.h"
#include "HTAnchor.h"
extern BOOL HTLoadAbsolute (const char * url, HTRequest * request);
extern BOOL HTLoadRelative (const char * 	relative,
			    HTParentAnchor *	base,
			    HTRequest *		request);
extern HTChunk * HTLoadToChunk (const char * url, HTRequest * request);
extern BOOL HTLoadToFile (const char * url, HTRequest * request,
			  const char * filename);
extern BOOL HTLoadToStream (const char * url, HTStream * output,
			    HTRequest * request);
extern BOOL HTLoadAnchor (HTAnchor * anchor, HTRequest * request);
extern HTChunk * HTLoadAnchorToChunk (HTAnchor * anchor, HTRequest * request);
extern BOOL HTLoadAnchorRecursive (HTAnchor * anchor, HTRequest * request);
extern BOOL HTLoadRules (const char * url);
extern BOOL HTLoadRulesAutomatically (const char * url);
extern BOOL HTSearchAbsolute (HTChunk *		keywords,
			      const char *	base,
			      HTRequest *	request);
extern BOOL HTSearchRelative (HTChunk *		keywords,
			      const char * 	relative,
			      HTParentAnchor *	base,
			      HTRequest *	request);
extern BOOL HTSearchAnchor (HTChunk *		keywords,
			    HTAnchor *		anchor,
			    HTRequest * 	request);
extern BOOL HTSearchString (const char *	keywords,
			    HTAnchor *		anchor,
			    HTRequest * 	request);
extern BOOL HTGetFormAbsolute (HTAssocList *	formdata,
			       const char *	base,
			       HTRequest *	request);
extern BOOL HTGetFormRelative (HTAssocList * 	formdata,
			       const char * 	relative,
			       HTParentAnchor *	base,
			       HTRequest *	request);
extern BOOL HTGetFormAnchor (HTAssocList *	formdata,
			     HTAnchor *		anchor,
			     HTRequest * 	request);
extern HTParentAnchor * HTPostFormAbsolute (HTAssocList *	formdata,
					    const char *	base,
					    HTRequest *	request);
extern HTParentAnchor * HTPostFormRelative (HTAssocList * 	formdata,
					    const char * 	relative,
					    HTParentAnchor *	base,
					    HTRequest *		request);
extern HTParentAnchor * HTPostFormAnchor (HTAssocList *	formdata,
					  HTAnchor *	anchor,
					  HTRequest * 	request);
extern HTChunk * HTPostFormAnchorToChunk (HTAssocList * formdata,
                                          HTAnchor *    anchor,
                                          HTRequest *   request);
extern BOOL HTHeadAbsolute (const char * url, HTRequest * request);
extern BOOL HTHeadRelative (const char * 	relative,
			    HTParentAnchor *	base,
			    HTRequest *		request);
extern BOOL HTHeadAnchor (HTAnchor * anchor, HTRequest * request);
extern BOOL HTDeleteAbsolute (const char * url, HTRequest * request);
extern BOOL HTDeleteRelative (const char * 	relative,
			    HTParentAnchor *	base,
			    HTRequest *		request);
extern BOOL HTDeleteAnchor (HTAnchor * anchor, HTRequest * request);
extern BOOL HTPutAbsolute (HTParentAnchor *	source,
			   const char *		destination,
			   HTRequest *		request);
extern BOOL HTPutRelative (HTParentAnchor *	source,
			   const char * 	relative,
			   HTParentAnchor *	destination_base,
			   HTRequest *		request);
extern BOOL HTPutAnchor (HTParentAnchor *	source,
			 HTAnchor *		dest,
			 HTRequest *	 	request);
extern BOOL HTPutStructuredAbsolute (HTParentAnchor *	source,
				     const char *	destination,
				     HTRequest *	request,
				     HTPostCallback *	input);
extern BOOL HTPutStructuredRelative (HTParentAnchor *	source,
				     const char * 	relative,
				     HTParentAnchor *	destination_base,
				     HTRequest *	request,
				     HTPostCallback *	input);
extern BOOL HTPutStructuredAnchor (HTParentAnchor *	source,
				   HTAnchor *		destination,
				   HTRequest *	 	request,
				   HTPostCallback *	input);
extern BOOL HTPutDocumentAbsolute (HTParentAnchor *	source,
				   const char *	        destination,
				   HTRequest *	        request);
extern BOOL HTPutDocumentRelative (HTParentAnchor *	source,
				   const char * 	relative,
				   HTParentAnchor *	destination_base,
				   HTRequest *          request);
extern BOOL HTPutDocumentAnchor (HTParentAnchor *	source,
				 HTAnchor *		destination,
				 HTRequest *	 	request);
extern BOOL HTPostAbsolute (HTParentAnchor *	source,
			   const char *		destination,
			   HTRequest *		request);
extern BOOL HTPostRelative (HTParentAnchor *	source,
			   const char * 	relative,
			   HTParentAnchor *	destination_base,
			   HTRequest *		request);
extern BOOL HTPostAnchor (HTParentAnchor *	source,
			 HTAnchor *		dest,
			 HTRequest *	 	request);
extern BOOL HTOptionsAbsolute (const char * url, HTRequest * request);
extern BOOL HTOptionsRelative (const char * 	relative,
			    HTParentAnchor *	base,
			    HTRequest *		request);
extern BOOL HTOptionsAnchor (HTAnchor * anchor, HTRequest * request);
extern BOOL HTTraceAbsolute (const char * url, HTRequest * request);
extern BOOL HTTraceRelative (const char * 	relative,
			     HTParentAnchor *	base,
			     HTRequest *	request);
extern BOOL HTTraceAnchor (HTAnchor * anchor, HTRequest * request);
extern BOOL HTServeAbsolute (const char * address, HTRequest * request);
extern BOOL HTCopyAnchor (HTAnchor * src_anchor, HTRequest * main_req);
extern BOOL HTUploadAnchor (HTAnchor *		source_anchor,
			    HTRequest * 	request,
			    HTPostCallback *	callback);
extern int HTUpload_callback (HTRequest * request, HTStream * target);
#endif /* HTACCESS_H */
