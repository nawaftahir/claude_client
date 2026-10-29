/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTEVTLST_H
#define HTEVTLST_H

#include "wwwsys.h"
#include "HTEvent.h"
#include "HTReq.h"
#if defined(WWW_WIN_ASYNC) || defined(WWW_WIN_DLL)
extern BOOL HTEventList_winHandle (HTRequest * request);
extern BOOL HTEventList_setWinHandle (HWND window, unsigned long message);
extern HWND HTEventList_getWinHandle (unsigned long * pMessage);
extern LRESULT CALLBACK AsyncWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif
extern HTEvent_registerCallback HTEventList_register;
extern HTEvent_unregisterCallback HTEventList_unregister;
extern int HTEventList_unregisterAll (void);
extern int HTEventList_dispatch (SOCKET s, HTEventType type, ms_t now);
extern HTEvent * HTEventList_lookup (SOCKET s, HTEventType type);
extern BOOL HTEventInit (void);
extern BOOL HTEventTerminate (void);
extern int HTEventList_newLoop (void);
extern int HTEventList_loop (HTRequest * request);
extern void HTEventList_stopLoop (void);
#endif /* HTEVTLST_H */
