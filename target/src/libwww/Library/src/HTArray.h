/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTARRAY_H
#define HTARRAY_H
typedef struct {
    int		size;		/* In numbers of elements	*/
    int		growby;		/* Allocation unit in elements	*/
    int		allocated;	/* Current size of *data	*/
    void **	data;		/* Pointer to malloced area or 0 */
} HTArray;
extern HTArray * HTArray_new (int grow);
extern BOOL HTArray_delete (HTArray * array);
extern BOOL HTArray_clear (HTArray * array);
extern BOOL HTArray_addObject (HTArray * array, void * object);
#define HTArray_firstObject(me, dp) \
	((me) && ((dp)=(me)->data) ? *(dp)++ : NULL)
#define HTArray_nextObject(me, dp) \
	((me) && (dp) ? *(dp)++ : NULL)
typedef int HTComparer (const void * a, const void * b);
extern BOOL HTArray_sort (HTArray * array, HTComparer * comp);
#define HTArray_data(me)	((me) ? (me)->data : NULL)
#define HTArray_size(me)	((me) ? (me)->size : -1)
#endif
