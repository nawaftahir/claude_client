// GetVariableIndexInternal with Internal2 expanded; pp_parentId, pp_name args, result in pp_result
unsigned int gvi_p = pp_parentId;
unsigned int gvi_n = pp_name;
unsigned int gvi_hash = ( gvi_n + gvi_p ) % ( VARIABLELIST_CHILD_SIZE - 1 ) + 1;
pp_result = FindVariableIndexInternal( gvi_n, gvi_hash );
if ( !pp_result )
{
#define i2_parentId gvi_p
#define i2_name gvi_n
#define i2_index gvi_hash
#define PP_RETVAL( v ) pp_result = v
#include "scr_new_variable_index2.inl"
#undef PP_RETVAL
#undef i2_index
#undef i2_name
#undef i2_parentId
}
