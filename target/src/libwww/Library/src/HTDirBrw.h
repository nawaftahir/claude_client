/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTDIRBRW_H
#define HTDIRBRW_H

#include "HTFTP.h"
#include "HTReq.h"
extern int HTDirAccess;		                   /* Directory access level */
#define HT_DIR_FORBID 		0	             /* Altogether forbidden */
#define HT_DIR_SELECTIVE	1	     /* If HT_DIR_ENABLE_FILE exists */
#define HT_DIR_OK		2	          /* Any accesible directory */
#define HT_DIR_ENABLE_FILE	".www_browsable"    /* If exists, can browse */
extern int HTDirReadme;			 /* List a file in the dir listings? */
#define HT_DIR_README_NONE	0	                               /* No */
#define HT_DIR_README_TOP	1	                       /* Yes, first */
#define HT_DIR_README_BOTTOM	2	                  /* Yes, at the end */
#define HT_DIR_README_FILE	"README"
extern int HTDirInfo;
#define HT_DIR_INFO_NONE	0	                               /* No */
#define HT_DIR_INFO_TOP		1	                       /* Yes, first */
#define HT_DIR_INFO_BOTTOM	2	                  /* Yes, at the end */
extern unsigned int HTDirShowMask;
typedef enum _HTDirShow {
    HT_DIR_SHOW_ICON  = 0x1,		                /* Show icons yes/no */
    HT_DIR_SHOW_MODE  = 0x2,		          /* Show permissions yes/no */
    HT_DIR_SHOW_NLINK = 0x4,		      /* Show number of links yes/no */
    HT_DIR_SHOW_OWNER = 0x8,		           /* Show owner name yes/no */
    HT_DIR_SHOW_GROUP = 0x10,			   /* Show group name yes/no */
    HT_DIR_SHOW_SIZE  = 0x20,	          /* Show file size using K, M and G */
    HT_DIR_SHOW_DATE  = 0x40,		                 /* Show date yes/no */
    HT_DIR_SHOW_HID   = 0x80,		         /* Show hidden files yes/no */
    HT_DIR_SHOW_SLINK = 0x200,      /* Show Symbolic links in italics yes/no */
    HT_DIR_ICON_ANCHOR= 0x400,	           /* Use icon OR filename as anchor */
    HT_DIR_SHOW_CASE  = 0x800,	      /* Sort case sensitive when string key */
    HT_DIR_KEY_SIZE   = 0x1000,	       	       /* Those are the sorting keys */
    HT_DIR_KEY_DATE   = 0x2000,
    HT_DIR_KEY_NAME   = 0x4000
    /* HT_DIR_KEY_TYPE   = 0x8000 not implemented yet!!! */
} HTDirShow;
extern int HTDirMaxFileLength;
extern int HTDirMinFileLength;
extern BOOL HTDirShowBytes;
extern BOOL HTDirDescriptions;
extern int HTDirMaxDescrLength;
extern int HTBrowseDirectory PARAMS((
		HTRequest *  	req,
		char *		directory));
typedef struct _dir_file_info {
    char *		f_name;
    long 		f_mode;
    int			f_nlink;
    char *		f_uid;
    char *		f_gid;
    unsigned long	f_size;
    time_t		f_mtime;
} dir_file_info;
typedef int (*HTDirLineInput) PARAMS((HTNet *, dir_file_info *));
extern int HTFTPBrowseDirectory PARAMS((
		HTRequest * 	req,
		char *		directory,
		HTDirLineInput  input));
#endif /* HTDIRBRW */
