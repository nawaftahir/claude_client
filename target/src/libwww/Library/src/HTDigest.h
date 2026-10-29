/*
**	(c) COPYRIGHT W3C, INRIA 1998.
**	Please first read the full copyright statement in the file COPYRIGHT.
*/
#ifndef HTDigest_H
#define HTDigest_H
/* Library include files */
#include "WWWLib.h"
/* add the MD algorithm header files here below */
#include "md5.h"
#define HTDaMD5 1
typedef struct _HTDigestContext {
  int algorithm;          
    union { 
           MD5_CTX md5;
    } context;
} HTDigestContext;
extern BOOL HTDigest_init (HTDigestContext *context, int digesttype);
extern BOOL HTDigest_update (HTDigestContext *context, char *input, unsigned int inputLen);
extern BOOL HTDigest_final (unsigned char *digest, HTDigestContext *context);
#endif	/* NOT HTDigest_H */
