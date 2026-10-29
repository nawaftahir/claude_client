/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTDESCRIPT_H
#define HTDESCRIPT_H
extern char * HTDescriptionFile;
/*
**	DESCRIBE  welcome.html	Our welcome page
**	DESCRIBE  map*.gif	Map as a GIF image
**	DESCRIBE  map*.ps	Map as a PostScript image
*/
extern BOOL HTPeekTitles;
extern HTList * HTReadDescriptions (char * dirname);
extern char * HTGetDescription (HTList *	descriptions,
				       char *	dirname,
				       char *	filename,
				       HTFormat	format);
extern void HTFreeDescriptions (HTList * descriptions);
#endif /* !HTDESCRIPT_H */
