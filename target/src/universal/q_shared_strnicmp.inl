// I_strnicmp body; s0, s1, n are lvalues it advances, PP_RETVAL(v) = return v
	int c0;
	int c1;

	do
	{
		c0 = *s0++;
		c1 = *s1++;

		if ( !n-- )
		{
			PP_RETVAL( 0 ); // strings are equal until end point
		}

		if ( c0 != c1 )
		{
			if ( I_islower( c0 ) )
			{
				c0 -= 'a' - 'A';
			}

			if ( I_islower( c1 ) )
			{
				c1 -= 'a' - 'A';
			}

			if ( c0 != c1 )
			{
				PP_RETVAL( c0 < c1 ? -1 : 1 );
			}
		}
	}
	while ( c0 );

	PP_RETVAL( 0 ); // strings are equal
