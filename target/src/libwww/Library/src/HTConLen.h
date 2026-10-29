/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTCONLEN_H
#define HTCONLEN_H
extern HTStream * HTBuffer_new		(HTStream *	target,
					 HTRequest *	request,
					int		max_size);
extern HTStream * HTContentCounter	(HTStream *	target,
					 HTRequest *	request,
					 int		max_size);
extern HTStream * HTDelayBuffer    	(HTStream *	target,
					 int		max_size);
extern HTStream * HTPipeBuffer    	(HTStream *	target,
					 int		max_size);
#endif /* HTCONLEN_H */
