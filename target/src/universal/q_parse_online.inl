// Com_ParseOnLine body; pp_data_p param, PP_RETVAL(v) = return v or store v and jump past the expansion
	ParseThreadInfo *parse;
	parseInfo_t *pi;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];

	if ( pi->ungetToken )
	{
		pi->ungetToken = false;

		if ( !pi->spaceDelimited )
		{
			PP_RETVAL( pi->token );
		}

		*pp_data_p = pi->backup_text;
		pi->lines = pi->backup_lines;
	}

	PP_RETVAL( Com_ParseExt( pp_data_p, qfalse ) );
