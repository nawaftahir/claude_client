/*
**	(c) COPYRIGHT MIT 199G.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTMEMLOG_H
#define HTMEMLOG_H
extern int HTMemLog_open (char * logName, size_t size, BOOL keepOpen);
extern int HTMemLog_add (char * buf, size_t len);
extern int HTMemLog_flush (void);
extern void HTMemLog_close (void);
extern HTTraceDataCallback HTMemLog_callback;
#endif /* !HTMEMLOG_H */
