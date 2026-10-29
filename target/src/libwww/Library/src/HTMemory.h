/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTMEMORY_H
#define HTMEMORY_H

#include "HTUtils.h"
extern void* HTMemory_malloc(size_t size);
extern void* HTMemory_calloc(size_t count, size_t size);
extern void* HTMemory_realloc(void * ptr, size_t size);
extern void HTMemory_free(void* ptr);
#define HT_MALLOC(size)		HTMemory_malloc((size))
#define HT_CALLOC(count, size)	HTMemory_calloc((count), (size))
#define HT_REALLOC(ptr, size)	HTMemory_realloc((ptr), (size))
#define HT_FREE(pointer)	{HTMemory_free((pointer));((pointer))=NULL;}
typedef void HTMemoryCallback(size_t size);

extern BOOL HTMemoryCall_add (HTMemoryCallback * cbf);
extern BOOL HTMemoryCall_delete (HTMemoryCallback * cbf);
extern BOOL HTMemoryCall_deleteAll (void);
typedef void HTMemory_exitCallback(char *name, char *file, unsigned long line);

extern void HTMemory_setExit(HTMemory_exitCallback * pExit);
extern HTMemory_exitCallback * HTMemory_exit(void);
#define outofmem(file, name)	HT_OUTOFMEM(name)
#define HT_OUTOFMEM(name)	HTMemory_outofmem((name), __FILE__, __LINE__)

extern void HTMemory_outofmem(char * name, char * file, unsigned long line);
#endif /* HTMEMORY_H */
