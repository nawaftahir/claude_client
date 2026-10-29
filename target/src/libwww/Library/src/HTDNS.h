/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTDNS_H
#define HTDNS_H
typedef struct _HTdns HTdns;
extern void HTDNS_setTimeout (time_t timeout);
extern time_t HTDNS_timeout  (time_t timeout);
extern HTdns * HTDNS_add (HTList * list, struct hostent * element,
			  char * host, int * homes);
extern BOOL HTDNS_delete (const char * host);
extern BOOL HTDNS_deleteAll (void);
extern BOOL HTDNS_updateWeigths (HTdns *dns, int cur, ms_t deltatime);
extern char * HTGetHostBySock (int soc);
extern int HTGetHostByName (HTHost * host, char *hostname, HTRequest * request);
#endif
