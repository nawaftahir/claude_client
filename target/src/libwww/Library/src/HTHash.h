#ifndef HTHASH_H
#define HTHASH_H

#include "HTList.h"

typedef struct _HTHashtable HTHashtable;

struct _HTHashtable {
    void **table;
    int count;
    int size;
};
    
typedef struct _keynode keynode;

struct _keynode {
    char *key;
    void *object;
};
extern HTHashtable *	HTHashtable_new	(int size);

extern BOOL	HTHashtable_delete (HTHashtable *me);
extern BOOL HTHashtable_addObject (HTHashtable *me, const char *key, void *newObject);
extern BOOL HTHashtable_removeObject (HTHashtable *me, const char *key);
extern void *	HTHashtable_object (HTHashtable * me, const char *key);
extern int	HTHashtable_count  (HTHashtable *me);
extern BOOL	HTHashtable_walk (HTHashtable *me, int (*walkFunc)(HTHashtable *, char *, void *));
extern HTArray * HTHashtable_keys  (HTHashtable *me);
extern void HTHashtable_print (HTHashtable *me);
#endif
