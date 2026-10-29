/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTFTP_H
#define HTFTP_H
#include "HTProt.h"

extern HTProtCallback HTLoadFTP;

typedef enum _FTPServerType {
    FTP_GENERIC 	= 0x1,
    FTP_MACHTEN		= 0x2,
    FTP_UNIX		= 0x4,
    FTP_VMS		= 0x8,
    FTP_CMS		= 0x10,
    FTP_DCTS		= 0x20,
    FTP_TCPC		= 0x40,
    FTP_PETER_LEWIS	= 0x80,
    FTP_NCSA		= 0x200,
    FTP_WINNT		= 0x400,
    FTP_UNSURE		= 0x8000
} FTPServerType;

#define MAX_FTP_LINE	128			 /* Don't use more than this */
typedef enum _FTPTransferMode {
    FTP_DEFAULT_TRANSFER_MODE   = 0,
    FTP_ASCII_TRANSFER_MODE     = 1,
    FTP_BINARY_TRANSFER_MODE    = 2,
    FTP_DIR_TRANSFER_MODE       = 3
} FTPTransferMode;

extern FTPTransferMode HTFTP_transferMode (void);
extern void HTFTP_setTransferMode (FTPTransferMode mode);
typedef enum _FTPControlMode {
    FTP_DEFAULT_CONTROL_MODE   = 0,
    FTP_ALWAYS_ASK_UID_PW      = 1
} FTPControlMode;

extern FTPControlMode HTFTP_controlMode (void);
extern void HTFTP_setControlMode (FTPControlMode mode);
#endif
