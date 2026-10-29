/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTCHUNK_H
#define HTCHUNK_H
typedef struct _HTChunk HTChunk;

extern HTChunk * HTChunk_new (int growby);
extern void HTChunk_delete (HTChunk * ch);
extern void HTChunk_clear (HTChunk * ch);
extern void HTChunk_ensure (HTChunk * ch, int extra_size);
extern void HTChunk_putc (HTChunk * ch, char c);
extern void HTChunk_puts (HTChunk * ch, const char *str);
extern void HTChunk_putb (HTChunk * ch, const char *block, int len);
extern char * HTChunk_data (HTChunk * ch);
extern int HTChunk_size (HTChunk * ch);
extern BOOL HTChunk_truncate (HTChunk * ch, int size);
extern BOOL HTChunk_setSize (HTChunk * ch, int size);
extern void HTChunk_terminate (HTChunk * ch);
extern HTChunk * HTChunk_fromCString	(char * str, int grow);
extern char * HTChunk_toCString		(HTChunk * ch);
extern HTChunk * HTChunk_fromBuffer (char * buf, int buflen, int size, int grow);
#define HTChunkCreate(growby) HTChunk_new(growby)
#define HTChunkFree(ch)       HTChunk_delete(ch)
#define HTChunkClear(ch)      HTChunk_clear(ch)
#define HTChunkEnsure(ch, s)  HTChunk_ensure((ch), (s))
#define HTChunkPutc(ch, c)    HTChunk_putc((ch), (c))
#define HTChunkPuts(ch, str)  HTChunk_puts((ch), (str))
#define HTChunkTerminate(ch)  HTChunk_terminate(ch)
#define HTChunkData(ch)       HTChunk_data(ch)
#define HTChunkSize(ch)       HTChunk_size(ch)
#endif
