#ifndef HTGROUP_H
#define HTGROUP_H

#include "HTUtils.h"
#include "HTList.h"

#ifdef SHORT_NAMES
#define HTAApGrD	HTAA_parseGroupDef
#define HTAArGrR	HTAA_resolveGroupReferences
#define HTAApGrD	HTAA_printGroupDef
#define HTAAGD_d	GroupDef_delete
#define HTAAuIIG	HTAA_userAndInetInGroup
#endif /* SHORT_NAMES */

typedef HTList GroupDefList;
typedef HTList ItemList;

typedef struct {
    char *	group_name;
    ItemList *	item_list;
} GroupDef;


/*
** Access Authorization failure reasons
*/
typedef enum {
    HTAA_OK,		/* 200 OK				*/
    HTAA_OK_GATEWAY,	/* 200 OK, acting as a gateway		*/
    HTAA_NO_AUTH,	/* 401 Unauthorized, not authenticated	*/
    HTAA_NOT_MEMBER,	/* 401 Unauthorized, not authorized	*/
    HTAA_IP_MASK,	/* 403 Forbidden by IP mask		*/
    HTAA_BY_RULE,	/* 403 Forbidden by rule		*/
    HTAA_NO_ACL,	/* 403 Forbidden, ACL non-existent	*/
    HTAA_NO_ENTRY,	/* 403 Forbidden, no ACL entry		*/
    HTAA_SETUP_ERROR,	/* 403 Forbidden, server setup error	*/
    HTAA_DOTDOT,	/* 403 Forbidden, URL with /../ illegal	*/
    HTAA_HTBIN,		/* 403 Forbidden, /htbin not enabled	*/
    HTAA_NOT_FOUND	/* 404 Not found, or read protected	*/
} HTAAFailReason;
PUBLIC GroupDef *HTAA_parseGroupDef PARAMS((FILE * fp));
PUBLIC void HTAA_resolveGroupReferences PARAMS((GroupDef *     group_def,
						GroupDefList * group_def_list));
PUBLIC GroupDefList *HTAA_readGroupFile PARAMS((CONST char * filename));
PUBLIC void GroupDef_delete PARAMS((GroupDef * group_def));
PUBLIC void HTAA_printGroupDef PARAMS((GroupDef * group_def));
/* PUBLIC					HTAA_userAndInetInGroup()
**		CHECK IF USER BELONGS TO TO A GIVEN GROUP
**		AND THAT THE CONNECTION COMES FROM AN
**		ADDRESS THAT IS ALLOWED BY THAT GROUP
** ON ENTRY:
**	group		the group definition structure.
**	username	connecting user.
**	ip_number	browser host IP number, optional.
**	ip_name		browser host IP name, optional.
**			However, one of ip_number or ip_name
**			must be given.
** ON EXIT:
**	returns		HTAA_IP_MASK, if IP address mask was
**			reason for failing.
**			HTAA_NOT_MEMBER, if user does not belong
**			to the group.
**			HTAA_OK if both IP address and user are ok.
*/
PUBLIC HTAAFailReason HTAA_userAndInetInGroup PARAMS((GroupDef * group,
						      char *     username,
						      char *     ip_number,
						      char *     ip_name));
#endif /* not HTGROUP_H */
