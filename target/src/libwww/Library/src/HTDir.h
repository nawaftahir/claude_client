/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTDIR_H
#define HTDIR_H
#include "HTReq.h"
#include "HTIcons.h"
typedef enum _HTDirShow {
    HT_DS_SIZE  = 0x1,			/* Show file size using K, M and G? */
    HT_DS_DATE  = 0x2,			/* Show last modified date? */
    HT_DS_HID   = 0x4,			/* Show hidden files? */
    HT_DS_DES	= 0x8,			/* Show descriptions? */
    HT_DS_ICON  = 0x10,			/* Show icons? */
    HT_DS_HOTI  = 0x20			/* Are Icons hot or cold? */
} HTDirShow;

typedef enum _HTDirKey {
    HT_DK_NONE	= 0,			/* No sorting */
    HT_DK_CSEN	= 1,			/* Case sensitive */
    HT_DK_CINS  = 2			/* Case insensitive */
} HTDirKey;
extern BOOL HTDir_setWidth (int minfile, int maxfile);
extern BOOL HTDir_setWidth (int minfile, int maxfile);
typedef struct _HTDir HTDir;

extern HTDir * HTDir_new (HTRequest * request, HTDirShow show, HTDirKey key);
extern BOOL HTDir_addElement	(HTDir *dir, char *name, char *date,
				 char *size, HTFileMode mode);
extern BOOL HTDir_free (HTDir * dir);
#endif /* HTDIR */
