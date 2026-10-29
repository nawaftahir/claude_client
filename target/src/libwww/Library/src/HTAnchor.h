/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTANCHOR_H
#define HTANCHOR_H
#include "WWWUtil.h"

typedef HTAtom * HTFormat;
typedef HTAtom * HTLevel;		       /* Used to specify HTML level */
typedef HTAtom * HTEncoding;				    /* C-E and C-T-E */
typedef HTAtom * HTCharset;
typedef HTAtom * HTLanguage;

typedef struct _HTAnchor	HTAnchor;
typedef struct _HTParentAnchor	HTParentAnchor;
typedef struct _HTChildAnchor	HTChildAnchor;

#include "HTLink.h"
#include "HTMethod.h"
#include "HTResponse.h"
extern HTAnchor * HTAnchor_findAddress		(const char * address);
extern HTChildAnchor * HTAnchor_findChild	(HTParentAnchor *parent,
						 const char *	tag);
extern HTChildAnchor * HTAnchor_findChildAndLink (
		HTParentAnchor * parent,		/* May not be 0 */
		const char * tag,			/* May be "" or 0 */
		const char * href,			/* May be "" or 0 */
		HTLinkType ltype);			/* May be 0 */
extern BOOL HTAnchor_delete	(HTParentAnchor *me);
extern BOOL HTAnchor_clearAll (HTList * documents);
extern BOOL HTAnchor_deleteAll	(HTList * objects);
extern HTArray * HTAnchor_getArray (int growby);
extern BOOL HTAnchor_setMainLink	(HTAnchor * anchor, HTLink * link);
extern HTLink * HTAnchor_mainLink	(HTAnchor * anchor);

extern HTAnchor * HTAnchor_followMainLink (HTAnchor * anchor);
extern BOOL HTAnchor_setSubLinks	(HTAnchor * anchor, HTList * list);
extern HTList * HTAnchor_subLinks	(HTAnchor * anchor);
extern HTLink * HTAnchor_findLinkType (HTAnchor * me, HTLinkType type);
extern HTParentAnchor * HTAnchor_parent	(HTAnchor *me);
extern BOOL HTAnchor_hasChildren	(HTParentAnchor *me);
extern BOOL HTAnchor_isChild (HTAnchor * me);
extern char * HTAnchor_view (HTAnchor * me);
extern char * HTAnchor_address		(HTAnchor * me);
extern char * HTAnchor_expandedAddress  (HTAnchor * me);
extern char * HTAnchor_physical		(HTParentAnchor * me);
extern void HTAnchor_setPhysical	(HTParentAnchor * me, char * protocol);
extern void HTAnchor_clearPhysical	(HTParentAnchor * me);
extern void HTAnchor_setDocument	(HTParentAnchor *me, void * doc);
extern void * HTAnchor_document		(HTParentAnchor *me);
extern void HTAnchor_clearHeader	(HTParentAnchor *me);
extern BOOL HTAnchor_update (HTParentAnchor * me, HTResponse * response);
extern void HTAnchor_clearIndex		(HTParentAnchor * me);
extern void HTAnchor_setIndex		(HTParentAnchor * me);
extern BOOL HTAnchor_isIndex		(HTParentAnchor * me);
extern const char * HTAnchor_title	(HTParentAnchor *me);
extern void HTAnchor_setTitle		(HTParentAnchor *me,
					 const char *	title);
extern void HTAnchor_appendTitle	(HTParentAnchor *me,
					 const char *	title);
extern HTAssocList * HTAnchor_meta (HTParentAnchor * me);
extern BOOL HTAnchor_addMeta (HTParentAnchor * me,
			      const char * name, const char * value);
extern char * HTAnchor_robots (HTParentAnchor * me);
extern char * HTAnchor_base	(HTParentAnchor * me);
extern BOOL HTAnchor_setBase 	(HTParentAnchor * me, char * base);
extern char * HTAnchor_location		(HTParentAnchor * me);
extern BOOL HTAnchor_setLocation	(HTParentAnchor * me, char * location);
extern HTFormat HTAnchor_format		(HTParentAnchor *me);
extern void HTAnchor_setFormat		(HTParentAnchor *me,
					 HTFormat	form);
extern HTAssocList * HTAnchor_formatParam (HTParentAnchor * me);

extern BOOL HTAnchor_addFormatParam 	(HTParentAnchor * me,
					const char * name, const char * value);
extern HTCharset HTAnchor_charset	(HTParentAnchor *me);
extern BOOL HTAnchor_setCharset		(HTParentAnchor *me,
					 HTCharset	charset);
extern HTLevel HTAnchor_level		(HTParentAnchor * me);
extern BOOL HTAnchor_setLevel		(HTParentAnchor * me,
					 HTLevel	level);
extern HTList * HTAnchor_language	(HTParentAnchor * me);
extern BOOL HTAnchor_addLanguage	(HTParentAnchor *me, HTLanguage lang);
extern BOOL HTAnchor_deleteLanguageAll  (HTParentAnchor * me);
extern HTList * HTAnchor_encoding	(HTParentAnchor * me);
extern BOOL HTAnchor_addEncoding	(HTParentAnchor * me, HTEncoding enc);
extern BOOL HTAnchor_deleteEncoding     (HTParentAnchor * me, HTEncoding enc);
extern BOOL HTAnchor_deleteEncodingAll  (HTParentAnchor * me);

#define HTAnchor_removeEncoding(a, e)   HTAnchor_deleteEncoding((a), (e))
extern HTEncoding HTAnchor_contentTransferEncoding	(HTParentAnchor *me);
extern void HTAnchor_setContentTransferEncoding	        (HTParentAnchor *me,
				 	                 HTEncoding	cte);
extern long int HTAnchor_length	(HTParentAnchor * me);
extern void HTAnchor_setLength	(HTParentAnchor * me, long int length);
extern void HTAnchor_addLength	(HTParentAnchor * me, long int deltalength);
extern char * HTAnchor_md5	(HTParentAnchor * me);
extern BOOL HTAnchor_setMd5	(HTParentAnchor * me, const char * hash);
extern HTMethod HTAnchor_allow   (HTParentAnchor * me);
extern void HTAnchor_setAllow    (HTParentAnchor * me, HTMethod methodset);
extern void HTAnchor_appendAllow (HTParentAnchor * me, HTMethod methodset);
extern char * HTAnchor_version	(HTParentAnchor * me);
extern void HTAnchor_setVersion	(HTParentAnchor * me, const char * version);
extern time_t HTAnchor_date		(HTParentAnchor * me);
extern void HTAnchor_setDate		(HTParentAnchor	* me, const time_t date);
extern time_t HTAnchor_lastModified	(HTParentAnchor * me);
extern void HTAnchor_setLastModified	(HTParentAnchor	* me, const time_t lm);
extern char * HTAnchor_etag 	(HTParentAnchor * me);
extern void HTAnchor_setEtag	(HTParentAnchor * me, const char * etag);
extern BOOL HTAnchor_isEtagWeak	(HTParentAnchor * me);
extern time_t HTAnchor_age    (HTParentAnchor * me);
extern void HTAnchor_setAge   (HTParentAnchor * me, const time_t age);
extern time_t HTAnchor_expires		(HTParentAnchor * me);
extern void HTAnchor_setExpires		(HTParentAnchor	* me, const time_t exp);
extern char * HTAnchor_derived	(HTParentAnchor *me);
extern void HTAnchor_setDerived	(HTParentAnchor *me, const char *derived_from);
extern BOOL HTAnchor_headerParsed	(HTParentAnchor *me);
extern void HTAnchor_setHeaderParsed	(HTParentAnchor *me);
extern BOOL HTAnchor_setHeader       (HTParentAnchor * me, HTAssocList * list);
extern HTAssocList * HTAnchor_header (HTParentAnchor * me);
#endif /* HTANCHOR_H */
