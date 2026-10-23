// I_stricmp body; pp_s0, pp_s1 params, result in pp_r
	{
		const char *s0 = pp_s0;
		const char *s1 = pp_s1;
		int c0;
		int c1;
		int n = 0x7FFFFFFF;

		pp_r = 0;
		do {
			c0 = *s0++;
			c1 = *s1++;
			if (!n--)
				break;
			if (c0 != c1) {
				if (I_islower(c0))
					c0 -= 'a' - 'A';
				if (I_islower(c1))
					c1 -= 'a' - 'A';
				if (c0 != c1) {
					pp_r = c0 < c1 ? -1 : 1;
					break;
				}
			}
		} while (c0);
	}
