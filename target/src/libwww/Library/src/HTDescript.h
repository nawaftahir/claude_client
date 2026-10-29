/*
**	(c) COPYRIGHT CERN 1994.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTDESCRIPT_H
#define HTDESCRIPT_H

#include "sysdep.h"
extern char * HTDescriptionFile;
/*
 * Example:
 *	DESCRIBE  welcome.html	Our welcome page
 *	DESCRIBE  map*.gif	Map as a GIF image
 *	DESCRIBE  map*.ps	Map as a PostScript image
 */
extern BOOL HTPeekTitles;
extern HTList * HTReadDescriptions PARAMS((char * dirname));
extern char * HTGetDescription PARAMS((HTList *	descriptions,
				       char *	dirname,
				       char *	filename,
				       HTFormat	format));
extern void HTFreeDescriptions PARAMS((HTList * descriptions));
#endif /* !HTDESCRIPT_H */
