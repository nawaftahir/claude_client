/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTHISTORY_H
#define HTHISTORY_H

#include "HTAnchor.h"
typedef struct _HTHistory HTHistory;

extern HTHistory *HTHistory_new		(void);
extern BOOL HTHistory_delete		(HTHistory *old);
extern BOOL HTHistory_record		(HTHistory *hist, HTAnchor *cur);
extern BOOL HTHistory_replace		(HTHistory *hist, HTAnchor *cur);
extern BOOL HTHistory_removeLast 	(HTHistory *hist);
extern BOOL HTHistory_removeFrom 	(HTHistory *hist, int pos);
extern int HTHistory_count		(HTHistory *hist);
extern int HTHistory_position		(HTHistory *hist);
extern HTAnchor * HTHistory_recall 	(HTHistory *hist, int pos);
extern HTAnchor * HTHistory_find 	(HTHistory *hist, int pos);
extern HTAnchor * HTHistory_list	(HTHistory *hist, int pos);
extern BOOL HTHistory_canBacktrack 	(HTHistory *hist);
extern HTAnchor * HTHistory_backtrack 	(HTHistory *hist);
extern HTAnchor * HTHistory_back 	(HTHistory *hist);
extern BOOL HTHistory_canForward 	(HTHistory *hist);
extern HTAnchor * HTHistory_forward 	(HTHistory *hist);

#endif /* HTHISTORY_H */
