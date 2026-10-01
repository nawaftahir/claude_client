#include <ctype.h>
#include "common_types.h"
#include "imports.h"
#include "bytematch.h"
#include "cod2_feature_config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
extern const vec4_t colorBlack;
extern const vec4_t colorLtGrey;
extern const vec4_t colorRed;
extern const vec4_t colorWhite;
extern const vec4_t colorYellow;
extern clientActive_t clients[1];
extern cg_t cgArray[1];
/* see cg_local.h: `cg` is a const pointer to the real object, so `cg->field` folds
 * into a single absolute access. */
static cg_t * const cg = &cgArray[0];
extern LegacyHacks *legacyHacks;
extern LegacyHacks legacyHacksData;

/* dvar globals */
extern const dvar_t *cl_voice;
extern const dvar_t *com_playerProfile;
extern dvar_t *sv_voice;

extern serverStatusDvar_t serverStatusDvars[23];

char etaText[18] = { 0x45, 0x58, 0x45, 0x5f, 0x45, 0x53, 0x54, 0x5f, 0x54, 0x49, 0x4d, 0x45, 0x5f, 0x4c, 0x45, 0x46, 0x54, 0x0 };
int tleEstimates[80] = { 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c };
char xferText[15] = { 0x45, 0x58, 0x45, 0x5f, 0x54, 0x52, 0x41, 0x4e, 0x53, 0x5f, 0x52, 0x41, 0x54, 0x45, 0x0 };

const char *MonthAbbrev[12] = { (const char *)&str_002a9d68, (const char *)&str_002a9d80, (const char *)&str_002a9d98, (const char *)&str_002a9dac, (const char *)&str_002a9dc0, (const char *)&str_002a9dd4, (const char *)&str_002a9de8, (const char *)&str_002a9dfc, (const char *)&str_002a9e14, (const char *)&str_002a9e2c, (const char *)&str_002a9e44, (const char *)&str_002a9e5c };
const char *netSources[3] = { (const char *)&str_002a9e74, (const char *)&str_002a9e80, (const char *)&str_002a9e90 };

int lastColumn = 0xffffffff;

#ifndef __EMSCRIPTEN__
#    define UI_REGPARM1 __attribute__((regparm(1)))
#else
#    define UI_REGPARM1
#endif

void diag_drawtext(const char *text, void *font, float x, float y, float scale)
{
    (void)text;
    (void)font;
    (void)x;
    (void)y;
    (void)scale;
}

extern char *va( const char *format, ... );
extern dvar_t *loc_warnings;
extern dvar_t *loc_warningsAsErrors;
extern const dvar_t *ui_smallFont;
extern const dvar_t *ui_bigFont;
extern const dvar_t *ui_extraBigFont;
extern const dvar_t *ui_netSource;
extern const dvar_t *ui_currentMap;
extern const dvar_t *ui_netGameType;
extern const dvar_t *ui_joinGameType;
extern const dvar_t *ui_dedicated;
extern const dvar_t *ui_currentNetMap;
extern const dvar_t *ui_browserShowFull;
extern const dvar_t *ui_browserShowEmpty;
extern const dvar_t *ui_browserShowPassword;
extern const dvar_t *ui_browserShowNoPassword;
extern const dvar_t *ui_browserShowPure;
#if COD2_IS_PATCH_13

const dvar_t *ui_browserShowPunkBuster;
#endif
extern const dvar_t *ui_browserFriendlyfire;
extern const dvar_t *ui_browserKillcam;
extern const dvar_t *ui_serverStatusTimeOut;
extern struct sharedUiInfo_t sharedUiInfo;
extern uiInfo_t *uiInfo;
extern uiInfo_t uiInfoArray[1];

COD2_ASSERT_FIELD(struct sharedUiInfo_t, mapCount, 4944);
COD2_ASSERT_FIELD(struct sharedUiInfo_t, mapList, 4948);
COD2_ASSERT_FIELD(struct mapInfo, active, 0xa0);
COD2_ASSERT_SIZE (struct mapInfo, 0xa4);
COD2_ASSERT_FIELD(struct sharedUiInfo_t, playerClientNums, 0x1048);
COD2_ASSERT_FIELD(struct sharedUiInfo_t, pendingServerStatus, 0x1b9f0);
COD2_ASSERT_FIELD(struct pendingServerStatus_t, server, 0x4);
COD2_ASSERT_FIELD(struct pendingServer_t, valid, 0x88);
COD2_ASSERT_SIZE (struct pendingServer_t, 0x8c);
COD2_ASSERT_FIELD(mapInfo, mapName, 0);
COD2_ASSERT_FIELD(mapInfo, active, 160);
COD2_ASSERT_SIZE(mapInfo, 164);
COD2_ASSERT_FIELD(struct sharedUiInfo_t, assets, 0);
COD2_ASSERT_FIELD(CachedAssets_t, whiteMaterial, 32);
COD2_ASSERT_FIELD(struct sharedUiInfo_t, serverHardwareIconList, 25940);
COD2_ASSERT_FIELD(struct sharedUiInfo_t, joinGameTypes, 4688);
COD2_ASSERT_FIELD(gameTypeInfo, gameType, 0);
COD2_ASSERT_FIELD(struct sharedUiInfo_t, serverStatus, 27528);
COD2_ASSERT_FIELD(serverStatus_t, sortKey, 1112);
COD2_ASSERT_FIELD(serverStatus_t, sortDir, 1116);
COD2_ASSERT_FIELD(serverStatus_t, currentServer, 1128);
COD2_ASSERT_FIELD(serverStatus_t, displayServers, 1132);
COD2_ASSERT_FIELD(serverStatus_t, motdWidth, 81164);
COD2_ASSERT_FIELD(struct sharedUiInfo_t, serverStatusInfo, 109800);
COD2_ASSERT_FIELD(serverStatusInfo_t, numLines, 3328);
COD2_ASSERT_FIELD(struct sharedUiInfo_t, serverStatusAddress, 109736);
COD2_ASSERT_FIELD(uiInfo_t, uiDC, 0);
COD2_ASSERT_FIELD(displayContextDef_t, bias, 0);
COD2_ASSERT_FIELD(displayContextDef_t, realTime, 4);
COD2_ASSERT_FIELD(dvar_t, current, 8);

#if COD2_FEATURE_GAMEPAD
extern void UI_ConsoleMode_Init(void *info);
extern int UI_ConsoleMode_SuppressCursor(void);
extern void UI_ConsoleMode_DrawGlyphs(void *info);
extern void UI_Gamepad_RegisterMenus(uiInfo_t *info);
#endif
extern qboolean g_editingField;
extern const dvar_t *ui_gametype;
extern const dvar_t *ui_netGameTypeName;
extern const dvar_t *ui_browserShowDedicated;
extern const dvar_t *ui_browserMod;
extern const dvar_t *ui_playerProfileCount;
extern const dvar_t *ui_playerProfileSelected;
extern const dvar_t *ui_playerProfileNameNew;
static char errorString[1024];
static char info[1024];
static qboolean bypassKeyClear;
static int numclean;
static char info_00ecf960[1024];
static char clientBuff[32];
extern int lastColumn;
static int lastTime;
/* 3 duplicate `static char info_00ecf960[1024];` removed (same name -> C++ C2086; only one is referenceable) */
static int numFound;
static int numTimeOuts;
extern char dlText[16];
extern char etaText[18];
extern char xferText[15];
extern int tleEstimates[80];
static int tleIndex;
static char g_mapname[64];
static char g_gametype[64];
static Bool g_ingameMenusLoaded;
extern const char *MonthAbbrev[12];
extern const char *netSources[3];
static const serverFilter_t serverFilters[1];
static char menuBuf2[32768];
static int ui_serverFilterType;

extern void Menus_CloseAll(displayContextDef_t *dc);
extern qboolean Menus_AnyFullScreenVisible(displayContextDef_t *dc);
extern void LAN_SaveServersToCache(void);
extern qboolean Menus_OpenByName(displayContextDef_t *dc, const char *name);
extern void Menus_CloseByName(displayContextDef_t *dc, const char *p);
extern char *Cmd_Args(int startIndex);

static void Text_PaintCenter(FontHandle font, const char *text, float x, float y, float scale, const vec_t *color);
const char *UI_ReplaceConversionInt(const char *sourceString, int replaceInt);
extern Bool IsTalking(void);

extern MaterialHandle CL_RegisterMaterialNoMip(const char *name, int imageTrack);   /* was int -> truncated MaterialHandle on x64 */
extern FontHandle CL_RegisterFont(const char *name, int flags);   /* was int -> truncated FontHandle on x64 */
extern snd_alias_list_t *Com_FindSoundAlias( const char *name );
extern void CL_DrawStretchPic(float x, float y, float w, float h, int horzAlign, int vertAlign, float s0, float t0, float s1, float t1, const vec_t *color, MaterialHandle material);
extern float CL_NormalizedTextScale(FontHandle font, float scale);
extern int CL_TextWidth(const char *text, int maxChars, FontHandle font);
extern int CL_TextHeight(FontHandle font);
extern float GetRealHeightFromVirtualHeight(float scale);
extern void CalcScreenPlacement(float *x, float *y, float *xScale, float *yScale, int horzAlign, int vertAlign);
extern void CalcSplitScreenTextOffset(FontHandle font, float *y);
extern void CL_DrawTextPhysical(const char *text, int maxChars, FontHandle font, float x, float y, float xScale, float yScale, const vec_t *color, int style);
extern void CL_DrawTextPhysicalWithCursor(const char *text, int maxChars, FontHandle font, float x, float y, float xScale, float yScale, const vec_t *color, int style, int cursorPos, int cursor);
extern void GetClientState(uiClientState_t *cstate);
extern const char *CL_GetConfigString(int index);
extern const char *Info_ValueForKey( const char *s, const char *key );
extern int GetClientname(int index, char *name, int nameSize);
extern void I_strncpyz(char *dest, const char *src, int len);
extern char *I_CleanStr(char *str);
extern int I_stricmp(const char *s1, const char *s2);
extern int I_strnicmp(const char *s1, const char *s2, int n);
extern void I_strncat(char *dest, int len, const char *src);
extern int FS_FOpenFileByMode(const char *name, int *f, fsMode_t mode);
extern int FS_Read(void *buf, int len, int f);
extern void FS_FCloseFile(fileHandle_t f);
extern void Com_Printf(const char *fmt, ...);
extern MenuList *UI_LoadMenu(const char *name, int imageTrack);
extern void UI_AddMenuList(displayContextDef_t *dc, MenuList *menuList);
extern void UI_MapLoadInfo(const char *csv);
extern void UI_FillRect(float x, float y, float width, float height, int horzAlign, int vertAlign, const vec_t *color);
extern menuDef_t *Menus_FindByName(displayContextDef_t *dc, const char *name);
extern void Menu_Paint(displayContextDef_t *dc, menuDef_t *menu, qboolean full);
extern void Dvar_SetInt(dvar_t *dvar, int value);
extern void Dvar_SetString(dvar_t *dvar, const char *value);
extern void Dvar_SetIntByName(const char *name, int value);
extern void Dvar_SetBoolByName(const char *dvarName, bool value);
extern void Dvar_SetStringByName(const char *name, const char *value);
extern int Dvar_GetInt(const char *name);
extern bool Dvar_GetBool(const char *dvarName);
extern float Dvar_GetFloat(const char *name);
extern const char* Dvar_GetString(const char *dvarName);
extern const char *Dvar_GetVariantString(const char *name);
extern void Dvar_SetFloatByName(const char *name, float value);
extern dvar_t *Dvar_SetFromStringByNameFromSource(const char *name, const char *value, DvarSetSource source);
extern dvar_t* Dvar_FindVar(const char* dvarName);
extern int SEH_VerifyLanguageSelection(int lang);
extern const char *SEH_StringEd_GetString(const char *ref);
extern void Com_Error(errorParm_t level, const char *fmt, ...);
extern qboolean String_Parse(const char **p, char *out, int len);
extern int LAN_GetServerStatus(char *serverAddress, char *serverStatus, int maxLen);
extern int LAN_AddServer(int source, const char *name, const char *addr);
extern void LAN_GetServerInfo(int source, int index, char *info, int infoSize);
extern int LAN_GetServerCount(int source);
extern void LAN_ResetPings(int source);
extern int LAN_WaitServerResponse(int source);
extern int LAN_CompareServers(int source, int sortKey, int sortDir, int s1, int s2);
extern int LAN_ServerIsDirty(int source, int index);
extern int LAN_GetServerPing(int source, int index);
extern void LAN_MarkServerDirty(int source, int index, int dirty);
extern int LAN_UpdateDirtyPings(int source);
extern void LAN_GetServerAddressString(int source, int index, char *addr, int addrSize);
extern void LAN_RemoveServer(int source, const char *addr);
extern void LAN_LoadCachedServers(void);
extern e_status CIN_StopCinematic(int handle);
extern int CIN_PlayCinematic(const char *name, int x, int y, int w, int h, int flags);
extern e_status CIN_RunCinematic(int handle);
extern void CIN_SetExtents(int handle, int x, int y, int w, int h);
extern void CIN_DrawCinematic(int handle);
extern void UI_DrawHandlePic(float x, float y, float w, float h, int horzAlign, int vertAlign, const vec_t *color, MaterialHandle material);
extern void Key_SetCatcher(int catcher);
extern int Key_GetCatcher(void);
extern void Key_ClearStates(void);
extern int Menu_Count(displayContextDef_t *dc);
extern menuDef_t *Menu_GetFocused(displayContextDef_t *dc);
extern void Menu_HandleKey(displayContextDef_t *dc, menuDef_t *menu, int key, qboolean down);
extern void Menu_PaintAll(displayContextDef_t *dc);
extern void Menu_SetFeederSelection(displayContextDef_t *dc, menuDef_t *menu, int feeder, int index, const char *name);
extern bool Window_IsVisible(Window *w);
extern void Menu_Setup(displayContextDef_t *dc);
extern qboolean Display_MouseMove(displayContextDef_t *dc, void *capture, int x, int y);
extern int Display_KeyBindPending(void);
extern void SND_FadeAllSounds(float gain, int time);
extern int Sys_Milliseconds(void);
extern float Voice_GetVoiceLevel(void);
extern int Com_RealTime(qtime_t *q);
extern int FS_GetFileList(const char *path, const char *ext, FsListBehavior behavior, char *buf, int bufSize);
extern char *Com_Parse( const char *( *data_p ) );
extern const char *String_Alloc(const char *s);
extern void String_Init(void);
extern void CL_GetScreenDimensions(int *width, int *height, float *aspect);
extern dvar_t *Dvar_RegisterInt(const char *dvarName, int value, int min, int max, unsigned short flags);
extern dvar_t *Dvar_RegisterFloat(const char *dvarName, float value, float min, float max, unsigned short flags);
dvar_t *Dvar_RegisterBool(const char *dvarName, bool value, unsigned short flags);
dvar_t *Dvar_RegisterString(const char *dvarName, const char *value, unsigned short flags);
extern void UI_LoadArenas(void);
extern MenuList *UI_LoadMenus(const char *name, int imageTrack);
extern void UI_LoadSoundAliases(void);
extern void Controls_SetDefaults(void);
extern void Controls_GetConfig(void);
extern void Controls_SetConfig(int apply);
extern void CG_OwnerDraw(float x, float y, float w, float h, int horzAlign, int vertAlign, float text_x, float text_y, int ownerDraw, int ownerDrawFlags, int align, float special, FontHandle font, float scale, vec_t *color, MaterialHandle material, int textStyle);
extern const char *CG_GameTypeString(void);
extern const char *CG_GetKillerText(void);
extern const char *SEH_LocalizeTextMessage(const char *msg, const char *fmt, msgLocErrType_t errType);
extern void LerpColor(vec_t *a, vec_t *b, vec_t *c, float t);
extern void Item_SetCursorPos(itemDef_t *item, int pos);
extern int Item_ListBox_MaxScroll(itemDef_t *item);
extern void ListBox_SetStartPos(listBoxDef_t *listBox, int startPos);
extern void ListBox_SetEndPos(listBoxDef_t *listBox, int endPos);
extern void ListBox_SetCursorPos(listBoxDef_t *listBox, int cursorPos);
extern Bool CL_IsPlayerTalking(int clientIndex);
extern Bool CL_IsPlayerMuted(int clientIndex);
extern void CL_MutePlayer(int clientIndex);
extern int Int_Parse(const char **args, int *out);
extern void CLUI_GetCDKey(char *buf, int bufSize, char *checksum, int checksumSize);
extern void CLUI_SetCDKey(char *key, char *checksum);
extern int CL_CDKeyValidate(const char *key1, const char *key2);
extern void CG_DrawInformation(int loading);
extern char **FS_ListFiles(const char *path, const char *ext, FsListBehavior behavior, int *numFiles, int allocTrackType);
extern void FS_FreeFileList(char **list, int allocTrackType);
extern char *I_strupr(char *str);
extern void Com_ChangePlayerProfile(const char *profileName);
extern bool Com_DeletePlayerProfile(const char *profileName);
extern bool Com_NewPlayerProfile(const char *name);

extern void Cbuf_ExecuteText(int execWhen, const char *text);
extern int Com_sprintf(char *dest, size_t size, const char *format, ...);

void UI_AssetCache(void);
void UI_DrawSides(float x, float y, float w, float h, int horzAlign, int vertAlign, float size, const vec_t *color);
void UI_DrawTopBottom(float x, float y, float w, float h, int horzAlign, int vertAlign, float size, const vec_t *color);
int UI_TextWidth(const char *text, int maxChars, FontHandle font, float scale);
int UI_TextHeight(FontHandle font, float scale);
FontHandle UI_GetFontHandle(int fontEnum, float scale);
void UI_UpdateTime(int realtime);
void UI_Shutdown(void);
char *GetMenuBuffer(const char *filename);
qboolean Load_ScriptMenu(const char *pszMenu, int imageTrack);
static void UI_DrawMapPreview(const rectDef_t *rect, const vec_t *color, int net);
const char *UI_GetMapDisplayName(const char *pszMap);
const char *UI_GetMapDisplayNameFromPartialLoadNameMatch(const char *pszMap, int *mapLoadNameLen);
const char *UI_GetGameTypeDisplayName(const char *pszGameType);
static void UI_BuildPlayerList(void);
void UI_DrawMapLevelshot(void);
void UI_LoadIngameMenus(void);
void UI_SetMap(const char *mapname, const char *gametype);
qboolean UI_OwnerDrawVisible(int flags);
static int UI_ServersQsortCompare(const void *arg1, const void *arg2);
static int UI_PlayerProfilesQsortCompare(const void *arg1, const void *arg2);
static void UI_SelectCurrentMap(void);
qboolean UI_CheckExecKey(int key);
static void UI_VerifyLanguage(void);
const char *UI_SafeTranslateString(const char *reference);
static void UI_AddServerToFavoritesList(const char *name, const char *address);
static Bool UI_GetOpenOrCloseMenuOnDvarArgs(const char **args, const char *cmd,
                                            char *dvarName, char *testValue, char *menuName);
static int __attribute_regparm__(2) UI_GetServerStatusInfo(const char *serverAddress, serverStatusInfo_t *info);
void UI_FeederSelection(float feederID, int index);
static void UI_GetGameTypesList(void);
void UI_Pause(qboolean b);
void UI_OpenMenu_f(void);
void UI_CloseMenu_f(void);
void UI_Init(void);
void UI_KeyEvent(int key, qboolean down);
void UI_MouseEvent(int dx, int dy);
uiMenuCommand_t UI_GetActiveMenu(void);
qboolean UI_SetActiveMenu(int menu);
qboolean UI_IsFullscreen(void);
static void __attribute_regparm__(3) UI_ReadableSize(char *buf, int bufsize, int value);
float UI_GetBlurRadius(void);
qboolean UI_AnyFullScreenMenuVisible(void);
const char *UI_ReplaceConversions(const char *sourceString, ConversionArguments *arguments);
const char *UI_ReplaceConversionString(const char *sourceString, const char *replaceString);
void UI_CloseAll(void);
void UI_CloseFocusedMenu(void);
void UI_OverrideCursorPos(itemDef_t *item);
Bool UI_DrawRecordLevel(rectDef_t *rect);
MaterialHandle UI_FeederItemImage(const float feederID, int index);
void UI_DrawText(const char *text, int maxChars, FontHandle font, float x, float y, int horzAlign, int vertAlign, float scale, const vec_t *color, int style);
void UI_DrawTextWithCursor(const char *text, int maxChars, FontHandle font, float x, float y, int horzAlign, int vertAlign, float scale, const vec_t *color, int style, int cursorPos, char cursor);
static void UI_StartServerRefresh(qboolean full);
static void __attribute_regparm__(0) UI_BuildServerDisplayList(qboolean force);
static inline __attribute__((always_inline)) void UI_UpdateServerCount(void);
qboolean UI_OwnerDrawHandleKey(int ownerDraw, int flags, float *special, int key);
const char *UI_FeederItemText(float feederID, int index, int column, MaterialHandle *handle);
void UI_DrawRect(float x, float y, float width, float height, int horzAlign, int vertAlign, float size, const vec_t *color);
int UI_OwnerDrawWidth(int ownerDraw, FontHandle font, float scale);
void UI_OwnerDraw(float x, float y, float w, float h, int horzAlign, int vertAlign, float text_x, float text_y, int ownerDraw, int ownerDrawFlags, int align, float special, FontHandle font, float scale, vec_t *color, MaterialHandle material, int textStyle);
static void BM_NOINLINE UI_BuildFindPlayerList(void);
static void UI_REGPARM1 UI_BuildServerStatus(int force);
void UI_Refresh(void);
void UI_RunMenuScript(const char **args);
int UI_FeederCount(float feederID);
static const char *UI_SelectedMap(int index, int *actual);
static void UI_DisplayDownloadInfo(const char *downloadName, float centerPoint, float yStart, FontHandle font, float scale);
void UI_DrawConnectScreen(void);

void UI_AssetCache(void)
{
    sharedUiInfo.assets.whiteMaterial = CL_RegisterMaterialNoMip("white", 3);
    sharedUiInfo.assets.scrollBar = CL_RegisterMaterialNoMip("ui/assets/scrollbar.tga", 3);
    sharedUiInfo.assets.scrollBarArrowDown = CL_RegisterMaterialNoMip("ui/assets/scrollbar_arrow_dwn_a.tga", 3);
    sharedUiInfo.assets.scrollBarArrowUp = CL_RegisterMaterialNoMip("ui/assets/scrollbar_arrow_up_a.tga", 3);
    sharedUiInfo.assets.scrollBarArrowLeft = CL_RegisterMaterialNoMip("ui/assets/scrollbar_arrow_left.tga", 3);
    sharedUiInfo.assets.scrollBarArrowRight = CL_RegisterMaterialNoMip("ui/assets/scrollbar_arrow_right.tga", 3);
    sharedUiInfo.assets.scrollBarThumb = CL_RegisterMaterialNoMip("ui/assets/scrollbar_thumb.tga", 3);
    sharedUiInfo.assets.sliderBar = CL_RegisterMaterialNoMip("ui/assets/slider2.tga", 3);
    sharedUiInfo.assets.sliderThumb = CL_RegisterMaterialNoMip("ui/assets/sliderbutt_1", 3);
    sharedUiInfo.assets.cursor = CL_RegisterMaterialNoMip("$cursor", 0);
    sharedUiInfo.assets.bigFont = CL_RegisterFont("$bigfont", 0);
    sharedUiInfo.assets.smallFont = CL_RegisterFont("$smallfont", 0);
    sharedUiInfo.assets.consoleFont = CL_RegisterFont("$consolefont", 0);
    sharedUiInfo.assets.boldFont = CL_RegisterFont("$boldfont", 0);
    sharedUiInfo.assets.textFont = CL_RegisterFont("$font", 0);
    sharedUiInfo.assets.extraBigFont = CL_RegisterFont("$extrabigfont", 0);
    sharedUiInfo.assets.itemFocusSound = (snd_alias_list_t *)Com_FindSoundAlias("$itemfocussound");
}

void UI_DrawSides(float x, float y, float w, float h, int horzAlign, int vertAlign, float size, const vec_t *color)
{
    CL_DrawStretchPic(x, y, size, h, horzAlign, vertAlign, 0, 0, 0, 0, color, sharedUiInfo.assets.whiteMaterial);
    CL_DrawStretchPic(x + w - size, y, size, h, horzAlign, vertAlign, 0, 0, 0, 0, color, sharedUiInfo.assets.whiteMaterial);
}

void UI_DrawTopBottom(float x, float y, float w, float h, int horzAlign, int vertAlign, float size, const vec_t *color)
{
    CL_DrawStretchPic(x, y, w, size, horzAlign, vertAlign, 0, 0, 0, 0, color, sharedUiInfo.assets.whiteMaterial);
    CL_DrawStretchPic(x, y + h - size, w, size, horzAlign, vertAlign, 0, 0, 0, 0, color, sharedUiInfo.assets.whiteMaterial);
}

int UI_TextWidth(const char *text, int maxChars, FontHandle font, float scale)
{
    float actualScale = CL_NormalizedTextScale(font, scale);
    return (int)((float)CL_TextWidth(text, maxChars, font) * actualScale);
}

int UI_TextHeight(FontHandle font, float scale)
{
    float actualScale = CL_NormalizedTextScale(font, scale);
    return (int)((float)CL_TextHeight(font) * actualScale);
}

FontHandle UI_GetFontHandle(int fontEnum, float scale)
{
    if (fontEnum == 2)
        return sharedUiInfo.assets.bigFont;
    if (fontEnum == 3)
        return sharedUiInfo.assets.smallFont;
    if (fontEnum == 5)
        return sharedUiInfo.assets.consoleFont;

    scale = GetRealHeightFromVirtualHeight(scale);

    if (fontEnum == 4) {
        if (scale <= ui_smallFont->current.value)
            return sharedUiInfo.assets.smallFont;
        if (scale >= ui_bigFont->current.value)
            return sharedUiInfo.assets.boldFont;
        return sharedUiInfo.assets.textFont;
    }

    if (scale <= ui_smallFont->current.value)
        return sharedUiInfo.assets.smallFont;
    if (scale >= ui_extraBigFont->current.value)
        return sharedUiInfo.assets.extraBigFont;
    if (scale >= ui_bigFont->current.value)
        return sharedUiInfo.assets.bigFont;
    return sharedUiInfo.assets.textFont;
}

void UI_UpdateTime(int realtime)
{
    int *ui = (int *)uiInfo;
    int slot, sum, i;

    ui[2] = realtime - ui[1];

    ui[1] = realtime;

    slot = uiInfo->timeIndex % 4;

    uiInfo->previousTimes[slot] = ui[2];

    uiInfo->timeIndex += 1;

    if (uiInfo->timeIndex <= 4)
        return;

    sum = 0;
    for (i = 0; i < 4; i++) {
        sum += uiInfo->previousTimes[i];
    }
    if (sum == 0)
        sum = 1;
    uiInfo->uiDC.FPS = (float)(4000 / sum);
}

void UI_Shutdown(void)
{
    Menus_CloseAll(&uiInfo->uiDC);
    sharedUiInfo.assets.whiteMaterial = 0;
    LAN_SaveServersToCache();
}

char *GetMenuBuffer(const char *filename)
{
    int f;
    int len;

    len = FS_FOpenFileByMode(filename, &f, FS_READ);
    if (!f) {
        Com_Printf("^1menu file not found: %s, using default\n", filename);
        return 0;
    }
    if (len >= 0x8000) {
        Com_Printf("^1menu file too large: %s is %i, max allowed is %i", filename, len, 0x8000);
        FS_FCloseFile(f);
        return 0;
    }
    FS_Read(menuBuf2, len, f);
    menuBuf2[len] = 0;
    FS_FCloseFile(f);
    return menuBuf2;
}

qboolean Load_ScriptMenu(const char *pszMenu, int imageTrack)
{
    char szMenuFile[256];
    MenuList *menuList;

    strcpy(szMenuFile, "ui_mp/scriptmenus/");

    I_strncat(szMenuFile, 0x100, pszMenu);
    I_strncat(szMenuFile, 0x100, ".menu");

    menuList = UI_LoadMenu(szMenuFile, imageTrack);
    if (!menuList)
        return 0;

    UI_AddMenuList(&uiInfo->uiDC, menuList);
    return 1;
}

#ifndef __EMSCRIPTEN__
static void UI_DrawMapPreview(const rectDef_t *rect, const vec_t *color, int net)
{
    int map;
    int mapCount;
    MaterialHandle material;

    if (net) {
        map = (ui_currentNetMap)->current.integer;
    } else {
        map = (ui_currentMap)->current.integer;
    }

    mapCount = sharedUiInfo.mapCount;
    if (map < 0 || map >= mapCount) {

        if (net) {
            Dvar_SetInt((dvar_t *)ui_currentNetMap, 0);
        } else {
            Dvar_SetInt((dvar_t *)ui_currentMap, 0);
        }
        map = 0;
    }

    material = sharedUiInfo.mapList[map].levelShot;

    if (material) {
        UI_DrawHandlePic(rect->x, rect->y, rect->w, rect->h,
                         rect->horzAlign, rect->vertAlign,
                         color, material);
        return;
    }
    material = CL_RegisterMaterialNoMip("menu/art/unknownmap", 3);
    UI_DrawHandlePic(rect->x, rect->y, rect->w, rect->h,
                     rect->horzAlign, rect->vertAlign,
                     color, material);
}
#endif

const char *UI_GetMapDisplayName(const char *pszMap)
{
    int i;
    for (i = 0; i < sharedUiInfo.mapCount; i++) {
        const char *loadName = sharedUiInfo.mapList[i].mapLoadName;
        if (I_stricmp(pszMap, loadName) == 0) {
            return sharedUiInfo.mapList[i].mapName;
        }
    }
    return pszMap;
}

const char *UI_GetMapDisplayNameFromPartialLoadNameMatch(const char *pszMap, int *mapLoadNameLen)
{
    int i;
    for (i = 0; i < sharedUiInfo.mapCount; i++) {
        int len = strlen(sharedUiInfo.mapList[i].mapLoadName);
        *mapLoadNameLen = len;
        if (I_strnicmp(pszMap, sharedUiInfo.mapList[i].mapLoadName, len) == 0) {
            return sharedUiInfo.mapList[i].mapName;
        }
    }
    return 0;
}

const char *UI_GetGameTypeDisplayName(const char *pszGameType)
{
    int i;
    for (i = 0; i < sharedUiInfo.numGameTypes; i++) {
        const char *name = sharedUiInfo.gameTypes[i].gameType;
        if (I_stricmp(pszGameType, name) == 0) {
            return sharedUiInfo.gameTypes[i].gameTypeName;
        }
    }
    return pszGameType;
}

#ifndef __EMSCRIPTEN__
void UI_BuildPlayerList(void)
{
    char cs[sizeof(uiClientState_t)];
    char name[0x20];
    const char *info;
    int count;
    int n;

    GetClientState((uiClientState_t *)cs);
    info = CL_GetConfigString(0);
    count = atoi(Info_ValueForKey(info, "sv_maxclients"));
    memset(sharedUiInfo.playerClientNums, -1, 0x100);
    sharedUiInfo.playerCount = 0;

    for (n = 0; n < count; n++) {
        if (!GetClientname(n, name, 0x20))
            continue;

        sharedUiInfo.playerClientNums[sharedUiInfo.playerCount] = n;
        I_strncpyz(sharedUiInfo.playerNames[sharedUiInfo.playerCount], name, 0x20);
        I_CleanStr(sharedUiInfo.playerNames[sharedUiInfo.playerCount]);
        sharedUiInfo.playerCount++;
    }
}
#endif

void UI_DrawMapLevelshot(void)
{
    menuDef_t *menu = NULL;

    if (g_mapname[0])
        menu = Menus_FindByName(&uiInfo->uiDC, "connect");

    if (!menu) {
        UI_FillRect(0, 0, 640.0f, 480.0f, 0, 0, (const vec_t *)((void *)&colorBlack));
        return;
    }
    uiInfo->uiDC.blurRadiusOut = 0;
    Menu_Paint(&uiInfo->uiDC, menu, 1);
}

#ifndef __EMSCRIPTEN__
void UI_LoadIngameMenus(void)
{
    MenuList *menuList;

    if (g_ingameMenusLoaded)
        return;

    g_ingameMenusLoaded = 1;

    menuList = UI_LoadMenus("ui_mp/ingame.txt", 3);
    UI_AddMenuList(&uiInfo->uiDC, menuList);
}
#endif

#ifndef __EMSCRIPTEN__
void UI_SetMap(const char *mapname, const char *gametype)
{
    I_strncpyz(g_mapname, mapname, 0x40);
    I_strncpyz(g_gametype, gametype, 0x40);
    if (g_mapname[0]) {
        UI_MapLoadInfo(va("maps/mp/%s.csv", g_mapname));
    }
}
#endif

qboolean UI_OwnerDrawVisible(int flags)
{
    qboolean visible = 1;

    if (flags & 4) {
        if (ui_netSource->current.integer != 2) {
            visible = 0;
        }
    }

    if (flags & 0x1000) {
        if (ui_netSource->current.integer == 2) {
            visible = 0;
        }
    }

    return visible;
}

#ifndef __EMSCRIPTEN__
static int UI_ServersQsortCompare(const void *arg1, const void *arg2)
{
    int s1 = *(const int *)arg1;
    int s2 = *(const int *)arg2;
    return LAN_CompareServers(
        ui_netSource->current.integer,
        sharedUiInfo.serverStatus.sortKey,
        sharedUiInfo.serverStatus.sortDir,
        s1, s2);
}
#endif

#ifndef __EMSCRIPTEN__
static void UI_LoadMods(void)
{
    int numdirs;
    char dirlist[2048];
    char *dirptr;
    char *descptr;
    int i;
    int dirlen;

    sharedUiInfo.modCount = 0;
    numdirs = FS_GetFileList("$modlist", "", FS_LIST_ALL, dirlist, sizeof(dirlist));
    dirptr = dirlist;
    for (i = 0; i < numdirs; i++) {
        dirlen = strlen(dirptr) + 1;
        descptr = dirptr + dirlen;
        sharedUiInfo.modList[sharedUiInfo.modCount].modName = String_Alloc(dirptr);
        sharedUiInfo.modList[sharedUiInfo.modCount].modDescr = String_Alloc(descptr);
        dirptr += dirlen + strlen(descptr) + 1;
        sharedUiInfo.modCount++;
        if (sharedUiInfo.modCount >= 64)
            break;
    }
}

static int UI_GetPlayerProfileListIndexFromName(const char *name)
{
    int profileIndex;

    for (profileIndex = 0; profileIndex < uiInfo->playerProfileCount; profileIndex++) {
        if (!I_stricmp(name, uiInfo->playerProfileName[uiInfo->playerProfileStatus.displayProfile[profileIndex]]))
            return profileIndex;
    }
    return -1;
}
#endif

#ifndef __EMSCRIPTEN__
static int UI_PlayerProfilesQsortCompare(const void *arg1, const void *arg2)
{
    int idx1 = *(const int *)arg1;
    int idx2 = *(const int *)arg2;
    int result;

    if (idx1 == idx2)
        return 0;

    result = I_stricmp(
        uiInfo->playerProfileName[idx1],
        uiInfo->playerProfileName[idx2]);

    if (uiInfo->playerProfileStatus.sortDir == 0)
        result = -result;

    return result;
}
#endif

#ifndef __EMSCRIPTEN__
static void UI_SelectPlayerProfileIndex(int index)
{
    int menuIndex;

    for (menuIndex = uiInfo->uiDC.openMenuCount - 1; menuIndex >= 0; menuIndex--) {
        if (Window_IsVisible(&uiInfo->uiDC.menuStack[menuIndex]->window))
            Menu_SetFeederSelection(&uiInfo->uiDC, uiInfo->uiDC.menuStack[menuIndex], 24, index, 0);
    }
}

static void UI_SortPlayerProfiles(int selectIndex)
{
    int profileIndex;

    if (!uiInfo->playerProfileCount)
        return;
    for (profileIndex = 0; profileIndex < uiInfo->playerProfileCount; profileIndex++)
        uiInfo->playerProfileStatus.displayProfile[profileIndex] = profileIndex;
    qsort(uiInfo->playerProfileStatus.displayProfile, uiInfo->playerProfileCount, sizeof(int), UI_PlayerProfilesQsortCompare);
    UI_SelectPlayerProfileIndex(selectIndex);
}

static void UI_AddPlayerProfiles(void)
{
    char **profileList;
    int profileCount;
    int profileIndex;

    uiInfo->playerProfileCount = 0;
    uiInfo->playerProfileStatus.sortDir = 1;
    profileList = FS_ListFiles("players", "/", FS_LIST_ALL, &profileCount, 3);
    for (profileIndex = 0; profileIndex < profileCount; profileIndex++)
        uiInfo->playerProfileName[uiInfo->playerProfileCount++] = String_Alloc(profileList[profileIndex]);
    FS_FreeFileList(profileList, 3);
    UI_SortPlayerProfiles(0);
    Dvar_SetInt((dvar_t *)ui_playerProfileCount, uiInfo->playerProfileCount);
}

static void UI_SelectActivePlayerProfile(void)
{
    int selIndex;

    selIndex = UI_GetPlayerProfileListIndexFromName(com_playerProfile->current.string);
    if (selIndex >= 0 && selIndex < uiInfo->playerProfileCount)
        UI_SelectPlayerProfileIndex(selIndex);
}

static void UI_CreatePlayerProfile(void)
{
    char name[32];
    int profileIndex;

    if (!strlen(ui_playerProfileNameNew->current.string))
        return;
    I_strncpyz(name, ui_playerProfileNameNew->current.string, sizeof(name));
    Dvar_SetString((dvar_t *)ui_playerProfileNameNew, "");
    if (uiInfo->playerProfileCount == 64) {
        Menus_OpenByName(&uiInfo->uiDC, "profile_create_too_many_popmenu");
        return;
    }
    for (profileIndex = 0; profileIndex < uiInfo->playerProfileCount; profileIndex++) {
        if (!I_stricmp(name, uiInfo->playerProfileName[profileIndex])) {
            Menus_OpenByName(&uiInfo->uiDC, "profile_exists_popmenu");
            return;
        }
    }
    if (!Com_NewPlayerProfile(name)) {
        Menus_OpenByName(&uiInfo->uiDC, "profile_create_fail_popmenu");
        return;
    }
    uiInfo->playerProfileName[uiInfo->playerProfileCount++] = String_Alloc(name);
    UI_SortPlayerProfiles(0);
    Dvar_SetInt((dvar_t *)ui_playerProfileCount, uiInfo->playerProfileCount);
    UI_SelectPlayerProfileIndex(UI_GetPlayerProfileListIndexFromName(name));
}

static void UI_DeletePlayerProfile(void)
{
    int curSelected;
    int nameIndex;

    if (!uiInfo->playerProfileCount)
        return;
    if (!Com_DeletePlayerProfile(ui_playerProfileSelected->current.string)) {
        Menus_OpenByName(&uiInfo->uiDC, "profile_delete_fail_popmenu");
        return;
    }
    curSelected = UI_GetPlayerProfileListIndexFromName(ui_playerProfileSelected->current.string);
    nameIndex = uiInfo->playerProfileStatus.displayProfile[curSelected];
    if (--uiInfo->playerProfileCount == 0) {
        Dvar_SetString((dvar_t *)ui_playerProfileSelected, "");
    } else {
        uiInfo->playerProfileName[nameIndex] = uiInfo->playerProfileName[uiInfo->playerProfileCount];
        if (curSelected == uiInfo->playerProfileCount)
            curSelected--;
        UI_SortPlayerProfiles(curSelected);
    }
    Dvar_SetInt((dvar_t *)ui_playerProfileCount, uiInfo->playerProfileCount);
}

static void UI_LoadPlayerProfile(void)
{
    if (ui_playerProfileSelected->current.string[0])
        Com_ChangePlayerProfile(ui_playerProfileSelected->current.string);
}

static void UI_LoadMovies(void)
{
    char movielist[4096];
    char *moviename;
    int i;
    int len;

    sharedUiInfo.movieCount = FS_GetFileList("video", "roq", FS_LIST_PURE_ONLY, movielist, sizeof(movielist));
    if (!sharedUiInfo.movieCount)
        return;
    if (sharedUiInfo.movieCount > 256)
        sharedUiInfo.movieCount = 256;
    moviename = movielist;
    for (i = 0; i < sharedUiInfo.movieCount; i++) {
        len = strlen(moviename);
        if (!I_stricmp(moviename + len - 4, ".roq"))
            moviename[len - 4] = 0;
        I_strupr(moviename);
        sharedUiInfo.movieList[i] = String_Alloc(moviename);
        moviename += len + 1;
    }
}
#endif

#ifndef __EMSCRIPTEN__
static void UI_SelectCurrentMap(void)
{
    byte cstate[sizeof(uiClientState_t)];
    char szMap[0x40];
    int i;
    int visibleIndex;

    GetClientState((uiClientState_t *)cstate);
    if (*(int *)cstate != 8)
        return;

    {
        const char *serverInfo = CL_GetConfigString(0);
        if (!serverInfo[0])
            return;
        I_strncpyz(szMap, Info_ValueForKey(serverInfo, "mapname"), sizeof(szMap));
    }

    visibleIndex = 0;
    for (i = 0; i < sharedUiInfo.mapCount; i++) {
        if (!sharedUiInfo.mapList[i].active)
            continue;

        if (I_stricmp(szMap, sharedUiInfo.mapList[i].mapName) == 0) {
            Menu_SetFeederSelection(&uiInfo->uiDC, 0, 4, visibleIndex, "createserver_maps");
            return;
        }

        visibleIndex++;
    }
}
#endif

qboolean UI_CheckExecKey(int key)
{
    menuDef_t *menu;
    ItemKeyHandler *khNode;

    menu = Menu_GetFocused(&uiInfo->uiDC);

    if (g_editingField)
        return 1;

    if (key > 0x100)
        return 0;
    if (!menu)
        return 0;

    khNode = menu->onKey;
    while (khNode) {
        if (khNode->key == key)
            return 1;
        khNode = (ItemKeyHandler *)khNode->next;
    }
    return 0;
}

#ifndef __EMSCRIPTEN__
static void UI_Update(const char *name)
{
    float rate;

    if (!I_stricmp(name, "ui_SetName")) {
        Dvar_SetStringByName("name", Dvar_GetVariantString("ui_Name"));
    } else if (!I_stricmp(name, "ui_GetName")) {
        Dvar_SetStringByName("ui_Name", Dvar_GetVariantString("name"));
    } else if (!I_stricmp(name, "ui_setRate")) {
        rate = (float)Dvar_GetInt("rate");
        if (rate >= 5000.0f) {
            Dvar_SetIntByName("cl_maxpackets", 30);
            Dvar_SetIntByName("cl_packetdup", 1);
        } else if (rate >= 4000.0f) {
            Dvar_SetIntByName("cl_maxpackets", 15);
            Dvar_SetIntByName("cl_packetdup", 2);
        } else {
            Dvar_SetIntByName("cl_maxpackets", 15);
            Dvar_SetIntByName("cl_packetdup", 1);
        }
    } else if (!I_stricmp(name, "ui_mousePitch")) {
        if (!Dvar_GetBool(name))
            Dvar_SetFloatByName("m_pitch", 0.022f);
        else
            Dvar_SetFloatByName("m_pitch", -0.022f);
    }
}
#endif

#ifndef __EMSCRIPTEN__
static void UI_VerifyLanguage(void)
{
    int oldLanguage = Dvar_GetInt("loc_language");
    int newLanguage = Dvar_GetInt("ui_language");
    int verified = SEH_VerifyLanguageSelection(newLanguage);

    if (verified != newLanguage)
        Dvar_SetIntByName("ui_language", verified);

    if (newLanguage != oldLanguage)
        Dvar_SetBoolByName("ui_languagechanged", 1);
    else
        Dvar_SetBoolByName("ui_languagechanged", 0);
}
#endif

#ifndef __EMSCRIPTEN__
const char *UI_SafeTranslateString(const char *reference)
{
    const char *translated;

    // 0x15-prefixed strings are already localized
    if (reference[0] == 0x15) {
        reference++;
    } else {
        translated = SEH_StringEd_GetString(reference);
        if (translated)
            return translated;
    }

    if ((loc_warnings)->current.enabled) {
        if ((loc_warningsAsErrors)->current.enabled) {
            Com_Error(ERR_LOCALIZATION, "Could not translate string \"%s\"", reference);
        } else {
            Com_Printf("^3WARNING: Could not translate string \"%s\"\n", reference);
        }

        strcpy(errorString, "^1UNLOCALIZED(^7");
        I_strncat(errorString, 0x400, reference);
        I_strncat(errorString, 0x400, "^1)^7");
        return errorString;
    }

    strcpy(errorString, reference);
    return errorString;
}
#endif

#ifndef __EMSCRIPTEN__
static void UI_AddServerToFavoritesList(const char *name, const char *address)
{
    const char *message;
    const char *dvarMessage;
    int addResult;

    if (!name[0]) {
        message = UI_SafeTranslateString("EXE_FAVORITENAMEEMPTY");
        Com_Printf("%s\n", message);
        Dvar_SetStringByName("ui_favorite_message", "@EXE_FAVORITENAMEEMPTY");
        return;
    }

    if (!address[0]) {
        message = UI_SafeTranslateString("EXE_FAVORITEADDRESSEMPTY");
        Com_Printf("%s\n", message);
        Dvar_SetStringByName("ui_favorite_message", "@EXE_FAVORITEADDRESSEMPTY");
        return;
    }

    addResult = LAN_AddServer(2, name, address);
    if (addResult == 0) {
        message = UI_SafeTranslateString("EXE_FAVORITEINLIST");
        dvarMessage = "@EXE_FAVORITEINLIST";
    } else if (addResult == -1) {
        message = UI_SafeTranslateString("EXE_FAVORITELISTFULL");
        dvarMessage = "@EXE_FAVORITELISTFULL";
    } else if (addResult == -2) {
        message = UI_SafeTranslateString("EXE_BADSERVERADDRESS");
        dvarMessage = "@EXE_BADSERVERADDRESS";
    } else {
        message = UI_SafeTranslateString("EXE_FAVORITEADDED");
        dvarMessage = "@EXE_FAVORITEADDED";
    }

    Com_Printf("%s\n", message);
    Dvar_SetStringByName("ui_favorite_message", dvarMessage);
}
#endif

#ifndef __EMSCRIPTEN__
Bool UI_GetOpenOrCloseMenuOnDvarArgs(const char **args, const char *cmd,
                                     char *dvarName, char *testValue, char *menuName)
{
    if (!String_Parse(args, dvarName, 0x400)) {
        Com_Printf("%s: invalid dvar name.\n", cmd);
        return 0;
    }

    if (!String_Parse(args, testValue, 0x400)) {
        Com_Printf("%s: invalid test value.\n", cmd);
        return 0;
    }

    if (!String_Parse(args, menuName, 0x400)) {
        Com_Printf("%s: invalid menu name.\n", cmd);
        return 0;
    }

    return 1;
}
#endif

#ifndef __EMSCRIPTEN__
static bool UI_DvarValueTest(const char *cmd, const char *dvarName, const char *testValue, bool wantMatch)
{
    if (!Dvar_FindVar(dvarName)) {
        Com_Printf("%s: cannot find dvar %s\n", cmd, dvarName);
        return 0;
    }
    bool match = I_stricmp(testValue, Dvar_GetVariantString(dvarName)) == 0;
    return match == wantMatch;
}

static void UI_CloseMenuOnDvar(const char *cmd, const char *menuName, const char *dvarName, const char *testValue)
{
    if (UI_DvarValueTest(cmd, dvarName, testValue, I_stricmp(cmd, "closeMenuOnDvar") == 0))
        Menus_CloseByName(&uiInfo->uiDC, menuName);
}

static void UI_OpenMenuOnDvar(const char *cmd, const char *menuName, const char *dvarName, const char *testValue)
{
    if (UI_DvarValueTest(cmd, dvarName, testValue, I_stricmp(cmd, "openMenuOnDvar") == 0))
        Menus_OpenByName(&uiInfo->uiDC, menuName);
}
#endif

static void UI_AddServerStatusDvarRows(serverStatusInfo_t *info)
{
    int index = 0;
    int dvarIndex;

    for (dvarIndex = 0; serverStatusDvars[dvarIndex].name; dvarIndex++) {
        int j;

        for (j = 0; j < info->numLines; j++) {
            const char *oldName;
            const char *oldValue;
            const serverStatusDvar_t *dvar;

            if (!info->lines[j][1] || info->lines[j][1][0] != '\0')
                continue;

            dvar = &serverStatusDvars[dvarIndex];
            if (I_stricmp(dvar->name, info->lines[j][0]) != 0)
                continue;

            oldName = info->lines[index][0];
            oldValue = info->lines[index][3];
            info->lines[index][0] = info->lines[j][0];
            info->lines[index][3] = info->lines[j][3];
            info->lines[j][0] = oldName;
            info->lines[j][3] = oldValue;

            if (dvar->altName[0])
                info->lines[index][0] = dvar->altName;

            if (dvar->type == 1)
                info->lines[index][3] = atoi(info->lines[index][3]) ? "@EXE_YES" : "@EXE_NO";

            index++;
            break;
        }
    }
}

static int __attribute_regparm__(2) UI_GetServerStatusInfo(const char *serverAddress, serverStatusInfo_t *info)
{
    char *p, *score, *ping, *name;
    int i, len;

    if (!info) {
        LAN_GetServerStatus((char *)serverAddress, 0, 0);
        return 0;
    }

    memset(info, 0, sizeof(*info));
    if (LAN_GetServerStatus((char *)serverAddress, info->text, sizeof(info->text))) {
        I_strncpyz(info->address, serverAddress, sizeof(info->address));
        p = info->text;
        info->numLines = 0;
        info->lines[info->numLines][0] = "address";
        info->lines[info->numLines][1] = "";
        info->lines[info->numLines][2] = "";
        info->lines[info->numLines][3] = info->address;
        info->numLines++;

        while (p && *p) {
            p = strchr(p, '\\');
            if (!p)
                break;
            *p++ = '\0';
            if (*p == '\\')
                break;
            info->lines[info->numLines][0] = p;
            info->lines[info->numLines][1] = "";
            info->lines[info->numLines][2] = "";
            p = strchr(p, '\\');
            if (!p)
                break;
            *p++ = '\0';
            info->lines[info->numLines][3] = p;
            info->numLines++;
            if (info->numLines >= 128)
                break;
        }

        if (info->numLines < 125) {
            info->lines[info->numLines][0] = "";
            info->lines[info->numLines][1] = "";
            info->lines[info->numLines][2] = "";
            info->lines[info->numLines][3] = "";
            info->numLines++;

            info->lines[info->numLines][0] = "@EXE_SV_INFO_NUM";
            info->lines[info->numLines][1] = "@EXE_SV_INFO_SCORE";
            info->lines[info->numLines][2] = "@EXE_SV_INFO_PING";
            info->lines[info->numLines][3] = "@EXE_SV_INFO_NAME";
            info->numLines++;

            i = 0;
            len = 0;
            while (p && *p) {
                if (*p == '\\')
                    *p++ = '\0';
                if (!p)
                    break;
                score = p;
                p = strchr(p, ' ');
                if (!p)
                    break;
                *p++ = '\0';
                ping = p;
                p = strchr(p, ' ');
                if (!p)
                    break;
                *p++ = '\0';
                name = p;
                Com_sprintf(&info->pings[len], sizeof(info->pings) - len, "%d", i);
                info->lines[info->numLines][0] = &info->pings[len];
                len += strlen(&info->pings[len]) + 1;
                info->lines[info->numLines][1] = score;
                info->lines[info->numLines][2] = ping;
                info->lines[info->numLines][3] = name;
                info->numLines++;
                if (info->numLines >= 128)
                    break;
                p = strchr(p, '\\');
                if (!p)
                    break;
                *p++ = '\0';
                i++;
            }
        }

        UI_AddServerStatusDvarRows(info);
        return 1;
    }
    return 0;
}

#ifndef __EMSCRIPTEN__
void UI_FeederSelection(float feederID, int index)
{
    if (feederID == 4.0f) {
        int actual, map;

        map = (ui_currentNetMap)->current.integer;
        if (sharedUiInfo.mapList[map].cinematic >= 0) {
            CIN_StopCinematic(sharedUiInfo.mapList[map].cinematic);
            sharedUiInfo.mapList[map].cinematic = -1;
        }
        UI_SelectedMap(index, &actual);
        Dvar_SetInt((dvar_t *)ui_currentMap, actual);
        Dvar_SetInt((dvar_t *)ui_currentNetMap, actual);
    } else if (feederID == 2.0f) {

        char *name;
        char *p;

        if (sharedUiInfo.serverStatus.numDisplayServers > 0)
            sharedUiInfo.serverStatus.currentServer = index;

        LAN_GetServerInfo((ui_netSource)->current.integer,
                          sharedUiInfo.serverStatus.displayServers[index],
                          info, 0x400);
        name = (char *)va("levelshots/%s", Info_ValueForKey(info, "mapname"));

        for (p = name; *p; p++)
            *p = tolower((int)(signed char)*p);

        sharedUiInfo.serverStatus.currentServerPreview = CL_RegisterMaterialNoMip(name, 3);
        if (sharedUiInfo.serverStatus.currentServerCinematic >= 0) {
            CIN_StopCinematic(sharedUiInfo.serverStatus.currentServerCinematic);
            sharedUiInfo.serverStatus.currentServerCinematic = -1;
        }

    } else if (feederID == 7.0f) {
        uiInfo->playerIndex = index;
    } else if (feederID == 9.0f) {
        sharedUiInfo.modIndex = index;
    } else if (feederID == 20.0f) {
        uiInfo->playerIndex = index;

    } else if (feederID == 24.0f) {

        if (index >= 0 && index < uiInfo->playerProfileCount) {
            int sortedIdx = uiInfo->playerProfileStatus.displayProfile[index];
            const char *profileName = uiInfo->playerProfileName[sortedIdx];
            Dvar_SetString((dvar_t *)ui_playerProfileSelected, profileName);
        }
    }
}
#endif

#ifndef __EMSCRIPTEN__
static void UI_GetGameTypesList(void)
{
    char szGameTypeList[0x1000];
    int iNumGameTypeScripts;
    int i;
    char *pszFileName;
    const char *pBuffParse;

    sharedUiInfo.numGameTypes = 0;
    sharedUiInfo.numJoinGameTypes = 0;

    sharedUiInfo.joinGameTypes[sharedUiInfo.numJoinGameTypes].gameType = String_Alloc("All");
    sharedUiInfo.joinGameTypes[sharedUiInfo.numJoinGameTypes].gameTypeName = "";
    sharedUiInfo.numJoinGameTypes++;

    iNumGameTypeScripts = FS_GetFileList("maps/mp/gametypes", "gsc", FS_LIST_PURE_ONLY, szGameTypeList, 0x1000);

    pszFileName = szGameTypeList;
    for (i = 0; i < iNumGameTypeScripts; i++) {
        int iLen = strlen(pszFileName);

        if (pszFileName[0] != '_') {
            char *pszExt = pszFileName + iLen - 4;

            if (!I_stricmp(pszExt, ".gsc"))
                *pszExt = '\0';

            if (sharedUiInfo.numGameTypes == 32 || sharedUiInfo.numJoinGameTypes == 32) {
                Com_Printf("Too many game type scripts found! Only loading the first %i\n", 31);
                break;
            }

            sharedUiInfo.gameTypes[sharedUiInfo.numGameTypes].gameType = String_Alloc(pszFileName);
            sharedUiInfo.joinGameTypes[sharedUiInfo.numJoinGameTypes].gameType = sharedUiInfo.gameTypes[sharedUiInfo.numGameTypes].gameType;

            pBuffParse = GetMenuBuffer(va("maps/mp/gametypes/%s.txt", pszFileName));
            if (!pBuffParse) {
                sharedUiInfo.gameTypes[sharedUiInfo.numGameTypes].gameTypeName = sharedUiInfo.gameTypes[sharedUiInfo.numGameTypes].gameType;
                sharedUiInfo.joinGameTypes[sharedUiInfo.numJoinGameTypes].gameTypeName = sharedUiInfo.gameTypes[sharedUiInfo.numGameTypes].gameTypeName;
            } else {
                sharedUiInfo.gameTypes[sharedUiInfo.numGameTypes].gameTypeName = String_Alloc(Com_Parse(&pBuffParse));
                sharedUiInfo.joinGameTypes[sharedUiInfo.numJoinGameTypes].gameTypeName = sharedUiInfo.gameTypes[sharedUiInfo.numGameTypes].gameTypeName;
            }

            sharedUiInfo.numGameTypes++;
            sharedUiInfo.numJoinGameTypes++;
        }
        pszFileName += iLen + 1;
    }

    if (!sharedUiInfo.numGameTypes)
        Com_Error(ERR_FATAL, "\x15No game type scripts found in maps/mp/gametypes folder");
}
#endif

void UI_Pause(qboolean b)
{
    if (b) {
        Dvar_SetIntByName("cl_paused", 1);
        Key_SetCatcher(8);
    } else {
        Key_SetCatcher(Key_GetCatcher() & ~8);
        Key_ClearStates();
        Dvar_SetIntByName("cl_paused", 0);
    }
}

#ifndef __EMSCRIPTEN__
void UI_OpenMenu_f(void)
{
    Menus_OpenByName(&uiInfo->uiDC, Cmd_Args(1));
}
#endif

#ifndef __EMSCRIPTEN__
void UI_CloseMenu_f(void)
{
    Menus_CloseByName(&uiInfo->uiDC, Cmd_Args(1));
}
#endif

#ifndef __EMSCRIPTEN__
void UI_RegisterDvars(void)
{
    Dvar_RegisterBool("g_allowvote", 1, 0x1001);
    Dvar_RegisterBool("cg_brass", 1, 0x1001);
    Dvar_RegisterBool("cg_marks", 1, 0x1001);

    Dvar_RegisterString("server1", "", 0x1001);
    Dvar_RegisterString("server2", "", 0x1001);
    Dvar_RegisterString("server3", "", 0x1001);
    Dvar_RegisterString("server4", "", 0x1001);
    Dvar_RegisterString("server5", "", 0x1001);
    Dvar_RegisterString("server6", "", 0x1001);
    Dvar_RegisterString("server7", "", 0x1001);
    Dvar_RegisterString("server8", "", 0x1001);
    Dvar_RegisterString("server9", "", 0x1001);
    Dvar_RegisterString("server10", "", 0x1001);
    Dvar_RegisterString("server11", "", 0x1001);
    Dvar_RegisterString("server12", "", 0x1001);
    Dvar_RegisterString("server13", "", 0x1001);
    Dvar_RegisterString("server14", "", 0x1001);
    Dvar_RegisterString("server15", "", 0x1001);
    Dvar_RegisterString("server16", "", 0x1001);

    ui_netSource = (const dvar_t *)(Dvar_RegisterInt("ui_netSource", 0, 0, 2, 0x1001));
    ui_smallFont = (const dvar_t *)(Dvar_RegisterFloat("ui_smallFont", 0.25f, 0.0f, 1.0f, 0x1001));
    ui_bigFont = (const dvar_t *)(Dvar_RegisterFloat("ui_bigFont", 0.4f, 0.0f, 1.0f, 0x1001));
    ui_extraBigFont = (const dvar_t *)(Dvar_RegisterFloat("ui_extraBigFont", 0.55f, 0.0f, 1.0f, 0x1001));
    ui_currentMap = (const dvar_t *)(Dvar_RegisterInt("ui_currentMap", 0, 0, 0x7fffffff, 0x1001));
    ui_gametype = (const dvar_t *)(Dvar_RegisterInt("ui_gametype", 3, 0, 0x7fffffff, 0x1001));
    ui_joinGameType = (const dvar_t *)(Dvar_RegisterInt("ui_joinGametype", 0, 0, 0x7fffffff, 0x1001));
    ui_netGameTypeName = (const dvar_t *)(Dvar_RegisterString("ui_netGametypeName", "", 0x1001));
    ui_dedicated = (const dvar_t *)(Dvar_RegisterInt("ui_dedicated", 0, 0, 2, 0x1001));
    ui_currentNetMap = (const dvar_t *)(Dvar_RegisterInt("ui_currentNetMap", 0, 0, 0x7fffffff, 0x1001));
    ui_browserShowFull = (const dvar_t *)(Dvar_RegisterBool("ui_browserShowFull", 1, 0x1001));
    ui_browserShowEmpty = (const dvar_t *)(Dvar_RegisterBool("ui_browserShowEmpty", 1, 0x1001));
    ui_browserShowPassword = (const dvar_t *)(Dvar_RegisterBool("ui_browserShowPassword", 1, 0x1001));
    ui_browserShowNoPassword = (const dvar_t *)(Dvar_RegisterBool("ui_browserShowNoPassword", 1, 0x1001));
    ui_browserShowPure = (const dvar_t *)(Dvar_RegisterBool("ui_browserShowPure", 1, 0x1001));
    ui_browserShowDedicated = (const dvar_t *)(Dvar_RegisterBool("ui_browserShowDedicated", 0, 0x1001));
#    if COD2_IS_PATCH_13
    ui_browserShowPunkBuster = (const dvar_t *)(Dvar_RegisterInt("ui_browserShowPunkBuster", -1, (int)0x80000000, 0x7fffffff, 0x1001));
#    endif
    ui_browserMod = (const dvar_t *)(Dvar_RegisterInt("ui_browserMod", -1, -1, 0x7fffffff, 0x1001));
    ui_browserFriendlyfire = (const dvar_t *)(Dvar_RegisterInt("ui_browserFriendlyfire", -1, (int)0x80000000, 0x7fffffff, 0x1001));
    ui_browserKillcam = (const dvar_t *)(Dvar_RegisterInt("ui_browserKillcam", -1, (int)0x80000000, 0x7fffffff, 0x1001));
    ui_serverStatusTimeOut = (const dvar_t *)(Dvar_RegisterInt("ui_serverStatusTimeOut", 7000, 0, 0x7fffffff, 0x1001));
    ui_playerProfileCount = (const dvar_t *)(Dvar_RegisterInt("ui_playerProfileCount", 0, (int)0x80000000, 0x7fffffff, 0x1040));
    ui_playerProfileSelected = (const dvar_t *)(Dvar_RegisterString("ui_playerProfileSelected", "", 0x1040));
    ui_playerProfileNameNew = (const dvar_t *)(Dvar_RegisterString("ui_playerProfileNameNew", "", 0x1000));
}

void UI_Init(void)
{
    int width, height;
    MenuList *menuList;
    int netGameTypeIdx;
    float mPitch;

    *(int *)uiInfo = 0;
    g_ingameMenusLoaded = 0;

    UI_LoadSoundAliases();

    UI_RegisterDvars();

    legacyHacksData.ui_newScriptMenu[0] = 0;
    legacyHacksData.ui_newScriptMenuIndex = -1;
    legacyHacksData.ui_scriptMenu[0] = 0;
    legacyHacksData.ui_scriptMenuIndex = -1;
    legacyHacksData.ui_scriptMenuAllowResponse = 1;
    legacyHacksData.ui_waitingScriptMenu[0] = 0;
    legacyHacksData.ui_waitingScriptMenuIndex = -1;
    legacyHacksData.ui_waitingScriptMenuNoMouse = 0;

    String_Init();
    Menu_Setup(&uiInfo->uiDC);

    CL_GetScreenDimensions(&uiInfo->uiDC.screenWidth, &uiInfo->uiDC.screenHeight, (float *)((int *)&uiInfo->uiDC.screenAspect));

    width = uiInfo->uiDC.screenWidth;
    height = uiInfo->uiDC.screenHeight;

    if (width * 480 > height * 640) {

        uiInfo->uiDC.bias = ((float)width + (float)height * -1.3333333730697632f) * 0.5f;
    } else {
        *(int *)uiInfo = 0;
    }

    Sys_Milliseconds();
    UI_GetGameTypesList();

    ui_netGameType = (const dvar_t *)(Dvar_RegisterInt("ui_netGametype", 0, 0, sharedUiInfo.numGameTypes - 1, 0x1001));

    UI_LoadArenas();

    menuList = UI_LoadMenus("ui_mp/menus.txt", 3);
    UI_AddMenuList(&uiInfo->uiDC, menuList);

    if (g_mapname[0] != '\0') {
        UI_MapLoadInfo(va("maps/mp/%s.csv", g_mapname));
    }

    UI_AssetCache();
    Menus_CloseAll(&uiInfo->uiDC);

    sharedUiInfo.serverHardwareIconList[0] = CL_RegisterMaterialNoMip("server_hardware_unknown", 3);
    sharedUiInfo.serverHardwareIconList[1] = CL_RegisterMaterialNoMip("server_hardware_linux_dedicated", 3);
    sharedUiInfo.serverHardwareIconList[2] = CL_RegisterMaterialNoMip("server_hardware_win_dedicated", 3);
    sharedUiInfo.serverHardwareIconList[3] = CL_RegisterMaterialNoMip("server_hardware_mac_dedicated", 3);
    sharedUiInfo.serverHardwareIconList[5] = CL_RegisterMaterialNoMip("server_hardware_win_listen", 3);
    sharedUiInfo.serverHardwareIconList[6] = CL_RegisterMaterialNoMip("server_hardware_mac_listen", 3);

    LAN_LoadCachedServers();

    if (sharedUiInfo.serverStatus.sortKey != 9) {
        sharedUiInfo.serverStatus.sortKey = 9;
        qsort(sharedUiInfo.serverStatus.displayServers, sharedUiInfo.serverStatus.numDisplayServers, 4, UI_ServersQsortCompare);
    }

    mPitch = Dvar_GetFloat("m_pitch");
    Dvar_SetBoolByName("ui_mousePitch", mPitch < 0.0f ? 1 : 0);

    sharedUiInfo.serverStatus.currentServerCinematic = -1;
    sharedUiInfo.previewMovie = -1;

    netGameTypeIdx = (ui_netGameType)->current.integer;
    Dvar_SetString((dvar_t *)ui_netGameTypeName, sharedUiInfo.gameTypes[netGameTypeIdx].gameType);

    Dvar_RegisterBool("ui_multiplayer", 1, 0x1040);

#    if COD2_FEATURE_GAMEPAD

    UI_ConsoleMode_Init(uiInfo);
    UI_Gamepad_RegisterMenus(uiInfo);
#    endif
}
#endif

void UI_KeyEvent(int key, qboolean down)
{
    menuDef_t *menu;

    if (Menu_Count(&uiInfo->uiDC) <= 0)
        return;

    menu = Menu_GetFocused(&uiInfo->uiDC);
    if (menu) {
        if (Dvar_GetBool("cl_bypassMouseInput"))
            bypassKeyClear = 1;

        if (key == 0x1b && down && !Menus_AnyFullScreenVisible(&uiInfo->uiDC) && menu->onESC == 0) {

            Menus_CloseAll(&uiInfo->uiDC);
        } else {
            Menu_HandleKey(&uiInfo->uiDC, menu, key, down);
        }

        if (Menu_GetFocused(&uiInfo->uiDC))
            return;
    }

    if (!(Key_GetCatcher() & 8))
        return;

    Key_SetCatcher(Key_GetCatcher() & ~8);
    if (!bypassKeyClear)
        Key_ClearStates();
    bypassKeyClear = 0;
    Dvar_SetIntByName("cl_paused", 0);
}

#ifndef __EMSCRIPTEN__
void UI_MouseEvent(int dx, int dy)
{
    int *cursorX = &uiInfo->uiDC.cursorx;
    int *cursorY = &uiInfo->uiDC.cursory;

    *cursorX += dx;
    if (*cursorX < 0)
        *cursorX = 0;
    else if (*cursorX > 640)
        *cursorX = 640;

    *cursorY += dy;
    if (*cursorY < 0)
        *cursorY = 0;
    else if (*cursorY > 480)
        *cursorY = 480;

    if (Menu_Count(&uiInfo->uiDC) > 0) {
        Display_MouseMove(&uiInfo->uiDC, 0, *cursorX, *cursorY);
    }
}
#endif


#ifndef __EMSCRIPTEN__
uiMenuCommand_t UI_GetActiveMenu(void)
{
    return uiInfo->currentMenuType;
}
#endif

#ifndef __EMSCRIPTEN__
qboolean UI_SetActiveMenu(int menu)
{
    menuDef_t *pFocus;
    const char *errorMsg;

    if (Menu_Count(&uiInfo->uiDC) <= 0)
        return 0;

    if (menu != UIMENU_SCRIPT_POPUP && menu != UIMENU_SCRIPT_POPUP_NO_MOUSE)
        uiInfo->currentMenuType = (uiMenuCommand_t)(menu);

    switch (menu) {
    case UIMENU_NONE:
        Key_SetCatcher(Key_GetCatcher() & ~8);
        Dvar_SetIntByName("cl_paused", 0);
        Menus_CloseAll(&uiInfo->uiDC);
        return 1;

    case UIMENU_MAIN:
        Key_SetCatcher(8);
        Menus_OpenByName(&uiInfo->uiDC, "main");
        errorMsg = Dvar_GetString("com_errorMessage");
        if (strlen(errorMsg) && I_stricmp(errorMsg, ";"))
            Menus_OpenByName(&uiInfo->uiDC, "error_popmenu");
        SND_FadeAllSounds(1.0f, 1000);
        return 1;

    case UIMENU_TEAM:
        Key_SetCatcher(8);
        Menus_OpenByName(&uiInfo->uiDC, "team");
        return 1;

    case UIMENU_NEED_CD:
        Key_SetCatcher(8);
        Menus_OpenByName(&uiInfo->uiDC, "needcd");
        return 1;

    case UIMENU_BAD_CD_KEY:
        Key_SetCatcher(8);
        Menus_OpenByName(&uiInfo->uiDC, "badcd");
        return 1;

    case UIMENU_INGAME:
        Key_SetCatcher(8);
        Menus_CloseAll(&uiInfo->uiDC);
        if (!Menus_OpenByName(&uiInfo->uiDC, cg->scriptMainMenu))
            Menus_OpenByName(&uiInfo->uiDC, "main");
        return 1;

    case UIMENU_WM_QUICKMESSAGE:
        uiInfo->uiDC.cursorx = 0x27f;
        uiInfo->uiDC.cursory = 0x1df;
        Key_SetCatcher(8);
        clients->displayHUDWithKeycatchUI = 1;
        Menus_CloseAll(&uiInfo->uiDC);
        Menus_OpenByName(&uiInfo->uiDC, "quickmessage");
        return 1;

    case UIMENU_WM_AUTOUPDATE:
        Menus_OpenByName(&uiInfo->uiDC, "autoupdate");
        return 1;

    case UIMENU_SCRIPT_POPUP:
    case UIMENU_SCRIPT_POPUP_NO_MOUSE:
        pFocus = Menu_GetFocused(&uiInfo->uiDC);
        if (pFocus && uiInfo->currentMenuType != UIMENU_SCRIPT_POPUP && uiInfo->currentMenuType != UIMENU_SCRIPT_POPUP_NO_MOUSE)
            return 0;
        if (pFocus && !I_stricmp(pFocus->window.name, legacyHacks->ui_newScriptMenu))
            return 1;
        uiInfo->currentMenuType = UIMENU_SCRIPT_POPUP;
        if (menu == UIMENU_SCRIPT_POPUP_NO_MOUSE) {
            uiInfo->uiDC.cursorx = 0x27f;
            uiInfo->uiDC.cursory = 0x1df;
        }
        Key_SetCatcher(8);
        Menus_CloseAll(&uiInfo->uiDC);
        strcpy(legacyHacks->ui_scriptMenu, legacyHacks->ui_newScriptMenu);
        legacyHacks->ui_scriptMenuIndex = legacyHacks->ui_newScriptMenuIndex;
        legacyHacks->ui_newScriptMenu[0] = 0;
        legacyHacks->ui_newScriptMenuIndex = -1;
        Menus_OpenByName(&uiInfo->uiDC, legacyHacks->ui_scriptMenu);
        return 1;

    case UIMENU_PLAYER_PROFILE:
        Key_SetCatcher(8);
        Menus_OpenByName(&uiInfo->uiDC, "player_profile");
        SND_FadeAllSounds(1.0f, 1000);
        return 1;
    }

    return 0;
}
#endif

#ifndef __EMSCRIPTEN__
qboolean UI_IsFullscreen(void)
{
    return Menus_AnyFullScreenVisible(&uiInfo->uiDC);
}
#endif

static void __attribute_regparm__(3) UI_ReadableSize(char *buf, int bufsize, int value)
{
    if (value > 1024 * 1024 * 1024) {
        Com_sprintf(buf, bufsize, "%d", value / (1024 * 1024 * 1024));
        Com_sprintf(buf + strlen(buf), bufsize - strlen(buf), ".%02d %s",
                    (value % (1024 * 1024 * 1024)) * 100 / (1024 * 1024 * 1024), UI_SafeTranslateString("EXE_GIGABYTE"));
    } else if (value > 1024 * 1024) {
        Com_sprintf(buf, bufsize, "%d", value / (1024 * 1024));
        Com_sprintf(buf + strlen(buf), bufsize - strlen(buf), ".%02d %s",
                    (value % (1024 * 1024)) * 100 / (1024 * 1024), UI_SafeTranslateString("EXE_MEGABYTE"));
    } else if (value > 1024) {
        Com_sprintf(buf, bufsize, "%d %s", value / 1024, UI_SafeTranslateString("EXE_KILOBYTE"));
    } else {
        Com_sprintf(buf, bufsize, "%d %s", value, UI_SafeTranslateString("EXE_BYTES"));
    }
}

float UI_GetBlurRadius(void)
{
    return uiInfo->uiDC.blurRadiusOut;
}

#ifndef __EMSCRIPTEN__
qboolean UI_AnyFullScreenMenuVisible(void)
{
    return Menus_AnyFullScreenVisible(&uiInfo->uiDC);
}
#endif

const char *UI_ReplaceConversions(const char *sourceString, ConversionArguments *arguments)
{
    char outputString[0x400];
    int sourceStringLength;
    int index;
    int outputStringCounter;
    int argIndex;
    int argLen;
    int j;

    if (!strstr(sourceString, "&&"))
        return sourceString;

    sourceStringLength = strlen(sourceString);
    memset(outputString, 0, 0x400);
    outputStringCounter = 0;

    for (index = 0; index < sourceStringLength; ) {
        if (!strncmp(&sourceString[index], "&&", 2) && isdigit(sourceString[index + 2])) {
            argIndex = sourceString[index + 2] - '1';
            argLen = strlen(arguments->args[argIndex]);
            for (j = 0; j < argLen; j++) {
                outputString[outputStringCounter] = arguments->args[argIndex][j];
                outputStringCounter++;
            }
            index += 3;
        } else {
            outputString[outputStringCounter] = sourceString[index];
            outputStringCounter++;
            index++;
        }
    }

    return va(outputString);
}

const char *UI_ReplaceConversionString(const char *sourceString, const char *replaceString)
{
    int convArgs[10];
    memset(convArgs, 0, sizeof(convArgs));
    convArgs[0] = 1;
    convArgs[1] = (int)replaceString;
    return UI_ReplaceConversions(sourceString, (ConversionArguments *)convArgs);
}

const char *UI_ReplaceConversionInt(const char *sourceString, int replaceInt)
{
    char tempString[32];
    int convArgs[10];
    memset(convArgs, 0, sizeof(convArgs));
    sprintf(tempString, "%d", replaceInt);
    convArgs[0] = 1;
    convArgs[1] = (int)tempString;
    return UI_ReplaceConversions(sourceString, (ConversionArguments *)convArgs);
}

#ifndef __EMSCRIPTEN__
void UI_CloseAll(void)
{
    Menus_CloseAll(&uiInfo->uiDC);
}
#endif

void UI_CloseFocusedMenu(void)
{
    if (Menu_Count(&uiInfo->uiDC) <= 0)
        return;

    if (!Menu_GetFocused(&uiInfo->uiDC)) {
        if (Key_GetCatcher() & 8) {
            Key_SetCatcher(Key_GetCatcher() & ~8);
        }
        return;
    }

    if (!Menus_AnyFullScreenVisible(&uiInfo->uiDC)) {
        Menus_CloseAll(&uiInfo->uiDC);
    }
}

// retail 0x5310a0
static int UI_GetIndexFromSelection(int actual)
{
    int i;
    int c;

    c = 0;
    for (i = 0; i < sharedUiInfo.mapCount; i++) {
        if (sharedUiInfo.mapList[i].active) {
            if (i == actual)
                return c;
            c++;
        }
    }
    return 0;
}

// retail 0x531560
void UI_OverrideCursorPos(itemDef_t *item)
{
    listBoxDef_s *listPtr;
    int delta;
    int max;

    if (item->special == 4.0f) {
        item->cursorPos[0] = UI_GetIndexFromSelection(ui_currentNetMap->current.integer);
    } else if (item->special == 2.0f) {
        listPtr = item->typeData.listBox;
        if (!listPtr->endPos[0]) {
            item->cursorPos[0] = -1;
            return;
        }
        if (sharedUiInfo.serverStatus.currentServer >= 0
            && item->cursorPos[0] >= listPtr->startPos[0] && item->cursorPos[0] <= listPtr->endPos[0]) {
            delta = sharedUiInfo.serverStatus.currentServer - item->cursorPos[0];
            listPtr->startPos[0] += delta;
            listPtr->endPos[0] += delta;
            listPtr->cursorPos[0] += delta;
            item->cursorPos[0] = sharedUiInfo.serverStatus.currentServer;
            max = Item_ListBox_MaxScroll(item);
            if (listPtr->startPos[0] > max)
                listPtr->startPos[0] = max;
            if (listPtr->startPos[0] < 0)
                listPtr->startPos[0] = 0;
        }
    }
}

Bool UI_DrawRecordLevel(rectDef_t *rect)
{
    float voiceLevel;
    const vec_t *white;

    voiceLevel = Voice_GetVoiceLevel();

    if (voiceLevel > 0.0) {
        UI_FillRect(rect->x + 2.0f, rect->y + 2.0f, voiceLevel * rect->w - 4.0f, rect->h - 4.0f, 0, 0, (const vec_t *)((void *)&colorWhite));
    }

    white = (const vec_t *)((void *)&colorWhite);

    UI_FillRect(rect->x, rect->y, rect->w, 1.0f, 0, 0, white);

    UI_FillRect(rect->x, rect->h + rect->y - 1.0f, rect->w, 1.0f, 0, 0, white);

    UI_FillRect(rect->x, rect->y + 1.0f, 1.0f, rect->h - 2.0f, 0, 0, white);

    UI_FillRect(rect->x + rect->w - 1.0f, rect->y + 1.0f, 1.0f, rect->h - 2.0f, 0, 0, white);

    return 1;
}

static const char *UI_SelectedMap(int index, int *actual)
{
    int i, c;

    c = 0;
    *actual = 0;
    for (i = 0; i < sharedUiInfo.mapCount; i++) {
        if (sharedUiInfo.mapList[i].active) {
            if (c == index) {
                *actual = i;
                return sharedUiInfo.mapList[i].mapName;
            }
            c++;
        }
    }
    return "";
}

MaterialHandle UI_FeederItemImage(const float feederID, int index)
{
    int actual;

    if (feederID == 4.0f) {
        UI_SelectedMap(index, &actual);
        index = actual;
        if (index >= 0 && index < sharedUiInfo.mapCount) {
            if (!sharedUiInfo.mapList[index].levelShot)
                sharedUiInfo.mapList[index].levelShot = CL_RegisterMaterialNoMip(sharedUiInfo.mapList[index].mapLoadName, 3);
            return sharedUiInfo.mapList[index].levelShot;
        }
    }
    return 0;
}

#ifndef __EMSCRIPTEN__
void UI_DrawText(const char *text, int maxChars, FontHandle font, float x, float y, int horzAlign, int vertAlign, float scale, const vec_t *color, int style)
{
    float xScale;
    float yScale;

    diag_drawtext(text, (void *)font, x, y, scale);

    xScale = CL_NormalizedTextScale(font, scale);
    yScale = xScale;
    CalcSplitScreenTextOffset(font, &y);
    CalcScreenPlacement(&x, &y, &xScale, &yScale, horzAlign, vertAlign);

    x = (float)I_fround(x);
    y = (float)I_fround(y);

    CL_DrawTextPhysical(text, maxChars, font, x, y, xScale, yScale, color, style);
}
#endif

void UI_DrawTextWithCursor(const char *text, int maxChars, FontHandle font, float x, float y, int horzAlign, int vertAlign, float scale, const vec_t *color, int style, int cursorPos, char cursor)
{
    float xScale;
    float yScale;

    xScale = CL_NormalizedTextScale(font, scale);
    yScale = xScale;
    CalcSplitScreenTextOffset(font, &y);
    CalcScreenPlacement(&x, &y, &xScale, &yScale, horzAlign, vertAlign);

    x = (float)I_fround(x);
    y = (float)I_fround(y);

    CL_DrawTextPhysicalWithCursor(text, maxChars, font, x, y, xScale, yScale, color, style, cursorPos, cursor);
}

#ifndef __EMSCRIPTEN__
static void UI_StopServerRefresh(void)
{
    int count;

    if (!sharedUiInfo.serverStatus.refreshActive)
        return;
    sharedUiInfo.serverStatus.refreshActive = 0;
    Com_Printf("%d servers listed in browser with %d players.\n",
               sharedUiInfo.serverStatus.numDisplayServers, sharedUiInfo.serverStatus.numPlayersOnServers);
    count = LAN_GetServerCount(ui_netSource->current.integer) - sharedUiInfo.serverStatus.numDisplayServers;
    if (count > 0)
        Com_Printf("%d servers not listed (filtered out by game browser settings)\n", count);
}
#endif

#ifndef __EMSCRIPTEN__
// retail 0x530040; name unknown
static void UI_ClearDisplayedServers(void)
{
    sharedUiInfo.serverStatus.numDisplayServers = 0;
    sharedUiInfo.serverStatus.numPlayersOnServers = 0;
    sharedUiInfo.serverStatus.serverCount = LAN_GetServerCount((ui_netSource)->current.integer);
}

static void UI_StartServerRefresh(qboolean full)
{
    qtime_t q;
    char dvarName[0x18];
    const char *ptr;

    Com_RealTime(&q);
    snprintf(dvarName, sizeof(dvarName), "ui_lastServerRefresh_%i", (ui_netSource)->current.integer);
    Dvar_SetStringByName(dvarName,
                         va("%s %i, %i   %i:%02i",
                            UI_SafeTranslateString(MonthAbbrev[q.tm_mon]),
                            q.tm_mday,
                            q.tm_year + 1900,
                            q.tm_hour,
                            q.tm_min));

    if (!full) {
        LAN_ResetPings((ui_netSource)->current.integer);
        sharedUiInfo.serverStatus.refreshActive = 1;
        sharedUiInfo.serverStatus.refreshtime = uiInfo->uiDC.realTime + 1000;
        return;
    }

    sharedUiInfo.serverStatus.refreshActive = 1;
    sharedUiInfo.serverStatus.nextDisplayRefresh = uiInfo->uiDC.realTime + 1000;
    UI_ClearDisplayedServers();
    LAN_MarkServerDirty((ui_netSource)->current.integer, -1, 1);
    LAN_ResetPings((ui_netSource)->current.integer);

    if ((ui_netSource)->current.integer == 0) {
        Cbuf_ExecuteText(0, "localservers\n");
        sharedUiInfo.serverStatus.refreshtime = uiInfo->uiDC.realTime + 1000;
        return;
    }

    sharedUiInfo.serverStatus.refreshtime = uiInfo->uiDC.realTime + 5000;

    if ((ui_netSource)->current.integer == 1) {
        ptr = Dvar_GetVariantString("debug_protocol");
        if (strlen(ptr))
            Cbuf_ExecuteText(0, va("globalservers %d %s full empty\n", 0, ptr));
        else
            Cbuf_ExecuteText(0, va("globalservers %d %d full empty\n", 0, 0x73));
    }
}
#endif

#ifndef __EMSCRIPTEN__
static void UI_InsertServerAtPosition(int serverIndex, int position)
{
    int i;

    if (position < 0 || position > sharedUiInfo.serverStatus.numDisplayServers)
        return;

    if (position <= sharedUiInfo.serverStatus.currentServer && sharedUiInfo.serverStatus.numDisplayServers)
        sharedUiInfo.serverStatus.currentServer++;

    sharedUiInfo.serverStatus.numDisplayServers++;
    for (i = sharedUiInfo.serverStatus.numDisplayServers; i > position; i--)
        sharedUiInfo.serverStatus.displayServers[i] = sharedUiInfo.serverStatus.displayServers[i - 1];
    sharedUiInfo.serverStatus.displayServers[position] = serverIndex;
}
#endif

#ifndef __EMSCRIPTEN__
static void UI_RemoveDuplicateFromFavorites(int serverIndex)
{
    int i, j;

    for (i = 0; i < sharedUiInfo.serverStatus.numDisplayServers; i++) {
        if (sharedUiInfo.serverStatus.displayServers[i] == serverIndex) {
            sharedUiInfo.serverStatus.numDisplayServers--;
            for (j = i; j < sharedUiInfo.serverStatus.numDisplayServers; j++)
                sharedUiInfo.serverStatus.displayServers[j] = sharedUiInfo.serverStatus.displayServers[j + 1];
            return;
        }
    }
}
#endif

#ifndef __EMSCRIPTEN__
static void UI_BinaryInsertServer(int serverIndex)
{
    int numDisplay = sharedUiInfo.serverStatus.numDisplayServers;
    int *displayServers = sharedUiInfo.serverStatus.displayServers;
    int lo, hi, mid, testIdx, cmp, position, lastCmp;
    int source = (ui_netSource)->current.integer;
    int sortKey = sharedUiInfo.serverStatus.sortKey;
    int sortDir = sharedUiInfo.serverStatus.sortDir;

    lo = 0;
    hi = numDisplay;
    position = 0;
    lastCmp = 0;

    while (hi > 0) {
        mid = hi / 2;
        testIdx = lo + mid;

        cmp = LAN_CompareServers(source, sortKey, sortDir, serverIndex, displayServers[testIdx]);
        lastCmp = cmp;

        if (cmp == 0) {
            UI_InsertServerAtPosition(serverIndex, testIdx);
            return;
        } else if (cmp > 0) {
            hi -= mid;
            position = testIdx + 1;
            lo = position;
            hi--;
        } else {
            hi -= mid;
        }
    }

    if (lastCmp >= 0)
        position = lo + 1;
    else
        position = lo;

    UI_InsertServerAtPosition(serverIndex, position);
}
#endif

#ifndef __EMSCRIPTEN__
static void __attribute_regparm__(0) UI_BuildServerDisplayList(qboolean force)
{
    char info_buf[0x400];
    int count, i, clients, ping;

    if (!force) {
        if (uiInfo->uiDC.realTime <= sharedUiInfo.serverStatus.nextDisplayRefresh)
            return;
    } else if (force == 2) {
        force = 0;
    }

    clients = 0;

    {
        const char *motdString = Dvar_GetString("cl_motdString");
        int motdLen;
        I_strncpyz(sharedUiInfo.serverStatus.motd, motdString, 0x400);
        motdLen = strlen(sharedUiInfo.serverStatus.motd);

        if (motdLen == 0) {
            strcpy(sharedUiInfo.serverStatus.motd, va("%s - %s", UI_SafeTranslateString("EXE_COD_MULTIPLAYER"), "1.0"));
            motdLen = strlen(sharedUiInfo.serverStatus.motd);
        }

        if (motdLen != sharedUiInfo.serverStatus.motdLen) {
            sharedUiInfo.serverStatus.motdLen = motdLen;
            sharedUiInfo.serverStatus.motdWidth = -1;
        }
    }

    if (force) {
        numclean = 0;
        UI_ClearDisplayedServers();

        if (sharedUiInfo.serverStatus.currentServer >= 0) {
            Menu_SetFeederSelection(&uiInfo->uiDC, 0, 2, 0, 0);
        }

        LAN_MarkServerDirty((ui_netSource)->current.integer, -1, 1);
    }

    count = LAN_GetServerCount((ui_netSource)->current.integer);
    if (LAN_WaitServerResponse((ui_netSource)->current.integer) || ((ui_netSource)->current.integer == 0 && count == 0)) {
        UI_ClearDisplayedServers();
        sharedUiInfo.serverStatus.nextDisplayRefresh = uiInfo->uiDC.realTime + 500;
        return;
    }

    qsort(sharedUiInfo.serverStatus.displayServers, sharedUiInfo.serverStatus.numDisplayServers, 4, UI_ServersQsortCompare);

    for (i = 0; i < count; i++) {
        if (!LAN_ServerIsDirty((ui_netSource)->current.integer, i))
            continue;

        ping = LAN_GetServerPing((ui_netSource)->current.integer, i);
        if (ping > 0 || (ui_netSource)->current.integer == 2) {
            LAN_GetServerInfo((ui_netSource)->current.integer, i, info_buf, 0x400);
            clients = atoi(Info_ValueForKey(info_buf, "clients"));
            sharedUiInfo.serverStatus.numPlayersOnServers += clients;
            if (!I_strnicmp(Info_ValueForKey(info_buf, "addr"), "000.000.000.000", 15)) {
                LAN_MarkServerDirty((ui_netSource)->current.integer, i, 0);
                continue;
            }
            if (!(ui_browserShowEmpty)->current.enabled && !clients) {
                LAN_MarkServerDirty((ui_netSource)->current.integer, i, 0);
                continue;
            }
            if (!(ui_browserShowFull)->current.enabled && clients == atoi(Info_ValueForKey(info_buf, "sv_maxclients"))) {
                LAN_MarkServerDirty((ui_netSource)->current.integer, i, 0);
                continue;
            }
            if (!(ui_browserShowPassword)->current.enabled && atoi(Info_ValueForKey(info_buf, "pswrd"))) {
                LAN_MarkServerDirty((ui_netSource)->current.integer, i, 0);
                continue;
            }
            if (!(ui_browserShowNoPassword)->current.enabled && !atoi(Info_ValueForKey(info_buf, "pswrd"))) {
                LAN_MarkServerDirty((ui_netSource)->current.integer, i, 0);
                continue;
            }
            if ((ui_browserShowPure)->current.enabled && !atoi(Info_ValueForKey(info_buf, "pure"))) {
                LAN_MarkServerDirty((ui_netSource)->current.integer, i, 0);
                continue;
            }
            if ((ui_browserShowDedicated)->current.enabled) {
                int hw = atoi(Info_ValueForKey(info_buf, "hw"));
                if (hw != 1 && hw != 2 && hw != 3) {
                    LAN_MarkServerDirty((ui_netSource)->current.integer, i, 0);
                    continue;
                }
            }
            if ((ui_browserMod)->current.integer >= 0 && atoi(Info_ValueForKey(info_buf, "mod")) != (ui_browserMod)->current.integer) {
                LAN_MarkServerDirty((ui_netSource)->current.integer, i, 0);
                continue;
            }
            if ((ui_browserFriendlyfire)->current.integer >= 0 && atoi(Info_ValueForKey(info_buf, "ff")) != (ui_browserFriendlyfire)->current.integer) {
                LAN_MarkServerDirty((ui_netSource)->current.integer, i, 0);
                continue;
            }
            if ((ui_browserKillcam)->current.integer >= 0 && atoi(Info_ValueForKey(info_buf, "kc")) != (ui_browserKillcam)->current.integer) {
                LAN_MarkServerDirty((ui_netSource)->current.integer, i, 0);
                continue;
            }
            if (sharedUiInfo.joinGameTypes[(ui_joinGameType)->current.integer].gameTypeName[0]
                && I_stricmp(Info_ValueForKey(info_buf, "gametype"), sharedUiInfo.joinGameTypes[(ui_joinGameType)->current.integer].gameType)) {
                LAN_MarkServerDirty((ui_netSource)->current.integer, i, 0);
                continue;
            }
            if (ui_serverFilterType > 0 && I_stricmp(Info_ValueForKey(info_buf, "game"), serverFilters[ui_serverFilterType].basedir)) {
                LAN_MarkServerDirty((ui_netSource)->current.integer, i, 0);
                continue;
            }

            if ((ui_netSource)->current.integer == 2)
                UI_RemoveDuplicateFromFavorites(i);
            UI_BinaryInsertServer(i);
            if (ping > 0) {
                LAN_MarkServerDirty((ui_netSource)->current.integer, i, 0);
                numclean++;
            }
        }
    }

    sharedUiInfo.serverStatus.refreshtime = uiInfo->uiDC.realTime;
}
#endif

static void UI_SelectCurrentGameType(void)
{
    const char *currType;
    int i;

    currType = Dvar_GetString("g_gametype");
    for (i = 0; i < sharedUiInfo.numGameTypes; i++) {
        if (!I_stricmp(currType, sharedUiInfo.gameTypes[i].gameType)) {
            Dvar_SetInt((dvar_t *)ui_netGameType, i);
            Dvar_SetString((dvar_t *)ui_netGameTypeName, sharedUiInfo.gameTypes[i].gameType);
            return;
        }
    }
}

static int UI_MapCountByGameType(void)
{
    int i, c, game;

    c = 0;
    game = ui_netGameType->current.integer;
    for (i = 0; i < sharedUiInfo.mapCount; i++) {
        sharedUiInfo.mapList[i].active = 0;
        if (sharedUiInfo.mapList[i].typeBits & (1 << game)) {
            c++;
            sharedUiInfo.mapList[i].active = 1;
        }
    }
    return c;
}

// retail 0x52fd50; name unknown
static void UI_SelectFirstActiveMap(void)
{
    int i;

    for (i = 0; i < sharedUiInfo.mapCount; i++) {
        if (sharedUiInfo.mapList[i].active) {
            Menu_SetFeederSelection(&uiInfo->uiDC, NULL, 4, 0, "createserver_maps");
            Dvar_SetInt((dvar_t *)ui_currentNetMap, i);
            return;
        }
    }
}

// name unknown
static int UI_ListIndexForMap(int map)
{
    int i;
    int listIndex;

    listIndex = 0;
    for (i = 0; i < sharedUiInfo.mapCount; i++) {
        if (sharedUiInfo.mapList[i].active) {
            if (i == map)
                return listIndex;
            listIndex++;
        }
    }
    return 0;
}

// retail 0x52fda0; name unknown
static void UI_SetMapFeederSelection(int map)
{
    Menu_SetFeederSelection(&uiInfo->uiDC, NULL, 4, UI_ListIndexForMap(map), "createserver_maps");
}

static qboolean UI_GameType_HandleKey(int flags, float *special, int key, qboolean resetMap)
{
    int oldCount;
    int value;

    if (key == 0xc8 || key == 0xc9 || key == 0xd || key == 0xbf) {
        oldCount = UI_MapCountByGameType();

        value = ui_gametype->current.integer;
        if (key == 0xc9) {
            value--;
            if (value == 2)
                value = 1;
            else if (value < 2)
                value = sharedUiInfo.numGameTypes - 1;
        } else {
            value++;
            if (value >= sharedUiInfo.numGameTypes)
                value = 1;
            else if (value == 2)
                value = 3;
        }
        Dvar_SetInt((dvar_t *)ui_gametype, value);

        if (resetMap && oldCount != UI_MapCountByGameType())
            Dvar_SetInt((dvar_t *)ui_currentMap, 0);
        return 1;
    }
    return 0;
}

static qboolean UI_NetGameType_HandleKey(int flags, float *special, int key)
{
    int value;

    if (key == 0xc8 || key == 0xc9 || key == 0xd || key == 0xbf) {
        if (key == 0xc9) {
            value = ui_netGameType->current.integer;
            if (!value)
                value = sharedUiInfo.numGameTypes;
            value--;
        } else {
            value = ui_netGameType->current.integer + 1;
            if (value == sharedUiInfo.numGameTypes)
                value = 0;
        }

        Dvar_SetInt((dvar_t *)ui_netGameType, value);
        Dvar_SetString((dvar_t *)ui_netGameTypeName, sharedUiInfo.gameTypes[ui_netGameType->current.integer].gameType);
        UI_MapCountByGameType();
        if (!sharedUiInfo.mapList[ui_currentNetMap->current.integer].active)
            UI_SelectFirstActiveMap();
        else
            UI_SetMapFeederSelection(ui_currentNetMap->current.integer);
        UI_SelectCurrentMap();
        return 1;
    }
    return 0;
}

static qboolean UI_JoinGameType_HandleKey(int flags, float *special, int key)
{
    int value;

    if (key == 0xc8 || key == 0xc9 || key == 0xd || key == 0xbf) {
        if (key == 0xc9) {
            value = ui_joinGameType->current.integer;
            if (!value)
                value = sharedUiInfo.numJoinGameTypes;
            value--;
        } else {
            value = ui_joinGameType->current.integer + 1;
            if (value == sharedUiInfo.numJoinGameTypes)
                value = 0;
        }

        Dvar_SetInt((dvar_t *)ui_joinGameType, value);
        UI_BuildServerDisplayList(1);
        return 1;
    }
    return 0;
}

static qboolean UI_NetSource_HandleKey(int flags, float *special, int key)
{
    int value;

    if (key == 0xc8 || key == 0xc9 || key == 0xd || key == 0xbf) {
        if (key == 0xc9) {
            value = ui_netSource->current.integer;
            if (!value)
                value = 3;
            value--;
        } else {
            value = ui_netSource->current.integer + 1;
            if (value == 3)
                value = 0;
        }

        UI_BuildServerDisplayList(1);
        Dvar_SetInt((dvar_t *)ui_netSource, value);
        if (ui_netSource->current.integer != 1) {
            UI_StartServerRefresh(1);
            UI_BuildServerDisplayList(1);
        }
        return 1;
    }
    return 0;
}

static qboolean UI_NetFilter_HandleKey(int flags, float *special, int key)
{
    if (key == 0xc8 || key == 0xc9 || key == 0xd || key == 0xbf) {
        if (key == 0xc9)
            ui_serverFilterType--;
        else
            ui_serverFilterType++;

        if (ui_serverFilterType >= 1)
            ui_serverFilterType = 0;
        else if (ui_serverFilterType < 0)
            ui_serverFilterType = 0;

        UI_BuildServerDisplayList(1);
        return 1;
    }
    return 0;
}

qboolean UI_OwnerDrawHandleKey(int ownerDraw, int flags, float *special, int key)
{
    switch (ownerDraw) {
    case 0xcd:
        return UI_GameType_HandleKey(flags, special, key, 1);
    case 0xf5:
        return UI_NetGameType_HandleKey(flags, special, key);
    case 0xfd:
        return UI_JoinGameType_HandleKey(flags, special, key);
    case 0xdc:
        UI_NetSource_HandleKey(flags, special, key);
        break;
    case 0xde:
        UI_NetFilter_HandleKey(flags, special, key);
        break;
    default:
        break;
    }
    return 0;
}

const char *UI_FeederItemText(float feederID, int index, int column, MaterialHandle *handle)
{
    int actual;

    *handle = 0;

    if (feederID == 4.0f) {
        return UI_SelectedMap(index, &actual);
    } else if (feederID == 2.0f) {

        int pingVal;

        UI_UpdateServerCount();

        if (index >= 0 && index < sharedUiInfo.serverStatus.numDisplayServers) {
            if (lastColumn != column || lastTime > uiInfo->uiDC.realTime + 5000) {
                LAN_GetServerInfo((ui_netSource)->current.integer,
                                  sharedUiInfo.serverStatus.displayServers[index],
                                  info, 0x400);
                lastColumn = column;
                lastTime = uiInfo->uiDC.realTime;
            }

            pingVal = atoi(Info_ValueForKey(info, "ping"));

            switch (column) {
            case 0:
                if (atoi(Info_ValueForKey(info, "pswrd")))
                    return "X";
                return "";
            case 1: {
                int hw = atoi(Info_ValueForKey(info, "hw"));
                if (hw >= 0 && hw < 8)
                    *handle = sharedUiInfo.serverHardwareIconList[hw];
                return "";
            }
            case 7:
                if (atoi(Info_ValueForKey(info, "pure")))
                    return "X";
                return "";
            case 8:
                if (atoi(Info_ValueForKey(info, "mod")))
                    return "X";
                return "";
            case 6:
                if (atoi(Info_ValueForKey(info, "voice")))
                    return "X";
                return "";
            case 2:
                if (pingVal <= 0)
                    return Info_ValueForKey(info, "addr");
                I_strncpyz(clientBuff, Info_ValueForKey(info, "hostname"), 0x14);
                return clientBuff;
            case 3:
                return UI_GetMapDisplayName(Info_ValueForKey(info, "mapname"));
            case 4:
                Com_sprintf(clientBuff, 0x20, "%s (%s)", Info_ValueForKey(info, "clients"), Info_ValueForKey(info, "sv_maxclients"));
                return clientBuff;
            case 5:
                if (Info_ValueForKey(info, "gametype") && *Info_ValueForKey(info, "gametype"))
                    return Info_ValueForKey(info, "gametype");
                return "?";
            case 9:
                if (pingVal <= 0)
                    return "...";
                return Info_ValueForKey(info, "ping");
            }
        }
    } else if (feederID == 13.0f) {
        if (index >= 0 && index < sharedUiInfo.serverStatusInfo.numLines) {
            if (column >= 0 && column < 4) {
                const char *text = *(const char **)((byte *)&sharedUiInfo + 109864 + (column + index * 4) * 4);
                if (text[0] == '@')
                    return UI_SafeTranslateString(text + 1);
                return text;
            }
        }
    } else if (feederID == 7.0f) {
        if (index >= 0 && index < sharedUiInfo.playerCount)
            return sharedUiInfo.playerNames[index];
    } else if (feederID == 9.0f) {
        if (index >= 0 && index < sharedUiInfo.modCount) {
            const char *desc = *(const char **)((byte *)&sharedUiInfo + 25976 + index * 8);
            if (desc && desc[0] != '\0')
                return desc;
            return *(const char **)((byte *)&sharedUiInfo + 25972 + index * 8);
        }
    } else if (feederID == 20.0f) {
        if (index >= 0 && index < sharedUiInfo.playerCount) {
            if (column == 1)
                return sharedUiInfo.playerNames[index];
            if (CL_IsPlayerMuted(sharedUiInfo.playerClientNums[index]))
                return UI_SafeTranslateString("MP_MUTED");
        }
    }
    if (feederID == 24.0f) {
        if (index >= 0 && index < uiInfo->playerProfileCount)
            return uiInfo->playerProfileName[uiInfo->playerProfileStatus.displayProfile[index]];
    }

    return "";
}

void UI_DrawRect(float x, float y, float width, float height, int horzAlign, int vertAlign, float size, const vec_t *color)
{
    UI_DrawTopBottom(x, y, width, height, horzAlign, vertAlign, size, color);
    UI_DrawSides(x, y, width, height, horzAlign, vertAlign, size, color);
}

int UI_OwnerDrawWidth(int ownerDraw, FontHandle font, float scale)
{
    const char *s = NULL;

    switch (ownerDraw) {
    case 0x27:
        s = CG_GameTypeString();
        break;
    case 0x32:
        s = CG_GetKillerText();
        break;
    case 0xcd:
        s = sharedUiInfo.gameTypes[(ui_gametype)->current.integer].gameTypeName;
        break;
    case 0xdc:
        if ((ui_netSource)->current.integer > sharedUiInfo.numJoinGameTypes)
            Dvar_SetInt((dvar_t *)ui_netSource, 0);
        s = SEH_LocalizeTextMessage(va("EXE_NETSOURCE\x14%s", netSources[(ui_netSource)->current.integer]), "net source", LOCMSG_SAFE);
        break;
    case 0xde: {
        if (ui_serverFilterType < 0 || ui_serverFilterType > 1)
            ui_serverFilterType = 0;
        s = SEH_LocalizeTextMessage(va("EXE_SERVERFILTER\x14%s", serverFilters[ui_serverFilterType].description), "server filter", LOCMSG_SAFE);
        break;
    }
    case 0xfa:
        if (Display_KeyBindPending())
            s = UI_SafeTranslateString("EXE_KEYWAIT");
        else
            s = UI_SafeTranslateString("EXE_KEYCHANGE");
        break;
    case 0xf7:
        s = Dvar_GetVariantString(va("ui_lastServerRefresh_%i", (ui_netSource)->current.integer));
        break;
    default:
        return 0;
    }

    if (!s)
        return 0;

    {
        float actualScale = CL_NormalizedTextScale(font, scale);
        return (int)((float)CL_TextWidth(s, 0, font) * actualScale);
    }
}

static void UI_DrawGameType(rectDef_t *rect, FontHandle font, float scale, vec_t *color, int textStyle)
{
    const char *text;

    if (sharedUiInfo.gameTypes[ui_gametype->current.integer].gameTypeName[0])
        text = UI_SafeTranslateString(sharedUiInfo.gameTypes[ui_gametype->current.integer].gameTypeName);
    else
        text = UI_SafeTranslateString("EXE_ALL");
    UI_DrawText(text, 0x7fffffff, font, rect->x, rect->y, rect->horzAlign, rect->vertAlign, scale, color, textStyle);
}

static void UI_DrawNetGameType(rectDef_t *rect, FontHandle font, float scale, vec_t *color, int textStyle)
{
    const char *text;

    if (ui_netGameType->current.integer > sharedUiInfo.numGameTypes) {
        Dvar_SetInt((dvar_t *)ui_netGameType, 0);
        Dvar_SetString((dvar_t *)ui_netGameTypeName, sharedUiInfo.gameTypes[0].gameType);
    }
    if (sharedUiInfo.gameTypes[ui_netGameType->current.integer].gameTypeName[0])
        text = UI_SafeTranslateString(sharedUiInfo.gameTypes[ui_netGameType->current.integer].gameTypeName);
    else
        text = UI_SafeTranslateString("EXE_ALL");
    UI_DrawText(text, 0x7fffffff, font, rect->x, rect->y, rect->horzAlign, rect->vertAlign, scale, color, textStyle);
}

static void UI_DrawJoinGameType(rectDef_t *rect, FontHandle font, float scale, vec_t *color, int textStyle)
{
    const char *text;

    if (ui_joinGameType->current.integer > sharedUiInfo.numJoinGameTypes)
        Dvar_SetInt((dvar_t *)ui_joinGameType, 0);
    if (sharedUiInfo.joinGameTypes[ui_joinGameType->current.integer].gameTypeName[0])
        text = UI_SafeTranslateString(sharedUiInfo.joinGameTypes[ui_joinGameType->current.integer].gameTypeName);
    else
        text = UI_SafeTranslateString("EXE_ALL");
    UI_DrawText(text, 0x7fffffff, font, rect->x, rect->y, rect->horzAlign, rect->vertAlign, scale, color, textStyle);
}

static void UI_DrawNetMapPreview(rectDef_t *rect, float scale, vec_t *color)
{
    if (sharedUiInfo.serverStatus.currentServerPreview > 0) {
        UI_DrawHandlePic(rect->x, rect->y, rect->w, rect->h, rect->horzAlign, rect->vertAlign, color, sharedUiInfo.serverStatus.currentServerPreview);
    } else {
        UI_DrawHandlePic(rect->x, rect->y, rect->w, rect->h, rect->horzAlign, rect->vertAlign, color, CL_RegisterMaterialNoMip("menu/art/unknownmap", 3));
    }
}

static void UI_DrawNetMapCinematic(rectDef_t *rect, float scale, vec_t *color)
{
    if (ui_currentNetMap->current.integer >= sharedUiInfo.mapCount)
        Dvar_SetInt((dvar_t *)ui_currentNetMap, 0);

    if (sharedUiInfo.serverStatus.currentServerCinematic >= 0) {
        CIN_RunCinematic(sharedUiInfo.serverStatus.currentServerCinematic);
        CIN_SetExtents(sharedUiInfo.serverStatus.currentServerCinematic, (int)rect->x, (int)rect->y, (int)rect->w, (int)rect->h);
        CIN_DrawCinematic(sharedUiInfo.serverStatus.currentServerCinematic);
    } else {
        UI_DrawNetMapPreview(rect, scale, color);
    }
}

static void UI_DrawNetFilter(rectDef_t *rect, FontHandle font, float scale, vec_t *color, int textStyle)
{
    const char *text;

    if (ui_serverFilterType < 0 || ui_serverFilterType > 1)
        ui_serverFilterType = 0;
    text = SEH_LocalizeTextMessage(va("EXE_SERVERFILTER\x14%s", serverFilters[ui_serverFilterType].description), "server filter", LOCMSG_SAFE);
    UI_DrawText(text, 0x7fffffff, font, rect->x, rect->y, rect->horzAlign, rect->vertAlign, scale, color, textStyle);
}

static void UI_DrawNetSource(rectDef_t *rect, FontHandle font, float scale, vec_t *color, int textStyle)
{
    const char *text;

    text = SEH_LocalizeTextMessage(va("EXE_NETSOURCE\x14%s", netSources[ui_netSource->current.integer]), "net source", LOCMSG_SAFE);
    UI_DrawText(text, 0x7fffffff, font, rect->x, rect->y, rect->horzAlign, rect->vertAlign, scale, color, textStyle);
}

static void UI_DrawServerRefreshDate(rectDef_t *rect, FontHandle font, float scale, vec_t *color, int textStyle)
{
    char buff[64];
    vec4_t lowLight, newColor;
    const char *string;

    if (sharedUiInfo.serverStatus.refreshActive) {
        lowLight[0] = 0.8 * color[0];
        lowLight[1] = 0.8 * color[1];
        lowLight[2] = 0.8 * color[2];
        lowLight[3] = 0.8 * color[3];
        LerpColor(color, lowLight, newColor, (sin((double)(uiInfo->uiDC.realTime / 75)) + 1.0) * 0.5);
        if (!LAN_WaitServerResponse(ui_netSource->current.integer))
            string = UI_ReplaceConversionInt(UI_SafeTranslateString("EXE_GETTINGINFOFORSERVERS"), LAN_GetServerCount(ui_netSource->current.integer));
        else
            string = UI_SafeTranslateString("EXE_WAITINGFORMASTERSERVERRESPONSE");
        UI_DrawText(string, 0x7fffffff, font, rect->x, rect->y, rect->horzAlign, rect->vertAlign, scale, newColor, textStyle);
    } else {
        I_strncpyz(buff, Dvar_GetVariantString(va("ui_lastServerRefresh_%i", ui_netSource->current.integer)), 64);
        string = UI_ReplaceConversionString(UI_SafeTranslateString("EXE_REFRESHTIME"), buff);
        UI_DrawText(string, 0x7fffffff, font, rect->x, rect->y, rect->horzAlign, rect->vertAlign, scale, color, textStyle);
    }
}

static void UI_DrawKeyBindStatus(rectDef_t *rect, FontHandle font, float scale, vec_t *color, int textStyle)
{
    const char *string;

    if (Display_KeyBindPending())
        string = "EXE_KEYWAIT";
    else
        string = "EXE_KEYCHANGE";
    UI_DrawText(UI_SafeTranslateString(string), 0x7fffffff, font, rect->x, rect->y, rect->horzAlign, rect->vertAlign, scale, color, textStyle);
}

static void UI_DrawLocalTalking(rectDef_t *rect, vec_t *color)
{
    if ((sv_voice)->current.enabled && (cl_voice)->current.enabled && IsTalking())
        UI_DrawHandlePic(rect->x, rect->y, rect->w, rect->h, rect->horzAlign, rect->vertAlign, color, CL_RegisterMaterialNoMip("voice_on", 7));
}

static int UI_GetTalkerClientNum(int num)
{
    int client;
    int talker;

    talker = 0;
    for (client = 0; client < 64; client++) {
        if (CL_IsPlayerTalking(client)) {
            if (talker == num)
                return client;
            talker++;
        }
    }
    return -1;
}

// Original name unknown.
static int UI_GetPlayerListIndex(int client)
{
    int index;

    for (index = 0; index < 64; index++) {
        if (sharedUiInfo.playerClientNums[index] == client)
            return index;
    }
    return -1;
}

static void UI_DrawTalkerNum(int num, rectDef_t *rect, FontHandle font, vec_t *color, float scale, int textStyle)
{
    int client;
    int index;
    MaterialHandle material;
    float actualScale;
    int textHeight;

    client = UI_GetTalkerClientNum(num);
    if (client < 0)
        return;

    UI_BuildPlayerList();
    index = UI_GetPlayerListIndex(client);

    material = CL_RegisterMaterialNoMip("voice_on", 7);
    actualScale = CL_NormalizedTextScale(font, scale);
    textHeight = (int)(CL_TextHeight(font) * actualScale);
    UI_DrawHandlePic(rect->x, rect->y, rect->w, rect->h, rect->horzAlign, rect->vertAlign, color, material);
    UI_DrawText(sharedUiInfo.playerNames[index], 32, font, rect->x + rect->w + 2.0f, (rect->h - (float)textHeight) * 0.5f + (float)textHeight + rect->y, rect->horzAlign, rect->vertAlign, scale, color, textStyle);
}

void UI_OwnerDraw(float x, float y, float w, float h, int horzAlign, int vertAlign, float text_x, float text_y, int ownerDraw, int ownerDrawFlags, int align, float special, FontHandle font, float scale, vec_t *color, MaterialHandle material, int textStyle)
{
    rectDef_t rect;

    if (clients->cgameInitialized)
        CG_OwnerDraw(x, y, w, h, horzAlign, vertAlign, text_x, text_y, ownerDraw, ownerDrawFlags, align, special, font, scale, color, material, textStyle);

    rect.x = x + text_x;
    rect.y = y + text_y;
    rect.w = w;
    rect.h = h;
    rect.horzAlign = horzAlign;
    rect.vertAlign = vertAlign;

    switch (ownerDraw) {
    case 254:
    {
        int cinHandle = sharedUiInfo.previewMovie;
        if (cinHandle <= -2)
            return;
        cinHandle = CIN_PlayCinematic(sharedUiInfo.movieList[sharedUiInfo.movieIndex], 0, 0, 0, 0, 10);
        sharedUiInfo.previewMovie = cinHandle;
        if (cinHandle < 0) {
            sharedUiInfo.previewMovie = -2;
            return;
        }
        CIN_RunCinematic(cinHandle);
        CIN_SetExtents(cinHandle, (int)rect.x, (int)rect.y, (int)rect.w, (int)rect.h);
        CIN_DrawCinematic(cinHandle);
        return;
    }
    case 205:
        UI_DrawGameType(&rect, font, scale, color, textStyle);
        break;
    case 245:
        UI_DrawNetGameType(&rect, font, scale, color, textStyle);
        break;
    case 253:
        UI_DrawJoinGameType(&rect, font, scale, color, textStyle);
        break;
    case 206:
        UI_DrawMapPreview(&rect, color, 1);
        break;
    case 244:
        UI_DrawMapPreview(&rect, color, 0);
        break;
    case 255:
        UI_DrawMapPreview(&rect, color, 1);
        break;
    case 221:
        UI_DrawNetMapPreview(&rect, scale, color);
        break;
    case 246:
        UI_DrawNetMapCinematic(&rect, scale, color);
        break;
    case 222:
        UI_DrawNetFilter(&rect, font, scale, color, textStyle);
        break;
    case 220:
        UI_DrawNetSource(&rect, font, scale, color, textStyle);
        break;
    case 247:
        UI_DrawServerRefreshDate(&rect, font, scale, color, textStyle);
        break;
    case 250:
        UI_DrawKeyBindStatus(&rect, font, scale, color, textStyle);
        break;
    case 265:
        UI_DrawRecordLevel(&rect);
        break;
    case 266:
        UI_DrawLocalTalking(&rect, color);
        break;
    case 267:
    case 268:
    case 269:
    case 270:
        UI_DrawTalkerNum(ownerDraw - 267, &rect, font, color, scale, textStyle);
        break;
    default:
        return;
    }
}

static const char *UI_StrContains(const char *str, const char *charset)
{
    int i;

    while (*str) {
        for (i = 0; charset[i] && str[i]; i++) {
            if (toupper((signed char)charset[i]) != toupper((signed char)str[i]))
                break;
        }
        if (!charset[i])
            return str;
        str++;
    }
    return NULL;
}

#ifndef __EMSCRIPTEN__
static inline __attribute__((always_inline)) void UI_UpdateServerCount(void)
{
    int serverCount = LAN_GetServerCount((ui_netSource)->current.integer);
    if (sharedUiInfo.serverStatus.serverCount != serverCount) {
        sharedUiInfo.serverStatus.serverCount = serverCount;
        if (sharedUiInfo.serverStatus.numDisplayServers != 0) {
            sharedUiInfo.serverStatus.currentServer = -1;
            UI_BuildServerDisplayList(1);
        }
    }
}
#endif

static void BM_NOINLINE UI_BuildFindPlayerList(void)
{
    serverStatusInfo_t info;
    char name[0x22];
    char infoString[0x400];
    int i, j;

    if (!uiInfo->nextFindPlayerRefresh || uiInfo->nextFindPlayerRefresh > uiInfo->uiDC.realTime)
        return;

    UI_UpdateServerCount();

    for (i = 0; i < 16; i++) {
        if (sharedUiInfo.pendingServerStatus.server[i].valid) {
            if (UI_GetServerStatusInfo(sharedUiInfo.pendingServerStatus.server[i].adrstr, &info)) {
                numFound++;
                for (j = 0; j < info.numLines; j++) {
                    if (!info.lines[j][2] || !info.lines[j][2][0])
                        continue;
                    I_strncpyz(name, info.lines[j][3], sizeof(name));
                    I_CleanStr(name);
                    if (UI_StrContains(name, uiInfo->findPlayerName)) {
                        if (uiInfo->numFoundPlayerServers < 15) {
                            I_strncpyz(uiInfo->foundPlayerServerAddresses[uiInfo->numFoundPlayerServers], sharedUiInfo.pendingServerStatus.server[i].adrstr, 64);
                            I_strncpyz(uiInfo->foundPlayerServerNames[uiInfo->numFoundPlayerServers], sharedUiInfo.pendingServerStatus.server[i].name, 64);
                            uiInfo->numFoundPlayerServers++;
                        } else {
                            sharedUiInfo.pendingServerStatus.num = sharedUiInfo.serverStatus.numDisplayServers;
                        }
                    }
                }
                Com_sprintf(uiInfo->foundPlayerServerNames[uiInfo->numFoundPlayerServers], 64, "searching %d/%d...", sharedUiInfo.pendingServerStatus.num, numFound);
                sharedUiInfo.pendingServerStatus.server[i].valid = 0;
            }
        }

        if (!sharedUiInfo.pendingServerStatus.server[i].valid
            || sharedUiInfo.pendingServerStatus.server[i].startTime < uiInfo->uiDC.realTime - ui_serverStatusTimeOut->current.integer) {
            if (sharedUiInfo.pendingServerStatus.server[i].valid)
                numTimeOuts++;

            LAN_GetServerStatus(sharedUiInfo.pendingServerStatus.server[i].adrstr, 0, 0);
            sharedUiInfo.pendingServerStatus.server[i].valid = 0;

            UI_UpdateServerCount();

            if (sharedUiInfo.pendingServerStatus.num < sharedUiInfo.serverStatus.numDisplayServers) {
                sharedUiInfo.pendingServerStatus.server[i].startTime = uiInfo->uiDC.realTime;
                LAN_GetServerAddressString(ui_netSource->current.integer, sharedUiInfo.serverStatus.displayServers[sharedUiInfo.pendingServerStatus.num], sharedUiInfo.pendingServerStatus.server[i].adrstr, 64);
                LAN_GetServerInfo(ui_netSource->current.integer, sharedUiInfo.serverStatus.displayServers[sharedUiInfo.pendingServerStatus.num], infoString, 0x400);
                I_strncpyz(sharedUiInfo.pendingServerStatus.server[i].name, Info_ValueForKey(infoString, "hostname"), 64);
                sharedUiInfo.pendingServerStatus.server[i].valid = 1;
                sharedUiInfo.pendingServerStatus.num++;
                Com_sprintf(uiInfo->foundPlayerServerNames[uiInfo->numFoundPlayerServers], 64, "searching %d/%d...", sharedUiInfo.pendingServerStatus.num, numFound);
            }
        }
    }

    for (i = 0; i < 16; i++) {
        if (sharedUiInfo.pendingServerStatus.server[i].valid)
            break;
    }

    if (i < 16) {
        uiInfo->nextFindPlayerRefresh = uiInfo->uiDC.realTime + 25;
    } else {
        if (uiInfo->numFoundPlayerServers == 0)
            Com_sprintf(uiInfo->foundPlayerServerNames[0], 64, "no servers found");
        else
            Com_sprintf(uiInfo->foundPlayerServerNames[uiInfo->numFoundPlayerServers], 64, "%d server%s found with player %s", uiInfo->numFoundPlayerServers - 1, uiInfo->numFoundPlayerServers == 2 ? "" : "s", uiInfo->findPlayerName);
        uiInfo->nextFindPlayerRefresh = 0;
    }
}

static void UI_BuildServerStatus_impl(int force)
{
    int serverStatusVisible = uiInfo->nextFindPlayerRefresh;

    if (serverStatusVisible)
        return;

    if (force) {

        Menu_SetFeederSelection(&uiInfo->uiDC, 0, 0xd, 0, 0);
        sharedUiInfo.serverStatusInfo.numLines = 0;
        LAN_GetServerStatus(0, 0, 0);
    } else {
        int nextRefresh = sharedUiInfo.nextServerStatusRefresh;
        if (nextRefresh == 0)
            return;
        if (nextRefresh > uiInfo->uiDC.realTime)
            return;
    }

    {
        int serverCount = LAN_GetServerCount((ui_netSource)->current.integer);
        if (serverCount != sharedUiInfo.serverStatus.serverCount) {
            sharedUiInfo.serverStatus.serverCount = serverCount;
            if (sharedUiInfo.serverStatus.numDisplayServers != 0) {
                sharedUiInfo.serverStatus.currentServer = -1;
                UI_BuildServerDisplayList(1);
            }
        }
    }

    {
        int selectedServer = sharedUiInfo.serverStatus.currentServer;
        int numServers;

        if (selectedServer < 0)
            return;
        numServers = sharedUiInfo.serverStatus.numDisplayServers;
        if (selectedServer > numServers)
            return;
        if (numServers == 0)
            return;

        {
            int result;
            const char *serverAddress = sharedUiInfo.serverStatusAddress;
            serverStatusInfo_t *statusInfo = &sharedUiInfo.serverStatusInfo;

            result = UI_GetServerStatusInfo(serverAddress, statusInfo);
            if (result) {

                sharedUiInfo.nextServerStatusRefresh = 0;
                UI_GetServerStatusInfo(serverAddress, 0);
            } else {

                sharedUiInfo.nextServerStatusRefresh = uiInfo->uiDC.realTime + 500;
            }
        }
    }
}

static void UI_REGPARM1 UI_BuildServerStatus(int force)
{
    UI_BuildServerStatus_impl(force);
}

void UI_Refresh(void)
{
    int netSource;
    int needRebuild;
#if COD2_TRACE
    static int refreshTraceCount;
    static int fileTraceCount;
#endif

    if (Menu_Count(&uiInfo->uiDC) <= 0)
        return;

#if COD2_TRACE
    if (fileTraceCount < 40) {
        FILE *f = fopen("x64_ui_trace.txt", fileTraceCount ? "a" : "w");
        if (f) {
            int i;
            fprintf(f, "ui[%d] active=%d menuCount=%d openCount=%d keyCatchers=0x%x\n",
                    fileTraceCount, uiInfo->currentMenuType, uiInfo->uiDC.menuCount,
                    uiInfo->uiDC.openMenuCount, (clients)->keyCatchers);
            for (i = 0; i < uiInfo->uiDC.openMenuCount && i < 16; i++) {
                menuDef_t *menu = uiInfo->uiDC.menuStack[i];
                fprintf(f, "  stack[%d]=%s flags=0x%x full=%d\n",
                        i,
                        menu && menu->window.name ? menu->window.name : "<null>",
                        menu ? menu->window.dynamicFlags[0] : 0,
                        menu ? menu->fullScreen : 0);
            }
            fclose(f);
        }
        fileTraceCount++;
    }

    if (refreshTraceCount < 80 && (uiInfo->currentMenuType || uiInfo->uiDC.openMenuCount > 0)) {
        int i;
        if (getenv("MTRACE"))
            Com_Printf("[menu-trace] UI_Refresh active=%d menuCount=%d openCount=%d",
                       uiInfo->currentMenuType, uiInfo->uiDC.menuCount, uiInfo->uiDC.openMenuCount);
        for (i = 0; i < uiInfo->uiDC.openMenuCount && i < 16; ++i) {
            menuDef_t *menu = uiInfo->uiDC.menuStack[i];
            Com_Printf(" stack[%d]=%s", i, menu && menu->window.name ? menu->window.name : "<null>");
        }
        Com_Printf("\n");
        ++refreshTraceCount;
    }
#endif

    Menu_PaintAll(&uiInfo->uiDC);

    if (sharedUiInfo.serverStatus.refreshActive) {
        netSource = (ui_netSource)->current.integer;

        if (netSource == 2) {
            needRebuild = 0;
        } else if (netSource == 0) {
            needRebuild = LAN_GetServerCount(0) == 0;
        } else {
            needRebuild = LAN_WaitServerResponse(netSource) != 0;
        }

        if (uiInfo->uiDC.realTime < sharedUiInfo.serverStatus.refreshtime) {
            if (needRebuild)
                goto after_browser;
        }

        UI_UpdateServerCount();

        if (LAN_UpdateDirtyPings(netSource)) {
            sharedUiInfo.serverStatus.refreshtime = uiInfo->uiDC.realTime + 1000;
        } else {
            if (needRebuild)
                goto rebuild_browser;

            UI_BuildServerDisplayList(2);

            if (sharedUiInfo.serverStatus.refreshActive) {
                int listedServers = sharedUiInfo.serverStatus.numDisplayServers;
                int listedPlayers = sharedUiInfo.serverStatus.numPlayersOnServers;
                int filteredServers;

                sharedUiInfo.serverStatus.refreshActive = 0;
                Com_Printf("%d servers listed in browser with %d players.\n",
                           listedServers, listedPlayers);

                filteredServers = LAN_GetServerCount(netSource) - listedServers;
                if (filteredServers > 0) {
                    Com_Printf("%d servers not listed (filtered out by game browser settings)\n",
                               filteredServers);
                }
            }
        }

    rebuild_browser:
        UI_BuildServerDisplayList(0);
    }

after_browser:
    UI_BuildServerStatus(0);
    UI_BuildFindPlayerList();

#if COD2_FEATURE_GAMEPAD

    UI_ConsoleMode_DrawGlyphs(uiInfo);
    if (UI_ConsoleMode_SuppressCursor())
        return;
#endif

    UI_DrawHandlePic((float)(uiInfo->uiDC.cursorx - 16),
                     (float)(uiInfo->uiDC.cursory - 16),
                     32.0f, 32.0f, 4, 4, 0,
                     sharedUiInfo.assets.cursor);
}

void UI_RunMenuScript(const char **args)
{
    char name[1024];
    char buff[1024];
    char buff2[1024];
    char out[32];
    char hostName[32];
    char name2[1024];
    char testValue[1024];
    char dvarName[1024];
    char menuName[1024];
    int sortColumn;

    if (!String_Parse(args, name, 0x400))
        return;

    if (I_stricmp(name, "StartServer") == 0) {

        Dvar_SetBoolByName("cg_thirdPerson", 0);
        Dvar_SetFromStringByNameFromSource("dedicated",
                                           va("%i", (ui_dedicated)->current.integer), DVAR_SOURCE_EXTERNAL);
        Dvar_SetStringByName("g_gametype",
                             sharedUiInfo.gameTypes[(ui_netGameType)->current.integer].gameType);
        {
            int mapIdx = (ui_currentNetMap)->current.integer;
            int offset = mapIdx * 41; /* 41 dwords = sizeof(mapInfo) 0xA4 */
            const char *mapName = *(const char **)((byte *)&sharedUiInfo.mapList[0].mapLoadName + offset * 4);
            Cbuf_ExecuteText(2, va("wait ; wait ; map %s\n", mapName));
        }
        return;
    }

    if (I_stricmp(name, "resetDefaults") == 0) {
        Cbuf_ExecuteText(0, "dvar_restart\n");
        Cbuf_ExecuteText(0, "exec default_mp.cfg\n");
        Cbuf_ExecuteText(0, "exec language.cfg\n");
        Cbuf_ExecuteText(0, "setRecommended\n");
        Controls_SetDefaults();
        Dvar_SetBoolByName("com_introPlayed", 1);
        Dvar_SetBoolByName("com_recommendedSet", 1);
        Cbuf_ExecuteText(2, "vid_restart\n");
        return;
    }

    if (I_stricmp(name, "getCDKey") == 0) {
        CLUI_GetCDKey(buff, 0x11, buff2, 5);
        Dvar_SetStringByName("cdkey1", "");
        Dvar_SetStringByName("cdkey2", "");
        Dvar_SetStringByName("cdkey3", "");
        Dvar_SetStringByName("cdkey4", "");
        Dvar_SetStringByName("cdkey5", "");
        {
            if (strlen(buff) == 16) {

                I_strncpyz(out, buff, 5);
                Dvar_SetStringByName("cdkey1", out);
                I_strncpyz(out, &buff[4], 5);
                Dvar_SetStringByName("cdkey2", out);
                I_strncpyz(out, &buff[8], 5);
                Dvar_SetStringByName("cdkey3", out);
                I_strncpyz(out, &buff[12], 5);
                Dvar_SetStringByName("cdkey4", out);
            }
        }
        {
            if (strlen(buff2) == 4) {
                I_strncpyz(out, buff2, 5);
                Dvar_SetStringByName("cdkey5", out);
            }
        }
        return;
    }

    if (I_stricmp(name, "verifyCDKey") == 0) {
        buff[0] = '\0';
        I_strncat(buff, 0x400, Dvar_GetVariantString("cdkey1"));
        I_strncat(buff, 0x400, Dvar_GetVariantString("cdkey2"));
        I_strncat(buff, 0x400, Dvar_GetVariantString("cdkey3"));
        I_strncat(buff, 0x400, Dvar_GetVariantString("cdkey4"));
        buff2[0] = '\0';
        I_strncat(buff2, 0x400, Dvar_GetVariantString("cdkey5"));
        if (CL_CDKeyValidate(buff, buff2)) {
            Dvar_SetStringByName("ui_cdkeyvalid", UI_SafeTranslateString("EXE_CDKEYVALID"));
            CLUI_SetCDKey(buff, buff2);
        } else {
            Dvar_SetStringByName("ui_cdkeyvalid", UI_SafeTranslateString("EXE_CDKEYINVALID"));
        }
        return;
    }

    if (I_stricmp(name, "loadArenas") == 0) {
        UI_LoadArenas();
        UI_SelectCurrentGameType();
        UI_MapCountByGameType();
        Menu_SetFeederSelection(&uiInfo->uiDC, 0, 4, 0, "createserver_maps");
        UI_SelectCurrentMap();
        return;
    }

    if (I_stricmp(name, "loadGameInfo") == 0) {
        UI_GetGameTypesList();
        return;
    }

    if (I_stricmp(name, "LoadMods") == 0) {
        UI_LoadMods();
        return;
    }

    if (I_stricmp(name, "voteTypeMap") == 0) {
        Cbuf_ExecuteText(2, va("callvote typemap %s %s\n", sharedUiInfo.gameTypes[ui_netGameType->current.integer].gameType,
                               sharedUiInfo.mapList[ui_currentNetMap->current.integer].mapLoadName));
        return;
    }

    if (I_stricmp(name, "voteMap") == 0) {
        if (ui_currentNetMap->current.integer >= 0 && ui_currentNetMap->current.integer < sharedUiInfo.mapCount)
            Cbuf_ExecuteText(2, va("callvote map %s\n", sharedUiInfo.mapList[ui_currentNetMap->current.integer].mapLoadName));
        return;
    }

    if (I_stricmp(name, "voteGame") == 0) {
        const char *gtName = sharedUiInfo.gameTypes[(ui_netGameType)->current.integer].gameType;
        Cbuf_ExecuteText(2, va("callvote g_gametype %s\n", gtName));
        return;
    }

    if (I_stricmp(name, "saveControls") == 0) {
        Controls_SetConfig(1);
        return;
    }

    if (I_stricmp(name, "loadControls") == 0) {
        Controls_GetConfig();
        return;
    }

    if (I_stricmp(name, "clearError") == 0) {
        Dvar_SetStringByName("com_errorMessage", "");
        Dvar_SetBoolByName("com_isNotice", 0);
        return;
    }

    if (I_stricmp(name, "RefreshServers") == 0) {
        UI_StartServerRefresh(1);
        UI_BuildServerDisplayList(1);
        return;
    }

    if (I_stricmp(name, "RefreshFilter") == 0) {
        UI_StartServerRefresh(0);
        UI_BuildServerDisplayList(1);
        return;
    }

    if (I_stricmp(name, "addPlayerProfiles") == 0) {
        UI_AddPlayerProfiles();
        return;
    }

    if (I_stricmp(name, "sortPlayerProfiles") == 0) {
        uiInfo->playerProfileStatus.sortDir = !uiInfo->playerProfileStatus.sortDir;
        UI_SortPlayerProfiles(0);
        return;
    }

    if (I_stricmp(name, "selectActivePlayerProfile") == 0) {
        UI_SelectActivePlayerProfile();
        return;
    }

    if (I_stricmp(name, "createPlayerProfile") == 0) {
        UI_CreatePlayerProfile();
        return;
    }

    if (I_stricmp(name, "deletePlayerProfile") == 0) {
        UI_DeletePlayerProfile();
        return;
    }

    if (I_stricmp(name, "loadPlayerProfile") == 0) {
        UI_LoadPlayerProfile();
        return;
    }

    if (I_stricmp(name, "LoadMovies") == 0) {
        UI_LoadMovies();
        return;
    }

    if (I_stricmp(name, "playMovie") == 0) {
        int cinHandle = sharedUiInfo.previewMovie;
        if (cinHandle >= 0)
            CIN_StopCinematic(cinHandle);
        Cbuf_ExecuteText(2, va("cinematic %s 2\n",
                               sharedUiInfo.movieList[sharedUiInfo.movieIndex]));
        return;
    }

    if (I_stricmp(name, "RunMod") == 0) {
        Dvar_SetStringByName("fs_game",
                             *(const char **)((byte *)&sharedUiInfo + 25972 + sharedUiInfo.modIndex * 8));
        Cbuf_ExecuteText(2, "vid_restart;");
        return;
    }

    if (I_stricmp(name, "closeJoin") == 0) {
        if (sharedUiInfo.serverStatus.refreshActive) {
            UI_StopServerRefresh();
            sharedUiInfo.serverStatus.nextDisplayRefresh = 0;
            sharedUiInfo.nextServerStatusRefresh = 0;
            uiInfo->nextFindPlayerRefresh = 0;
            UI_BuildServerDisplayList(1);
        } else {
            Menus_CloseByName(&uiInfo->uiDC, "joinserver");
            Menus_OpenByName(&uiInfo->uiDC, "main");
        }
        return;
    }

    if (I_stricmp(name, "StopRefresh") == 0) {
        UI_StopServerRefresh();
        sharedUiInfo.serverStatus.nextDisplayRefresh = 0;
        sharedUiInfo.nextServerStatusRefresh = 0;
        uiInfo->nextFindPlayerRefresh = 0;
        return;
    }

    if (I_stricmp(name, "ServerStatus") == 0) {

        UI_UpdateServerCount();

        {
            int selectedServer = sharedUiInfo.serverStatus.currentServer;
            if (selectedServer < 0 || selectedServer >= sharedUiInfo.serverStatus.numDisplayServers)
                return;

            LAN_GetServerAddressString((ui_netSource)->current.integer,
                                       sharedUiInfo.serverStatus.displayServers[selectedServer],
                                       sharedUiInfo.serverStatusAddress, 0x40);
#ifdef __EMSCRIPTEN__
            Menu_SetFeederSelection(&uiInfo->uiDC, 0, 0xd, 0, 0);
            sharedUiInfo.serverStatusInfo.numLines = 0;
            sharedUiInfo.nextServerStatusRefresh = uiInfo->uiDC.realTime + 500;
#else
            UI_BuildServerStatus(1);
#endif
        }
        return;
    }

    if (I_stricmp(name, "UpdateFilter") == 0) {
        if ((ui_netSource)->current.integer == 0) {
            UI_StartServerRefresh(1);
        }
        UI_BuildServerDisplayList(1);
        UI_FeederSelection(2.0f, 0);
        return;
    }

    if (I_stricmp(name, "JoinServer") == 0) {
        Dvar_SetBoolByName("cg_thirdPerson", 0);

        UI_UpdateServerCount();

        {
            int selectedServer = sharedUiInfo.serverStatus.currentServer;
            if (selectedServer < 0 || selectedServer >= sharedUiInfo.serverStatus.numDisplayServers)
                return;

            LAN_GetServerAddressString((ui_netSource)->current.integer,
                                       sharedUiInfo.serverStatus.displayServers[selectedServer],
                                       buff, 0x400);
            Cbuf_ExecuteText(2, va("connect %s\n", buff));
        }
        return;
    }

    if (I_stricmp(name, "Quit") == 0) {
        Cbuf_ExecuteText(0, "quit");
        return;
    }

    if (I_stricmp(name, "Controls") == 0) {
        Dvar_SetIntByName("cl_paused", 1);
        Key_SetCatcher(8);
        Menus_CloseAll(&uiInfo->uiDC);
        Menus_OpenByName(&uiInfo->uiDC, "setup_menu2");
        return;
    }

    if (I_stricmp(name, "Leave") == 0) {
        Cbuf_ExecuteText(2, "disconnect\n");
        Key_SetCatcher(8);
        Menus_CloseAll(&uiInfo->uiDC);
        Menus_OpenByName(&uiInfo->uiDC, "main");
        return;
    }

    if (I_stricmp(name, "ServerSort") == 0) {
        if (!Int_Parse(args, &sortColumn))
            return;

        if (sharedUiInfo.serverStatus.sortKey == sortColumn) {
            int dir = sharedUiInfo.serverStatus.sortDir;
            sharedUiInfo.serverStatus.sortDir = !dir;
        }

        sharedUiInfo.serverStatus.sortKey = sortColumn;
        qsort(sharedUiInfo.serverStatus.displayServers,
              sharedUiInfo.serverStatus.numDisplayServers, 4, UI_ServersQsortCompare);
        return;
    }

    if (I_stricmp(name, "nextSkirmish") == 0)
        return;

    if (I_stricmp(name, "SkirmishStart") == 0)
        return;

    if (I_stricmp(name, "closeingame") == 0) {
        Key_SetCatcher(Key_GetCatcher() & ~8);
        Key_ClearStates();
        Dvar_SetIntByName("cl_paused", 0);
        Menus_CloseAll(&uiInfo->uiDC);
        return;
    }

    if (I_stricmp(name, "voteKick") == 0) {
        int sel = uiInfo->playerIndex;
        if (sel < 0 || sel >= sharedUiInfo.playerCount)
            return;
        Cbuf_ExecuteText(2, va("callvote kick \"%s\"\n",
                               sharedUiInfo.playerNames[sel]));
        return;
    }

    if (I_stricmp(name, "voteTempBan") == 0) {
        int sel = uiInfo->playerIndex;
        if (sel < 0 || sel >= sharedUiInfo.playerCount)
            return;
        Cbuf_ExecuteText(2, va("callvote tempBanUser \"%s\"\n",
                               sharedUiInfo.playerNames[sel]));
        return;
    }

    if (I_stricmp(name, "addFavorite") == 0) {
        if ((ui_netSource)->current.integer == 2)
            return;

        out[0] = '\0';
        hostName[0] = '\0';

        UI_UpdateServerCount();

        {
            int selectedServer = sharedUiInfo.serverStatus.currentServer;
            if (selectedServer >= 0 && selectedServer < sharedUiInfo.serverStatus.numDisplayServers) {

                LAN_GetServerInfo((ui_netSource)->current.integer,
                                  sharedUiInfo.serverStatus.displayServers[selectedServer],
                                  buff, 0x400);
                I_strncpyz(hostName, Info_ValueForKey(buff, "hostname"), 0x20);
                I_strncpyz(out, Info_ValueForKey(buff, "addr"), 0x20);
            }
            UI_AddServerToFavoritesList(hostName, out);
        }
        return;
    }

    if (I_stricmp(name, "deleteFavorite") == 0) {
        if ((ui_netSource)->current.integer != 2)
            return;

        if (sharedUiInfo.serverStatus.currentServer < 0
            || sharedUiInfo.serverStatus.currentServer >= sharedUiInfo.serverStatus.numDisplayServers)
            return;

        UI_UpdateServerCount();

        LAN_GetServerInfo((ui_netSource)->current.integer,
                          sharedUiInfo.serverStatus.displayServers[sharedUiInfo.serverStatus.currentServer],
                          buff, 0x400);
        out[0] = '\0';
        I_strncpyz(out, Info_ValueForKey(buff, "addr"), 0x20);
        if (strlen(out) > 0)
            LAN_RemoveServer(2, out);
        return;
    }

    if (I_stricmp(name, "createFavorite") == 0) {
        if ((ui_netSource)->current.integer != 2)
            return;

        hostName[0] = '\0';
        out[0] = '\0';
        I_strncpyz(out, Dvar_GetVariantString("ui_favoriteName"), 0x20);
        I_strncpyz(hostName, Dvar_GetVariantString("ui_favoriteAddress"), 0x20);
        UI_AddServerToFavoritesList(out, hostName);
        return;
    }

    if (I_stricmp(name, "update") == 0) {
        if (String_Parse(args, name2, 0x400))
            UI_Update(name2);
        return;
    }

    if (I_stricmp(name, "startSingleplayer") == 0) {
        Cbuf_ExecuteText(2, "startSingleplayer\n");
        return;
    }

    if (I_stricmp(name, "getLanguage") == 0) {
        Dvar_SetIntByName("ui_language", Dvar_GetInt("loc_language"));
        UI_VerifyLanguage();
        return;
    }

    if (I_stricmp(name, "verifyLanguage") == 0) {
        UI_VerifyLanguage();
        return;
    }

    if (I_stricmp(name, "updateLanguage") == 0) {
        Dvar_SetIntByName("loc_language", Dvar_GetInt("ui_language"));
        UI_VerifyLanguage();
        Cbuf_ExecuteText(2, "vid_restart\n");
        return;
    }

    if (I_stricmp(name, "mutePlayer") == 0) {
        int sel = uiInfo->playerIndex;
        if (sel < 0 || sel >= sharedUiInfo.playerCount)
            return;
        CL_MutePlayer(sharedUiInfo.playerClientNums[sel]);
        return;
    }

    if (I_stricmp(name, "openMenuOnDvar") == 0 || I_stricmp(name, "openMenuOnDvarNot") == 0) {
        if (UI_GetOpenOrCloseMenuOnDvarArgs(args, name, dvarName, testValue, menuName))
            UI_OpenMenuOnDvar(name, menuName, dvarName, testValue);
        return;
    }

    if (I_stricmp(name, "closeMenuOnDvar") == 0 || I_stricmp(name, "closeMenuOnDvarNot") == 0) {
        if (UI_GetOpenOrCloseMenuOnDvarArgs(args, name, dvarName, testValue, menuName))
            UI_CloseMenuOnDvar(name, menuName, dvarName, testValue);
        return;
    }

    Com_Printf("unknown UI script %s\n", name);
}

#ifndef __EMSCRIPTEN__

int UI_FeederCount(float feederID)
{
    if (feederID == 4.0f) {
        return UI_MapCountByGameType();
    } else if (feederID == 9.0f) {
        return sharedUiInfo.modCount;
    } else if (feederID == 2.0f) {
        UI_UpdateServerCount();
        return sharedUiInfo.serverStatus.numDisplayServers;
    } else if (feederID == 13.0f) {
        return sharedUiInfo.serverStatusInfo.numLines;
    }
    if (feederID == 7.0f) {
        if (uiInfo->uiDC.realTime > uiInfo->playerRefresh) {
            uiInfo->playerRefresh = uiInfo->uiDC.realTime + 3000;
            UI_BuildPlayerList();
        }
        return sharedUiInfo.playerCount;
    }

    if (feederID == 20.0f) {
        if (uiInfo->uiDC.realTime > uiInfo->playerRefresh) {
            uiInfo->playerRefresh = uiInfo->uiDC.realTime + 3000;
            UI_BuildPlayerList();
        }
        return sharedUiInfo.playerCount;
    }
    if (feederID == 24.0f)
        return uiInfo->playerProfileCount;
    return 0;
}

static void Text_PaintCenter(FontHandle font, const char *text, float x, float y, float scale, const vec_t *color)
{
    float actualScale = CL_NormalizedTextScale(font, scale);
    int len = (int)((float)CL_TextWidth(text, 0, font) * actualScale);
    UI_DrawText(text, 0x7fffffff, font, x - (float)(len / 2), y, 0, 0, scale, color, 6);
}

static void UI_ReadableSize_wrap(char *buf, int bufsize, int value)
{
    UI_ReadableSize(buf, bufsize, value);
}

static void UI_PrintTime(char *buf, int bufsize, int time)
{
    if (time > 3600) {
        Com_sprintf(buf, bufsize, "%d %s %d %s", time / 3600, UI_SafeTranslateString("EXE_HOURS"),
                    (time % 3600) / 60, UI_SafeTranslateString("EXE_MINUTES"));
    } else if (time > 60) {
        Com_sprintf(buf, bufsize, "%d %s %d %s", time / 60, UI_SafeTranslateString("EXE_MINUTES"),
                    time % 60, UI_SafeTranslateString("EXE_SECONDS"));
    } else {
        Com_sprintf(buf, bufsize, "%d %s", time, UI_SafeTranslateString("EXE_SECONDS"));
    }
}

static void UI_DisplayDownloadInfo(const char *downloadName, float centerPoint, float yStart, FontHandle font, float scale)
{
    LegacyHacks *legacyBase = legacyHacks;
    int downloadSize = legacyBase->cl_downloadSize;
    int downloadCount = legacyBase->cl_downloadCount;
    int downloadTime = legacyBase->cl_downloadTime;
    vec_t color[4];
    float y1, y2, y3;
    int xferRate = 0;
    char dlSizeBuf[64];
    char totalSizeBuf[64];
    char xferRateBuf[64];
    char dlTimeBuf[64];
    const char *text;

    {
        const vec_t *cBlack = (const vec_t *)((void *)&colorBlack);
        color[0] = cBlack[0];
        color[1] = cBlack[1];
        color[2] = cBlack[2];
    }
    color[3] = 0.2f;

    y1 = yStart + 184.0f;
    UI_FillRect(0, y1, 640.0f, 85.0f, 0, 0, color);
    y2 = yStart + 185.0f;
    UI_FillRect(0, y2, 640.0f, 83.0f, 0, 0, color);
    y3 = yStart + 186.0f;
    UI_FillRect(0, y3, 640.0f, 81.0f, 0, 0, color);

    if (downloadSize > 0) {

        const vec_t *cRed = (const vec_t *)((void *)&colorRed);
        int width;
        color[0] = cRed[0];
        color[1] = cRed[1];
        color[2] = cRed[2];
        color[3] = 0.15f;

        width = (int)((float)downloadCount / (float)downloadSize * 640.0f);
        UI_FillRect(0, y1, (float)(width + 2), 85.0f, 0, 0, color);
        UI_FillRect(0, y2, (float)(width + 1), 83.0f, 0, 0, color);
        UI_FillRect(0, y3, (float)width, 81.0f, 0, 0, color);
    }

    {
        float yDl = yStart + 210.0f;
        float yEta = yStart + 235.0f;
        float yXfer = yStart + 260.0f;
        const vec_t *ltGrey = (const vec_t *)((void *)&colorLtGrey);

        UI_DrawText(UI_SafeTranslateString(dlText), 64, font, 24.0f, yDl, 0, 0, scale, ltGrey, 3);
        UI_DrawText(UI_SafeTranslateString(etaText), 64, font, 24.0f, yEta, 0, 0, scale, ltGrey, 3);
        UI_DrawText(UI_SafeTranslateString(xferText), 64, font, 24.0f, yXfer, 0, 0, scale, ltGrey, 3);

        if (downloadSize > 0)
            text = va("%s (%d%%)", downloadName, downloadCount * 100 / downloadSize);
        else
            text = downloadName;

        UI_DrawText(text, 0x7fffffff, font, 192.0f, yDl, 0, 0, scale, ltGrey, 3);

        UI_ReadableSize_wrap(dlSizeBuf, 64, downloadCount);
        UI_ReadableSize_wrap(totalSizeBuf, 64, downloadSize);

        if (downloadCount < 0x1000 || !downloadTime) {

            text = UI_SafeTranslateString("EXE_ESTIMATING");
            Text_PaintCenter(font, text, centerPoint, yEta, scale, ltGrey);
            text = va("(%s %s %s %s)", dlSizeBuf, UI_SafeTranslateString("EXE_OF"), totalSizeBuf, UI_SafeTranslateString("EXE_COPIED"));
            Text_PaintCenter(font, text, centerPoint, yStart + 320.0f, scale, ltGrey);
        } else {

            if ((uiInfo->uiDC.realTime - downloadTime) / 1000)
                xferRate = downloadCount / ((uiInfo->uiDC.realTime - downloadTime) / 1000);
            else
                xferRate = 0;

            UI_ReadableSize_wrap(xferRateBuf, 64, xferRate);

            if (downloadSize && xferRate) {
                int n = downloadSize / xferRate;
                int sum, ti;

                n = n - (((downloadCount / 1024) * n) / (downloadSize / 1024));

                tleEstimates[tleIndex] = n;
                tleIndex++;
                if (tleIndex >= 80)
                    tleIndex = 0;

                sum = 0;
                for (ti = 0; ti < 80; ti++)
                    sum += tleEstimates[ti];

                UI_PrintTime(dlTimeBuf, 64, sum / 80);
                UI_DrawText(dlTimeBuf, 0x7fffffff, font, 264.0f, yEta, 0, 0, scale, ltGrey, 3);
                text = va("(%s %s %s %s)", dlSizeBuf, UI_SafeTranslateString("EXE_OF"), totalSizeBuf, UI_SafeTranslateString("EXE_COPIED"));
                Text_PaintCenter(font, text, centerPoint, yStart + 320.0f, scale, ltGrey);
            } else {

                text = UI_SafeTranslateString("EXE_ESTIMATING");
                Text_PaintCenter(font, text, centerPoint, yEta, scale, ltGrey);
                if (downloadSize)
                    text = va("(%s %s %s %s)", dlSizeBuf, UI_SafeTranslateString("EXE_OF"), totalSizeBuf, UI_SafeTranslateString("EXE_COPIED"));
                else
                    text = va("(%s %s)", dlSizeBuf, UI_SafeTranslateString("EXE_COPIED"));
                Text_PaintCenter(font, text, centerPoint, yStart + 320.0f, scale, ltGrey);
            }

            if (xferRate) {
                text = va("%s/%s", xferRateBuf, UI_SafeTranslateString("EXE_SECONDS"));
                UI_DrawText(text, 0x7fffffff, font, 200.0f, yXfer, 0, 0, scale, ltGrey, 3);
            }
        }
    }
}

void UI_DrawConnectScreen(void)
{
    uiClientState_t cstate;
    char text[256];
    char ps[60];
    FontHandle font;
    int displayed;
    int i, len, index, yPrint;
    int neednewline;
    const char *s;

    displayed = 0;
    CG_DrawInformation(legacyHacks->cl_serverloadmap[0] || legacyHacks->cl_serverloadgametype[0] ? 1 : 0);
    font = UI_GetFontHandle(0, 0.5f);

    GetClientState(&cstate);

    if (!I_stricmp(cstate.servername, "Auto-Updater")) {
        if (legacyHacks->cl_downloadName[0])
            strcpy(text, UI_ReplaceConversionString(UI_SafeTranslateString("EXE_DOWNLOADINGUPDATE"), Dvar_GetString("cl_updateversion")));
        else
            strcpy(text, UI_ReplaceConversionString(UI_SafeTranslateString("EXE_CONNECTINGTO"), cstate.servername));
        Text_PaintCenter(font, text, 320.0f, 55.0f, 0.5f, colorWhite);
        displayed = 1;
    } else if (g_mapname[0]) {
        s = UI_SafeTranslateString(UI_GetGameTypeDisplayName(g_gametype));
        Text_PaintCenter(font, s, 320.0f, 89.0f, 0.5f, colorWhite);
        Text_PaintCenter(font, UI_GetMapDisplayName(g_mapname), 320.0f, 119.0f, 0.5f, colorWhite);
        displayed = 1;
    }

    // server messages (full, bad version, ...) wrapped into lines
    if (cstate.connState < CA_CONNECTED) {
        neednewline = 0;
        index = 0;
        yPrint = 299;
        s = UI_SafeTranslateString(cstate.messageString);
        len = strlen(s);
        for (i = 0; i < len; i++, index++) {
            ps[index] = s[i];
            if (index > 40 && i > 0)
                neednewline = 1;
            if (index >= 58 || i == len - 1 || (neednewline && s[i] == ' ')) {
                ps[index + 1] = 0;
                Text_PaintCenter(font, ps, 320.0f, (float)yPrint, 0.5f, colorYellow);
                neednewline = 0;
                yPrint += 22;
                index = -1;
            }
        }
    }

    switch (cstate.connState) {
    case CA_CONNECTING:
        if (displayed)
            return;
        s = UI_ReplaceConversionInt(UI_SafeTranslateString("EXE_AWAITINGCONNECTION"), cstate.connectPacketCount);
        break;
    case CA_CHALLENGING:
        if (displayed)
            return;
        s = UI_ReplaceConversionInt(UI_SafeTranslateString("EXE_AWAITINGCHALLENGE"), cstate.connectPacketCount);
        break;
    case CA_CONNECTED:
        if (legacyHacks->cl_downloadName[0])
            UI_DisplayDownloadInfo(legacyHacks->cl_downloadName, 320.0f, 89.0f, font, 0.5f);
        return;
    default:
        return;
    }

    if (s && I_stricmp(cstate.servername, "localhost"))
        Text_PaintCenter(font, s, 320.0f, 145.0f, 0.5f, colorWhite);
}
#else
static int UI_PlayerProfilesQsortCompare(const void *arg1, const void *arg2)
{
    int idx1 = *(const int *)arg1;
    int idx2 = *(const int *)arg2;
    int result;

    if (idx1 == idx2)
        return 0;

    result = I_stricmp(
        uiInfo->playerProfileName[idx1],
        uiInfo->playerProfileName[idx2]);

    if (uiInfo->playerProfileStatus.sortDir == 0)
        result = -result;

    return result;
}

static void UI_SelectCurrentMap(void)
{
    byte cstate[sizeof(uiClientState_t)];
    char szMap[0x40];
    int numMaps;
    int i;
    int visibleIndex;

    GetClientState((uiClientState_t *)cstate);
    if (*(int *)cstate != 8)
        return;

    {
        const char *serverInfo = CL_GetConfigString(0);
        if (!serverInfo[0])
            return;
        I_strncpyz(szMap, Info_ValueForKey(serverInfo, "mapname"), sizeof(szMap));
    }

    numMaps = sharedUiInfo.mapCount;
    if (numMaps <= 0)
        return;

    visibleIndex = 0;
    for (i = 0; i < numMaps; i++) {
        byte *entry = (byte *)&sharedUiInfo + i * 0xa4;

        if (!*(int *)(entry + 0x13f4))
            continue;

        if (I_stricmp(szMap, *(const char **)(entry + 0x1354)) == 0) {
            Menu_SetFeederSelection(&uiInfo->uiDC, 0, 4, visibleIndex, "createserver_maps");
            return;
        }

        visibleIndex++;
    }
}

static void UI_VerifyLanguage(void)
{
    int oldLanguage = Dvar_GetInt("loc_language");
    int newLanguage = Dvar_GetInt("ui_language");
    int verified = SEH_VerifyLanguageSelection(newLanguage);

    if (verified != newLanguage)
        Dvar_SetIntByName("ui_language", verified);

    Dvar_SetBoolByName("ui_languagechanged", oldLanguage != newLanguage);
}

static void UI_AddServerToFavoritesList(const char *name, const char *address)
{
    const char *message;
    const char *dvarMessage;
    int addResult;

    if (!name[0]) {
        message = UI_SafeTranslateString("EXE_FAVORITENAMEEMPTY");
        Com_Printf("%s\n", message);
        Dvar_SetStringByName("ui_favorite_message", "@EXE_FAVORITENAMEEMPTY");
        return;
    }

    if (!address[0]) {
        message = UI_SafeTranslateString("EXE_FAVORITEADDRESSEMPTY");
        Com_Printf("%s\n", message);
        Dvar_SetStringByName("ui_favorite_message", "@EXE_FAVORITEADDRESSEMPTY");
        return;
    }

    addResult = LAN_AddServer(2, name, address);
    if (addResult == 0) {
        message = UI_SafeTranslateString("EXE_FAVORITEINLIST");
        dvarMessage = "@EXE_FAVORITEINLIST";
    } else if (addResult == -1) {
        message = UI_SafeTranslateString("EXE_FAVORITELISTFULL");
        dvarMessage = "@EXE_FAVORITELISTFULL";
    } else if (addResult == -2) {
        message = UI_SafeTranslateString("EXE_BADSERVERADDRESS");
        dvarMessage = "@EXE_BADSERVERADDRESS";
    } else {
        message = UI_SafeTranslateString("EXE_FAVORITEADDED");
        dvarMessage = "@EXE_FAVORITEADDED";
    }

    Com_Printf("%s\n", message);
    Dvar_SetStringByName("ui_favorite_message", dvarMessage);
}

static Bool UI_GetOpenOrCloseMenuOnDvarArgs(const char **args, const char *cmd,
                                            char *dvarName, char *testValue, char *menuName)
{
    if (!String_Parse(args, dvarName, 0x400)) {
        Com_Printf("%s: invalid dvar name.\n", cmd);
        return 0;
    }

    if (!String_Parse(args, testValue, 0x400)) {
        Com_Printf("%s: invalid test value.\n", cmd);
        return 0;
    }

    if (!String_Parse(args, menuName, 0x400)) {
        Com_Printf("%s: invalid menu name.\n", cmd);
        return 0;
    }

    return 1;
}

static void UI_UpdateServerCount(void)
{
    int serverCount = LAN_GetServerCount((ui_netSource)->current.integer);
    if (serverCount != sharedUiInfo.serverStatus.serverCount) {
        sharedUiInfo.serverStatus.serverCount = serverCount;
        if (sharedUiInfo.serverStatus.numDisplayServers != 0) {
            sharedUiInfo.serverStatus.currentServer = -1;
            UI_BuildServerDisplayList(1);
        }
    }
}

static void UI_StartServerRefresh(qboolean full)
{
    qtime_t q;
    char dvarName[0x18];
    int source;
    int now;

    Com_RealTime(&q);
    source = (ui_netSource)->current.integer;
    snprintf(dvarName, sizeof(dvarName), "ui_lastServerRefresh_%i", source);
    Dvar_SetStringByName(dvarName,
                         va("%s %i, %i   %i:%02i",
                            UI_SafeTranslateString(MonthAbbrev[q.tm_mon]),
                            q.tm_mday,
                            q.tm_year + 1900,
                            q.tm_hour,
                            q.tm_min));

    now = uiInfo->uiDC.realTime;

    if (!full) {
        LAN_ResetPings(source);
        sharedUiInfo.serverStatus.refreshActive = 1;
        sharedUiInfo.serverStatus.refreshtime = now + 1000;
        return;
    }

    sharedUiInfo.serverStatus.refreshActive = 1;
    sharedUiInfo.serverStatus.nextDisplayRefresh = now + 1000;
    sharedUiInfo.serverStatus.numDisplayServers = 0;
    sharedUiInfo.serverStatus.numPlayersOnServers = 0;
    sharedUiInfo.serverStatus.serverCount = LAN_GetServerCount(source);
    LAN_MarkServerDirty(source, -1, 1);
    LAN_ResetPings(source);

    if (source == 0) {
        Cbuf_ExecuteText(0, "localservers\n");
        sharedUiInfo.serverStatus.refreshtime = now + 1000;
        return;
    }

    sharedUiInfo.serverStatus.refreshtime = now + 5000;

    if (source == 1) {
        const char *debugProtocol = Dvar_GetVariantString("debug_protocol");
        if (debugProtocol[0])
            Cbuf_ExecuteText(0, va("globalservers %d %s full empty\n", 0, debugProtocol));
        else
            Cbuf_ExecuteText(0, va("globalservers %d %d full empty\n", 0, 0x73));
    }
}

const char *UI_SafeTranslateString(const char *reference)
{
    const char *ref = reference;
    const char *translated;

    if (ref[0] == 0x15)
        ref++;

    translated = SEH_StringEd_GetString(ref);
    if (translated)
        return translated;

    if ((loc_warnings)->current.enabled) {
        if ((loc_warningsAsErrors)->current.enabled) {
            Com_Error(ERR_LOCALIZATION, "Could not translate string \"%s\"", ref);
        } else {
            Com_Printf("^3WARNING: Could not translate string \"%s\"\n", ref);
        }

        strcpy(errorString, "^1UNLOCALIZED(^7");
        I_strncat(errorString, 0x400, ref);
        I_strncat(errorString, 0x400, "^1)^7");
        return errorString;
    }

    strcpy(errorString, ref);
    return errorString;
}

void UI_FeederSelection(float feederID, int index)
{
    if (feederID == 4.0f) {
        int actual;

        if (sharedUiInfo.mapList[(ui_currentNetMap)->current.integer].cinematic >= 0) {
            CIN_StopCinematic(sharedUiInfo.mapList[(ui_currentNetMap)->current.integer].cinematic);
            sharedUiInfo.mapList[(ui_currentNetMap)->current.integer].cinematic = -1;
        }
        UI_SelectedMap(index, &actual);
        Dvar_SetInt(ui_currentMap, actual);
        Dvar_SetInt(ui_currentNetMap, actual);

    } else if (feederID == 2.0f) {
        char *name;
        char *p;

        if (sharedUiInfo.serverStatus.numDisplayServers >= 1)
            sharedUiInfo.serverStatus.currentServer = index;

        LAN_GetServerInfo((ui_netSource)->current.integer,
                          sharedUiInfo.serverStatus.displayServers[index],
                          info, 0x400);
        name = (char *)va("levelshots/%s", Info_ValueForKey(info, "mapname"));

        for (p = name; *p; p++)
            *p = tolower((int)(signed char)*p);

        sharedUiInfo.serverStatus.currentServerPreview = CL_RegisterMaterialNoMip(name, 3);
        if (sharedUiInfo.serverStatus.currentServerCinematic >= 0) {
            CIN_StopCinematic(sharedUiInfo.serverStatus.currentServerCinematic);
            sharedUiInfo.serverStatus.currentServerCinematic = -1;
        }

    } else if (feederID == 7.0f || feederID == 20.0f) {
        uiInfo->playerIndex = index;

    } else if (feederID == 9.0f) {
        sharedUiInfo.modIndex = index;

    } else if (feederID == 24.0f) {
        if (index >= 0 && index < uiInfo->playerProfileCount) {
            int sortedIdx = uiInfo->playerProfileStatus.displayProfile[index];
            const char *profileName = uiInfo->playerProfileName[sortedIdx];
            Dvar_SetString((dvar_t *)ui_playerProfileSelected, profileName);
        }
    }
}

static void UI_GetGameTypesList(void)
{
    char szGameTypeList[0x1000];
    int iNumGameTypeScripts;
    int i;
    char *pszFileName;
    const char *pBuffParse;

    sharedUiInfo.numGameTypes = 0;
    sharedUiInfo.numJoinGameTypes = 0;

    *(const char **)((byte *)&sharedUiInfo + 4688) = String_Alloc("All");
    {
        int idx = sharedUiInfo.numJoinGameTypes;
        sharedUiInfo.joinGameTypes[idx].gameTypeName = "";
    }
    sharedUiInfo.numJoinGameTypes += 1;

    iNumGameTypeScripts = FS_GetFileList("maps/mp/gametypes", "gsc", FS_LIST_PURE_ONLY, szGameTypeList, 0x1000);

    if (iNumGameTypeScripts > 0) {
        pszFileName = szGameTypeList;

        for (i = 0; i < iNumGameTypeScripts; i++) {
            int iLen = strlen(pszFileName);
            char *pszEnd;
            int numTypes;

            if (pszFileName[0] == '_') {
                pszFileName += iLen + 1;
                continue;
            }

            pszEnd = pszFileName + iLen;
            if (I_stricmp(pszEnd - 4, ".gsc") == 0) {
                pszFileName[iLen - 4] = '\0';
            }

            numTypes = sharedUiInfo.numGameTypes;
            if (numTypes == 32 || sharedUiInfo.numJoinGameTypes == 32) {
                Com_Printf("Too many game type scripts found! Only loading the first %i\n", 31);
                break;
            }

            sharedUiInfo.gameTypes[numTypes].gameType = String_Alloc(pszFileName);

            {
                int gt = sharedUiInfo.numGameTypes;
                const char *scriptName = sharedUiInfo.gameTypes[gt].gameType;
                int nameIdx = sharedUiInfo.numJoinGameTypes;
                sharedUiInfo.joinGameTypes[nameIdx].gameType = scriptName;
            }

            pBuffParse = GetMenuBuffer(va("maps/mp/gametypes/%s.txt", pszFileName));
            if (pBuffParse) {
                const char *parsed = Com_Parse(&pBuffParse);
                int gt2 = sharedUiInfo.numGameTypes;
                sharedUiInfo.gameTypes[gt2].gameTypeName = String_Alloc(parsed);
                {
                    const char *displayName = sharedUiInfo.gameTypes[sharedUiInfo.numGameTypes].gameTypeName;
                    int nameIdx2 = sharedUiInfo.numJoinGameTypes;
                    sharedUiInfo.joinGameTypes[nameIdx2].gameTypeName = displayName;
                }
            } else {
                int gt3 = sharedUiInfo.numGameTypes;
                *(const char **)((byte *)&sharedUiInfo + 4432 + gt3 * 8) = *(const char **)((byte *)&sharedUiInfo + 4428 + gt3 * 8);
                {
                    const char *displayName2 = sharedUiInfo.gameTypes[sharedUiInfo.numGameTypes].gameTypeName;
                    int nameIdx3 = sharedUiInfo.numJoinGameTypes;
                    sharedUiInfo.joinGameTypes[nameIdx3].gameTypeName = displayName2;
                }
            }

            sharedUiInfo.numGameTypes += 1;
            sharedUiInfo.numJoinGameTypes += 1;
            pszFileName = pszEnd + 1;
        }
    }

    if (sharedUiInfo.numGameTypes == 0) {
        Com_Error(ERR_FATAL, "\x15No game type scripts found in maps/mp/gametypes folder");
    }
}

void UI_RegisterDvars(void)
{
    Dvar_RegisterBool("g_allowvote", 1, 0x1001);
    Dvar_RegisterBool("cg_brass", 1, 0x1001);
    Dvar_RegisterBool("cg_marks", 1, 0x1001);

    Dvar_RegisterString("server1", "", 0x1001);
    Dvar_RegisterString("server2", "", 0x1001);
    Dvar_RegisterString("server3", "", 0x1001);
    Dvar_RegisterString("server4", "", 0x1001);
    Dvar_RegisterString("server5", "", 0x1001);
    Dvar_RegisterString("server6", "", 0x1001);
    Dvar_RegisterString("server7", "", 0x1001);
    Dvar_RegisterString("server8", "", 0x1001);
    Dvar_RegisterString("server9", "", 0x1001);
    Dvar_RegisterString("server10", "", 0x1001);
    Dvar_RegisterString("server11", "", 0x1001);
    Dvar_RegisterString("server12", "", 0x1001);
    Dvar_RegisterString("server13", "", 0x1001);
    Dvar_RegisterString("server14", "", 0x1001);
    Dvar_RegisterString("server15", "", 0x1001);
    Dvar_RegisterString("server16", "", 0x1001);

    ui_netSource = Dvar_RegisterInt("ui_netSource", 0, 0, 2, 0x1001);
    ui_smallFont = Dvar_RegisterFloat("ui_smallFont", 0.25f, 0.0f, 1.0f, 0x1001);
    ui_bigFont = Dvar_RegisterFloat("ui_bigFont", 0.4f, 0.0f, 1.0f, 0x1001);
    ui_extraBigFont = Dvar_RegisterFloat("ui_extraBigFont", 0.55f, 0.0f, 1.0f, 0x1001);
    ui_currentMap = Dvar_RegisterInt("ui_currentMap", 0, 0, 0x7fffffff, 0x1001);
    ui_gametype = Dvar_RegisterInt("ui_gametype", 3, 0, 0x7fffffff, 0x1001);
    ui_joinGameType = Dvar_RegisterInt("ui_joinGametype", 0, 0, 0x7fffffff, 0x1001);
    ui_netGameTypeName = Dvar_RegisterString("ui_netGametypeName", "", 0x1001);
    ui_dedicated = Dvar_RegisterInt("ui_dedicated", 0, 0, 2, 0x1001);
    ui_currentNetMap = Dvar_RegisterInt("ui_currentNetMap", 0, 0, 0x7fffffff, 0x1001);
    ui_browserShowFull = Dvar_RegisterBool("ui_browserShowFull", 1, 0x1001);
    ui_browserShowEmpty = Dvar_RegisterBool("ui_browserShowEmpty", 1, 0x1001);
    ui_browserShowPassword = Dvar_RegisterBool("ui_browserShowPassword", 1, 0x1001);
    ui_browserShowNoPassword = Dvar_RegisterBool("ui_browserShowNoPassword", 1, 0x1001);
    ui_browserShowPure = Dvar_RegisterBool("ui_browserShowPure", 1, 0x1001);
    ui_browserShowDedicated = Dvar_RegisterBool("ui_browserShowDedicated", 0, 0x1001);
#    if COD2_IS_PATCH_13
    ui_browserShowPunkBuster = Dvar_RegisterInt("ui_browserShowPunkBuster", -1, (int)0x80000000, 0x7fffffff, 0x1001);
#    endif
    ui_browserMod = Dvar_RegisterInt("ui_browserMod", -1, -1, 0x7fffffff, 0x1001);
    ui_browserFriendlyfire = Dvar_RegisterInt("ui_browserFriendlyfire", -1, (int)0x80000000, 0x7fffffff, 0x1001);
    ui_browserKillcam = Dvar_RegisterInt("ui_browserKillcam", -1, (int)0x80000000, 0x7fffffff, 0x1001);
    ui_serverStatusTimeOut = Dvar_RegisterInt("ui_serverStatusTimeOut", 7000, 0, 0x7fffffff, 0x1001);
    ui_playerProfileCount = Dvar_RegisterInt("ui_playerProfileCount", 0, (int)0x80000000, 0x7fffffff, 0x1040);
    ui_playerProfileSelected = Dvar_RegisterString("ui_playerProfileSelected", "", 0x1040);
    ui_playerProfileNameNew = Dvar_RegisterString("ui_playerProfileNameNew", "", 0x1000);
}

void UI_Init(void)
{
    byte *legacyBase;
    int width, height;
    MenuList *menuList;
    int netGameTypeIdx;
    float mPitch;

    *(int *)uiInfo = 0;
    g_ingameMenusLoaded = 0;

    UI_LoadSoundAliases();

    UI_RegisterDvars();

    legacyBase = (byte *)imp_legacyHacksArray;
    (*(byte *)&((LegacyHacks *)legacyBase)->ui_newScriptMenu[0]) = 0;
    ((LegacyHacks *)legacyBase)->ui_newScriptMenuIndex = -1;
    (*(byte *)&((LegacyHacks *)legacyBase)->ui_scriptMenu[0]) = 0;
    ((LegacyHacks *)legacyBase)->ui_scriptMenuIndex = -1;
    ((LegacyHacks *)legacyBase)->ui_scriptMenuAllowResponse = 1;
    (*(byte *)&((LegacyHacks *)legacyBase)->ui_waitingScriptMenu[0]) = 0;
    ((LegacyHacks *)legacyBase)->ui_waitingScriptMenuIndex = -1;
    ((LegacyHacks *)legacyBase)->ui_waitingScriptMenuNoMouse = 0;

    String_Init();
    Menu_Setup(&uiInfo->uiDC);

    CL_GetScreenDimensions(&uiInfo->uiDC.screenWidth, &uiInfo->uiDC.screenHeight, (int *)&uiInfo->uiDC.screenAspect);

    width = uiInfo->uiDC.screenWidth;
    height = uiInfo->uiDC.screenHeight;

    if (width * 480 > height * 640) {
        *(float *)uiInfo = ((float)width + (float)height * -1.3333333730697632f) * 0.5f;
    } else {
        *(int *)uiInfo = 0;
    }

    Sys_Milliseconds();
    UI_GetGameTypesList();

    ui_netGameType = Dvar_RegisterInt("ui_netGametype", 0, 0, sharedUiInfo.numGameTypes - 1, 0x1001);

    UI_LoadArenas();

    menuList = UI_LoadMenus("ui_mp/menus.txt", 3);
    UI_AddMenuList(&uiInfo->uiDC, menuList);

    if (g_mapname[0] != '\0') {
        UI_MapLoadInfo(va("maps/mp/%s.csv", g_mapname));
    }

    UI_AssetCache();
    Menus_CloseAll(&uiInfo->uiDC);

    sharedUiInfo.serverHardwareIconList[0] = CL_RegisterMaterialNoMip("server_hardware_unknown", 3);
    sharedUiInfo.serverHardwareIconList[1] = CL_RegisterMaterialNoMip("server_hardware_linux_dedicated", 3);
    sharedUiInfo.serverHardwareIconList[2] = CL_RegisterMaterialNoMip("server_hardware_win_dedicated", 3);
    sharedUiInfo.serverHardwareIconList[3] = CL_RegisterMaterialNoMip("server_hardware_mac_dedicated", 3);
    sharedUiInfo.serverHardwareIconList[5] = CL_RegisterMaterialNoMip("server_hardware_win_listen", 3);
    sharedUiInfo.serverHardwareIconList[6] = CL_RegisterMaterialNoMip("server_hardware_mac_listen", 3);

    LAN_LoadCachedServers();

    if (sharedUiInfo.serverStatus.sortKey != 9) {
        sharedUiInfo.serverStatus.sortKey = 9;
        qsort(sharedUiInfo.serverStatus.displayServers, sharedUiInfo.serverStatus.numDisplayServers, 4, UI_ServersQsortCompare);
    }

    mPitch = Dvar_GetFloat("m_pitch");
    Dvar_SetBoolByName("ui_mousePitch", mPitch < 0.0f ? 1 : 0);

    sharedUiInfo.serverStatus.currentServerCinematic = -1;
    sharedUiInfo.previewMovie = -1;

    netGameTypeIdx = (ui_netGameType)->current.integer;
    Dvar_SetString((dvar_t *)ui_netGameTypeName, sharedUiInfo.gameTypes[netGameTypeIdx].gameType);

    Dvar_RegisterBool("ui_multiplayer", 1, 0x1040);
}


static void UI_BuildPlayerList(void)
{
    char cs[sizeof(uiClientState_t)];
    char name[0x20];
    int count;
    int n;

    GetClientState((uiClientState_t *)cs);
    count = atoi(Info_ValueForKey(CL_GetConfigString(0), "sv_maxclients"));
    memset(sharedUiInfo.playerClientNums, -1, 0x100);
    sharedUiInfo.playerCount = 0;

    for (n = 0; n < count; n++) {
        int playerCount;

        if (!GetClientname(n, name, 0x20))
            continue;

        playerCount = sharedUiInfo.playerCount;
        sharedUiInfo.playerClientNums[playerCount] = n;
        I_strncpyz(sharedUiInfo.playerNames[playerCount], name, 0x20);
        I_CleanStr(sharedUiInfo.playerNames[playerCount]);
        sharedUiInfo.playerCount = playerCount + 1;
    }
}

void UI_LoadIngameMenus(void)
{
    if (g_ingameMenusLoaded)
        return;

    g_ingameMenusLoaded = 1;

    Load_ScriptMenu("ingame", 3);
    Load_ScriptMenu("callvote", 3);
    Load_ScriptMenu("muteplayer", 3);
    Load_ScriptMenu("quickcommands", 3);
    Load_ScriptMenu("quickresponses", 3);
    Load_ScriptMenu("quickstatements", 3);
    Load_ScriptMenu("serverinfo_dm", 3);
    Load_ScriptMenu("serverinfo_tdm", 3);
    Load_ScriptMenu("serverinfo_ctf", 3);
    Load_ScriptMenu("serverinfo_hq", 3);
    Load_ScriptMenu("serverinfo_sd", 3);
    Load_ScriptMenu("team_americangerman", 3);
    Load_ScriptMenu("team_britishgerman", 3);
    Load_ScriptMenu("team_russiangerman", 3);
    Load_ScriptMenu("weapon_american", 3);
    Load_ScriptMenu("weapon_british", 3);
    Load_ScriptMenu("weapon_german", 3);
    Load_ScriptMenu("weapon_russian", 3);
}

static int UI_ServersQsortCompare(const void *arg1, const void *arg2)
{
    int s1 = *(const int *)arg1;
    int s2 = *(const int *)arg2;

    return LAN_CompareServers(
        (ui_netSource)->current.integer,
        sharedUiInfo.serverStatus.sortKey,
        sharedUiInfo.serverStatus.sortDir,
        s1, s2);
}

static void UI_InsertServerAtPosition(int serverIndex, int position)
{
    int numServers = sharedUiInfo.serverStatus.numDisplayServers;
    int *displayServers = sharedUiInfo.serverStatus.displayServers;
    int *selectedServer = &sharedUiInfo.serverStatus.currentServer;
    int j;

    if (position < 0 || numServers < position)
        return;

    if (*selectedServer >= position && numServers != 0)
        *selectedServer += 1;

    numServers++;
    sharedUiInfo.serverStatus.numDisplayServers = numServers;

    for (j = numServers - 1; j > position; j--)
        displayServers[j] = displayServers[j - 1];

    displayServers[position] = serverIndex;
}

static void UI_RemoveDuplicateFromFavorites(int serverIndex)
{
    int numServers = sharedUiInfo.serverStatus.numDisplayServers;
    int *displayServers = sharedUiInfo.serverStatus.displayServers;
    int i, j;

    if (numServers <= 0)
        return;

    for (i = 0; i < numServers; i++) {
        if (displayServers[i] == serverIndex)
            break;
    }
    if (i >= numServers)
        return;

    numServers--;
    sharedUiInfo.serverStatus.numDisplayServers = numServers;

    for (j = i; j < numServers; j++)
        displayServers[j] = displayServers[j + 1];
}

static void UI_BinaryInsertServer(int serverIndex)
{
    int numDisplay = sharedUiInfo.serverStatus.numDisplayServers;
    int *displayServers = sharedUiInfo.serverStatus.displayServers;
    int lo = 0;
    int hi = numDisplay;
    int position = 0;
    int lastCmp = 0;
    int source = (ui_netSource)->current.integer;
    int sortKey = sharedUiInfo.serverStatus.sortKey;
    int sortDir = sharedUiInfo.serverStatus.sortDir;

    while (hi > 0) {
        int mid = hi / 2;
        int testIdx = lo + mid;
        int cmp = LAN_CompareServers(source, sortKey, sortDir, serverIndex, displayServers[testIdx]);

        lastCmp = cmp;
        if (cmp == 0) {
            UI_InsertServerAtPosition(serverIndex, testIdx);
            return;
        }
        if (cmp > 0) {
            hi -= mid;
            position = testIdx + 1;
            lo = position;
            hi--;
        } else {
            hi -= mid;
        }
    }

    if (lastCmp >= 0)
        position = lo + 1;
    else
        position = lo;

    UI_InsertServerAtPosition(serverIndex, position);
}

static void UI_BuildServerDisplayList(qboolean force)
{
    char info_buf[0x400];
    int count, i, clients, ping;
    int netSource;

    if (!force) {
        if (uiInfo->uiDC.realTime <= sharedUiInfo.serverStatus.nextDisplayRefresh)
            return;
    }

    clients = 0;

    {
        const char *motdString = Dvar_GetString("cl_motdString");
        int motdLen;

        I_strncpyz(sharedUiInfo.serverStatus.motd, motdString, 0x400);
        motdLen = strlen(sharedUiInfo.serverStatus.motd);
        if (motdLen == 0) {
            const char *translated = UI_SafeTranslateString("EXE_COD_MULTIPLAYER");
            strcpy(sharedUiInfo.serverStatus.motd, va("%s - %s", translated, "1.0"));
            motdLen = strlen(sharedUiInfo.serverStatus.motd);
        }
        if (motdLen != sharedUiInfo.serverStatus.motdLen) {
            sharedUiInfo.serverStatus.motdLen = motdLen;
            sharedUiInfo.serverStatus.motdWidth = -1;
        }
    }

    if (force) {
        numclean = 0;
        sharedUiInfo.serverStatus.numDisplayServers = 0;
        sharedUiInfo.serverStatus.numPlayersOnServers = 0;
        netSource = (ui_netSource)->current.integer;
        sharedUiInfo.serverStatus.serverCount = LAN_GetServerCount(netSource);

        if (sharedUiInfo.serverStatus.currentServer >= 0)
            Menu_SetFeederSelection(&uiInfo->uiDC, 0, 2, 0, 0);

        LAN_MarkServerDirty((ui_netSource)->current.integer, -1, 1);
    }

    netSource = (ui_netSource)->current.integer;
    count = LAN_GetServerCount(netSource);

    if (LAN_WaitServerResponse(netSource) || (netSource == 0 && count == 0)) {
        sharedUiInfo.serverStatus.numDisplayServers = 0;
        sharedUiInfo.serverStatus.numPlayersOnServers = 0;
        sharedUiInfo.serverStatus.serverCount = LAN_GetServerCount((ui_netSource)->current.integer);
        sharedUiInfo.serverStatus.nextDisplayRefresh = uiInfo->uiDC.realTime + 500;
        return;
    }

    qsort(sharedUiInfo.serverStatus.displayServers, sharedUiInfo.serverStatus.numDisplayServers, 4, UI_ServersQsortCompare);

    for (i = 0; i < count; i++) {
        if (!LAN_ServerIsDirty((ui_netSource)->current.integer, i))
            continue;

        ping = LAN_GetServerPing((ui_netSource)->current.integer, i);
        if (ping <= 0 && (ui_netSource)->current.integer != 2)
            continue;

        LAN_GetServerInfo((ui_netSource)->current.integer, i, info_buf, 0x400);
        clients = atoi(Info_ValueForKey(info_buf, "clients"));
        sharedUiInfo.serverStatus.numPlayersOnServers += clients;

        if (I_strnicmp(Info_ValueForKey(info_buf, "addr"), "000.000.000.000", 15) == 0)
            goto reject;
        if (!(ui_browserShowEmpty)->current.enabled && clients == 0)
            goto reject;
        if (!(ui_browserShowFull)->current.enabled &&
            clients == atoi(Info_ValueForKey(info_buf, "sv_maxclients")))
            goto reject;
        if (!(ui_browserShowPassword)->current.enabled &&
            atoi(Info_ValueForKey(info_buf, "pswrd")) != 0)
            goto reject;
        if (!(ui_browserShowNoPassword)->current.enabled &&
            atoi(Info_ValueForKey(info_buf, "pswrd")) == 0)
            goto reject;
        if ((ui_browserShowPure)->current.enabled &&
            atoi(Info_ValueForKey(info_buf, "pure")) == 0)
            goto reject;
        if ((ui_browserShowDedicated)->current.enabled &&
            (unsigned)(atoi(Info_ValueForKey(info_buf, "hw")) - 1) > 2)
            goto reject;
        if ((ui_browserMod)->current.integer >= 0 &&
            atoi(Info_ValueForKey(info_buf, "mod")) != (ui_browserMod)->current.integer)
            goto reject;
        if ((ui_browserFriendlyfire)->current.integer >= 0 &&
            atoi(Info_ValueForKey(info_buf, "ff")) != (ui_browserFriendlyfire)->current.integer)
            goto reject;
        if ((ui_browserKillcam)->current.integer >= 0 &&
            atoi(Info_ValueForKey(info_buf, "kc")) != (ui_browserKillcam)->current.integer)
            goto reject;

        {
            int joinGTIdx = (ui_joinGameType)->current.integer;
            const char *joinGTName = sharedUiInfo.joinGameTypes[joinGTIdx].gameTypeName;
            if (joinGTName[0] != '\0') {
                const char *joinGTShort = sharedUiInfo.joinGameTypes[joinGTIdx].gameType;
                if (I_stricmp(Info_ValueForKey(info_buf, "gametype"), joinGTShort) != 0)
                    goto reject;
            }
        }

        if (ui_serverFilterType > 0) {
            const char *filterBaseName = *(const char **)((byte *)serverFilters + ui_serverFilterType * 8 + 4);
            if (I_stricmp(Info_ValueForKey(info_buf, "game"), filterBaseName) != 0)
                goto reject;
        }

        if ((ui_netSource)->current.integer == 2)
            UI_RemoveDuplicateFromFavorites(i);

        UI_BinaryInsertServer(i);

        if (ping > 0) {
            LAN_MarkServerDirty((ui_netSource)->current.integer, i, 0);
            numclean++;
        }
        continue;

    reject:
        LAN_MarkServerDirty((ui_netSource)->current.integer, i, 0);
    }

    sharedUiInfo.serverStatus.refreshtime = uiInfo->uiDC.realTime;
}

int UI_FeederCount(float feederID)
{
    if (feederID == 4.0f) {
        int gameType = (ui_netGameType)->current.integer;
        int numMaps = sharedUiInfo.mapCount;
        int count = 0;
        int i;

        if (numMaps <= 0)
            return 0;

        for (i = 0; i < numMaps; i++) {
            byte *entry = (byte *)&sharedUiInfo + i * 0xa4;
            *(int *)(entry + 0x13f4) = 0;
            if (*(int *)(entry + 0x1368) & (1 << gameType)) {
                count++;
                *(int *)(entry + 0x13f4) = 1;
            }
        }
        return count;
    }

    if (feederID == 9.0f)
        return sharedUiInfo.modCount;

    if (feederID == 2.0f) {
        int serverCount = LAN_GetServerCount((ui_netSource)->current.integer);
        if (serverCount != sharedUiInfo.serverStatus.serverCount) {
            sharedUiInfo.serverStatus.serverCount = serverCount;
            if (sharedUiInfo.serverStatus.numDisplayServers != 0) {
                sharedUiInfo.serverStatus.currentServer = -1;
                UI_BuildServerDisplayList(1);
            }
        }
        return sharedUiInfo.serverStatus.numDisplayServers;
    }

    if (feederID == 13.0f)
        return sharedUiInfo.serverStatusInfo.numLines;

    if (feederID == 7.0f || feederID == 20.0f) {
        int curTime = uiInfo->uiDC.realTime;
        if (curTime > uiInfo->playerRefresh) {
            uiInfo->playerRefresh = curTime + 3000;
            UI_BuildPlayerList();
        }
        return sharedUiInfo.playerCount;
    }

    if (feederID == 24.0f)
        return uiInfo->playerProfileCount;

    return 0;
}

static void UI_DrawMapPreview(const rectDef_t *rect, const vec_t *color, int net)
{
    int map;
    int mapCount;
    MaterialHandle material;

    if (net) {
        map = (ui_currentNetMap)->current.integer;
    } else {
        map = (ui_currentMap)->current.integer;
    }

    mapCount = sharedUiInfo.mapCount;
    if (map < 0 || map >= mapCount) {

        if (net) {
            Dvar_SetInt(ui_currentNetMap, 0);
        } else {
            Dvar_SetInt(ui_currentMap, 0);
        }
        map = 0;
    }

    material = sharedUiInfo.mapList[map].levelShot;

    if (!material) {
        material = CL_RegisterMaterialNoMip("menu/art/unknownmap", 3);
    }

    UI_DrawHandlePic(rect->x, rect->y, rect->w, rect->h,
                     rect->horzAlign, rect->vertAlign,
                     color, material);
}

void UI_SetMap(const char *mapname, const char *gametype)
{
    I_strncpyz(g_mapname, mapname, 0x40);
    I_strncpyz(g_gametype, gametype, 0x40);
    if (g_mapname[0]) {
        UI_MapLoadInfo(va("maps/mp/%s.csv", g_mapname));
    }
}

void UI_OpenMenu_f(void)
{
    Menus_OpenByName(&uiInfo->uiDC, Cmd_Args(1));
}

void UI_CloseMenu_f(void)
{
    Menus_CloseByName(&uiInfo->uiDC, Cmd_Args(1));
}

void UI_MouseEvent(int dx, int dy)
{
    int *cursorX = &uiInfo->uiDC.cursorx;
    int *cursorY = &uiInfo->uiDC.cursory;

    *cursorX += dx;
    if (*cursorX < 0)
        *cursorX = 0;
    else if (*cursorX > 640)
        *cursorX = 640;

    *cursorY += dy;
    if (*cursorY < 0)
        *cursorY = 0;
    else if (*cursorY > 480)
        *cursorY = 480;

    if (Menu_Count(&uiInfo->uiDC) > 0) {
        Display_MouseMove(&uiInfo->uiDC, 0, *cursorX, *cursorY);
    }
}


uiMenuCommand_t UI_GetActiveMenu(void)
{
    return uiInfo->currentMenuType;
}

qboolean UI_SetActiveMenu(int menu)
{
    menuDef_t *pFocus;
    const char *errorMsg;
    LegacyHacks *hacks;

    if (Menu_Count(&uiInfo->uiDC) <= 0)
        return 0;

    if (menu != UIMENU_SCRIPT_POPUP && menu != UIMENU_SCRIPT_POPUP_NO_MOUSE)
        uiInfo->currentMenuType = menu;

    switch (menu) {
    case UIMENU_NONE:
        Key_SetCatcher(Key_GetCatcher() & ~8);
        Dvar_SetIntByName("cl_paused", 0);
        Menus_CloseAll(&uiInfo->uiDC);
        return 1;

    case UIMENU_MAIN:
        Key_SetCatcher(8);
        Menus_OpenByName(&uiInfo->uiDC, "main");
        errorMsg = Dvar_GetString("com_errorMessage");
        if (strlen(errorMsg) && I_stricmp(errorMsg, ";"))
            Menus_OpenByName(&uiInfo->uiDC, "error_popmenu");
        SND_FadeAllSounds(1.0f, 1000);
        return 1;

    case UIMENU_INGAME:
        Key_SetCatcher(8);
        Menus_CloseAll(&uiInfo->uiDC);
        if (!Menus_OpenByName(&uiInfo->uiDC, cg->scriptMainMenu))
            Menus_OpenByName(&uiInfo->uiDC, "main");
        return 1;

    case UIMENU_NEED_CD:
        Key_SetCatcher(8);
        Menus_OpenByName(&uiInfo->uiDC, "needcd");
        return 1;

    case UIMENU_BAD_CD_KEY:
        Key_SetCatcher(8);
        Menus_OpenByName(&uiInfo->uiDC, "badcd");
        return 1;

    case UIMENU_TEAM:
        Key_SetCatcher(8);
        Menus_OpenByName(&uiInfo->uiDC, "team");
        return 1;

    case UIMENU_PREGAME:
    case UIMENU_POSTGAME:
        return 0;

    case UIMENU_WM_QUICKMESSAGE:
        uiInfo->uiDC.cursorx = 639;
        uiInfo->uiDC.cursory = 479;
        Key_SetCatcher(8);
        clients->displayHUDWithKeycatchUI = 1;
        Menus_CloseAll(&uiInfo->uiDC);
        Menus_OpenByName(&uiInfo->uiDC, "quickmessage");
        return 1;

    case UIMENU_WM_AUTOUPDATE:
        Menus_OpenByName(&uiInfo->uiDC, "autoupdate");
        return 1;

    case UIMENU_SCRIPT_POPUP:
    case UIMENU_SCRIPT_POPUP_NO_MOUSE:
        pFocus = Menu_GetFocused(&uiInfo->uiDC);
        if (pFocus && uiInfo->currentMenuType != UIMENU_SCRIPT_POPUP && uiInfo->currentMenuType != UIMENU_SCRIPT_POPUP_NO_MOUSE)
            return 0;
        if (pFocus && !I_stricmp(pFocus->window.name, legacyHacks->ui_newScriptMenu))
            return 1;
        uiInfo->currentMenuType = UIMENU_SCRIPT_POPUP;
        if (menu == UIMENU_SCRIPT_POPUP_NO_MOUSE) {
            uiInfo->uiDC.cursorx = 639;
            uiInfo->uiDC.cursory = 479;
        }
        Key_SetCatcher(8);
        Menus_CloseAll(&uiInfo->uiDC);
        hacks = legacyHacks;
        strcpy(hacks->ui_scriptMenu, hacks->ui_newScriptMenu);
        hacks->ui_scriptMenuIndex = hacks->ui_newScriptMenuIndex;
        hacks->ui_newScriptMenu[0] = 0;
        hacks->ui_newScriptMenuIndex = -1;
        Menus_OpenByName(&uiInfo->uiDC, hacks->ui_scriptMenu);
        return 1;

    case UIMENU_PLAYER_PROFILE:
        Key_SetCatcher(8);
        Menus_OpenByName(&uiInfo->uiDC, "player_profile");
        SND_FadeAllSounds(1.0f, 1000);
        return 1;
    }

    return 0;
}

qboolean UI_IsFullscreen(void)
{
    return Menus_AnyFullScreenVisible(&uiInfo->uiDC);
}

qboolean UI_AnyFullScreenMenuVisible(void)
{
    return Menus_AnyFullScreenVisible(&uiInfo->uiDC);
}

void UI_CloseAll(void)
{
    Menus_CloseAll(&uiInfo->uiDC);
}

void UI_DrawText(const char *text, int maxChars, FontHandle font, float x, float y, int horzAlign, int vertAlign, float scale, const vec_t *color, int style)
{
    float xScale;
    float yScale;

    diag_drawtext(text, (void *)font, x, y, scale);

    xScale = CL_NormalizedTextScale(font, scale);
    yScale = xScale;
    CalcSplitScreenTextOffset(font, &y);
    CalcScreenPlacement(&x, &y, &xScale, &yScale, horzAlign, vertAlign);

    x = (float)I_fround(x);
    y = (float)I_fround(y);

    CL_DrawTextPhysical(text, maxChars, font, x, y, xScale, yScale, color, style);
}
#endif

serverStatusDvar_t serverStatusDvars[23] = {
    { (const char *)&str_002a714c, (const char *)&str_002a9ab8, (sscType_t)0x0 },
    { (const char *)&str_002a9ad0, (const char *)&str_002a9ad8, (sscType_t)0x0 },
    { (const char *)&str_002a9af0, (const char *)&str_002a9af8, (sscType_t)0x1 },
    { (const char *)&str_002a9b10, (const char *)&str_002a9b1c, (sscType_t)0x0 },
    { (const char *)&str_002a7100, (const char *)&str_002a9b34, (sscType_t)0x0 },
    { (const char *)&str_002a71f0, (const char *)&str_002a9b4c, (sscType_t)0x1 },
    { (const char *)&str_002a7124, (const char *)&str_002a9b60, (sscType_t)0x0 },
    { (const char *)&str_002168fc, (const char *)&str_002a9b74, (sscType_t)0x0 },
    { (const char *)&str_002a7118, (const char *)&str_002a9b8c, (sscType_t)0x0 },
    { (const char *)&str_002a9ba4, (const char *)&str_002a9bb0, (sscType_t)0x0 },
    { (const char *)&str_002a9bc8, (const char *)&str_002a9bd4, (sscType_t)0x0 },
    { (const char *)&str_002a9bec, (const char *)&str_002a9bf8, (sscType_t)0x0 },
    { (const char *)&str_002a9c10, (const char *)&str_002a9c20, (sscType_t)0x1 },
    { (const char *)&str_002a9c3c, (const char *)&str_002a9c50, (sscType_t)0x0 },
    { (const char *)&str_002a70dc, (const char *)&str_002a9c68, (sscType_t)0x0 },
    { (const char *)&str_002a9c80, (const char *)&str_002a9c94, (sscType_t)0x0 },
    { (const char *)&str_002a9cb0, (const char *)&str_002a9cc4, (sscType_t)0x0 },
    { (const char *)&str_00216d64, (const char *)&str_002a9ce0, (sscType_t)0x0 },
    { (const char *)&str_002a9cf4, (const char *)&str_002a9cf8, (sscType_t)0x1 },
    { (const char *)&str_002a9d04, (const char *)&str_002a9d10, (sscType_t)0x1 },
    { (const char *)&str_002a9d28, (const char *)&str_002a9d34, (sscType_t)0x1 },
    { (const char *)&str_002a71c8, (const char *)&str_002a9d4c, (sscType_t)0x1 },
    { 0, 0, (sscType_t)0x0 }
};

const char str_002168fc[] = "shortversion";
const char str_00216d64[] = "fs_game";
const char str_002a70dc[] = "sv_maxclients";
const char str_002a7100[] = "g_gametype";
const char str_002a7118[] = "protocol";
const char str_002a7124[] = "mapname";
const char str_002a714c[] = "sv_hostname";
const char str_002a71c8[] = "sv_voice";
const char str_002a71f0[] = "sv_pure";
const char str_002a9ab8[] = "@EXE_SV_INFO_SERVERNAME";
const char str_002a9ad0[] = "address";
const char str_002a9ad8[] = "@EXE_SV_INFO_ADDRESS";
const char str_002a9af0[] = "pswrd";
const char str_002a9af8[] = "@EXE_SV_INFO_PASSWORD";
const char str_002a9b10[] = "gamename";
const char str_002a9b1c[] = "@EXE_SV_INFO_GAMENAME";
const char str_002a9b34[] = "@EXE_SV_INFO_GAMETYPE";
const char str_002a9b4c[] = "@EXE_SV_INFO_PURE";
const char str_002a9b60[] = "@EXE_SV_INFO_MAP";
const char str_002a9b74[] = "@EXE_SV_INFO_VERSION";
const char str_002a9b8c[] = "@EXE_SV_INFO_PROTOCOL";
const char str_002a9ba4[] = "sv_maxping";
const char str_002a9bb0[] = "@EXE_SV_INFO_MAXPING";
const char str_002a9bc8[] = "sv_minping";
const char str_002a9bd4[] = "@EXE_SV_INFO_MINPING";
const char str_002a9bec[] = "sv_maxrate";
const char str_002a9bf8[] = "@EXE_SV_INFO_MAXRATE";
const char str_002a9c10[] = "sv_floodprotect";
const char str_002a9c20[] = "@EXE_SV_INFO_FLOODPROTECT";
const char str_002a9c3c[] = "sv_allowanonymous";
const char str_002a9c50[] = "@EXE_SV_INFO_ALLOWANON";
const char str_002a9c68[] = "@EXE_SV_INFO_MAXCLIENTS";
const char str_002a9c80[] = "sv_privateclients";
const char str_002a9c94[] = "@EXE_SV_INFO_PRIVATECLIENTS";
const char str_002a9cb0[] = "scr_friendlyFire";
const char str_002a9cc4[] = "@EXE_SV_INFO_FRIENDLY_FIRE";
const char str_002a9ce0[] = "@EXE_SV_INFO_MOD";
const char str_002a9cf4[] = "mod";
const char str_002a9cf8[] = "@MENU_MODS";
const char str_002a9d04[] = "scr_killcam";
const char str_002a9d10[] = "@EXE_SV_INFO_KILLCAM";
const char str_002a9d28[] = "g_antilag";
const char str_002a9d34[] = "@EXE_SV_INFO_ANTILAG";
const char str_002a9d4c[] = "@EXE_SV_INFO_VOICE";
const char str_002a9d68[] = "EXE_MONTH_ABV_JANUARY";
const char str_002a9d80[] = "EXE_MONTH_ABV_FEBRUARY";
const char str_002a9d98[] = "EXE_MONTH_ABV_MARCH";
const char str_002a9dac[] = "EXE_MONTH_ABV_APRIL";
const char str_002a9dc0[] = "EXE_MONTH_ABV_MAY";
const char str_002a9dd4[] = "EXE_MONTH_ABV_JUN";
const char str_002a9de8[] = "EXE_MONTH_ABV_JULY";
const char str_002a9dfc[] = "EXE_MONTH_ABV_AUGUST";
const char str_002a9e14[] = "EXE_MONTH_ABV_SEPTEMBER";
const char str_002a9e2c[] = "EXE_MONTH_ABV_OCTOBER";
const char str_002a9e44[] = "EXE_MONTH_ABV_NOVEMBER";
const char str_002a9e5c[] = "EXE_MONTH_ABV_DECEMBER";
const char str_002a9e74[] = "EXE_LOCAL";
const char str_002a9e80[] = "EXE_INTERNET";
const char str_002a9e90[] = "EXE_FAVORITES";
