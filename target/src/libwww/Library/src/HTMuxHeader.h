/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTMUXHEADER_H
#define HTMUXHEADER_H
#ifdef WORDS_BIGENDIAN
#define HT_WORDSWAP(a) ( ((a) << 24) | \
		         (((a) << 8) & 0x00ff0000) | \
		         (((a) >> 8) & 0x0000ff00) | \
	                 ((unsigned int)(a) >> 24) )
#else
#define HT_WORDSWAP(a) ((a))
#endif
typedef unsigned int  HTMuxHeader;
#define MUX_LONG_LENGTH	0x80000000
#define MUX_CONTROL	0x40000000
#define MUX_FLAGS       0x3C000000
#define MUX_SYN		0x20000000
#define MUX_FIN		0x10000000
#define MUX_RST		0x08000000
#define MUX_PUSH 	0x04000000
#define MUX_OPCODES     0x3C000000
#define MUX_STRING	0x00000000
#define MUX_STACK	0x04000000
#define MUX_FRAGMENT	0x08000000
#define MUX_CREDIT	0x0C000000
#define MUX_SESSION	0x03FC0000
#define MUX_LENGTH	0x0003FFFF
#define MUX_SET_SID(sid)	((sid) << 18)
#define MUX_SET_LEN(len)	((len))
#define MUX_SET_PID             MUX_SET_LEN
#define MUX_SET_OPCODE(op)      ((op) << 26)

#define MUX_GET_SID(sid)	(((sid) & MUX_SESSION) >> 18)
#define MUX_GET_LEN(len)	((len) & MUX_LENGTH)
#define MUX_GET_PID             MUX_GET_LEN
#define MUX_GET_OPCODE(op)      ((op) & MUX_OPCODES >> 26)
#define LONG_LENGTH	0			     /* 4 or 8 bytes header? */
#define MUX_IS_LONG(hdr)     ((hdr) & MUX_LONG_LENGTH)

#define MUX_ALIGN(len)       ((len) + ((4-(len)%4) % 4))
#define MUX_LONG_ALIGN(len)  ((len) + ((8-(len)%8) % 8))
#define DEFAULT_CREDIT          4096
#endif
