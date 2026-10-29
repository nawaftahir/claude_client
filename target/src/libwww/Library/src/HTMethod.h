/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTMETHOD_H
#define HTMETHOD_H
typedef enum {
    METHOD_INVALID	= 0x0,
    METHOD_GET		= 0x1,
    METHOD_HEAD		= 0x2,    
    METHOD_POST		= 0x4,    
    METHOD_PUT		= 0x8,
    METHOD_PATCH	= 0x10,
    METHOD_DELETE	= 0x20,
    METHOD_TRACE	= 0x40,
    METHOD_OPTIONS	= 0x80,
    METHOD_LINK		= 0x100,
    METHOD_UNLINK	= 0x200
} HTMethod;
extern HTMethod HTMethod_enum (const char * name);
extern const char * HTMethod_name (HTMethod method);
#define HTMethod_isSafe(me)	((me) & (METHOD_GET|METHOD_HEAD))
#define HTMethod_addMeta(me)  ((me) & (METHOD_TRACE | METHOD_OPTIONS))
#define HTMethod_hasEntity(me)	((me) & (METHOD_PUT | METHOD_POST))
#endif /* HTMETHOD_H */
