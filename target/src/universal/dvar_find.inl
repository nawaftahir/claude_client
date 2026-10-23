// Dvar_FindMalleableVar body with I_stricmp expanded; pp_name in, pp_dvar out, PP_LABEL unique per site
	for ( pp_dvar = dvarHashTable[generateHashValue( pp_name )]; pp_dvar; pp_dvar = pp_dvar->hashNext )
	{
		const char *s0 = pp_name;
		const char *s1 = pp_dvar->name;
		int n = 0x7fffffff;
		int cmp;

#define PP_RETVAL( v ) { cmp = ( v ); goto PP_LABEL; }
#include "q_shared_strnicmp.inl"
#undef PP_RETVAL
PP_LABEL:
		if ( !cmp )
			break;
	}
