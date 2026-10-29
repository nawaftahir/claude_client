/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTTRANS_H
#define HTTRANS_H
typedef struct _HTTransport HTTransport;

typedef enum _HTTransportMode {
    HT_TP_SINGLE	= 0,		/* One single request at a time */
    HT_TP_PIPELINE	= 1,		/* Use pipelined requests */
    HT_TP_INTERLEAVE	= 2		/* Can we interleave requests? */
} HTTransportMode;

#include "HTIOStream.h"
#include "HTReq.h"
extern BOOL HTTransport_add (const char *		name,
			     HTTransportMode		mode,
			     HTInput_new *		get_input,
			     HTOutput_new *		get_output);
extern BOOL HTTransport_delete (const char * name);
extern BOOL HTTransport_deleteAll (void);
extern HTTransport * HTTransport_find (HTRequest * request, const char * name);
extern HTTransportMode HTTransport_mode (HTTransport * tp);
extern BOOL HTTransport_setMode (HTTransport * tp, HTTransportMode mode);
struct _HTTransport {
    char *		name;
    HTTransportMode	mode;			      /* Flow mode supported */
    HTInput_new *	input_new; 	     /* Input stream creation method */
    HTOutput_new *	output_new;	    /* Output stream creation method */
};
#endif /* HTTRANS_H */
