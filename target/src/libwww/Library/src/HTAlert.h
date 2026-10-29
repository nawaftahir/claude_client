/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTALERT_H
#define HTALERT_H

#include "HTReq.h"
typedef enum _HTAlertOpcode {
    HT_PROG_DNS		= 0x1,		/* Doing DNS resolution */
    HT_PROG_CONNECT	= 0x2,		/* Connecting Active */
    HT_PROG_ACCEPT	= 0x4,		/* Connecting Passive */
    HT_PROG_READ	= 0x8,		/* Read data */
    HT_PROG_WRITE	= 0x10,		/* Write data */
    HT_PROG_DONE	= 0x20,		/* Request finished */
    HT_PROG_INTERRUPT   = 0x40,         /* Request interrupted */
    HT_PROG_OTHER       = 0x80,         /* Other progress notes */
    HT_PROG_TIMEOUT     = 0x100,        /* Request timed out */
    HT_PROG_LOGIN	= 0x200,	/* Automatic login notifications */
    HT_A_PROGRESS	= 0xFFFF,	/* Get all progress reports - no reply */

    /* First word are reserved for progresss notifications */

    HT_A_MESSAGE	= 0x1<<16, /* Send a message - no reply */
    HT_A_CONFIRM	= 0x2<<16, /* Want YES or NO back */
    HT_A_PROMPT		= 0x4<<16, /* Want full dialog */
    HT_A_SECRET		= 0x8<<16, /* Secret dialog (e.g. password) */
    HT_A_USER_PW	= 0x10<<16 /* Atomic userid and password */
} HTAlertOpcode;

typedef struct _HTAlertPar HTAlertPar;

typedef BOOL HTAlertCallback   (HTRequest * request, HTAlertOpcode op,
				int msgnum, const char * dfault, void * input,
				HTAlertPar * reply);
typedef enum _HTAlertMsg {
    HT_MSG_NULL = -1,
    HT_MSG_UID = 0,
    HT_MSG_PROXY_UID,
    HT_MSG_FTP_UID,
    HT_MSG_PW,
    HT_MSG_FILENAME,
    HT_MSG_ACCOUNT,
    HT_MSG_METHOD,
    HT_MSG_MOVED,
    HT_MSG_RULES,
    HT_MSG_FILE_REPLACE,
    HT_MSG_RETRY_AUTHENTICATION,
    HT_MSG_RETRY_PROXY_AUTH,
    HT_MSG_REDO,
    HT_MSG_BIG_PUT,
    HT_MSG_SOURCE_MOVED,
    HT_MSG_DESTINATION_MOVED,
    HT_MSG_REDIRECTION,
    HT_MSG_PROXY,
    HT_MSG_CACHE_LOCK,
    HT_MSG_ACCEPT_COOKIE,
    HT_MSG_ELEMENTS		            /* This MUST be the last element */
} HTAlertMsg;
extern void HTAlert_setInteractive	(BOOL interative);
extern BOOL HTAlert_interactive		(void);
extern BOOL HTAlertCall_add (HTList * list, HTAlertCallback * cbf,
			     HTAlertOpcode opcode);
extern BOOL HTAlertCall_delete (HTList * list, HTAlertCallback * cbf);
extern BOOL HTAlertCall_deleteOpcode (HTList * list, HTAlertOpcode opcode);
extern BOOL HTAlertCall_deleteAll (HTList * list);
extern HTAlertCallback * HTAlertCall_find(HTList * list, HTAlertOpcode opcode);
extern HTAlertPar * HTAlert_newReply	(void);
extern void HTAlert_deleteReply		(HTAlertPar * old);
extern BOOL HTAlert_setReplyMessage	(HTAlertPar * me, const char *message);
extern BOOL HTAlert_assignReplyMessage	(HTAlertPar * me, char * message);
extern char * HTAlert_replyMessage	(HTAlertPar * me);
extern char * HTAlert_replySecret	(HTAlertPar * me);
extern BOOL HTAlert_setReplySecret	(HTAlertPar * me, const char * secret);

extern void * HTAlert_replyOutput	(HTAlertPar * me);
extern BOOL HTAlert_setReplyOutput	(HTAlertPar * me, void * output);
extern void HTAlert_setGlobal	(HTList * list);
extern HTList * HTAlert_global	(void);
extern BOOL HTAlert_add	(HTAlertCallback * cbf, HTAlertOpcode opcode);
extern BOOL HTAlert_delete (HTAlertCallback * cbf);
extern BOOL HTAlert_deleteOpcode (HTAlertOpcode opcode);
extern BOOL HTAlert_deleteAll (void);
extern HTAlertCallback * HTAlert_find (HTAlertOpcode opcode);
#endif
