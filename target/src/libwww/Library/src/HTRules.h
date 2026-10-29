/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTRULE_H
#define HTRULE_H

#include "HTList.h"
#include "HTReq.h"
#include "HTFormat.h"
extern HTConverter HTRules, HTRules_parseAutomatically;
extern BOOL HTRule_parseLine (HTList * list, const char * config);
typedef struct _HTRule HTRule;

typedef enum _HTRuleOp {
    HT_Invalid, 
    HT_Map, 
    HT_Pass, 
    HT_Fail,
    HT_DefProt,
    HT_Protect,
    HT_Exec,
    HT_Redirect,
    HT_UseProxy
} HTRuleOp;

extern BOOL HTRule_add (HTList * list, HTRuleOp op,
			const char * pattern, const char * replace);
extern BOOL HTRule_deleteAll (HTList *list);
extern HTList * HTRule_global(void);
extern BOOL HTRule_setGlobal (HTList * list);
extern BOOL HTRule_addGlobal (HTRuleOp op, const char * pattern, const char * replace);
extern char * HTRule_translate (HTList * list, const char * token,
				BOOL ignore_case);
#endif
