#include "../qcommon/qcommon.h"
#include "com_sndalias.h"

g_sa_t g_sa;

static inline int Com_SoundAliasRandom()
{
	g_sa.randSeed = g_sa.randSeed * 214013 + 2531011;
	return (g_sa.randSeed >> 16) & 0x7fff;
}

int Com_SoundAliasHash( const char *name )
{
	int hash;

	hash = 0;
	while ( *name )
	{
		hash = hash * 31337 + tolower(*name);
		name++;
	}
	return hash & 0x3ff;
}

snd_alias_list_t *Com_FindSoundAlias( const char *name )
{
	int hash;
	snd_alias_list_t *alias;

	if ( !name )
	{
		return NULL;
	}

	hash = Com_SoundAliasHash(name);

	for ( alias = g_sa.hash[hash]; alias; alias = alias->next )
	{
		if ( !I_stricmp(name, alias->aliasName) )
		{
			return alias;
		}
	}

	return NULL;
}

int SND_GetAliasOffset( const snd_alias_t *alias )
{
	snd_alias_list_t *aliasList;
	snd_alias_t *aliasOffset;
	int i;

	aliasList = Com_FindSoundAlias(alias->aliasName);
	aliasOffset = aliasList->head;
	for ( i = 0; i < aliasList->count; i++ )
	{
		if ( aliasOffset == alias )
		{
			return i;
		}
		aliasOffset++;
	}
	return 0;
}

snd_alias_t *SND_GetAliasWithOffset( const char *name, int offset )
{
	snd_alias_list_t *aliasList;
	snd_alias_t *alias;
	int i;

	aliasList = Com_FindSoundAlias(name);
	alias = aliasList->head;
	for ( i = 0; i < aliasList->count; i++ )
	{
		if ( i == offset )
		{
			return alias;
		}
		alias++;
	}
	if ( aliasList->count )
	{
		return aliasList->head;
	}
	Com_Error(ERR_DROP, "SND_GetAliasWithOffset: could not find sound alias '%s' with offset %d", name, offset);
	return NULL;
}

bool Com_AddAliasList( const char *name, snd_alias_list_t *aliasList )
{
	int hash;
	snd_alias_list_t *alias;

	hash = Com_SoundAliasHash(name);
	for ( alias = g_sa.hash[hash]; alias; alias = alias->next )
	{
		if ( !I_stricmp(name, alias->aliasName) )
		{
			return false;
		}
	}
	aliasList->next = g_sa.hash[hash];
	g_sa.hash[hash] = aliasList;
	return true;
}

void Com_DuplicateSoundAlias( const snd_alias_list_t *aliasCopy, const char *name )
{
	int hash;
	snd_alias_list_t *aliasList;
	const char *aliasName;
	char *nameCopy;

	hash = Com_SoundAliasHash(name);
	for ( aliasList = g_sa.hash[hash]; aliasList; aliasList = aliasList->next )
	{
		if ( I_stricmp(name, aliasList->aliasName) )
		{
			continue;
		}
		aliasName = aliasList->aliasName;
		*aliasList = *aliasCopy;
		aliasList->aliasName = aliasName;
		return;
	}
	aliasList = (snd_alias_list_t *)Com_AllocSoundMemory(sizeof(snd_alias_list_t), "Com_DuplicateSoundAlias", 13);
	*aliasList = *aliasCopy;
	nameCopy = (char *)Com_AllocSoundMemory(strlen(name) + 1, "Com_DuplicateSoundAlias", 13);
	strcpy(nameCopy, name);
	aliasList->aliasName = nameCopy;
	aliasList->next = g_sa.hash[hash];
	g_sa.hash[hash] = aliasList;
}

const char *Com_GetSoundFileName( const snd_alias_t *alias )
{
	return alias->soundFile->soundName;
}

void *Com_GetSoundFileMem( const snd_alias_t *alias )
{
	return alias->soundFile->fileMem;
}

void Com_InitVolumeFalloffCurves()
{
	char **fileList;
	int fileCount;
	int i;
	char *name;

	memset(g_sa.volumeFalloffCurves, 0, sizeof(g_sa.volumeFalloffCurves));
	Com_InitDefaultSoundAliasVolumeFalloffCurve(&g_sa.volumeFalloffCurves[0]);
	fileList = FS_ListFiles("soundaliases", "vfcurve", FS_LIST_PURE_ONLY, &fileCount, 10);
	if ( fileCount > MAX_VOLUMEFALLOFFCURVES - 1 )
	{
		Com_Error(ERR_DROP, "\x15Snd_Alias Curve initialization: '.vfcurve' file count (%d) exceeds maximum (%d)", fileCount, MAX_VOLUMEFALLOFFCURVES - 1);
	}
	for ( i = 0; i < fileCount; i++ )
	{
		name = g_sa.volumeFalloffCurveNames[i + 1];
		I_strncpyz(name, fileList[i], strlen(fileList[i]) - 7);
		if ( !Com_LoadVolumeFalloffCurve(name, &g_sa.volumeFalloffCurves[i + 1]) )
		{
			Com_Error(ERR_FATAL, "\x15" "Failed to load sndcurve file '%s'", fileList[i]);
		}
	}
	FS_FreeFileList(fileList, 10);
	g_sa.curvesInitialized = 1;
}

void Com_LoadSoundAliases( const char *loadspec, const char *loadspecCurGame, snd_alias_system_t system )
{
	char **fileList;
	int fileCount;
	int i;
	char mapName[64];
	int tempMemory;

	if ( !I_strnicmpInline(loadspec, "maps/mp/", 8) )
	{
		Com_StripExtension(loadspec + 8, mapName);
	}
	else if ( !I_strnicmpInline(loadspec, "maps/", 5) )
	{
		Com_StripExtension(loadspec + 5, mapName);
	}
	else
	{
		strcpy(mapName, loadspec);
	}
	strlwr(mapName);
	if ( system != SASYS_CGAME || !com_sv_running->current.boolean )
	{
		fileList = FS_ListFiles("soundaliases", "csv", FS_LIST_PURE_ONLY, &fileCount, 10);
		if ( !fileCount )
		{
			Com_Printf("WARNING: can't find any sound alias files (soundaliases/*.csv)\n");
			return;
		}
		tempMemory = Hunk_HideTempMemory();
		Com_InitSoundAlias();
		for ( i = 0; i < fileCount; i++ )
		{
			Com_LoadSoundAliasFile(mapName, loadspecCurGame, fileList[i]);
		}
		Com_MakeSoundAliasesPermanent(&g_sa.aliasInfo[system], &g_sa.soundFileInfo[system]);
		Hunk_ClearTempMemoryInternal();
		Hunk_ShowTempMemory(tempMemory);
		FS_FreeFileList(fileList, 10);
	}
	else
	{
		g_sa.aliasInfo[SASYS_CGAME] = g_sa.aliasInfo[SASYS_GAME];
		g_sa.soundFileInfo[SASYS_CGAME] = g_sa.soundFileInfo[SASYS_GAME];
	}
	g_sa.initialized[system] = 1;
}

void Com_UnloadSoundAliases( snd_alias_system_t system )
{
	if ( !g_sa.initialized[system] )
	{
		return;
	}
	if ( g_sa.aliasInfo[system].head )
	{
		g_sa.aliasInfo[system].head = NULL;
		g_sa.aliasInfo[system].count = 0;
		memset(g_sa.hash, 0, sizeof(g_sa.hash));
	}
	g_sa.initialized[system] = 0;
}

const char *Com_GetSoundAliasName( const char *name )
{
	snd_alias_list_t *aliasList;

	aliasList = Com_FindSoundAlias(name);
	if ( !aliasList )
	{
		return NULL;
	}
	return aliasList->aliasName;
}

int Com_GetSoundAliasRandomSeed()
{
	return g_sa.randSeed;
}

void Com_SetSoundAliasRandomSeed( int seed )
{
	g_sa.randSeed = seed;
}

snd_alias_t *Com_PickSoundAliasFromList( snd_alias_list_t *aliasList )
{
	snd_alias_t *head;
	snd_alias_t *alias;
	snd_alias_t *select;
	float totalProb;
	int maxSequence;
	int i;

	if ( !aliasList )
	{
		return NULL;
	}
	head = aliasList->head;
	select = head;
	totalProb = head->probability;
	maxSequence = head->sequence;
	alias = head;
	i = 0;
	while ( ++i != aliasList->count )
	{
		alias++;
		totalProb += alias->probability;
		if ( Com_SoundAliasRandom() * totalProb < alias->probability * -2147483648.0f )
		{
			select = alias;
		}
		if ( maxSequence < alias->sequence )
		{
			maxSequence = alias->sequence;
		}
	}
	if ( aliasList->count > 2 && maxSequence == select->sequence )
	{
		totalProb = 0;
		alias = head;
		for ( i = 0; i < aliasList->count; alias++, i++ )
		{
			if ( maxSequence == alias->sequence )
			{
				continue;
			}
			totalProb += alias->probability;
			if ( Com_SoundAliasRandom() * totalProb < alias->probability * -2147483648.0f )
			{
				select = alias;
			}
		}
	}
	select->sequence = maxSequence + 1;
	return select;
}

snd_alias_t *Com_PickSoundAlias( const char *aliasName )
{
	snd_alias_list_t *aliasList;

	aliasList = Com_FindSoundAlias(aliasName);
	return Com_PickSoundAliasFromList(aliasList);
}

// the dedicated server has no custom falloff curves
SndCurve *Com_RegisterSoundAliasVolumeFalloffCurve( const char *filename, const char *sourceFile )
{
	int curveIndex;

	return NULL;
}

SndCurve *Com_GetDefaultSoundAliasVolumeFalloffCurve()
{
	return g_sa.volumeFalloffCurves;
}

void *Com_AllocateTempSoundMemory( int size, const char *name )
{
	return Hunk_AllocateTempMemoryInternal(size);
}

void *Com_AllocSoundMemory( int size, const char *name, int type )
{
	return Hunk_AllocInternal(size);
}
