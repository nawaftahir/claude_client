/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTTPUTIL_H
#define HTTPUTIL_H
typedef enum _HTTPVersion {
    HTTP = 0,
    HTTP_09,		
    HTTP_10,
    HTTP_11,
    HTTP_12
} HTTPVersion;
#define HTTP_VERSION_10	"HTTP/1.0"
#define HTTP_VERSION	"HTTP/1.1"
#endif
