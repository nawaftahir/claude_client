/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTPROF_H
#define HTPROF_H
#include "WWWLib.h"
#include "WWWInit.h"
extern void HTProfile_newClient (
	const char * AppName,
	const char * AppVersion);
extern void HTProfile_newHTMLClient (
	const char * AppName,
	const char * AppVersion);
extern void HTProfile_newNoCacheClient (
	const char * AppName,
	const char * AppVersion);
extern void HTProfile_newHTMLNoCacheClient (
	const char * AppName,
	const char * AppVersion);
extern void HTProfile_newPreemptiveClient (
	const char * AppName,
	const char * AppVersion);
extern void HTProfile_newRobot (
	const char * AppName,
	const char * AppVersion);
extern void HTProfile_newPreemptiveRobot (
	const char * AppName,
	const char * AppVersion);
extern void HTProfile_delete (void);
#endif
