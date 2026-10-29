/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTSQL_H
#define HTSQL_H

#include <mysql/mysql.h>
typedef struct _HTSQL HTSQL;

extern HTSQL * HTSQL_new (const char * host,
			  const char * user, const char * pw,
			  int flags);
extern BOOL HTSQL_delete (HTSQL * me);
extern BOOL HTSQL_connect (HTSQL * me);
extern BOOL HTSQL_close (HTSQL * me);
extern BOOL HTSQL_version (HTSQL *		me,
			   char **		server,
			   unsigned int * 	protocol_version,
			   char **		server_version,
			   char **		client_version);
extern BOOL HTSQL_selectDB (HTSQL * me, const char * db);
extern MYSQL * HTSQL_getMysql (HTSQL * me);
extern char * HTSQL_printf (char * buf, int length, char * fmt, ...);
extern BOOL HTSQL_query (HTSQL * me, const char * query);
extern int HTSQL_getLastInsertId (HTSQL * me);
extern int HTSQL_GetAffectedRows (HTSQL * me);
extern MYSQL_RES * HTSQL_storeResult (HTSQL * me);
extern BOOL HTSQL_freeResult (MYSQL_RES * me);
#endif
