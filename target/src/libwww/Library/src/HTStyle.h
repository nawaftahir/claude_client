/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTSTYLE_H
#define HTSTYLE_H
typedef struct _HTStyleSheet HTStyleSheet;

extern HTStyleSheet * HTStyleSheet_new (const char * name);
extern BOOL HTStyleSheet_delete (HTStyleSheet * me);
typedef struct _HTStyle HTStyle;

extern HTStyle * HTStyle_new (const char * name, int element, void * context);
extern BOOL HTStyle_delete (HTStyle * me);
extern BOOL HTStyleSheet_addStyle (HTStyleSheet * me, HTStyle * style);
extern BOOL HTStyleSheet_deleteStyle (HTStyleSheet * me, HTStyle * style);
extern HTStyle * HTStyleSheet_findStyleWithName (HTStyleSheet * me, const char * name);
extern HTStyle * HTStyleSheet_findStyleForElement (HTStyleSheet * me, int element);
#endif /* HTSTYLE_H */
