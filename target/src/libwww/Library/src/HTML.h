/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTML_H
#define HTML_H

#include "HTFormat.h"
#include "HTMLPDTD.h"
extern HTConverter HTMLToPlain;
extern HTConverter HTMLToC;
extern HTConverter HTMLPresent;
typedef enum _HTMLCharacterSet {
	HTML_ISO_LATIN1,
	HTML_NEXT_CHARS,
	HTML_PC_CP950
} HTMLCharacterSet;

extern BOOL HTMLUseCharacterSet (HTMLCharacterSet charset);
#endif		/* end HTML_H */
