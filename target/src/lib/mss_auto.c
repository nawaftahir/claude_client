/* MSS_auto_cleanup: Miles mss.h startup hook, a plain (non-LTCG) object inside mss32.lib. */
#include <stdlib.h>

__declspec(dllimport) void __stdcall AIL_shutdown(void);

/* atexit target (retail 0x541970, name unknown): tail jump through the import */
static void __cdecl MSS_atexit_shutdown(void)
{
	AIL_shutdown();
}

int __cdecl MSS_auto_cleanup(void)
{
	atexit(MSS_atexit_shutdown);
	return 0;
}
