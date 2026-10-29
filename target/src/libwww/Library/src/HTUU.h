/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTUU_H
#define HTUU_H


extern int HTUU_encode (unsigned char * bufin, unsigned int nbytes,
			char * bufcoded);

extern int HTUU_decode (char * bufcoded, unsigned char * bufplain,
			int outbufsize);

#endif
