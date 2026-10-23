// Com_SkipRestOfLine body; pp_ params, PP_RETURN = return or a jump past the expansion
	const char *p;
	int c;
	ParseThreadInfo *parse;
	parseInfo_t *pi;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];
	p = *pp_data;

	if ( !p )
	{
		PP_RETURN;
	}

	while ( ( c = *p ) != 0 )
	{
		p++;

		if ( c == '\n' )
		{
			pi->lines++;
			break;
		}
	}

	*pp_data = p;
