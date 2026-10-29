/*
**        (c) COPYRIGHT MIT 1995.
**        Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTLINK_H
#define HTLINK_H

typedef struct _HTLink        HTLink;

#include "WWWUtil.h"
#include "HTMethod.h"
#include "HTAnchor.h"
typedef HTAtom *         HTLinkType;

typedef enum _HTLinkResult {
    HT_LINK_INVALID = -1,
    HT_LINK_NONE = 0,
    HT_LINK_ERROR,
    HT_LINK_OK
} HTLinkResult;

struct _HTLink {
    HTAnchor *          dest;              /* The anchor to which this leads */
    HTLinkType          type;                      /* Semantics of this link */
    HTMethod            method;            /* Method for this link, e.g. PUT */
    HTLinkResult        result;    /* Result of any attempt to get this link */
};

HTLink * HTLink_new (void);
BOOL HTLink_delete (HTLink * link);
extern BOOL HTLink_removeAll (HTAnchor * me);
#define HT_LR_PERM_REDIRECT        HTAtom_for("PERMANENT_REDIRECTION")
#define HT_LR_TEMP_REDIRECT        HTAtom_for("TEMPORARY_REDIRECTION")
#define HT_LR_SEE_OTHER            HTAtom_for("SEE_OTHER")
extern BOOL HTLink_add (
        HTAnchor *        source,
        HTAnchor *        destination, 
        HTLinkType        type,
        HTMethod          method);
extern BOOL HTLink_remove (
        HTAnchor *        source,
        HTAnchor *        destination);
extern HTLink * HTLink_find (HTAnchor * source, HTAnchor * destination);
extern HTLink * HTLink_findType (HTAnchor * me, HTLinkType type);
extern BOOL HTLink_setDestination (HTLink * link, HTAnchor * dest);
extern HTAnchor * HTLink_destination (HTLink * link);
extern BOOL HTLink_setType (HTLink * link, HTLinkType type);
extern HTLinkType HTLink_type (HTLink * link);
extern BOOL HTLink_setMethod (HTLink * link, HTMethod method);
extern HTMethod HTLink_method (HTLink * link);
extern BOOL HTLink_setResult (HTLink * link, HTLinkResult result);
extern HTLinkResult HTLink_result (HTLink * link);
#endif /* HTLINK_H */
