/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTMLGEN_H
#define HTMLGEN_H

#include "HTStruct.h"
#include "HTFormat.h"
extern HTStructured* HTMLGenerator (HTRequest *	request,
				   void *	param,
				   HTFormat	input_format,
				   HTFormat	output_format,
				   HTStream *	output_stream);


extern HTConverter HTPlainToHTML;

#endif
