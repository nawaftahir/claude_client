// GetNewVariableIndexInternal with Internal2 expanded; pp_parentId, pp_name args, result in pp_result
unsigned int gnvi_p = pp_parentId;
unsigned int gnvi_n = pp_name;
unsigned int gnvi_hash = ( gnvi_n + gnvi_p ) % ( VARIABLELIST_CHILD_SIZE - 1 ) + 1;
#define i2_parentId gnvi_p
#define i2_name gnvi_n
#define i2_index gnvi_hash
#define PP_RETVAL( v ) pp_result = v
#include "scr_new_variable_index2.inl"
#undef PP_RETVAL
#undef i2_index
#undef i2_name
#undef i2_parentId
