#include "../qcommon/qcommon.h"

static bool shouldQuitOnError = false;

/*
============
RefreshQuitOnErrorCondition
============
*/
static void RefreshQuitOnErrorCondition()
{
	if ( Dvar_IsSystemActive() )
		shouldQuitOnError = Dvar_GetBool("QuitOnError") || Dvar_GetInt("r_vc_compile") == 2;
}

/*
============
QuitOnError
============
*/
bool QuitOnError()
{
	RefreshQuitOnErrorCondition();
	return shouldQuitOnError;
}
