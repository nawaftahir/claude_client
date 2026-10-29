/*
**	(c) COPYRIGHT CERN 1994.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTMLDTD_H
#define HTMLDTD_H

#include "sysdep.h"
#include "HTUtils.h"
#include "SGML.h"

typedef enum _HTMLElement {
	HTML_A, 	HTML_ADDRESS,
	HTML_B,		HTML_BLOCKQUOTE,	HTML_BODY,
	HTML_BR,
	HTML_CITE,	HTML_CODE,	HTML_COMMENT,
	HTML_DD, 	HTML_DFN,	HTML_DIR,
 	HTML_DL, 	HTML_DLC,	HTML_DT,
	HTML_EM,
	HTML_HEAD,
	HTML_H1, 	HTML_H2, 	HTML_H3,
	HTML_H4,	HTML_H5, 	HTML_H6, 	HTML_H7,
	HTML_HR,
	HTML_HTML,
	HTML_I,		HTML_IMG,	HTML_ISINDEX,
 	HTML_KBD,	
	HTML_LI, 	HTML_LINK,	HTML_LISTING,
	HTML_MENU,	HTML_NEXTID,
	HTML_OL,	HTML_P,		HTML_PLAINTEXT, HTML_PRE,
	HTML_SAMP,	HTML_STRONG,
	HTML_TITLE,	HTML_TT,
	HTML_U,		HTML_UL,
	HTML_VAR,	HTML_XMP } HTMLElement;

#define HTML_ELEMENTS 47 

#define HTML_A_HREF 		0
#define HTML_A_NAME 		1
#define HTML_A_REL		2
#define HTML_A_REV		3
#define HTML_A_TITLE		4
#define HTML_A_TYPE 		5
#define HTML_A_URN		6
#define HTML_A_ATTRIBUTES	7

#define DL_COMPACT 0

#define HTML_IMG_ALIGN		0

#define HTML_IMG_ALT		1
#define HTML_IMG_ISMAP		2	/* Obsolete but supported */
#define HTML_IMG_SRC 		3
#define HTML_IMG_ATTRIBUTES	4

#define HTML_NEXTID_ATTRIBUTES  1
#define HTML_NEXTID_N 0

#define HTML_PRE_WIDTH		0
#define HTML_PRE_ATTRIBUTES	1

extern CONST SGML_dtd HTML_dtd;
extern void HTStartAnchor PARAMS((
		HTStructured * targetstream,
		CONST char *  	name,
		CONST char *  	href));


#endif /* HTMLDTD_H */
extern void HTNextID PARAMS((HTStructured * targetStream, int n));
