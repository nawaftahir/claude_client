/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTBIND_H
#define HTBIND_H

#include "HTFormat.h"
#include "HTAnchor.h"
extern BOOL HTBind_init		(void);
extern BOOL HTBind_deleteAll	(void);
extern void HTBind_caseSensitive	(BOOL sensitive);
extern const char *HTBind_delimiters	(void);
extern void HTBind_setDelimiters	(const char * new_suffixes);
extern BOOL HTBind_add		(const char *	suffix,
				 const char *	representation,
				 const char *	encoding,
				 const char *	transfer,
				 const char *	language,
				 double		value);

extern BOOL HTBind_addType	(const char *	suffix,
				 const char *	format,
				 double		value);

extern BOOL HTBind_addEncoding	(const char *	suffix,
				 const char *	encoding,
				 double		value);

extern BOOL HTBind_addTransfer	(const char *	suffix,
				 const char *	transfer,
				 double		value);

extern BOOL HTBind_addLanguage	(const char *	suffix,
				 const char *	language,
				 double		value);
extern char * HTBind_getSuffix	(HTParentAnchor * anchor);
extern BOOL HTBind_getAnchorBindings	(HTParentAnchor * anchor);
extern BOOL HTBind_getResponseBindings	(HTResponse * response,
                                         const char * url);
extern BOOL HTBind_getFormat (const char *	filename,
			      HTFormat *	format,
			      HTEncoding *	enc,
			      HTEncoding *	cte,
			      HTLanguage *	lang,
			      double *		quality);
#endif /* HTBIND_H */
