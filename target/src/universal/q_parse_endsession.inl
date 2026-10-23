// Com_EndParseSession body; pp_ params, PP_RETURN = return or a jump past the expansion
extern const char *const com_endParseSessionUnderflow;
	ParseThreadInfo *parse;

	parse = Com_GetParseThreadInfo();

	if ( parse->parseInfoNum == 0 )
	{
		Com_Error( ERR_FATAL, com_endParseSessionUnderflow );
	}

	parse->parseInfoNum--;
