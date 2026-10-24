// Stand-in for client code that passes these by pointer (renderer imports, callbacks)
#include "../qcommon/qcommon.h"
#include "../server/server.h"
#include "../bgame/bg_public.h"
#include "../qcommon/cm_public.h"
#include "../qcommon/cmd.h"
#include "../universal/com_memory.h"
#include "../universal/com_files.h"
#include "../universal/dvar.h"
#include "../xanim/xanim_public.h"
#include "../game/g_shared.h"
#include "../qcommon/netchan.h"
void G_LogPrintf( const char *fmt, ... );
