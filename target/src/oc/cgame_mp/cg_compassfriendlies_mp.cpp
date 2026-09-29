#include "common_types.h"
#include "imports.h"
#include "headers/PC/cgame_mp/cg_local.h"

#include <math.h>

extern const dvar_t *cg_hudCompassSize;
extern const dvar_t *cg_hudCompassMinRange;
extern const dvar_t *cg_hudCompassMinRadius;
extern const dvar_t *cg_hudCompassMaxRange;
extern const dvar_t *cg_hudObjectiveMaxRange;
extern const dvar_t *cg_hudObjectiveMinAlpha;
extern const dvar_t *cg_hudCompassSoundPingFadeTime;
extern const dvar_t *hud_fade_compass;

extern float CG_FadeHudMenu(const dvar_t *fadeDvar, int displayStartTime, int duration);
extern void CG_UpdateCompPointerOrientation(void);
extern void UI_DrawHandlePic(float x, float y, float w, float h, int horzAlign, int vertAlign, const vec_t *color, MaterialHandle hMaterial);
extern void CG_DrawRotatedPic(float x, float y, float width, float height, int horzAlign, int vertAlign, float angle, const vec_t *color, MaterialHandle material);

extern float vectoyaw( const vec3_t vec );
extern float AngleNormalize360( float angle );
extern vec_t Vec2Normalize( vec3_t v );

#define COMPASS_ACTOR_COUNT 64

// universal/com_math.h inlines
static inline float I_fabs(const float value)
{
    return fabs(value);
}

static inline float I_sqrt(const float value)
{
    return sqrt(value);
}

static inline vec_t Vec2Length(const vec2_t v)
{
    return I_sqrt(v[0] * v[0] + v[1] * v[1]);
}

static inline void FastSinCos(const float value, float *pSin, float *pCos)
{
    __asm
    {
        push eax
        push ecx
        fld value
        fsincos
        mov ecx, pCos
        mov eax, pSin
        fstp dword ptr [ecx]
        fstp dword ptr [eax]
        pop ecx
        pop eax
    }
}


void CG_ApplyCompassPointerRadiusScale(float *radiusScale);
void CG_CompassAddWeaponPingInfo(centity_t *cent, const vec_t *origin, int msec);
void CG_DrawCompassFriendlies(rectDef_t *rect, MaterialHandle material, vec_t *color);

// retail 0x4d4a60
void CG_ApplyCompassPointerRadiusScale(float *radiusScale)
{
    *radiusScale = cg_hudCompassSize->current.value * *radiusScale * 43.75;
}

void CG_CompassAddWeaponPingInfo(centity_t *cent, const vec_t *origin, int msec)
{
    int eType = cent->nextState.eType;

    if (eType == 2)
        return;

    int localClientNum = cg->clientNum;

    clientInfo_t *localClientInfo = &cg->bgs.clientinfo[localClientNum];

    int entClientNum = cent->nextState.number;

    clientInfo_t *entClientInfo = &cg->bgs.clientinfo[entClientNum];

    if (localClientInfo == entClientInfo)
        return;

    int entTeam = entClientInfo->team;
    if (entTeam == 3)
        return;

    int cgTime = cg->time;
    compassactor_t *actor = &cg->compassActors[entClientNum];
    actor->beginFadeTime = cgTime + msec;

    int localTeam = localClientInfo->team;
    byte enemyFlag;
    if (localTeam == 0) {

        enemyFlag = 1;
    } else if (localTeam == entTeam) {

        enemyFlag = 0;
    } else {

        enemyFlag = 1;
    }
    actor->enemy = enemyFlag;

    if (actor->enemy == 0)
        return;

    actor->vLastPos[0] = origin[0];
    actor->vLastPos[1] = origin[1];
}

// retail 0x4d4a80, name unknown
void CompassDistToRadius(float dist, float *radius)
{
    *radius = (dist - cg_hudCompassMinRange->current.value) / (cg_hudCompassMaxRange->current.value - cg_hudCompassMinRange->current.value);
    *radius *= 1.0f - cg_hudCompassMinRadius->current.value;
    *radius += cg_hudCompassMinRadius->current.value;
    CG_ApplyCompassPointerRadiusScale(radius);
}

// retail 0x4d4ad0, name unknown
void CompassIconSize(float *w, float *h)
{
    *w = cg_hudCompassSize->current.value * 10.0f;
    *h = cg_hudCompassSize->current.value * 10.0f;
}

// retail 0x4d4af0
void CG_DrawCompassFriendlies(rectDef_t *rect, MaterialHandle material, vec_t *color)
{
    float compassFadeOutAlpha;
    float centerX;
    float centerY;
    int team;
    int i;
    int num;
    int eFlags;
    centity_t *cent;
    clientInfo_t *ci;
    compassactor_t *actor;
    vec2_t delta;
    float yaw;
    float dist;
    float radius;
    float s;
    float c;
    float x;
    float y;
    float w;
    float h;
    float angle;
    int flash;
    float fadeTime;
    MaterialHandle pingMat;
    vec4_t fadedColor;
    vec4_t actorColor;

    const dvar_t *fadeDvar = hud_fade_compass;
    int duration = I_fround(fadeDvar->current.value * 1000.0f);
    compassFadeOutAlpha = CG_FadeHudMenu(fadeDvar, cg->compassFadeTime, duration);

    if (compassFadeOutAlpha == 0.0f)
        return;

    ci = &cg->bgs.clientinfo[cg->snap->ps.clientNum];
    if (!ci->infoValid)
        return;

    team = ci->team;
    if (team == 3)
        return;

    CG_UpdateCompPointerOrientation();

    centerX = rect->x + rect->w * cg_hudCompassSize->current.value * 0.5f;
    centerY = rect->y + rect->h - cg_hudCompassSize->current.value * rect->h * 0.5f;

    fadedColor[0] = color[0];
    fadedColor[1] = color[1];
    fadedColor[2] = color[2];
    fadedColor[3] = color[3];

    if (team != 0)
    {
        for (i = 0; i < cg->snap->numEntities; i++)
        {
            num = cg->snap->entities[i].number;
            cent = &cg_entities[num];
            if (cent->nextState.eType != 1)
                continue;
            eFlags = cent->nextState.eFlags;
            if (eFlags & 0x20000)
                continue;
            if (num < 0)
                continue;
            if (!cg->bgs.clientinfo[num].infoValid)
                continue;
            if (cg->bgs.clientinfo[num].team != team)
                continue;

            actor = &cg->compassActors[num];
            actor->iLastUpdate = cg->time;
            actor->vLastPos[0] = cent->lerpOrigin[0];
            actor->vLastPos[1] = cent->lerpOrigin[1];
            actor->fLastYaw = cent->lerpAngles[1];
            if ((eFlags & 0x400000) && actor->pingTime <= cg->time)
                actor->pingTime = cg->time + 3000;
        }

        if (cg->snap->ps.iCompassFriendInfo)
        {
            actor = &cg->compassActors[cg->snap->ps.iCompassFriendInfo & 0x3f];
            actor->iLastUpdate = cg->time;
            delta[0] = (float)(((cg->snap->ps.iCompassFriendInfo >> 6) & 0x1ff) * 4 - 1020);
            delta[1] = (float)(((cg->snap->ps.iCompassFriendInfo >> 15) & 0x1ff) * 4 - 1020);
            if (delta[0] != 1024.0f && delta[0] != -1020.0f && delta[1] != 1024.0f && delta[1] != -1020.0f)
            {
                actor->vLastPos[0] = cg->refdef.vieworg[0] + delta[0];
                actor->vLastPos[1] = cg->refdef.vieworg[1] + delta[1];
            }
            else
            {
                Vec2Normalize(delta);
                actor->vLastPos[0] = delta[0];
                actor->vLastPos[1] = delta[1];
            }
            actor->fLastYaw = (float)(char)(cg->snap->ps.iCompassFriendInfo >> 24) * 1.40625f;
            if ((cg->snap->ps.eFlags & 0x800000) && actor->pingTime <= cg->time)
                actor->pingTime = cg->time + 3000;
        }

        if (compassFadeOutAlpha < color[3])
            color[3] = compassFadeOutAlpha;
        if (compassFadeOutAlpha < fadedColor[3])
            fadedColor[3] = compassFadeOutAlpha;

        for (i = 0; i < COMPASS_ACTOR_COUNT; i++)
        {
            actor = &cg->compassActors[i];
            if (actor->iLastUpdate > cg->time)
                actor->iLastUpdate = 0;
            if (actor->iLastUpdate < cg->time - 800)
                continue;
            if (cg->snap->ps.clientNum == i)
                continue;

            if (I_fabs(actor->vLastPos[0]) <= 1.0f && I_fabs(actor->vLastPos[1]) <= 1.0f)
            {
                yaw = AngleNormalize360(vectoyaw(actor->vLastPos) - cg->compPointerYaw);
                dist = cg_hudCompassMaxRange->current.value;
                fadedColor[3] = (1.0f - cg_hudObjectiveMinAlpha->current.value) * 0.5f + cg_hudObjectiveMinAlpha->current.value;
            }
            else
            {
                delta[0] = actor->vLastPos[0] - cg->refdef.vieworg[0];
                delta[1] = actor->vLastPos[1] - cg->refdef.vieworg[1];
                yaw = AngleNormalize360(vectoyaw(delta) - cg->compPointerYaw);
                dist = Vec2Length(delta);

                fadedColor[3] = dist;
                if (dist > cg_hudObjectiveMaxRange->current.value)
                    fadedColor[3] = cg_hudObjectiveMaxRange->current.value;
                else if (dist < cg_hudCompassMaxRange->current.value)
                    fadedColor[3] = cg_hudCompassMaxRange->current.value;
                fadedColor[3] = (fadedColor[3] - cg_hudCompassMaxRange->current.value) / (cg_hudObjectiveMaxRange->current.value - cg_hudCompassMaxRange->current.value);
                fadedColor[3] = (cg_hudObjectiveMinAlpha->current.value - 1.0f) * fadedColor[3] + 1.0f;

                if (dist > cg_hudCompassMaxRange->current.value)
                    dist = cg_hudCompassMaxRange->current.value;
                else if (dist < cg_hudCompassMinRange->current.value)
                    dist = cg_hudCompassMinRange->current.value;
            }

            CompassDistToRadius(dist, &radius);
            FastSinCos(yaw * 0.017453292f, &s, &c);
            CompassIconSize(&w, &h);
            x = centerX - w * 0.5f - s * radius;
            y = centerY - h * 0.5f - c * radius;
            angle = AngleNormalize360(cg->refdefViewAngles[1] - actor->fLastYaw);

            if (actor->pingTime > cg->time)
                flash = (actor->pingTime - cg->time) % 500 >= 250;
            else
                flash = 0;

            fadeTime = cg_hudCompassSoundPingFadeTime->current.value * 1000.0f;
            pingMat = NULL;
            if ((float)actor->beginFadeTime + fadeTime >= (float)cg->time)
            {
                if (actor->beginFadeTime >= cg->time)
                    fadedColor[3] = 1.0f;
                else
                    fadedColor[3] = 1.0f - (float)(cg->time - actor->beginFadeTime) / fadeTime;
                pingMat = cgs->media.compassping_friendlyfiring;
            }

            actorColor[0] = color[0];
            actorColor[1] = color[1];
            actorColor[2] = color[2];
            actorColor[3] = color[3];
            if (compassFadeOutAlpha < actorColor[3])
                actorColor[3] = compassFadeOutAlpha;
            if (compassFadeOutAlpha < fadedColor[3])
                fadedColor[3] = compassFadeOutAlpha;

            if (flash == 1)
            {
                UI_DrawHandlePic(x, y, w, h, rect->horzAlign, rect->vertAlign, fadedColor, cgs->media.friendMaterials[1]);
            }
            else
            {
                if (!pingMat || fadedColor[3] != 1.0f)
                    CG_DrawRotatedPic(x, y, w, h, rect->horzAlign, rect->vertAlign, angle, actorColor, cgs->media.friendMaterials[flash]);
                if (pingMat)
                    CG_DrawRotatedPic(x, y, w, h, rect->horzAlign, rect->vertAlign, angle, fadedColor, pingMat);
            }
        }
    }

    for (i = 0; i < COMPASS_ACTOR_COUNT; i++)
    {
        actor = &cg->compassActors[i];
        if (!actor->enemy)
            continue;
        if (cg_hudCompassSoundPingFadeTime->current.value * 1000.0f + (float)actor->beginFadeTime < (float)cg->time)
            continue;
        if (!actor->beginFadeTime)
            continue;

        delta[0] = actor->vLastPos[0] - cg->refdef.vieworg[0];
        delta[1] = actor->vLastPos[1] - cg->refdef.vieworg[1];
        yaw = AngleNormalize360(vectoyaw(delta) - cg->compPointerYaw);
        dist = Vec2Length(delta);
        if (dist > cg_hudCompassMaxRange->current.value)
            dist = cg_hudCompassMaxRange->current.value;

        CompassDistToRadius(dist, &radius);
        FastSinCos(yaw * 0.017453292f, &s, &c);
        CompassIconSize(&w, &h);
        x = centerX - w * 0.5f - s * radius;
        y = centerY - h * 0.5f - c * radius;

        if (actor->beginFadeTime >= cg->time)
            fadedColor[3] = 1.0f;
        else
            fadedColor[3] = 1.0f - (float)(cg->time - actor->beginFadeTime) / (cg_hudCompassSoundPingFadeTime->current.value * 1000.0f);
        if (compassFadeOutAlpha < fadedColor[3])
            fadedColor[3] = compassFadeOutAlpha;

        UI_DrawHandlePic(x, y, w, h, rect->horzAlign, rect->vertAlign, fadedColor, cgs->media.compassping_enemyfiring);
    }
}
