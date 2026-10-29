/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTCACHE_H
#define HTCACHE_H

#include "WWWLib.h"
extern BOOL HTCacheInit (const char * cache_root, int size);
extern BOOL HTCacheTerminate (void);
extern void HTCacheMode_setEnabled (BOOL mode);
extern BOOL HTCacheMode_enabled (void);
extern void HTCacheMode_setProtected (BOOL mode);
extern BOOL HTCacheMode_protected (void);
extern char * HTCacheMode_getRoot (void);
extern BOOL HTCacheMode_setMaxSize (int size);
extern int  HTCacheMode_maxSize    (void);
extern BOOL HTCacheMode_setMaxCacheEntrySize (int size);
extern int HTCacheMode_maxCacheEntrySize (void);
extern void HTCacheMode_setDefaultExpiration (const int exp_time);
extern int HTCacheMode_DefaultExpiration (void);
typedef enum _HTExpiresMode {
    HT_EXPIRES_IGNORE = 0,
    HT_EXPIRES_NOTIFY,
    HT_EXPIRES_AUTO
} HTExpiresMode;

extern void HTCacheMode_setExpires (HTExpiresMode mode);
extern HTExpiresMode HTCacheMode_expires (void);
typedef enum _HTDisconnectedMode {
    HT_DISCONNECT_NONE     = 0,
    HT_DISCONNECT_NORMAL   = 1,
    HT_DISCONNECT_EXTERNAL = 2
} HTDisconnectedMode;

extern void HTCacheMode_setDisconnected (HTDisconnectedMode mode);
extern HTDisconnectedMode HTCacheMode_disconnected (void);
extern BOOL HTCacheMode_isDisconnected (HTReload mode);
extern BOOL HTCacheIndex_read (const char * cache_root);
extern BOOL HTCacheIndex_write (const char * cache_root);
typedef struct _HTCache HTCache;
extern HTConverter HTCacheWriter, HTCacheAppend;
extern BOOL HTCache_writeMeta (HTCache * cache, HTRequest * request,
                               HTResponse * response);
extern BOOL HTCache_updateMeta (HTCache * cache, HTRequest * request,
                                HTResponse * response);
extern HTCache * HTCache_touch (HTRequest * request, HTResponse * response,
                                HTParentAnchor * anchor);
extern HTProtCallback HTLoadCache;
extern BOOL HTCache_remove (HTCache * cache);
extern BOOL HTCache_deleteAll (void);
extern BOOL HTCache_flushAll (void);
extern HTCache * HTCache_find (HTParentAnchor * anchor, char * default_name);
extern HTReload HTCache_isFresh (HTCache * me, HTRequest * request);
extern BOOL HTCache_addHit (HTCache * cache);
extern char * HTCache_name (HTCache * cache);
extern BOOL HTCache_getLock     (HTCache * cache, HTRequest * request);
extern BOOL HTCache_breakLock   (HTCache * cache, HTRequest * request);
extern BOOL HTCache_hasLock     (HTCache * cache);
extern BOOL HTCache_releaseLock (HTCache * cache);
#endif
