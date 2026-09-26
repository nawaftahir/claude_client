#include "common_types.h"
#include "imports.h"
#include <string.h>

// On-screen performance warning icons (gfx/2d/warning@*.jpg), one slot per source.

#define MAX_STATMON_ENTRIES 7

extern const dvar_t *com_statmon;
extern clientStatic_t cls;
extern refexport_t re;
extern int Sys_Milliseconds(void);
extern void Com_Error(int code, const char *fmt, ...);

static statmonitor_t stats[MAX_STATMON_ENTRIES];
static int statCount;

void StatMon_Warning(int type, int duration, const char *materialName)
{
	if (!com_statmon->current.enabled)
		return;

	if (type < 0 || type >= MAX_STATMON_ENTRIES)
		Com_Error(1, "\x15StatMon_UpdateEntry: invalid entry '%i'\n", type);

	stats[type].endtime = Sys_Milliseconds() + duration;

	if (!stats[type].material && cls.rendererStarted)
		stats[type].material = re.RegisterMaterial(materialName, 0x30, 1);

	if (type >= statCount)
		statCount = type + 1;
}

void StatMon_GetStatsArray(const statmonitor_t **array, int *count)
{
	*array = stats;
	*count = statCount;
}

void StatMon_Reset(void)
{
	memset(stats, 0, sizeof(stats));
	statCount = 0;
}
