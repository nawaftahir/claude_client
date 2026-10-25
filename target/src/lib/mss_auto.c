/* MSS_auto_cleanup: Miles mss.h startup hook, a plain (non-LTCG) object inside mss32.lib.
   Source: Miles SDK 6.5 win/cleanup.c (github.com/sigmaco/milesss-v6.5c). */
#include <stdlib.h>

__declspec(dllimport) void __stdcall AIL_shutdown(void);

static void __cdecl MSS_cleanup(void)
{
  AIL_shutdown();
};

int __cdecl MSS_auto_cleanup(void)
{
  atexit(MSS_cleanup);
  return(0);
}
