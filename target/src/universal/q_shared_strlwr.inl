// I_strlwr body; pp_s param, PP_RETVAL(v) = return v (empty where the result is unused)
	char *iter;

	for ( iter = pp_s; *iter; iter++ )
	{
		if ( I_isupper( *iter ) )
		{
			*iter += 'a' - 'A';
		}
	}

	PP_RETVAL( pp_s );
