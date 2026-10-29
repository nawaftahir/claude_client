/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTFILE_H
#define HTFILE_H

#include "HTProt.h"

extern HTProtCallback HTLoadFile;
typedef enum _HTDirAccess {
    HT_DIR_FORBID,			/* Altogether forbidden */
    HT_DIR_SELECTIVE,			/* Only if "selfile" exists */
    HT_DIR_OK				/* Directory reading always OK */
} HTDirAccess;

#define DEFAULT_DIR_FILE	".www_browsable"    /* If exists, can browse */

extern HTDirAccess  HTFile_dirAccess	(void);
extern BOOL HTFile_setDirAccess		(HTDirAccess mode);
typedef enum _HTDirReadme {
    HT_DIR_README_NONE,
    HT_DIR_README_TOP,
    HT_DIR_README_BOTTOM
} HTDirReadme;

#define DEFAULT_README		"README"

extern HTDirReadme  HTFile_dirReadme	(void);
extern BOOL HTFile_setDirReadme		(HTDirReadme mode);
extern BOOL HTFile_doFileSuffixBinding (BOOL mode);
extern BOOL HTFile_fileSuffixBinding (void);
#endif /* HTFILE_H */
