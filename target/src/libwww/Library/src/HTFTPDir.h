/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTFTPDIR_H
#define HTFTPDIR_H

#include "HTStream.h"
#include "HTFTP.h"

extern HTStream * HTFTPDir_new (HTRequest *	request,
				FTPServerType	server,
				char		list);
#endif
