/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTESCAPE_H
#define HTESCAPE_H
typedef enum _HTURIEncoding {
    URL_XALPHAS		= 0x1,     /* Escape all unsafe characters */
    URL_XPALPHAS	= 0x2,     /* As URL_XALPHAS but allows '+' */
    URL_PATH		= 0x4,     /* As URL_XPALPHAS but allows '/' */
    URL_DOSFILE         = 0x8      /* As URL_URLPATH but allows ':' */
} HTURIEncoding;

extern char * HTEscape (const char * str, HTURIEncoding mask);
extern char HTAsciiHexToChar (char c);
extern char * HTUnEscape (char * str);
#endif	/* HTESCAPE_H */
