extern void Com_Printf(const char *fmt, ...);
#include "common_types.h"
#include "imports.h"
#include "bytematch.h"
#include "headers/PC/cgame_mp/cg_local.h"
extern weaponInfo_t cg_weaponsArray[];
extern displayContextDef_t cgDC;

extern clientStatic_t cls;
extern const dvar_t *cg_hudDamageIconHeight;
extern const dvar_t *cg_hudDamageIconInScope;
extern const dvar_t *cg_hudDamageIconOffset;
extern const dvar_t *cg_hudDamageIconWidth;
extern const dvar_t *cg_hudGrenadeIconHeight;
extern const dvar_t *cg_hudGrenadeIconInScope;
extern const dvar_t *cg_hudGrenadeIconMaxHeight;
extern const dvar_t *cg_hudGrenadeIconMaxRange;
extern const dvar_t *cg_hudGrenadeIconOffset;
extern const dvar_t *cg_hudGrenadeIconWidth;
extern const dvar_t *cg_hudGrenadePointerHeight;
extern const dvar_t *cg_hudGrenadePointerPivot;
extern const dvar_t *cg_hudGrenadePointerPulseFreq;
extern const dvar_t *cg_hudGrenadePointerPulseMax;
extern const dvar_t *cg_hudGrenadePointerPulseMin;
extern const dvar_t *cg_hudGrenadePointerWidth;
extern const dvar_t *cg_centerPrintY;
extern void CalcScreenX(float *x, int align);
extern void CalcScreenY(float *y, int align);
extern const dvar_t *cg_centertime;
extern const dvar_t *cg_chatHeight;
extern const dvar_t *cg_chatTime;
extern const dvar_t *cg_crosshairAlpha;
extern const dvar_t *cg_crosshairAlphaMin;
extern const dvar_t *cg_crosshairDynamic;
extern const dvar_t *cg_crosshairEnemyColor;
extern const dvar_t *cg_draw2D;
extern const dvar_t *cg_drawCrosshair;
extern const dvar_t *cg_drawCrosshairNames;
extern const dvar_t *cg_drawCrosshairNamesPosX;
extern const dvar_t *cg_drawCrosshairNamesPosY;
extern const dvar_t *cg_drawFPS;
extern const dvar_t *cg_drawGameMessages;
extern const dvar_t *cg_drawGun;
extern const dvar_t *cg_drawLagometer;
extern const dvar_t *cg_drawMaterial;
extern const dvar_t *cg_drawScriptUsage;
extern const dvar_t *cg_drawSnapshot;
extern const dvar_t *cg_drawSoundOverlay;
extern const dvar_t *cg_drawTurretCrosshair;
extern const dvar_t *cg_hudChatPosition;
extern const dvar_t *cg_hudCompassSize;
extern const dvar_t *cg_hudSayPosition;
extern const dvar_t *cg_minicon;
extern const dvar_t *cg_nopredict;
extern const dvar_t *cg_paused;
extern const dvar_t *cg_subtitleCharHeight;
extern const dvar_t *cg_subtitlePosX;
extern const dvar_t *cg_subtitlePosY;
extern const dvar_t *cg_subtitles;
extern const dvar_t *cg_synchronousClients;

extern const dvar_t *hud_fade_ammodisplay;
extern const dvar_t *hud_fade_compass;
extern const dvar_t *hud_fade_healthbar;
extern const dvar_t *hud_fade_offhand;
extern const dvar_t *hud_fade_stance;
extern const dvar_t *hud_health_startpulse_injured;

extern Bool CG_AreHudElemsHidden(void);
extern Bool CG_AreHudMenusHidden(void);
extern void CG_Draw2dHudElems(qboolean foreground);
extern int CG_DrawBigDevString(float x, float y, const char *s, float alpha, int align);
extern qboolean CG_DrawScoreboard(void);
extern void CL_TrackStatistics(trStatistics_t *pStats);
extern void Con_DrawMiniConsole(int xPos, int yPos, float alpha);
extern void Con_DrawNotify(int xPos, int yPos, float alpha, msgwnd_mode_t mode);
extern void Con_DrawSay(int y);
extern void Con_DrawSubtitles(int xPos, int yPos, int charHeight, float alpha, msgwnd_mode_t mode);
extern void Menu_PaintAll(displayContextDef_t *dc);
extern float floorf(float x);
extern int stricmp(const char *s1, const char *s2);

extern struct lagometer_t lagometer;
static int previous;
static struct trStatistics_t rendererStats;
static int fps_previousTimes[32];
static int fps_index;
static vec4_t colorWhiteFaded;
static vec4_t colorGreenFaded;
static vec4_t colorRedFaded;

extern const vec_t colorWhite[4];
extern void UI_DrawHandlePic(float x, float y, float w, float h, int horzAlign, int vertAlign, const vec_t *color, MaterialHandle material);
extern void CG_DrawStringExt(float x, float y, const char *string, const vec_t *setColor, qboolean forceColor, qboolean shadow, float charHeight, qboolean adjust);
extern void CL_LookupColor(unsigned char c, vec_t *color);
extern char *va( const char *format, ... );
extern unsigned int Scr_GetNumScriptVars(void);
extern unsigned int Scr_GetNumScriptThreads(void);
extern int Scr_GetStringUsage(void);
extern int Sys_Milliseconds(void);
extern int CL_GetCurrentCmdNumber(void);
extern int CL_GetKeyCatchers(void);
extern qboolean CL_GetUserCmd(int cmdNumber, usercmd_t *ucmd);
extern Bool CL_IsRenderingSplitScreen(void);
extern void CL_SetUserCmdAimValues(vec_t *kickAngles);
extern void CL_SetUserCmdValue(int weapon, int offHandIndex, float sensitivity);
extern void CL_RenderScene(const refdef_t *fd);
extern qboolean CG_DrawShellShockSavedScreenBlend(const shellshock_parms_t *parms, int start, int duration);
extern void CG_TileClear(void);
extern const char *UI_SafeTranslateString(const char *ref);
extern const char *UI_ReplaceConversionString(const char *sourceString, const char *replaceString);
extern const char *SEH_LocalizeTextMessage(const char *msg, const char *context, msgLocErrType_t errType);
extern FontHandle UI_GetFontHandle(int fontEnum, float scale);
extern int UI_TextWidth(const char *text, int maxChars, FontHandle font, float scale);
extern int UI_TextHeight(FontHandle font, float fontScale);
extern void UI_DrawText(const char *text, int maxChars, FontHandle font, float x, float y, int horzAlign, int vertAlign, float scale, const vec_t *color, int style);
extern MaterialHandle CL_RegisterMaterial(const char *name, int flags);
extern void CG_TranslateHudElemMessage(const char *message, const char *messageType, char *hudElemString);
extern void CG_TraceCapsule(trace_t *result, const vec_t *start, const vec_t *mins, const vec_t *maxs, const vec_t *end, int skipNumber, int mask);
extern int BG_GetViewmodelWeaponIndex(const playerState_t *ps);
extern WeaponDef * BG_GetWeaponDef(int iWeapon);
extern int BG_GetNumWeapons(void);
extern void CL_DrawStretchPic(float x, float y, float w, float h, int horzAlign, int vertAlign, float s1, float t1, float s2, float t2, const vec_t *color, MaterialHandle material);
extern void CL_DrawStretchPicPhysical(float x, float y, float w, float h, float s1, float t1, float s2, float t2, const vec_t *color, MaterialHandle material);
extern void CalcScreenPlacement(float *x, float *y, float *w, float *h, int horzAlign, int vertAlign);
extern void CG_DrawRotatedPic(float x, float y, float width, float height, int horzAlign, int vertAlign, float angle, const vec_t *color, MaterialHandle material);
extern qboolean CG_ScoreboardDisplayed(void);
extern void Con_DrawBoldMessages(int xPos, int yPos, float alpha, msgwnd_mode_t mode);
extern float *CG_FadeColor(int startMsec, int totalMsec, int fadeMsec);
extern void Controls_GetConfig(void);
extern int GetKeyBindingLocalizedString(const char *command, char *keys);
extern void I_strncpyz(char *dest, const char *src, int destsize);
extern char *I_CleanStr(char *string);
extern unsigned int SEH_ReadCharFromString(const char **ppsText, qboolean *pbIsTrailingPunctuation);
extern void CG_PlayerSprites(centity_t *cent);
extern qboolean CL_PickMaterial(const vec_t *org, const vec_t *dir, char *pszName, char *pszSurfaceFlags, char *pszContents, int iMaxChars);
extern int CG_DrawSmallDevStringColor(float x, float y, const char *s, const vec_t *color, int align);
extern int CG_DrawBigDevStringColor(float x, float y, const char *s, const vec_t *color, int align);
extern int __mh_execute_header;
extern int SND_GetSoundOverlay(snd_overlay_type_t type, snd_overlay_info_t *info, int maxcount, int *cpu);
extern const char* Dvar_GetString(const char *dvarName);
extern int Dvar_GetInt(const char *dvarName);
extern bool Dvar_GetBool(const char *dvarName);
extern void AngleVectors( const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up );
extern float Vec3Distance(const vec3_t v1, const vec3_t v2);
extern float crandom(void);
extern double sin(double);
extern double tan(double);
extern float GetRealWidthFromVirtualWidth(float width);
extern float GetRealHeightFromVirtualHeight(float height);
extern float GetVirtualWidthFromRealWidth(float width);
extern float GetVirtualHeightFromRealHeight(float height);
extern void BG_GetSpreadForWeapon(const playerState_t *ps, int weaponIndex, float *minSpread, float *maxSpread);
extern float AlignHudElemX(int alignOrg, float x, float width);
extern float AlignHudElemY(int alignOrg, float y, float height);
extern void UI_FillRectPhysical(float x, float y, float width, float height, const vec_t *color);
extern void StatMon_GetStatsArray(const statmonitor_t **array, int *count);
extern void CL_CloseAllMenus(void);
extern void CG_DrawRotatedQuadPic(float x, float y, vec2_t *verts, float angle, const vec_t *color, MaterialHandle material);
extern void CL_DrawQuadPic(vec2_t *verts, const vec_t *color, MaterialHandle material);
extern float vectoyaw( const vec3_t vec );
extern float AngleNormalize360( float angle );
extern float sinf(float);
extern float cosf(float);
extern float fabsf(float);

extern const vec_t colorYellow[4];
extern const vec_t colorRed[4];
extern const vec_t colorGreen[4];
extern const vec_t colorBlue[4];

extern void Menus_CloseByName(displayContextDef_t *dc, const char *p);
extern menuDef_t *Menus_FindByName(displayContextDef_t *dc, const char *name);
extern void Window_AddDynamicFlags(Window *w, const int newFlags);
/* Declared int here, but the real function returns void and takes a Window*. A caller
   does `return Window_RemoveDynamicFlags(..)`, so correcting the signature would change
   that caller's return value -- needs care. */
extern void Window_RemoveDynamicFlags(Window *w, const int newFlags);
extern void CG_MenuShowNotify(int menuToShow);
extern float CG_CalcPlayerHealth(void);
extern Bool CG_CheckPlayerForLowAmmo(void);
extern Bool CG_CheckPlayerForLowClip(void);
extern int CL_GetLocalClientActiveCount(void);

void CG_DrawTeamBackground(float x, float y, float w, float h, float alpha, int team);
static void CG_DrawScriptUsage(void);
void CG_CalculateFPS(void);
void CG_AddLagometerFrameInfo(void);
void CG_AddLagometerSnapshotInfo(snapshot_t *snap);
static void CG_DrawDisconnect(void);
void CG_PriorityCenterPrint(const char *str, float charWidth, int priority);
bool CG_GetWeapReticleZoom(float *pfZoom);
void CG_DrawFrameOverlay(float innerLeft, float innerRight, float innerTop, float innerBottom, const vec_t *color, MaterialHandle material);
static unsigned int CG_DrawCrosshairNames(void);
void CG_CheckTimedMenus(void);
static void CG_DrawSoundOverlay(void);
static void CG_DrawMaterial(void);
void CG_ShakeCamera(void);
static qboolean CG_DrawFollow(void);
void CG_DrawPlayerSprites(void);
void CG_DrawActive(void);
static unsigned int CG_DrawChatMessages(void);
static void __attribute_regparm__(2) CG_CalcCrosshairPosition(float *x, float *y);
static float CG_DrawFPS(float y);
void CG_DrawBoldGameMessages(void);
void CG_DrawTurretCrossHair(void);
static unsigned int CG_DrawSpectatorMessage(void);
void CG_StartShakeCamera(float scale, int duration, const vec_t *src, float radius);
static float CG_DrawWeapReticle(void);
unsigned int CG_DrawCrosshair(void);
unsigned int CG_Draw2D(void);

static inline __attribute__((always_inline)) qboolean CG_UpdateCameraShakeStrength(cameraShake_t *shake)
{
    float elapsed;
    float distanceScale;
    float rumbleScale;

    int elapsedMs = cg->time - shake->time;
    if (elapsedMs < 0) {
        return 0;
    }
    elapsed = (float)elapsedMs;
    if (elapsed >= shake->length) {
        return 0;
    }

    distanceScale = 1.0f - Vec3Distance(shake->src, cg->refdef.vieworg) / shake->radius;
    rumbleScale = (1.0f - elapsed / shake->length) * shake->scale;
    if (rumbleScale <= 0.0f) {
        return 0;
    }

    shake->size = distanceScale >= 0.0f ? distanceScale * rumbleScale : distanceScale / rumbleScale;
    shake->rumbleScale = rumbleScale;
    return 1;
}

void CG_StartShakeCamera(float scale, int duration, const vec_t *src, float radius)
{
    cameraShake_t temp;
    cameraShake_t *shake;
    int bestIndex;
    float bestSize;
    int i;

    temp.scale = scale;
    temp.length = (float)duration;
    temp.time = cg->time;
    temp.src[0] = src[0];
    temp.src[1] = src[1];
    temp.src[2] = src[2];
    temp.radius = radius;
    CG_UpdateCameraShakeStrength(&temp);

    for (i = 0; i < 4; ++i) {
        shake = &cg->cameraShake[i];
        if (shake->time > cg->time)
            goto store;
        if ((float)shake->time + shake->length <= (float)cg->time)
            goto store;
    }

    bestIndex = i;
    bestSize = temp.size;
    for (i = 0; i < 4; ++i) {
        if (cg->cameraShake[i].size < bestSize) {
            bestIndex = i;
            bestSize = cg->cameraShake[i].size;
        }
    }
    if (bestIndex == 4)
        return;
    i = bestIndex;

store:
    shake = &cg->cameraShake[i];
    *shake = temp;
}

void CG_DrawTeamBackground(float x, float y, float w, float h, float alpha, int team)
{
    vec4_t color;

    color[3] = alpha;
    if (team == 1) {
        color[0] = 1.0f;
        color[1] = 0.0f;
        color[2] = 0.0f;
    } else if (team == 2) {
        color[0] = 0.0f;
        color[1] = 0.0f;
        color[2] = 1.0f;
    } else {
        return;
    }

    UI_DrawHandlePic(x, y, w, h, 0, 0, color, cgs->media.teamStatusBar);
}

void CG_DrawScriptUsage(void)
{
    CG_DrawStringExt(480.0f, 80.0f, va("num vars:    %d", Scr_GetNumScriptVars()), colorWhite, 1, 1, 16.0f, 1);
    CG_DrawStringExt(480.0f, 96.0f, va("num threads: %d", Scr_GetNumScriptThreads()), colorWhite, 1, 1, 16.0f, 1);
    CG_DrawStringExt(480.0f, 112.0f, va("string usage: %d", Scr_GetStringUsage()), colorWhite, 1, 1, 16.0f, 1);
}

void CG_CalculateFPS(void)
{
    int now;
    int index;

    int delta;

    now = Sys_Milliseconds();
    delta = now - previous;
    previous = now;
    index = fps_index;
    fps_previousTimes[index % 32] = delta;
    fps_index = index + 1;
}

void CG_AddLagometerFrameInfo(void)
{
    int index = lagometer.frameCount;
    lagometer.frameSamples[index & 0x7f] = cg->time - cg->latestSnapshotTime;
    lagometer.frameCount = index + 1;
}

void CG_AddLagometerSnapshotInfo(snapshot_t *snap)
{
    int index;
    if (!snap) {
        index = lagometer.snapshotCount;
        lagometer.snapshotSamples[index & 0x7f] = -1;
        lagometer.snapshotCount = index + 1;
        return;
    }
    index = lagometer.snapshotCount;
    lagometer.snapshotSamples[index & 0x7f] = *(int *)((byte *)snap + 4);
    lagometer.snapshotFlags[index & 0x7f] = *(int *)snap;
    lagometer.snapshotCount = index + 1;
}

void CG_DrawDisconnect(void)
{
    usercmd_t cmd;
    snapshot_t *snap;
    const char *text;
    FontHandle font;
    int textWidth;
    float x;
    vec4_t color = { 1.0f, 1.0f, 1.0f, 1.0f };

    CL_GetUserCmd(CL_GetCurrentCmdNumber() - 127, &cmd);
    snap = cg->nextSnap;
#if COD2_TRACE
    if (getenv("COD2_CNXDIAG")) {
        static int c;
        if ((c++ & 0x3f) == 0)
            Com_Printf("[cnxdiag] cmd.serverTime=%d snap->ps.commandTime=%d cg->time=%d cmdNum=%d\n",
                       cmd.serverTime, snap->ps.commandTime, cg->time, CL_GetCurrentCmdNumber());
    }
#endif
    if (cmd.serverTime <= snap->ps.commandTime || cmd.serverTime > cg->time) {
        return;
    }

    text = UI_SafeTranslateString("CGAME_CONNECTIONINTERUPTED");
    font = UI_GetFontHandle(0, 0.5f);
    textWidth = UI_TextWidth(text, 0, font, 0.5f);
    textWidth = (int)(float)textWidth;
    x = (float)((640 - textWidth) / 2);
    UI_DrawText(text, 0x7fffffff, font, x, 100.0f, 0, 0, 0.5f, color, 3);
    if (!(cg->time & 0x200)) {
        UI_DrawHandlePic(296.0f, 416.0f, 48.0f, 48.0f, 0, 0, 0, CL_RegisterMaterial("net_disconnect", 7));
    }
}

void CG_PriorityCenterPrint(const char *str, float charWidth, int priority)
{
    char hudElemString[256];
    const char *s;
    qboolean needNewline = 0;
    int count;
    unsigned int letter;

    if (cgArray[0].centerPrintTime && priority < cgArray[0].centerPrintPriority) {
        return;
    }

    CG_TranslateHudElemMessage(str, "Center Print", hudElemString);
    I_strncpyz(cgArray[0].centerPrint, hudElemString, 0x100);
    cgArray[0].centerPrintPriority = priority;

    s = cgArray[0].centerPrint;
    count = 0;
    while (*s) {
        letter = SEH_ReadCharFromString(&s, 0);
        if (letter == '\n') {
            needNewline = 0;
            count = 0;
            continue;
        }

        ++count;
        if (count >= 0x4b) {
            needNewline = 1;
        }

        if (needNewline && letter == ' ') {
            ((char *)s)[-1] = '\n';
            needNewline = 0;
            count = 0;
        }
    }

    cgArray[0].centerPrintTime = cgArray[0].time + 0x7d0;
    cgArray[0].centerPrintCharWidth = (int)charWidth;
    cgArray[0].centerPrintLines = 1;

    s = cgArray[0].centerPrint;
    while (*s) {
        letter = SEH_ReadCharFromString(&s, 0);
        if (letter == '\n') {
            ++cgArray[0].centerPrintLines;
            continue;
        }

        if (letter == '\\' && *s == 'n') {
            ++cgArray[0].centerPrintLines;
            ++s;
        }
    }
}

/* qboolean (int), not Bool: callers (cg_weapons.c, cg_predict_mp.c) declare it
 * `extern qboolean` and read full eax; a 1-byte Bool return leaves eax's upper
 * bytes dirty under MSVC -> garbage non-zero -> !CG_GetWeapReticleZoom() == 0 ->
 * drawGun never set -> the viewmodel was hidden even when spawned. */
bool CG_GetWeapReticleZoom(float *pfZoom)
{
    WeaponDef *weaponDef;
    float zoom;

    weaponDef = (WeaponDef *)BG_GetWeaponDef(BG_GetViewmodelWeaponIndex(&cg->predictedPlayerState));
    zoom = cg->predictedPlayerState.fWeaponPosFrac;
    *pfZoom = 0.0f;

    if (!weaponDef->szOverlayMaterial[0] && !weaponDef->overlayReticle)
    {
        return 0;
    }

    if (zoom == 0.0f) {
        return 0;
    }

    if (cg->playerEntity.bPositionToADS) {
        *pfZoom = zoom - (1.0f - weaponDef->fAdsZoomInFrac);
        if (*pfZoom > 0.0f) {
            *pfZoom /= weaponDef->fAdsZoomInFrac;
        }
    } else {
        *pfZoom = zoom - (1.0f - weaponDef->fAdsZoomOutFrac);
        if (*pfZoom > 0.0f) {
            *pfZoom /= weaponDef->fAdsZoomOutFrac;
        }
    }

    if (*pfZoom <= 0.01f) {
        return 0;
    }

    if (*pfZoom > 1.0f) {
        *pfZoom = 1.0f;
    }

    return 1;
}

void CG_DrawFrameOverlay(float innerLeft, float innerRight, float innerTop, float innerBottom, const vec_t *color, MaterialHandle material)
{
    float screenWidth;
    float screenHeight;

    screenWidth = (float)cls.vidConfig.width ;
    screenHeight = (float)cls.vidConfig.height ;

    if (innerLeft > 0.0f) {
        CL_DrawStretchPicPhysical(0.0f, 0.0f, innerLeft, screenHeight, 0.0f, 0.0f, 0.0f, 1.0f, color, material);
    }

    if (innerRight < screenWidth) {
        CL_DrawStretchPicPhysical(innerRight, 0.0f, screenWidth - innerRight, screenHeight, 0.0f, 0.0f, 0.0f, 1.0f, color, material);
    }

    if (innerTop > 0.0f) {
        CL_DrawStretchPicPhysical(innerLeft, 0.0f, innerRight - innerLeft, innerTop, 0.0f, 0.0f, 1.0f, 0.0f, color, material);
    }

    if (innerBottom < screenHeight) {
        CL_DrawStretchPicPhysical(innerLeft, innerBottom, innerRight - innerLeft, screenHeight - innerBottom, 0.0f, 0.0f, 1.0f, 0.0f, color, material);
    }
}

unsigned int CG_DrawCrosshairNames(void)
{
    trace_t trace;
    vec3_t start;
    vec3_t end;
    vec3_t traceMins = { 0.0f, 0.0f, 0.0f };
    vec3_t traceMaxs = { 0.0f, 0.0f, 0.0f };
    float *baseColor;
    clientInfo_t *localClientInfo;
    clientInfo_t *targetClientInfo;
    char *name;
    FontHandle font;
    vec4_t actualColor;
    float healthFrac;
    int targetClientNum;
    int x;
    int y;

    if (!cg_drawCrosshairNames->current.enabled) {
        return 0;
    }

    if (cg->renderingThirdPerson) {
        return 0;
    }

    start[0] = cg->refdef.vieworg[0];
    start[1] = cg->refdef.vieworg[1];
    start[2] = cg->refdef.vieworg[2];
    end[0] = start[0] + cg->refdef.viewaxis[0][0] * 8192.0f;
    end[1] = start[1] + cg->refdef.viewaxis[0][1] * 8192.0f;
    end[2] = start[2] + cg->refdef.viewaxis[0][2] * 8192.0f;
    CG_TraceCapsule(&trace, start, traceMins, traceMaxs, end, cg->snap->ps.clientNum, 0x2000001);

    if (trace.entityNum <= 63) {
        cg->crosshairClientNum = trace.entityNum;
        cg->crosshairClientTime = cg->time;
    }

    baseColor = CG_FadeColor(cg->crosshairClientTime, 150, 100);
    if (!baseColor) {
        return 0;
    }

    targetClientNum = cg->crosshairClientNum;
    if (targetClientNum < 0 || targetClientNum > 64) {
        return 0;
    }

    localClientInfo = &cg->bgs.clientinfo[cg->snap->ps.clientNum];
    if (!localClientInfo->infoValid || !localClientInfo->team) {
        return 0;
    }

    targetClientInfo = &cg->bgs.clientinfo[targetClientNum];
    if (!targetClientInfo->infoValid) {
        return 0;
    }

    if (localClientInfo->team != 3 && localClientInfo->team != targetClientInfo->team) {
        return 0;
    }

    name = (char *)va("%s", targetClientInfo->name);
    if (!name || !name[0]) {
        return 0;
    }
    I_CleanStr(name);

    x = cg_drawCrosshairNamesPosX->current.integer;
    y = cg_drawCrosshairNamesPosY->current.integer + 16;

    if (cg->crosshairClientNum != cg->identifyClientNum) {
        actualColor[0] = 1.0f;
        actualColor[1] = 1.0f;
        actualColor[2] = 1.0f;
    } else {
        healthFrac = (float)cg->identifyClientHealth / 100.0f;
        if (healthFrac < 0.0f) {
            healthFrac = 0.0f;
        } else if (healthFrac > 1.0f) {
            healthFrac = 1.0f;
        }

        actualColor[2] = 0.0f;
        if (healthFrac > 0.5f) {
            actualColor[0] = (1.0f - healthFrac) * 2.0f;
            actualColor[1] = 1.0f;
        } else {
            actualColor[0] = 1.0f;
            actualColor[1] = healthFrac * 2.0f;
        }
    }

    actualColor[3] = baseColor[3] * 0.60000002f;
    font = UI_GetFontHandle(0, 1.0f / 3.0f);
    UI_DrawText(name, 0x7fffffff, font, (float)x, (float)y, 0, 0, 1.0f / 3.0f, actualColor, 3);
    return 0;
}

int CG_CheckPlayerMovement(usercmd_t oldCmd, usercmd_t newCmd)
{
    if (memcmp(oldCmd.angles, newCmd.angles, sizeof(oldCmd.angles)))
        return 1;
    return newCmd.forwardmove || newCmd.rightmove;
}

int CG_CheckPlayerStanceChange(int newButtons, int changedButtons)
{
    if (changedButtons & 0x2300)
    {
        CG_MenuShowNotify(3);
        return 1;
    }
    if (newButtons & 0x2300)
        CG_MenuShowNotify(3);
    return 0;
}

int CG_CheckPlayerTryReload(int buttons)
{
    return (buttons & 0x30)
        && !(cg->predictedPlayerState.pm_flags & 4)
        && !(cg->snap->ps.cursorHint & 2)
        && !(cg->predictedPlayerState.eFlags & 0x300);
}

int CG_CheckPlayerFireNonTurret(int buttons)
{
    return (buttons & 1) && !(cg->predictedPlayerState.eFlags & 0x300);
}

int CG_CheckPlayerWeaponUsage(int buttons)
{
    if (!CG_CheckPlayerFireNonTurret(buttons) && !CG_CheckPlayerTryReload(buttons))
        return 0;
    CG_MenuShowNotify(1);
    return 1;
}

int CG_CheckPlayerOffHandUsage(int buttons)
{
    if (buttons & 0x30000)
    {
        CG_MenuShowNotify(4);
        return 1;
    }
    return 0;
}

int CG_CheckPlayerMiscInput(int buttons)
{
    return buttons & 0xffffd4ff;
}

void CG_CheckForPlayerInput(void)
{
    usercmd_t oldCmd;
    usercmd_t newCmd;
    int newCmdIndex;
    int changedButtons;
    int newInput;

    newCmdIndex = CL_GetCurrentCmdNumber();
    if (newCmdIndex > 1)
    {
        CL_GetUserCmd(newCmdIndex - 1, &oldCmd);
        CL_GetUserCmd(newCmdIndex, &newCmd);
        changedButtons = oldCmd.buttons ^ newCmd.buttons;
        newInput = CG_CheckPlayerMovement(oldCmd, newCmd);
        if (CG_CheckPlayerWeaponUsage(newCmd.buttons))
            newInput = 1;
        if (CG_CheckPlayerOffHandUsage(newCmd.buttons))
            newInput = 1;
        if (CG_CheckPlayerStanceChange(newCmd.buttons, changedButtons))
            newInput = 1;
        if (newInput || CG_CheckPlayerMiscInput(changedButtons))
            CG_MenuShowNotify(2);
    }
}

void CG_CheckHudHealthDisplay(void)
{
    menuDef_t *menu;

    if (CG_CalcPlayerHealth() < hud_health_startpulse_injured->current.value)
    {
        CG_MenuShowNotify(0);
    }
    else
    {
        if (hud_fade_healthbar->current.value != 0.0f
            && cg->healthFadeTime
            && (float)(cg->time - cg->healthFadeTime) > hud_fade_healthbar->current.value * 1000.0f)
        {
            menu = Menus_FindByName(&cgDC, "Health");
            if (menu)
                Window_RemoveDynamicFlags((Window *)menu, 4);
            cg->healthFadeTime = 0;
        }
    }
}

void CG_CheckHudAmmoDisplay(void)
{
    menuDef_t *menu;

    if (CG_CheckPlayerForLowAmmo() || CG_CheckPlayerForLowClip())
        CG_MenuShowNotify(1);
    if (hud_fade_ammodisplay->current.value != 0.0f
        && cg->ammoFadeTime
        && (float)(cg->time - cg->ammoFadeTime) > hud_fade_ammodisplay->current.value * 1000.0f)
    {
        menu = Menus_FindByName(&cgDC, "weaponinfo");
        if (menu)
            Window_RemoveDynamicFlags((Window *)menu, 4);
        cg->ammoFadeTime = 0;
    }
}

void CG_CheckHudCompassDisplay(void)
{
    menuDef_t *menu;

    if (hud_fade_compass->current.value != 0.0f
        && cg->compassFadeTime
        && (float)(cg->time - cg->compassFadeTime) > hud_fade_compass->current.value * 1000.0f)
    {
        menu = Menus_FindByName(&cgDC, "Compass");
        if (menu)
            Window_RemoveDynamicFlags((Window *)menu, 4);
        cg->compassFadeTime = 0;
    }
}

void CG_CheckHudStanceDisplay(void)
{
    menuDef_t *menu;

    if ((cg->nextSnap->ps.eFlags & 8) && (cg->nextSnap->ps.eFlags & 0x100)
        || (cg->nextSnap->ps.eFlags & 4) && (cg->nextSnap->ps.eFlags & 0x200))
    {
        CG_MenuShowNotify(3);
    }
    if (hud_fade_stance->current.value != 0.0f
        && cg->stanceFadeTime
        && (float)(cg->time - cg->stanceFadeTime) > hud_fade_stance->current.value * 1000.0f)
    {
        menu = Menus_FindByName(&cgDC, "stance");
        if (menu)
            Window_RemoveDynamicFlags((Window *)menu, 4);
        cg->stanceFadeTime = 0;
    }
}

void CG_CheckHudOffHandDisplay(void)
{
    menuDef_t *menu;

    if (hud_fade_offhand->current.value != 0.0f
        && cg->offhandFadeTime
        && (float)(cg->time - cg->offhandFadeTime) > hud_fade_offhand->current.value * 1000.0f)
    {
        menu = Menus_FindByName(&cgDC, "offhandinfo");
        if (menu)
            Window_RemoveDynamicFlags((Window *)menu, 4);
        cg->offhandFadeTime = 0;
    }
}

void CG_CheckHudObjectiveDisplay(void)
{
    menuDef_t *menu;

    if (CG_ScoreboardDisplayed())
    {
        if (cg->time - cg->scoreFadeTime > 100)
        {
            menu = Menus_FindByName(&cgDC, "objectiveinfo");
            if (menu)
                Window_RemoveDynamicFlags((Window *)menu, 4);
        }
    }
}

void CG_CheckTimedMenus(void)
{
    if (cg->voiceTime && cg->time - cg->voiceTime > 2500)
    {
        Menus_CloseByName(&cgDC, "voiceMenu");
        cg->voiceTime = 0;
    }
    CG_CheckForPlayerInput();
    CG_CheckHudHealthDisplay();
    CG_CheckHudAmmoDisplay();
    CG_CheckHudCompassDisplay();
    CG_CheckHudStanceDisplay();
    CG_CheckHudOffHandDisplay();
    CG_CheckHudObjectiveDisplay();
}

static void CG_DrawSoundOverlay(void)
{
    snd_overlay_info_t info[64];
    const char *provider;
    const char *line;
    int cpu;
    int count;
    int bits;
    int khz;
    int channels;
    int i;
    float y;

    count = SND_GetSoundOverlay(
        (snd_overlay_type_t)cg_drawSoundOverlay->current.integer,
        info,
        (int)(sizeof(info) / sizeof(info[0])),
        &cpu);
    if (count <= 0)
        return;

    provider = Dvar_GetString("mss_3d_provider");
    bits = Dvar_GetInt("snd_bits");
    khz = Dvar_GetInt("snd_khz");
    channels = Dvar_GetBool("snd_stereo") ? 2 : 1;

    CG_DrawStringExt(
        2.0f,
        82.0f,
        va("CPU: ^3%%%i ^73D provider: ^3%s ^7bits: ^3%i ^7kHz: ^3%i ^7chan: ^3%i", cpu, provider, bits, khz, channels),
        colorWhite,
        0,
        1,
        10.0f,
        0);

    y = 92.0f;
    for (i = 0; i < count; ++i) {
        if (!info[i].pszSampleName) {
            line = va("%2i", i);
        } else {
            line = va(
                "%2i %-50s vol^3%04.2f ^7rvol^3%04.2f ^7dist^3%5i ^7pit^3%04.2f",
                i,
                info[i].pszSampleName,
                info[i].fBaseVolume,
                info[i].fCurVolume,
                info[i].dist,
                info[i].fPitch);
        }

        CG_DrawStringExt(2.0f, y, line, colorWhite, 0, 1, 10.0f, 0);
        y += 10.0f;
    }
}

void CG_DrawMaterial(void)
{
    char szName[64];
    char szSurfaceFlags[0x1000];
    char szContents[0x1000];
    float y;

    if (!CL_PickMaterial((const vec_t *)cg->refdef.vieworg, (const vec_t *)cg->refdef.viewaxis, szName, szSurfaceFlags, szContents, 0x1000)) {
        return;
    }

    y = 240.0f;
    y += (float)CG_DrawSmallDevStringColor(8.0f, y, szName, colorWhite, 5);
    y += (float)CG_DrawSmallDevStringColor(8.0f, y, szSurfaceFlags, colorWhite, 5);
    CG_DrawSmallDevStringColor(8.0f, y, szContents, colorWhite, 5);
}

void CG_ShakeCamera(void)
{
    cameraShake_t *shake;
    float sx;
    float bestSize;
    float bestRumble;
    float angle;
    int i;

    sx = (float)cg->time / 600.0f;
    bestRumble = 0.0f;
    bestSize = 0.0f;

    for (i = 0; i < 4; ++i) {
        shake = &cg->cameraShake[i];
        if (!CG_UpdateCameraShakeStrength(shake)) {
            continue;
        }

        if (shake->size > bestSize) {
            bestSize = shake->size;
            bestRumble = shake->rumbleScale;
        }
    }

    if (cg->rumbleScale > bestSize) {
        bestSize = cg->rumbleScale;
        bestRumble = cg->rumbleScale;
    }

    if (bestSize <= 0.0f) {
        cg->cameraShakePhase = crandom() * 3.14159265358979323846f;
        return;
    }

    if (bestSize > 1.0f) {
        bestSize = 1.0f;
    }
    angle = (float)sin(sx * 25.132741228718345f + cg->cameraShakePhase) * bestSize * bestRumble * 18.0f;
    cg->refdefViewAngles[0] += angle;
    angle = (float)sin(sx * 47.12388980384689f + cg->cameraShakePhase) * bestSize * bestRumble * 16.0f;
    cg->refdefViewAngles[1] += angle;
    angle = (float)sin(sx * 37.69911184307752f + cg->cameraShakePhase) * bestSize * bestRumble * 10.0f;
    cg->refdefViewAngles[2] += angle;
}

qboolean CG_DrawFollow(void)
{
    const char *clientName;
    const char *followingString;
    FontHandle font;
    float scale;
    float x;
    int textWidth;

    if (!(cg->snap->ps.pm_flags & 0x400000)) {
        return 0;
    }

    if (cg->inKillCam) {
        return 0;
    }

    if (cg->bgs.clientinfo[cg->snap->ps.clientNum].infoValid)
        clientName = cg->bgs.clientinfo[cg->snap->ps.clientNum].name;
    else
        clientName = "?";

    followingString = SEH_LocalizeTextMessage("CGAME_FOLLOWING\x15", "spectator follow string", (msgLocErrType_t)0);
    scale = CL_IsRenderingSplitScreen() ? 0.5f : (1.0f / 3.0f);
    font = UI_GetFontHandle(0, 1.0f / 3.0f);

    textWidth = UI_TextWidth(followingString, 0, font, scale);
    x = (float)(-textWidth) * 0.5f;
    UI_DrawText(followingString, 0x7fffffff, font, x, 40.0f, 7, 1, scale, colorWhite, 3);

    textWidth = UI_TextWidth(clientName, 0, font, scale);
    x = (float)(-textWidth) * 0.5f;
    UI_DrawText(clientName, 0x7fffffff, font, x, 65.0f, 7, 1, scale, colorWhite, 3);

    return 1;
}

void CG_DrawTurretCrossHair(void)
{
    const dvar_t *drawTurretCrosshair;
    vec4_t color = { 1.0f, 1.0f, 1.0f, 0.75f };

    drawTurretCrosshair = cg_drawTurretCrosshair;
    if (drawTurretCrosshair && !drawTurretCrosshair->current.enabled)
        return;

    if (!cgs)
        return;

    UI_DrawHandlePic(316.0f, 236.0f, 8.0f, 8.0f, 0, 0, color, cgs->media.whiteMaterial);
}

static unsigned int CG_DrawSpectatorMessage(void)
{
    const char *text;
    FontHandle font;
    float scale;
    int textWidth;

    if (!cg || !cg->snap)
        return 0;

    if (cg->snap->ps.pm_type != 5)
        return 0;

    text = UI_SafeTranslateString("CGAME_SPECTATOR");
    scale = CL_IsRenderingSplitScreen() ? 0.5f : (1.0f / 3.0f);
    font = UI_GetFontHandle(0, scale);
    textWidth = UI_TextWidth(text, 0, font, scale);
    UI_DrawText(text, 0x7fffffff, font, (float)(-textWidth) * 0.5f, 40.0f, 7, 1, scale, colorWhite, 3);

    return 1;
}

void CG_DrawPlayerSprites(void)
{
    int entityIndex;
    centity_t *cent;

    for (entityIndex = 0; entityIndex < cg->nextSnap->numEntities; ++entityIndex) {
        cent = &cg_entities[cg->nextSnap->entities[entityIndex].number];
        if (cent->nextState.eType == 1) {
            CG_PlayerSprites(cent);
        }
    }
}

void CG_DrawActive(void)
{
    float sensitivity;
    float angles[3];
    vec_t *ka;
    vec_t *oa;

    cg->refdef.time = cg->time;
    cg->refdef.viewIndex = 0;

    sensitivity = cg->zoomSensitivity;
    if (cg->shellshock.sensitivity != 0.0f)
        sensitivity = cg->shellshock.sensitivity * cg->zoomSensitivity;

    ka = cg->kickAngles;
    oa = cg->offsetAngles;
    angles[0] = ka[0] + oa[0];
    angles[1] = ka[1] + oa[1];
    angles[2] = ka[2] + oa[2];

    CL_SetUserCmdAimValues(angles);

    CL_SetUserCmdValue(cg->weaponSelect, cg->equippedOffHand, sensitivity);

    CL_RenderScene(&cg->refdef);

    if (!CL_IsRenderingSplitScreen()) {
        /* Pass parms/startTime/duration with their real types. The decomp emitted
         * `*(float*)&field` reinterpretations here; on x86 they harmlessly launder
         * the same 4 bytes, but on x64 (Win64 ABI) a float arg is passed in an xmm
         * register while the pointer/int params are read from rcx/rdx/r8 -- so all
         * three args arrived as garbage (and the parms pointer was truncated to 32
         * bits), making the code see a bogus active shellshock and set
         * hasSavedScreen=1 every frame, which suppressed the viewmodel. */
        CG_DrawShellShockSavedScreenBlend(
            cg->shellshock.parms,
            cg->shellshock.startTime,
            cg->shellshock.duration);
    }

    CG_TileClear();
}

unsigned int CG_DrawChatMessages(void)
{
    FontHandle font;
    const char *msg;
    vec4_t color;
    float alphaPercent;
    float backgroundAlpha;
    float y;
    float timeRemaining;
    int chatHeight;
    int hudChatX;
    int hudChatY;
    int chatTime;
    int index;
    int i;
    int textWidth;

    chatHeight = cg_chatHeight->current.integer;
    if (!chatHeight) {
        return 0;
    }

    hudChatX = (int)cg_hudChatPosition->current.vector[0];
    hudChatY = (int)cg_hudChatPosition->current.vector[1];
    if (cgs->teamLastChatPos == cgs->teamChatPos) {
        return 0;
    }

    index = cgs->teamLastChatPos % chatHeight;
    chatTime = cg_chatTime->current.integer;
    if (cg->time - cgs->teamChatMsgTimes[index] > chatTime) {
        ++cgs->teamLastChatPos;
    }

    font = UI_GetFontHandle(0, 1.0f / 3.0f);
    for (i = cgs->teamChatPos - 1; i >= cgs->teamLastChatPos; --i) {
        index = i % chatHeight;
        timeRemaining = (float)chatTime - (float)(cg->time - cgs->teamChatMsgTimes[index]);
        if (timeRemaining > 200.0f) {
            alphaPercent = 1.0f;
            backgroundAlpha = 0.60000002f;
        } else {
            alphaPercent = timeRemaining / 200.0f;
            if (alphaPercent <= 0.0f) {
                continue;
            }

            backgroundAlpha = alphaPercent * 0.60000002f;
        }

        msg = cgs->teamChatMsgs[index];
        if (msg[0] == '^' && msg[1] && msg[1] != '^' && msg[1] >= '0' && msg[1] <= '9') {
            CL_LookupColor((unsigned char)msg[1], color);
        } else {
            color[0] = 1.0f;
            color[1] = 1.0f;
            color[2] = 1.0f;
        }

        color[0] *= 0.25f;
        color[1] *= 0.25f;
        color[2] *= 0.25f;
        color[3] = backgroundAlpha;

        y = (float)(hudChatY - (cgs->teamChatPos - i) * 10);
        textWidth = UI_TextWidth(msg, 0, font, 1.0f / 3.0f);
        UI_DrawHandlePic(0.0f, y, (float)(textWidth + 24), 10.0f, 1, 1, color, cgs->media.whiteMaterial);

        color[0] = 1.0f;
        color[1] = 1.0f;
        color[2] = 1.0f;
        color[3] = alphaPercent;
        UI_DrawText(msg, 0x7fffffff, font, (float)hudChatX, y + 9.0f, 1, 1, 1.0f / 3.0f, color, 3);
    }

    return 0;
}

static void __attribute_regparm__(2) CG_CalcCrosshairPosition(float *x, float *y)
{
    vec3_t gunAng;
    vec3_t gunDir;
    float forward;
    float horiz;
    float vert;

    gunAng[2] = cg->refdefViewAngles[2];
    gunAng[1] = cg->gunYaw;
    gunAng[0] = cg->gunPitch;
    AngleVectors(gunAng, gunDir, NULL, NULL);

    {
        const vec_t *axis = cg->refdef.viewaxis[0];
        forward = gunDir[0] * axis[0] + gunDir[1] * axis[1] + gunDir[2] * axis[2];
    }
    if (forward <= 0.0f || cg->refdef.fov_x <= 0.0f || cg->refdef.fov_y <= 0.0f) {
        *x = 0.0f;
        *y = 0.0f;
        return;
    }

    {
        const vec_t *axis = cg->refdef.viewaxis[1];
        horiz = gunDir[0] * axis[0] + gunDir[1] * axis[1] + gunDir[2] * axis[2];
    }
    *x = horiz / (forward * (float)tan((double)cg->refdef.fov_x * 0.008726646259971648)) * -320.0f;

    {
        const vec_t *axis = cg->refdef.viewaxis[2];
        vert = gunDir[0] * axis[0] + gunDir[1] * axis[1] + gunDir[2] * axis[2];
    }
    *y = vert / (forward * (float)tan((double)cg->refdef.fov_y * 0.008726646259971648)) * -240.0f;
}

#ifndef __EMSCRIPTEN__
static inline int CG_RoundFPS(float value)
{
#if defined(_MSC_VER) && defined(_M_IX86)
    int result;
    /* Break exact half-way ties upward before x87's round-to-nearest. */
    const double roundEpsilon = 1.0 / 1073741824.0;

    __asm fld value
    __asm fadd roundEpsilon
    __asm fistp result

    return result;
#else
    return I_fround(value);
#endif
}

extern int I_stricmp(const char *s1, const char *s2);

static float CG_DrawFPS(float y)
{
    const char *s;
    int total;
    int fpsMin;
    int fpsMax;
    int minTime = 0x7fffffff;
    int maxTime = 0;
    float variance;
    float average;
    const float *color;
    int i;

    if (fps_index < 32)
        return y;

    total = 0;

    for (i = 0; i < 32; ++i) {
        total += fps_previousTimes[i];

        if (minTime > fps_previousTimes[i])
            minTime = fps_previousTimes[i];

        if (maxTime < fps_previousTimes[i])
            maxTime = fps_previousTimes[i];
    }

    average = (float)total * (1.0f / 32.0f);

    variance = 0.0f;

    for (i = 0; i < 32; ++i)
        variance += fabsf((float)fps_previousTimes[i] - average);

    variance *= 1.0f / 32.0f;

    if (!total)
        total = 1;

    if (minTime <= 0)
        minTime = 1;

    fpsMin = CG_RoundFPS(1000.0f / (float)maxTime);
    fpsMax = CG_RoundFPS(1000.0f / (float)minTime);

    if (cg_drawFPS->current.integer > 2)
        s = va("%1.2fmspf(%i-%i)", (float)total * (1.0f / 32.0f), minTime, maxTime);
    else {
        int fpsAvg = CG_RoundFPS(32000.0f / (float)total);
        int fpsVar = CG_RoundFPS(variance);

        s = va("%ifps(%i-%i,%i)", fpsAvg, fpsMin, fpsMax, fpsVar);
    }

    y += (float)CG_DrawBigDevStringColor(620.0f, y, s, colorWhiteFaded, 6);

    if (cg_drawFPS->current.integer <= 1)
        return y;

    if (!I_stricmp(Dvar_GetString("gfx_driver"), "ogl")) {
        y += (float)CG_DrawSmallDevStringColor(620.0f, y, "use d3d for stats", colorWhiteFaded, 6);
        return y;
    }

    CL_TrackStatistics(&rendererStats);
    y += (float)CG_DrawSmallDevStringColor(620.0f, y, "scene", colorGreenFaded, 6);

    CG_DrawSmallDevStringColor(620.0f, y, va("view tris          "), colorWhiteFaded, 6);
    y += (float)CG_DrawSmallDevStringColor(620.0f, y, va("%i", rendererStats.c_viewIndexes / 3), colorWhiteFaded, 6);

    if (rendererStats.c_shadowIndexes) {
        CG_DrawSmallDevStringColor(620.0f, y, va("shadow tris          "), colorWhiteFaded, 6);
        y += (float)CG_DrawSmallDevStringColor(620.0f, y, va("%i", rendererStats.c_shadowIndexes / 3), colorWhiteFaded, 6);
    }

    CG_DrawSmallDevStringColor(620.0f, y, va("raw geo tris          "), colorWhiteFaded, 6);
    y += (float)CG_DrawSmallDevStringColor(620.0f, y, va("%i", rendererStats.c_indexes / 3), colorWhiteFaded, 6);

    CG_DrawSmallDevStringColor(620.0f, y, va("raw fx tris          "), colorWhiteFaded, 6);
    y += (float)CG_DrawSmallDevStringColor(620.0f, y, va("%i", rendererStats.c_fxIndexes / 3), colorWhiteFaded, 6);

    CG_DrawSmallDevStringColor(620.0f, y, va("prim          "), colorWhiteFaded, 6);
    y += (float)CG_DrawSmallDevStringColor(620.0f, y, va("%i", rendererStats.c_batches), colorWhiteFaded, 6);

    y += (float)CG_DrawSmallDevStringColor(620.0f, y, "level", colorGreenFaded, 6);

    CG_DrawSmallDevStringColor(620.0f, y, va("tex      "), colorWhiteFaded, 6);
    y += (float)CG_DrawSmallDevStringColor(
        620.0f, y, va("%d", rendererStats.c_imageUsage.total / (1024 * 1024)), colorWhiteFaded, 6);

    if (rendererStats.c_imageUsage.minspec > 48 * 1024 * 1024) {
        s = va("min pc tex (%g)      ", 48.0);
        color = colorRedFaded;
    } else {
        s = va("min pc tex      ");
        color = colorWhiteFaded;
    }

    CG_DrawSmallDevStringColor(620.0f, y, s, color, 6);
    y += (float)CG_DrawSmallDevStringColor(
        620.0f, y, va("%d", rendererStats.c_imageUsage.minspec / (1024 * 1024)), color, 6);

    CG_DrawSmallDevStringColor(620.0f, y, va("lightmap      "), colorWhiteFaded, 6);
    y += (float)CG_DrawSmallDevStringColor(
        620.0f, y, va("%d", rendererStats.c_imageUsage.lightmap / (1024 * 1024)), colorWhiteFaded, 6);

    return y;
}

float CG_DrawWeapReticle(void)
{
    playerState_t *ps = &cg->predictedPlayerState;
    int weapIndex = BG_GetViewmodelWeaponIndex(ps);
    WeaponDef *weapDef = (WeaponDef *)BG_GetWeaponDef(weapIndex);
    float zoom = ps->fWeaponPosFrac;
    float zoomFrac;
    float adsFrac;
    vec4_t color;
    vec2_t crosshairPos;
    weaponInfo_t *weaponInfo;
    MaterialHandle material;
    vec2_t drawPos;
    vec2_t drawSize;

    if ((!weapDef->szOverlayMaterial || !weapDef->szOverlayMaterial[0]) && !weapDef->overlayReticle)
        return 1.0f;

    if (zoom == 0.0f)
        return 1.0f;

    adsFrac = cg->playerEntity.bPositionToADS ? weapDef->fAdsZoomInFrac : weapDef->fAdsZoomOutFrac;
    zoomFrac = zoom - (1.0f - adsFrac);
    if (zoomFrac > 0.0f)
        zoomFrac /= adsFrac;

    if (zoomFrac <= 0.01f)
        return 1.0f;

    if (zoomFrac > 1.0f)
        zoomFrac = 1.0f;

    weapIndex = BG_GetViewmodelWeaponIndex(ps);
    weapDef = (WeaponDef *)BG_GetWeaponDef(weapIndex);
    color[0] = 1.0f;
    color[1] = 1.0f;
    color[2] = 1.0f;
    color[3] = zoomFrac;

    CG_CalcCrosshairPosition(&crosshairPos[0], &crosshairPos[1]);

    weaponInfo = &(cg_weaponsArray)[weapIndex];
    material = weaponInfo->hADSOverlay;
    if (material) {
        drawSize[0] = weapDef->overlayWidth;
        drawSize[1] = weapDef->overlayHeight;

        if (drawSize[0] > 320.0f || drawSize[1] > 240.0f) {
            drawPos[0] = crosshairPos[0] - drawSize[0] * 0.5f;
            drawPos[1] = crosshairPos[1] - drawSize[1] * 0.5f;
            CL_DrawStretchPic(drawPos[0], drawPos[1], drawSize[0], drawSize[1], 2, 2,
                              0.0f, 0.0f, 1.0f, 1.0f, color, material);

            CalcScreenPlacement(&drawPos[0], &drawPos[1], &drawSize[0], &drawSize[1], 2, 2);
            CG_DrawFrameOverlay(drawPos[0], drawPos[0] + drawSize[0],
                                drawPos[1], drawPos[1] + drawSize[1], color, material);
        } else {
            drawPos[0] = crosshairPos[0] - drawSize[0];
            drawPos[1] = crosshairPos[1] - drawSize[1];
            CL_DrawStretchPic(drawPos[0], drawPos[1], drawSize[0], drawSize[1], 2, 2,
                              0.0f, 0.0f, 1.0f, 1.0f, color, material);

            drawPos[0] = crosshairPos[0];
            drawPos[1] = crosshairPos[1] - drawSize[1];
            CL_DrawStretchPic(drawPos[0], drawPos[1], drawSize[0], drawSize[1], 2, 2,
                              1.0f, 0.0f, 0.0f, 1.0f, color, material);

            drawPos[0] = crosshairPos[0] - drawSize[0];
            drawPos[1] = crosshairPos[1];
            CL_DrawStretchPic(drawPos[0], drawPos[1], drawSize[0], drawSize[1], 2, 2,
                              0.0f, 1.0f, 1.0f, 0.0f, color, material);

            drawPos[0] = crosshairPos[0];
            drawPos[1] = crosshairPos[1];
            CL_DrawStretchPic(drawPos[0], drawPos[1], drawSize[0], drawSize[1], 2, 2,
                              1.0f, 1.0f, 0.0f, 0.0f, color, material);

            drawPos[0] = crosshairPos[0] - drawSize[0];
            drawPos[1] = crosshairPos[1] - drawSize[1];
            CalcScreenPlacement(&drawPos[0], &drawPos[1], &drawSize[0], &drawSize[1], 2, 2);
            CG_DrawFrameOverlay(drawPos[0], drawPos[0] + drawSize[0] * 2.0f,
                                drawPos[1], drawPos[1] + drawSize[1] * 2.0f, color, material);
        }
    }

    return 1.0f - zoomFrac;
}

unsigned int CG_DrawCrosshair(void)
{
    playerState_t *ps = &cg->predictedPlayerState;
    qboolean drawHudMenus;
    int weapIndex;
    WeaponDef *weapDef;
    weaponInfo_t *weapons;
    MaterialHandle material;
    vec4_t color;
    vec4_t reticleColor;
    float posLerp;
    float transScale;
    float centerX;
    float centerY;
    int pmFlags;
    int weaponstate;

    if (cg->renderingThirdPerson)
        return 0;

    drawHudMenus = !CG_AreHudMenusHidden();
    posLerp = ps->fWeaponPosFrac;

    if (ps->eFlags & 0x300) {
        if (drawHudMenus && ps->viewlocked_entNum != 0x3ff)
            CG_DrawTurretCrossHair();
        return 0;
    }

    weapIndex = BG_GetViewmodelWeaponIndex(ps);
    if (weapIndex == 0)
        return 0;

    weapDef = (WeaponDef *)BG_GetWeaponDef(weapIndex);
    CG_DrawWeapReticle();

    if (!drawHudMenus)
        return 0;

    pmFlags = ps->pm_flags;
    if (pmFlags & 0x100000) {
        color[0] = 0.25f;
        color[1] = 1.0f;
        color[2] = 0.25f;
    } else if (pmFlags & 0x200000) {
        if (!cg_crosshairEnemyColor->current.enabled) {
            color[0] = 1.0f;
            color[1] = 1.0f;
            color[2] = 1.0f;
        } else {
            color[0] = 1.0f;
            color[1] = 0.25f;
            color[2] = 0.25f;
        }
    } else {
        color[0] = 1.0f;
        color[1] = 1.0f;
        color[2] = 1.0f;
    }

    color[3] = posLerp * cg_crosshairAlpha->current.value;
    if (color[3] < 0.01f)
        return 0;

    if (posLerp == 1.0f && cg_drawGun->current.enabled)
        return 0;
    if (cg_paused->current.integer)
        return 0;
    if (!cg_drawCrosshair->current.enabled)
        return 0;

    weaponstate = ps->weaponstate;
    if ((unsigned int)(weaponstate - 10) <= 1 || weaponstate == 5 ||
        (unsigned int)(weaponstate - 1) <= 1)
        return 0;

    CG_CalcCrosshairPosition(&centerX, &centerY);

    if (posLerp == 0.0f) {
        transScale = 1.0f;
    } else {
        float adsFrac = cg->playerEntity.bPositionToADS ? weapDef->fAdsCrosshairInFrac : weapDef->fAdsCrosshairOutFrac;
        float zoomFrac = posLerp - (1.0f - adsFrac);
        float scaled;
        if (zoomFrac <= 0.0f) {
            transScale = 1.0f;
        } else {
            scaled = zoomFrac / adsFrac;
            if (scaled <= 0.0f)
                transScale = 1.0f;
            else
                transScale = scaled * -0.5f + 1.0f;
        }

        if (!cg_drawGun->current.enabled && transScale < 1.0f) {
            weapons = &(cg_weaponsArray)[weapIndex];
            material = weapons->hReticleCenter;
            if (material) {
                float size = (float)weapDef->iReticleCenterSize * (1.5f - transScale);
                float half = size * -0.5f;
                CL_DrawStretchPic(centerX + half, centerY + half, size, size, 2, 2,
                                  0.0f, 0.0f, 1.0f, 1.0f, color, material);
            }
        }
    }

    if (posLerp == 1.0f && !cg_drawGun->current.enabled)
        return 0;

    if (!cg_crosshairDynamic->current.enabled) {
        centerX = 0.0f;
        centerY = 0.0f;
    }

    weapons = &(cg_weaponsArray)[weapIndex];
    material = weapons->hReticleCenter;
    if (material) {
        float centerSize = (float)weapDef->iReticleCenterSize;
        float drawSize;
        float half;
        if (weapDef->weapType == WEAPTYPE_GRENADE) {
            int grenadeTime = ps->grenadeTimeLeft;
            if (grenadeTime != 0 && weapDef->bCookOffHold)
                centerSize += (float)(grenadeTime % 1000) / 100.0f;
        }
        drawSize = centerSize * transScale;
        if (CL_IsRenderingSplitScreen())
            drawSize += drawSize;
        half = drawSize * -0.5f;
        CL_DrawStretchPic(half + centerX, centerY + half, drawSize, drawSize, 2, 2,
                          0.0f, 0.0f, 1.0f, 1.0f, color, material);
    }

    weapons = &(cg_weaponsArray)[weapIndex];
    material = weapons->hReticleSide;
    if (!material)
        return 0;

    CG_DrawWeapReticle();
    reticleColor[0] = color[0];
    reticleColor[1] = color[1];
    reticleColor[2] = color[2];
    {
        float alpha = transScale * cg_crosshairAlpha->current.value *
                      (ps->aimSpreadScale / -255.0f + 1.0f);
        float alphaMin = (cg_crosshairAlphaMin)->current.value;
        reticleColor[3] = alpha >= alphaMin ? alpha : alphaMin;
    }

    {
        float sideSizeF = (float)weapDef->iReticleSideSize * transScale;
        float sideSize;
        float realW, realH;
        float spreadMin, spreadMax, spread;
        double spreadTan, fovTan;
        float minOfs;
        float divW, divH;
        float vw, vh;
        float ax, ay;

        if (CL_IsRenderingSplitScreen())
            sideSize = sideSizeF + sideSizeF;
        else
            sideSize = sideSizeF;

        BG_GetSpreadForWeapon(ps, weapIndex, &spreadMin, &spreadMax);
        spread = ((spreadMax - spreadMin) * (ps->aimSpreadScale / 255.0f) + spreadMin) * transScale;
        spreadTan = tan((double)spread * 0.017453292519943295);
        fovTan = tan((double)cg->refdef.fov_y * 0.017453292519943295 * 0.5);
        minOfs = spreadTan * 240.0 / fovTan;
        if ( minOfs < (float)weapDef->iReticleMinOfs )
            minOfs = (float)weapDef->iReticleMinOfs;
        minOfs -= sideSizeF * weapDef->fHipReticleSidePos;

        realW = GetRealWidthFromVirtualWidth(sideSize);
        realH = GetRealHeightFromVirtualHeight(sideSize);
        divW = (float)cgs->viewWidth;
        divH = (float)cgs->viewHeight;
        vw = GetVirtualWidthFromRealWidth(divW * (realW / divW * 0.125f));
        vh = GetVirtualHeightFromRealHeight(divH * (realH / divH * 0.125f));

        ax = AlignHudElemX(4, centerX, sideSize);
        ay = AlignHudElemY(2, centerY, sideSize);
        CG_DrawRotatedPic(ax, ay - minOfs - vh, sideSize, sideSize, 2, 2, 0.0f, reticleColor, material);

        ax = AlignHudElemX(0, centerX, sideSize);
        ay = AlignHudElemY(1, centerY, sideSize);
        CG_DrawRotatedPic(minOfs + ax, ay, sideSize, sideSize, 2, 2, 90.0f, reticleColor, material);

        ax = AlignHudElemX(4, centerX, sideSize);
        ay = AlignHudElemY(0, centerY, sideSize);
        CG_DrawRotatedPic(ax - vw, ay + minOfs, sideSize, sideSize, 2, 2, 180.0f, reticleColor, material);

        ax = AlignHudElemX(8, centerX, sideSize);
        ay = AlignHudElemY(1, centerY, sideSize);
        CG_DrawRotatedPic(ax - minOfs - vw, ay - vh, sideSize, sideSize, 2, 2, 270.0f, reticleColor, material);
    }

    return 0;
}

static void CG_ScreenFade(void)
{
    int msec;
    int i;
    float t;
    float invt;
    vec4_t color;

    if (cg->fadeRate == 0.0f)
        return;

    msec = cg->fadeTime - cg->time;
    if (msec <= 0)
    {
        cg->fadeColor1[0] = cg->fadeColor2[0];
        cg->fadeColor1[3] = cg->fadeColor2[3];
        cg->fadeColor1[1] = cg->fadeColor2[1];
        cg->fadeColor1[2] = cg->fadeColor2[2];

        if (cg->fadeColor2[3] == 0.0f)
        {
            cg->fadeRate = 0.0f;
            return;
        }

        UI_FillRectPhysical(0.0f, 0.0f, (float)cls.vidConfig.width, (float)cls.vidConfig.height, cg->fadeColor1);
    }
    else
    {
        t = (float)msec * cg->fadeRate;
        invt = 1.0f - t;

        for (i = 0; i < 4; i++)
            color[i] = cg->fadeColor2[i] * invt + cg->fadeColor1[i] * t;

        if (color[3] != 0.0f)
            UI_FillRectPhysical(0.0f, 0.0f, (float)cls.vidConfig.width, (float)cls.vidConfig.height, color);
    }
}

static void CG_DrawSpectator(void)
{
    const char *s;
    FontHandle font;
    float x;

    s = UI_SafeTranslateString("CGAME_SPECTATOR");
    font = UI_GetFontHandle(4, 0.5f);
    x = (float)-UI_TextWidth(s, 0, font, 0.5f) * 0.5f;
    UI_DrawText(s, 0x7fffffff, font, x, -27.0f, 7, 3, 0.5f, colorWhite, 3);
}

static void CG_DrawIntermission(void)
{
    CL_CloseAllMenus();
    cg->showScores = 1;
    cg->scoreFadeTime = cg->time;
    CG_DrawScoreboard();
}

static void CG_DrawFlashDamage(void)
{
    float redFlash;
    vec4_t col;

    if (!cg->nextSnap)
        return;

    if (cg->v_dmg_time > cg->time)
    {
        redFlash = fabsf((float)(cg->v_dmg_time - cg->time) * cg->v_dmg_pitch * 0.002f);
        if (redFlash > 5.0f)
            redFlash = 5.0f;

        col[0] = 0.2f;
        col[1] = 0.0f;
        col[2] = 0.0f;
        col[3] = 0.7f * (redFlash / 5.0f);

        UI_FillRectPhysical(-10.0f, -10.0f, (float)cls.vidConfig.width + 10.0f, (float)cls.vidConfig.height + 10.0f, col);
    }
}

static void CG_DrawDamageDirectionIndicators(void)
{
    float centerY;
    float centerX;
    float zoom;
    float halfWidth;
    float height;
    float radius;
    float xy[4][2];
    float color[4];
    int slot;
    int maxTime;
    float angle;
    int t;
    float alpha;

    if (!cg->nextSnap)
        return;

    if (CG_GetWeapReticleZoom(&zoom))
    {
        if (!cg_hudDamageIconInScope->current.enabled)
            return;
        CG_CalcCrosshairPosition(&centerX, &centerY);
        centerX = centerX + 320.0f;
        centerY = centerY + 240.0f;
    }
    else
    {
        centerX = 320.0f;
        centerY = 240.0f;
    }

    halfWidth = cg_hudDamageIconWidth->current.value * 0.5f;
    height = cg_hudDamageIconHeight->current.value;
    radius = cg_hudDamageIconOffset->current.value;
    xy[0][0] = halfWidth;
    xy[0][1] = height + radius;
    xy[1][0] = -halfWidth;
    xy[1][1] = xy[0][1];
    xy[2][0] = xy[1][0];
    xy[2][1] = radius;
    xy[3][0] = halfWidth;
    xy[3][1] = radius;
    color[0] = 1.0f;
    color[1] = 1.0f;
    color[2] = 1.0f;
    color[3] = 1.0f;
    CalcScreenX(&centerX, 0);
    CalcScreenY(&centerY, 0);

    for (slot = 0; slot < 8; slot++)
    {
        maxTime = cg->viewDamage[slot].duration;
        t = cg->time - cg->viewDamage[slot].time;
        if (t > 0 && t < maxTime)
        {
            angle = vectoyaw(cg->refdef.viewaxis[0]) - cg->viewDamage[slot].yaw;
            alpha = 2.0f - ((float)t + (float)t) / maxTime;
            color[3] = alpha - 1.0f >= 0.0f ? 1.0f : alpha;
            CG_DrawRotatedQuadPic(centerX, centerY, xy, angle, color, cgs->media.damageMaterial);
        }
    }
}

// Original name unknown.
static bool CG_ShouldDrawGrenadeIndicator(const entityState_t *es)
{
    vec3_t offset;
    float maxRange;

    if (es->eType != 4)
        return false;
    if (es->pos.trDelta[0] * es->pos.trDelta[0] + es->pos.trDelta[1] * es->pos.trDelta[1] + es->pos.trDelta[2] * es->pos.trDelta[2] > 1.0f)
        return false;
    if (BG_GetWeaponDef(es->weapon)->projExplosion)
        return false;

    offset[0] = es->pos.trBase[0] - cg->predictedPlayerState.origin[0];
    offset[1] = es->pos.trBase[1] - cg->predictedPlayerState.origin[1];
    offset[2] = es->pos.trBase[2] - cg->predictedPlayerState.origin[2];
    if (offset[2] < 0.0f)
    {
        if (offset[2] < -cg_hudGrenadeIconMaxHeight->current.value)
            return false;
    }
    else if (offset[2] > cg->predictedPlayerState.viewHeightCurrent + cg_hudGrenadeIconMaxHeight->current.value)
    {
        return false;
    }

    maxRange = cg_hudGrenadeIconMaxRange->current.value;
    if (offset[2] * offset[2] + (offset[0] * offset[0] + offset[1] * offset[1]) > maxRange * maxRange)
        return false;
    return true;
}

static void CG_DrawGrenadePointer(float centerX, float centerY, const float *grenadeOffset, const float *color)
{
    float width;
    float height;
    float radius;
    float pivot[2];
    float yaw;
    float sinYaw;
    float cosYaw;
    float pointerPos[2];
    vec2_t verts[4];

    width = GetRealWidthFromVirtualWidth(cg_hudGrenadePointerWidth->current.value);
    height = GetRealHeightFromVirtualHeight(cg_hudGrenadePointerHeight->current.value);
    radius = GetRealHeightFromVirtualHeight(cg_hudGrenadeIconOffset->current.value);
    pivot[0] = GetRealWidthFromVirtualWidth(cg_hudGrenadePointerPivot->current.vector[0]);
    pivot[1] = GetRealHeightFromVirtualHeight(cg_hudGrenadePointerPivot->current.vector[1]);

    yaw = AngleNormalize360(vectoyaw(grenadeOffset) - cg->predictedPlayerState.viewangles[1]);
    sinYaw = sinf(yaw * 0.017453292f);
    cosYaw = cosf(yaw * 0.017453292f);

    pointerPos[0] = centerX - sinYaw * radius;
    pointerPos[1] = centerY - cosYaw * radius;

    verts[0][0] = -pivot[0];
    verts[0][1] = -pivot[1];
    verts[1][0] = width - pivot[0];
    verts[1][1] = -pivot[1];
    verts[2][0] = width - pivot[0];
    verts[2][1] = height - pivot[1];
    verts[3][0] = -pivot[0];
    verts[3][1] = height - pivot[1];

    CG_DrawRotatedQuadPic(pointerPos[0], pointerPos[1], verts, -yaw, color, cgs->media.grenadePointer);
}

static void CG_DrawGrenadeIcon(float centerX, float centerY, const float *grenadeOffset, const float *color)
{
    float width;
    float height;
    float radius;
    float yaw;
    float sinYaw;
    float cosYaw;
    vec2_t verts[4];

    width = GetRealWidthFromVirtualWidth(cg_hudGrenadeIconWidth->current.value);
    height = GetRealHeightFromVirtualHeight(cg_hudGrenadeIconHeight->current.value);
    radius = GetRealHeightFromVirtualHeight(cg_hudGrenadeIconOffset->current.value);

    yaw = AngleNormalize360(vectoyaw(grenadeOffset) - cg->predictedPlayerState.viewangles[1]);
    sinYaw = sinf(yaw * 0.017453292f);
    cosYaw = cosf(yaw * 0.017453292f);

    verts[0][0] = centerX - width * 0.5f - sinYaw * radius;
    verts[1][0] = centerX + width * 0.5f - sinYaw * radius;
    verts[2][0] = verts[1][0];
    verts[3][0] = verts[0][0];
    verts[0][1] = centerY - height * 0.5f - cosYaw * radius;
    verts[1][1] = verts[0][1];
    verts[2][1] = centerY + height * 0.5f - cosYaw * radius;
    verts[3][1] = verts[2][1];

    CL_DrawQuadPic(verts, color, cgs->media.grenadeIcon);
}

static void CG_DrawGrenadeIndicators(void)
{
    float centerX;
    float centerY;
    float zoom;
    int i;
    const entityState_t *es;
    vec3_t grenadeOffset;
    vec4_t color;
    float amplitude;
    float clamped;

    if (!cg->nextSnap)
        return;
    if (cg->predictedPlayerState.pm_type == 4)
        return;
    if (cg->predictedPlayerState.pm_flags & 0x400000)
        return;

    if (CG_GetWeapReticleZoom(&zoom))
    {
        if (!cg_hudGrenadeIconInScope->current.enabled)
            return;
        CG_CalcCrosshairPosition(&centerX, &centerY);
    }
    else
    {
        centerX = 0.0f;
        centerY = 0.0f;
    }
    CalcScreenX(&centerX, 2);
    CalcScreenY(&centerY, 2);

    for (i = 0; i < cg->nextSnap->numEntities; i++)
    {
        es = &cg->nextSnap->entities[i];
        if (!CG_ShouldDrawGrenadeIndicator(es))
            continue;

        grenadeOffset[0] = es->pos.trBase[0] - cg->predictedPlayerState.origin[0];
        grenadeOffset[1] = es->pos.trBase[1] - cg->predictedPlayerState.origin[1];
        grenadeOffset[2] = es->pos.trBase[2] - cg->predictedPlayerState.origin[2];

        color[0] = 1.0f;
        color[1] = 1.0f;
        color[2] = 1.0f;
        color[3] = 1.0f;

        amplitude = (cg_hudGrenadePointerPulseMax->current.value - cg_hudGrenadePointerPulseMin->current.value) * 0.5f;
        color[3] = (float)sin((float)cg->time * cg_hudGrenadePointerPulseFreq->current.value * 0.0062831853f) * amplitude
            + (cg_hudGrenadePointerPulseMin->current.value + amplitude);
        clamped = color[3] - 1.0f >= 0.0f ? 1.0f : color[3];
        color[3] = 0.0f - color[3] >= 0.0f ? 0.0f : clamped;

        CG_DrawGrenadePointer(centerX, centerY, grenadeOffset, color);
        CG_DrawGrenadeIcon(centerX, centerY, grenadeOffset, color);
    }
}

static void CG_DrawVote(void)
{
    char szVoteYes[256];
    char szVoteNo[256];
    char binding[256];
    FontHandle font;
    int sec;
    const char *s;

    if (!cgs->voteTime)
        return;

    font = UI_GetFontHandle(0, 0.5f);

    if (GetKeyBindingLocalizedString("vote yes", binding))
        I_strncpyz(szVoteYes, binding, 256);
    else
        I_strncpyz(szVoteYes, "vote yes", 256);

    if (GetKeyBindingLocalizedString("vote no", binding))
        I_strncpyz(szVoteNo, binding, 256);
    else
        I_strncpyz(szVoteNo, "vote no", 256);

    sec = (cgs->voteTime - cg->time) / 1000;
    if (sec < 0)
        sec = 0;

    if (cg->nextSnap->ps.eFlags & 0x100000)
    {
        s = va("%s(%i):%s", UI_SafeTranslateString("CGAME_VOTE"), sec, cgs->voteString);
        UI_DrawText(s, 0x7fffffff, font, 8.0f, 200.0f, 0, 0, 0.5f, colorYellow, 3);
        s = va("%s:%i, %s:%i", UI_SafeTranslateString("CGAME_YES"), cgs->voteYes,
               UI_SafeTranslateString("CGAME_NO"), cgs->voteNo);
    }
    else
    {
        s = va("%s(%i):%s", UI_SafeTranslateString("CGAME_VOTE"), sec, cgs->voteString);
        UI_DrawText(s, 0x7fffffff, font, 8.0f, 200.0f, 0, 0, 0.5f, colorYellow, 3);
        s = va("%s(%s):%i, %s(%s):%i", UI_SafeTranslateString("CGAME_YES"), szVoteYes, cgs->voteYes,
               UI_SafeTranslateString("CGAME_NO"), szVoteNo, cgs->voteNo);
    }
    UI_DrawText(s, 0x7fffffff, font, 8.0f, 210.0f, 0, 0, 0.5f, colorYellow, 3);
}

static void CG_DrawLagometer(void)
{
    int a;
    int i;
    float v;
    float ax;
    float ay;
    float aw;
    float ah;
    float mid;
    float range;
    float vscale;
    const float *color;

    if (cg_drawLagometer->current.enabled && !cgs->localServer)
    {
        UI_DrawHandlePic(-55.0f, -140.0f, 48.0f, 48.0f, 3, 3, 0, cgs->media.lagometerMaterial);

        ax = -55.0f;
        ay = -140.0f;
        aw = 48.0f;
        ah = 48.0f;

        range = ah / 3.0f;
        mid = ay + range;

        // draw the frame interpolate / extrapolate graph
        for (a = 0; a < aw; a++)
        {
            i = (lagometer.frameCount - a - 1) & 127;
            v = (float)lagometer.frameSamples[i];
            vscale = range / 300.0f;
            v *= vscale;
            if (v > 0.0f)
            {
                if (v > range)
                    v = range;
                CL_DrawStretchPic(ax + aw - a, mid - v, 1.0f, v, 3, 3, 0.0f, 0.0f, 0.0f, 0.0f, colorYellow, cgs->media.whiteMaterial);
            }
            else if (v < 0.0f)
            {
                v = -v;
                if (v > range)
                    v = range;
                CL_DrawStretchPic(ax + aw - a, mid, 1.0f, v, 3, 3, 0.0f, 0.0f, 0.0f, 0.0f, colorBlue, cgs->media.whiteMaterial);
            }
        }

        // draw the snapshot latency / drop graph
        range = ah / 2.0f;

        for (a = 0; a < aw; a++)
        {
            i = (lagometer.snapshotCount - a - 1) & 127;
            v = (float)lagometer.snapshotSamples[i];
            if (v > 0.0f)
            {
                if (lagometer.snapshotFlags[i] & 1)
                    color = colorYellow;
                else
                    color = colorGreen;
                vscale = range / 900.0f;
                v = v * vscale;
                if (v > range)
                    v = range;
                CL_DrawStretchPic(ax + aw - a, ay + ah - v, 1.0f, v, 3, 3, 0.0f, 0.0f, 0.0f, 0.0f, color, cgs->media.whiteMaterial);
            }
            else if (v < 0.0f)
            {
                CL_DrawStretchPic(ax + aw - a, ay + ah - range, 1.0f, range, 3, 3, 0.0f, 0.0f, 0.0f, 0.0f, colorRed, cgs->media.whiteMaterial);
            }
        }

        if (cg_nopredict->current.enabled || cg_synchronousClients->current.enabled)
            CG_DrawBigDevString(ax, ay, "snc", 1.0f, 9);
    }

    CG_DrawDisconnect();
}

static void CG_DrawPerformanceWarnings(void)
{
    int now;
    const void *statsArray;
    int statsCount;
    int i;
    float x;
    float y;

    now = Sys_Milliseconds();
    StatMon_GetStatsArray((const statmonitor_t **)&statsArray, &statsCount);

    x = 2.0f;
    y = 200.0f;
    for (i = 0; i < statsCount; i++)
    {
        const int *entry = (const int *)((const char *)statsArray + i * 8);
        if (entry[0] >= now)
            UI_DrawHandlePic(x, y, 32.0f, 32.0f, 1, 0, 0, (MaterialHandle)entry[1]);
        x += 34.0f;
        if (x + 32.0f > 68.0f)
        {
            x = 2.0f;
            y += 34.0f;
        }
    }
}

static void CG_DrawDebugOverlays(void)
{
    if (cg_drawSoundOverlay->current.integer)
        CG_DrawSoundOverlay();
    else if (cg_drawScriptUsage->current.enabled)
        CG_DrawScriptUsage();
    else if (cg_drawMaterial->current.enabled)
        CG_DrawMaterial();
}

static float CG_DrawSnapshot(float y)
{
    return y + (float)CG_DrawBigDevStringColor(620.0f, y,
        va("time:%i snap:%i cmd:%i", cg->nextSnap->serverTime, cg->latestSnapshotNum, cgs->serverCommandSequence),
        colorWhite, 6);
}

static void CG_DrawUpperRight(void)
{
    float y;

    CL_TrackStatistics(0);
    y = 0.0f;
    if (cg_drawSnapshot->current.enabled)
        y = CG_DrawSnapshot(y);
    if (cg_drawFPS->current.integer)
        y = CG_DrawFPS(y);
}

static void CG_DrawCenterString(void)
{
    char *start;
    int l;
    int h;
    float x;
    float lineHeight;
    float y;
    float scale;
    float *color;
    FontHandle font;
    char linebuffer[1024];

    if (!cg->centerPrintTime)
        return;

    color = CG_FadeColor(cg->centerPrintTime, (int)(cg_centertime->current.value * 1000.0f), 100);
    if (!color)
    {
        cg->centerPrintTime = 0;
        cg->centerPrintPriority = 0;
        return;
    }

    start = cg->centerPrint;

    scale = cg->centerPrintCharWidth / 32.0f;
    h = UI_TextHeight(UI_GetFontHandle(0, scale), scale);

    lineHeight = h * 1.2f;
    y = cg_centerPrintY->current.value - cg->centerPrintLines * lineHeight;

    while (1)
    {
        for (l = 0; l < 75; l++)
        {
            if (!start[l] || start[l] == '\n')
                break;
            linebuffer[l] = start[l];
        }
        linebuffer[l] = 0;

        font = UI_GetFontHandle(0, scale);
        x = UI_TextWidth(linebuffer, 0, font, scale) * -0.5;
        UI_DrawText(linebuffer, 0x7fffffff, font, x, y, 7, 3, scale, color, 3);

        y += lineHeight;

        while (*start && *start != '\n')
            start++;
        if (!*start)
            break;
        start++;
    }
}

static void CG_DrawGameMessages(void)
{
    int y;
    float alpha;
    float *color;

    y = I_fround(cgs->compassY - (cg_hudCompassSize->current.value - 1.0f) * cgs->compassHeight + 12.0f);

    if (cg->showScores)
    {
        color = CG_FadeColor(cg->scoreFadeTime, 100, 100);
        if (!color)
            return;
        alpha = color[3];
    }
    else
    {
        alpha = 1.0f;
    }

    Con_DrawNotify(6, y, alpha, MWM_BOTTOMUP);
}

void CG_DrawBoldGameMessages(void)
{
    float x;
    float y;
    float alpha;
    float *color;

    x = 0.0f;
    y = -60.0f;

    if (cg->showScores)
    {
        color = CG_FadeColor(cg->scoreFadeTime, 100, 100);
        if (!color)
            return;
        alpha = color[3];
    }
    else
    {
        alpha = 1.0f;
    }

    Con_DrawBoldMessages(I_fround(x), I_fround(y), alpha, MWM_BOTTOMUP_CENTERED);
}

static void CG_DrawMiniConsole(void)
{
    if (cg_minicon->current.enabled)
        Con_DrawMiniConsole(2, 4, 1.0f);
}

static void CG_DrawSubtitles(void)
{
    if (cg_subtitles->current.enabled)
        Con_DrawSubtitles(cg_subtitlePosX->current.integer, cg_subtitlePosY->current.integer,
                          cg_subtitleCharHeight->current.integer, 1.0f, MWM_BOTTOMUP);
}

static void CG_DrawSay(void)
{
    Con_DrawSay((int)cg_hudSayPosition->current.vector[1] + 24);
}

unsigned int CG_Draw2D(void)
{
    bool drawHudMenus;
    bool drawHudElems;
    qboolean drewScoreboard;

    if (cg->cubemapShot)
        return 0;

    if (!cg_draw2D->current.enabled)
        return 0;

    CG_ScreenFade();

    drawHudMenus = !CG_AreHudMenusHidden();
    drawHudElems = !CG_AreHudElemsHidden();

    if (cg->nextSnap->ps.pm_type == 5)
    {
        CG_DrawIntermission();
        return 0;
    }

    if (cg->nextSnap->ps.pm_type == 4)
    {
        if (drawHudMenus)
        {
            CG_DrawSpectator();
            CG_DrawCrosshairNames();
            CG_DrawChatMessages();
        }
        if (drawHudElems)
        {
            CG_Draw2dHudElems(0);
            CG_Draw2dHudElems(1);
        }
    }
    else
    {
        if (cg->nextSnap->ps.pm_type < 6)
            CG_DrawCrosshair();
        if (drawHudMenus)
        {
            if (cg->nextSnap->ps.pm_type < 6)
                CG_DrawCrosshairNames();
            CG_DrawChatMessages();
        }
        CG_CheckTimedMenus();
        if (drawHudElems)
            CG_Draw2dHudElems(0);
        if (drawHudMenus)
            Menu_PaintAll(&cgDC);
        CG_Draw2dHudElems(1);
    }

    drewScoreboard = CG_DrawScoreboard();
    if (!drawHudMenus)
        return 0;

    CG_DrawFlashDamage();
    CG_DrawDamageDirectionIndicators();
    CG_DrawGrenadeIndicators();
    if (!drewScoreboard)
    {
        CG_DrawSpectatorMessage();
        CG_DrawFollow();
    }
    CG_DrawVote();
    CG_DrawLagometer();
    CG_DrawPerformanceWarnings();
    CG_DrawDebugOverlays();
    CG_DrawUpperRight();
    if (!drewScoreboard)
    {
        CG_DrawCenterString();
        if (cg_drawGameMessages->current.enabled)
            CG_DrawGameMessages();
        CG_DrawBoldGameMessages();
        CG_DrawMiniConsole();
        CG_DrawSubtitles();
        CG_DrawSay();
    }
    return 0;
}

#else
static float CG_DrawFPS(float y)
{
    return 0.0f;
}
#endif
