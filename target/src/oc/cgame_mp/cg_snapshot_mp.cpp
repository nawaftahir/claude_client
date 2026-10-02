#include "common_types.h"
#include "imports.h"
#include "headers/PC/cgame_mp/cg_local.h"
#include "bytematch.h"

extern const dvar_t *cg_nopredict;
extern const dvar_t *cg_synchronousClients;

extern void BG_EvaluateTrajectory( const trajectory_t *tr, int atTime, vec3_t result );
extern void BG_PlayerStateToEntityState(playerState_s *ps, entityState_s *s, int snap, byte handler);
extern void CG_ResetPlayerEntity(centity_t *cent);
extern void XAnimCloneAnimTree(const struct XAnimTree_s *srcTree, struct XAnimTree_s *destTree);
extern int I_stricmp(const char *s1, const char *s2);
extern void I_strncpyz(char *dest, const char *src, int destsize);
extern void CL_ResetSkeletonCache(int val);
extern void CG_SetFrameInterpolation(void);
extern void CG_ExecuteNewServerCommands(int serverCommandSequence);
extern void CG_CheckOpenWaitingScriptMenu(void);
extern void CG_Respawn(void);
extern void CG_BuildSolidList(void);
extern void CG_ClearSolidList(void);
extern void CG_PlaySmokeGrenadesAtTime(int time);
extern void CG_SetEquippedOffHand(int offHandIndex);
extern void CG_TransitionPlayerState(playerState_t *ps, playerState_t *ops);
extern void CG_CheckEvents(centity_t *cent);
extern void CG_UpdatePlayerDObj(centity_t *cent);
extern void CG_UpdateHandViewmodels(const char *configStr);
extern void CG_AddLagometerSnapshotInfo(snapshot_t *snap);
extern int CL_GetSnapshot(int snapshotNumber, snapshot_t *snap);
extern void CL_GetCurrentSnapshotNumber(int *snapshotNumber, int *serverTime);
extern const char *CL_GetConfigString(int index);
extern void CG_GameMessage(const char *msg);
extern void CG_BoldGameMessage(const char *msg);
extern void CG_SafeDObjFree(int entNum);
extern void CG_InitView(void);
extern void SND_SetListener(int entnum, const vec_t *origin, vec3_t *axis);
extern void SND_FadeAllSounds(float volume, int duration);
extern void AnglesToAxis( const vec3_t angles, vec3_t axis[3] );
extern char *va( const char *format, ... );
extern const char *UI_SafeTranslateString(const char *ref);
extern void Com_Error(errorParm_t level, const char *fmt, ...);
extern void Com_Printf(const char *fmt, ...);
extern int strcmp(const char *s1, const char *s2);
extern XAnim *XAnimGetAnims(const XAnimTree *tree);
extern bool XAnimIsLooped(const XAnim_s *anim, unsigned int animIndex);
extern int XAnimGetNumChildren(const struct XAnim_s *anims, unsigned int animIndex);
extern void XAnimSetTime(XAnimTree *tree, unsigned int animIndex, float time);

#define CENT_STRIDE 0x224

#define CENT_CS_NUMBER 0x000
#define CENT_CS_ETYPE 0x004
#define CENT_CS_EFLAGS 0x008
#define CENT_CS_POS 0x00c
#define CENT_CS_APOS 0x030
#define CENT_CS_CLIENTNUM 0x090
#define CENT_CS_EVENTSEQUENCE 0x0a4
#define CENT_CS_LEANF 0x0d4
#define CENT_CS_DMGFLAGS 0x0d8

#define CENT_NS_NUMBER 0x0f0
#define CENT_NS_ETYPE 0x0f4
#define CENT_NS_EFLAGS 0x0f8
#define CENT_NS_POS 0x0fc
#define CENT_NS_APOS 0x120
#define CENT_NS_TIME 0x144
#define CENT_NS_TIME2 0x148
#define CENT_NS_INDEX 0x17c
#define CENT_NS_CLIENTNUM 0x180
#define CENT_NS_EVENTSEQUENCE 0x194
#define CENT_NS_LEANF 0x1c4

#define CENT_NEXTVALID 0x1e0
#define CENT_CULLIN 0x1e1
#define CENT_BMUZZLEFLASH 0x1e2
#define CENT_BTRAILMADE 0x1e3
#define CENT_PREVIOUSEVENTSEQ 0x1e4
#define CENT_MISCTIME 0x1e8
#define CENT_LERPORIGIN 0x1ec
#define CENT_LERPANGLES 0x1f8
#define CENT_LIGHTINGORIGIN 0x204
#define CENT_TREE 0x21c

#define SNAP_NUMCLIENTS 0x26b8
#define SNAP_CLIENTS 0x116b0

#define CI_STRIDE 0x4b8
#define CI_INFOVALID 0x000
#define CI_NEXTVALID 0x004
#define CI_CLIENTNUM 0x008
#define CI_NAME 0x00c
#define CI_TEAM 0x02c
#define CI_MODEL 0x040
#define CI_ATTACHMODELNAMES 0x080
#define CI_ATTACHTAGNAMES 0x200
#define CI_DOBJDIRTY 0x3f8
#define CI_PXANIMTREE 0x4a4

#define CLSTATE_STRIDE 0x5c

#define ES_BINSIZE 0x0f0

#define VectorCopy3(src, dst)                  \
    do {                                       \
        ((int *)(dst))[0] = ((int *)(src))[0]; \
        ((int *)(dst))[1] = ((int *)(src))[1]; \
        ((int *)(dst))[2] = ((int *)(src))[2]; \
    } while (0)

#define VectorClear3(v)      \
    do {                     \
        ((int *)(v))[0] = 0; \
        ((int *)(v))[1] = 0; \
        ((int *)(v))[2] = 0; \
    } while (0)

static void CG_ResetEntity(char *cent);
void CG_SetNextSnap(snapshot_t *snap);
void CG_SetInitialSnapshot(snapshot_t *snap);
void CG_ProcessSnapshots(void);

static char *CG_EntityPtr(int entNum)
{
    return (char *)&cg_entities[entNum];
}

static char *CG_ClientInfoPtr(char *cg, int clientNum)
{
    return (char *)&((cg_t *)cg)->bgs.clientinfo[clientNum];
}

static void CG_ResetEntity(char *cent)
{
    int eType;

    VectorClear3((char *)((centity_t *)cent)->lightingOrigin);

    memcpy(cent, cent + ES_BINSIZE, ES_BINSIZE) ;

    ((centity_t *)cent)->bTrailMade = 0;
    ((centity_t *)cent)->cullIn = 0;

    BG_EvaluateTrajectory(&((centity_t *)cent)->nextState.pos, cg->time, ((centity_t *)cent)->lerpOrigin);

    BG_EvaluateTrajectory(&((centity_t *)cent)->nextState.apos, cg->time, ((centity_t *)cent)->lerpAngles);

    eType = ((centity_t *)cent)->nextState.eType;

    switch (eType) {
    case 0:
    case 4:

        ((centity_t *)cent)->previousEventSequence = 0;
        break;

    case 1:
    {
        char *ci;
        int clientNum;

        ((centity_t *)cent)->previousEventSequence = ((centity_t *)cent)->nextState.eventSequence;

        clientNum = ((centity_t *)cent)->nextState.clientNum;
        ci = (char *)&((cg_t *)cg)->bgs.clientinfo[clientNum] - 0x14 ;

        *(int *)(((char *)ci + offsetof(clientInfo_t, name[8])) + 0x3e0)  = *(int *)(cent + 0x15c) ;

        *(int *)(((char *)ci + offsetof(clientInfo_t, name[8])) + 0x3e4)  = ((centity_t *)cent)->nextState.leanf;

        {
            char *to = ((char *)ci + offsetof(clientInfo_t, angles[0][0]));
            VectorCopy3((char *)((centity_t *)cent)->lerpAngles, to);
        }

        *(int *)((char *)((centity_t *)cent)->lerpAngles) = 0;
        *(int *)((char *)((centity_t *)cent)->lerpAngles + 8) = 0;

        CG_ResetPlayerEntity((centity_t *)cent);
        break;
    }

    case 2:
    {
        char *ci;
        char *corpseInfo;
        char *corpseBase;
        void *pXAnimTree;
        int entNum;
        int clientNum;

        clientNum = ((centity_t *)cent)->nextState.clientNum;
        ci = (char *)&((cg_t *)cg)->bgs.clientinfo[clientNum];

        entNum = ((centity_t *)cent)->nextState.number;

        {
            char *cgs_ptr = (char *)cgs;

            corpseBase = cgs_ptr + entNum * CI_STRIDE - 0x6bf0;
            corpseInfo = corpseBase + 4;
        }

        pXAnimTree = ((clientInfo_t *)corpseInfo)->pXAnimTree;

        if (((centity_t *)cent)->nextState.eFlags & 8) {
            int attachIndex;

            memcpy(corpseInfo, ci, CI_STRIDE);

            {
                char *tagSrc = corpseBase + 0x204;
                char *modelDst = corpseInfo;
                for (attachIndex = 0; attachIndex < 6; attachIndex++) {
                    if (I_stricmp(tagSrc, (const char *)"J_Spine4") == 0) {
                        *(char *)(modelDst + 0x80)  = 0;
                        *(char *)(modelDst + 0x200)  = 0;
                    }
                    tagSrc += 0x40;
                    modelDst += 0x40;
                }
            }

            ((clientInfo_t *)corpseInfo)->pXAnimTree = (XAnimTree_s *)(pXAnimTree);

            XAnimCloneAnimTree((const struct XAnimTree_s *)((clientInfo_t *)ci)->pXAnimTree, (struct XAnimTree_s *)pXAnimTree);

            ((centity_t *)cent)->previousEventSequence = 0;
        } else {

            if (((clientInfo_t *)corpseInfo)->model[0] != 0) {

                if (((clientInfo_t *)corpseInfo)->clientNum != ((clientInfo_t *)ci)->clientNum) {

                    goto do_corpse_copy_from_ci;
                }

            } else {

            do_corpse_copy_from_ci: {
                int attachIndex;
                char *src = ci;

                memcpy(corpseInfo, src, CI_STRIDE);

                {
                    char *tagSrc = corpseBase + 0x204;
                    char *modelDst = corpseInfo;
                    for (attachIndex = 0; attachIndex < 6; attachIndex++) {
                        if (I_stricmp(tagSrc, (const char *)"J_Spine4") == 0) {
                            *(char *)(modelDst + 0x80)  = 0;
                            *(char *)(modelDst + 0x200)  = 0;
                        }
                        tagSrc += 0x40;
                        modelDst += 0x40;
                    }
                }

                ((clientInfo_t *)corpseInfo)->pXAnimTree = (XAnimTree_s *)(pXAnimTree);
            }
            }

            ((centity_t *)cent)->previousEventSequence = ((centity_t *)cent)->nextState.eventSequence;
        }

        ((clientInfo_t *)corpseInfo)->dobjDirty = 1;
        break;
    }

    default:

        ((centity_t *)cent)->previousEventSequence = ((centity_t *)cent)->nextState.eventSequence;
        break;
    }
}

static void CG_ClearClientInfos_Inline(char *dest, char *src, char *tagBase, int size)
{
    int attachIndex;
    char *tagSrc = tagBase;
    char *dst = dest;

    memcpy(dest, src, size);

    for (attachIndex = 0; attachIndex < 6; attachIndex++) {
        if (I_stricmp(tagSrc, (const char *)"J_Spine4") == 0) {
            *(char *)(dst + 0x80)  = 0;
            *(char *)(dst + 0x200)  = 0;
        }
        tagSrc += 0x40;
        dst += 0x40;
    }
}

static void CG_TransitionSnapshot(void)
{
    snapshot_t *snap;
    centity_t *cent;
    int i;

    for (i = 0; i < cg->snap->numClients; i++) {
        clientState_t *cs = &cg->snap->clients[i];
        clientInfo_t *ci = &cg->bgs.clientinfo[cs->clientIndex];

        if (ci->nextValid) {
            ci->nextValid = 0;
        } else {
            XAnimTree_s *tree = ci->pXAnimTree;

            memset(ci, 0, sizeof(*ci));
            ci->pXAnimTree = tree;
            CG_SafeDObjFree(cs->clientIndex);
        }
    }

    snap = cg->nextSnap;
    cg->snap = snap;

    if (snap->ps.pm_flags & 0xc00000) {
        cent = &cg_entities[snap->ps.clientNum];
        memcpy(&cent->currentState, &cent->nextState, sizeof(cent->currentState));
    }

    for (i = 0; i < snap->numEntities; i++) {
        cent = &cg_entities[snap->entities[i].number];
        memcpy(&cent->currentState, &cent->nextState, sizeof(cent->currentState));
    }
}

// Original name unknown.
static void CG_UpdateKillCam(void)
{
    snapshot_t *nextSnap;
    int i;

    if (!cg->inKillCam) {
        if (!cg->nextSnap->ps.deltaTime)
            return;

        cg->inKillCam = 1;
        CG_SetEquippedOffHand(0);
        CG_PlaySmokeGrenadesAtTime(cg->time);
        if (!cg->inKillCam)
            return;
    }

    if (cg->nextSnap->ps.deltaTime)
        return;

    cg->inKillCam = 0;
    CG_PlaySmokeGrenadesAtTime(cg->time);

    nextSnap = cg->nextSnap;
    for (i = 0; i < nextSnap->numEntities; i++) {
        centity_t *cent = &cg_entities[nextSnap->entities[i].number];
        char *corpseCI;
        XAnimTree *savedTree;
        int animState;
        void *anims;

        if (cent->nextState.eType != 2)
            continue;

        corpseCI = (char *)cgs + cent->nextState.number * CI_STRIDE - 0x6bf0 + 4;
        savedTree = *(XAnimTree **)(corpseCI + CI_PXANIMTREE);
        animState = *(int *)(corpseCI + 0x390) & ~0x200;
        anims = XAnimGetAnims(savedTree);

        if (!animState)
            continue;
        if (XAnimIsLooped((const struct XAnim_s *)anims, animState))
            continue;
        if (XAnimGetNumChildren((const struct XAnim_s *)anims, animState) != 0)
            continue;

        XAnimSetTime(savedTree, animState, 1.0f);
    }
}

void CG_SetNextSnap(snapshot_t *snap_param)
{
    char centInPrevSnapshot[1024];
    char *snap = (char *)snap_param;
    char *prevSnap;
    int i;

    memset(centInPrevSnapshot, 0, 0x400);

    prevSnap = (char *)cg->nextSnap;

    if (prevSnap) {
        for (i = 0; i < ((snapshot_t *)prevSnap)->numEntities; i++) {
            int entNum = ((snapshot_t *)prevSnap)->entities[i].number;

            cg_entities[entNum].nextValid = 0;
            centInPrevSnapshot[entNum] = 1;
        }

        {
            int playerNum = ((snapshot_t *)prevSnap)->ps.clientNum;
            char *playerEnt = (char *)&cg_entities[playerNum];
            if (((centity_t *)playerEnt)->nextValid) {
                ((centity_t *)playerEnt)->nextValid = 0;
                centInPrevSnapshot[playerNum] = 1;
            }
        }
    }

    cg->nextSnap = (snapshot_t *)snap;

    if (!snap) {

        CG_ClearSolidList();
        return;
    }

    CL_ResetSkeletonCache(0);

    {
        int delta = ((snapshot_t *)snap)->serverTime - cg->snap->serverTime;

        if (!delta || (cg->frameInterpolation = (float)(cg->time - cg->snap->serverTime) / delta) < 0.0f)
            cg->frameInterpolation = 0.0f;
    }

    CG_ExecuteNewServerCommands(((snapshot_t *)snap)->serverCommandSequence);

    CG_CheckOpenWaitingScriptMenu();

    for (i = 0; i < ((snapshot_t *)snap)->numClients; i++) {
        clientState_t *cs = &((snapshot_t *)snap)->clients[i];
        clientInfo_t *ci = &cg->bgs.clientinfo[cs->clientIndex];
        const char *configStr;
        int part;

        ci->oldteam = ci->infoValid ? ci->team : cs->team;
        ci->infoValid = 1;
        ci->nextValid = 1;
        ci->clientNum = cs->clientIndex;
        ci->team = cs->team;

        if (strcmp(ci->name, cs->name)) {
            if (ci->name[0])
                CG_BoldGameMessage(va("%s^7 %s %s", ci->name, UI_SafeTranslateString("CGAME_PLAYERRENAMES"), cs->name));
            I_strncpyz(ci->name, cs->name, sizeof(ci->name));
        }

        configStr = CL_GetConfigString(cs->modelindex + 0x14e);
        if (strcmp(ci->model, configStr)) {
            I_strncpyz(ci->model, configStr, sizeof(ci->model));
            ci->dobjDirty = 1;
        }

        for (part = 0; part < 6; part++) {
            configStr = CL_GetConfigString(cs->attachModelIndex[part] + 0x14e);
            if (strcmp(ci->attachModelNames[part], configStr)) {
                I_strncpyz(ci->attachModelNames[part], configStr, sizeof(ci->attachModelNames[part]));
                ci->dobjDirty = 1;
            }

            configStr = CL_GetConfigString(cs->attachTagIndex[part] + 0x6e);
            if (strcmp(ci->attachTagNames[part], configStr)) {
                I_strncpyz(ci->attachTagNames[part], configStr, sizeof(ci->attachTagNames[part]));
                ci->dobjDirty = 1;
            }
        }
    }

    cg->identifyClientNum = ((snapshot_t *)snap)->ps.stats[3];
    cg->identifyClientHealth = ((snapshot_t *)snap)->ps.stats[4];

    {
        int entnum = ((snapshot_t *)snap)->ps.clientNum;

        if (((snapshot_t *)snap)->ps.pm_flags & 0xc00000) {

            char *playerEnt = CG_EntityPtr(entnum);

            ((centity_t *)playerEnt)->nextState.number = (unsigned short)entnum;

            BG_PlayerStateToEntityState(&((snapshot_t *)snap)->ps, (entityState_t *)(playerEnt + ES_BINSIZE), 0, 0);

            ((centity_t *)playerEnt)->nextValid = 1;

            {
                char *oldSnap = (char *)cg->snap;

                if (cg->mapRestart ||
                    ((snapshot_t *)snap)->ps.stats[5] != ((snapshot_t *)oldSnap)->ps.stats[5] ||
                    entnum != ((snapshot_t *)oldSnap)->ps.clientNum) {

                    memcpy((char *)&((snapshot_t *)oldSnap)->ps, (char *)&((snapshot_t *)snap)->ps, sizeof(playerState_t));

                    CG_ResetEntity(playerEnt);
                    CG_Respawn();
                } else if (!centInPrevSnapshot[entnum] ||
                           (((centity_t *)playerEnt)->currentState.eFlags ^ ((centity_t *)playerEnt)->nextState.eFlags) & 2) {

                    memcpy((char *)&((snapshot_t *)oldSnap)->ps, (char *)&((snapshot_t *)snap)->ps, sizeof(playerState_t));
                    CG_ResetEntity(playerEnt);

                    cg->predictedError[0] = 0.0f;
                    cg->predictedError[1] = 0.0f;
                    cg->predictedError[2] = 0.0f;
                }

            }
        } else {

            char *oldSnap = (char *)cg->snap;

            if (cg->mapRestart ||
                ((snapshot_t *)snap)->ps.stats[5] != ((snapshot_t *)oldSnap)->ps.stats[5] ||
                entnum != ((snapshot_t *)oldSnap)->ps.clientNum) {

                memcpy(&((snapshot_t *)oldSnap)->ps, &((snapshot_t *)snap)->ps, sizeof(playerState_t));
                CG_Respawn();
            }
        }
    }

    {
        int numEnts = ((snapshot_t *)snap)->numEntities;
        for (i = 0; i < numEnts; i++) {
            char *snapEnt = (char *)&((snapshot_t *)snap)->entities[i];
            int entNum = ((entityState_t *)snapEnt)->number ;
            char *cent = CG_EntityPtr(entNum);

            memcpy(cent + ES_BINSIZE, snapEnt, ES_BINSIZE);

            ((centity_t *)cent)->nextValid = 1;

            if (!centInPrevSnapshot[entNum] || ((((entityState_t *)snapEnt)->eFlags ^ ((centity_t *)cent)->currentState.eFlags) & 2))
                CG_ResetEntity(cent);
        }
    }

    {
        int numClients = ((snapshot_t *)snap)->numClients;
        for (i = 0; i < numClients; i++) {
            char *clState = (char *)snap + SNAP_CLIENTS + i * CLSTATE_STRIDE;
            int clientNum = *(int *)(clState + 0xc) ;
            CG_UpdatePlayerDObj( (centity_t *)(CG_EntityPtr(clientNum)));
        }
    }

    {
        int viewModelIndex = ((snapshot_t *)snap)->ps.viewmodelIndex;
        if (viewModelIndex > 0) {
            const char *cfgStr = CL_GetConfigString(viewModelIndex + 0x14e);
            CG_UpdateHandViewmodels(cfgStr);
        }
    }

    CG_BuildSolidList();

    CG_UpdateKillCam();

    {
        int numEnts = ((snapshot_t *)snap)->numEntities;
        for (i = 0; i < numEnts; i++) {
            char *snapEnt = (char *)&((snapshot_t *)snap)->entities[i];
            int entNum = ((entityState_t *)snapEnt)->number ;
            CG_CheckEvents( (centity_t *)(CG_EntityPtr(entNum)));
        }
    }

    {
        int isDemo = cg->demoType;

        if (!isDemo) {
            char *nextSnap = (char *)cg->nextSnap;
            if (!(((snapshot_t *)nextSnap)->ps.pm_flags & 0x400000)) {

                if (!cg_nopredict->current.enabled && !cg_synchronousClients->current.enabled)
                    return;
            }
        }

        {
            char *oldSnap = (char *)cg->snap;
            char *newSnap = (char *)cg->nextSnap;
            CG_TransitionPlayerState( (playerState_t *)((char *)&((snapshot_t *)newSnap)->ps), (playerState_t *)((char *)&((snapshot_t *)oldSnap)->ps));
        }
    }
}

void CG_SetInitialSnapshot(snapshot_t *snap_param)
{
    char *snap = (char *)snap_param;
    float clientViewOrigin[3];
    float clientViewAxis[9];

    CG_SetNextSnap(NULL);

    cg->snap = (snapshot_t *)snap;
    cg->nextSnap = (snapshot_t *)snap;

    {
        int serverTime = ((snapshot_t *)snap)->serverTime;
        cg->time = serverTime;
        cg->bgs.time = serverTime;
        cg->oldTime = serverTime;
    }

    clientViewOrigin[0] = ((snapshot_t *)snap)->ps.origin[0];
    clientViewOrigin[1] = ((snapshot_t *)snap)->ps.origin[1];
    clientViewOrigin[2] = ((snapshot_t *)snap)->ps.origin[2];
    clientViewOrigin[2] += ((snapshot_t *)snap)->ps.viewHeightCurrent;

    AnglesToAxis(((snapshot_t *)snap)->ps.viewangles, (vec3_t *)clientViewAxis);

    {
        int clientNum = ((snapshot_t *)snap)->ps.clientNum;
        SND_SetListener(clientNum, clientViewOrigin, (vec3_t (*))(clientViewAxis));
    }

    SND_FadeAllSounds(1.0f, 0);

    CG_Respawn();

    CG_PlaySmokeGrenadesAtTime(cg->time);

    CG_InitView();

    cg->nextSnap = NULL;
}

static inline __attribute__((always_inline)) char *CG_ReadNextSnapshot(void)
{
    int snapshotNum;
    int latestNum;
    char *dest;

    latestNum = cg->latestSnapshotNum;
    snapshotNum = cgs->processedSnapshotNum;

    if (latestNum > snapshotNum + 0x3e8) {
        Com_Printf("WARNING: CG_ReadNextSnapshot: way out of range, %i > %i\n",
                   latestNum, snapshotNum);
    }

    while (snapshotNum < latestNum) {

        dest = (char *)&cg->activeSnapshots[0];
        if ((char *)cg->snap == dest)
            dest = (char *)&cg->activeSnapshots[1];

        snapshotNum++;
        cgs->processedSnapshotNum = snapshotNum;

        if (CL_GetSnapshot(cgs->processedSnapshotNum, (snapshot_t *)dest)) {

            CG_AddLagometerSnapshotInfo( (snapshot_t *)(dest));
            return dest;
        }

        CG_AddLagometerSnapshotInfo(NULL);

        snapshotNum = cgs->processedSnapshotNum;
        latestNum = cg->latestSnapshotNum;
    }

    return NULL;
}

void CG_ProcessSnapshots(void)
{
    int n;
    int cgTime;
    int snapTime;
    char *snap;
    char *curSnap;
    char *nextSnap;

    CL_GetCurrentSnapshotNumber(&n, &cg->latestSnapshotTime);

    if (n != cg->latestSnapshotNum) {
        if (n < cg->latestSnapshotNum) {

            Com_Error(ERR_DROP, (const char *)"\x15"
                                       "CG_ProcessSnapshots: n < cg->latestSnapshotNum");
        }
        cg->latestSnapshotNum = n;
    }

    cg->bgs.latestSnapshotTime = cg->latestSnapshotTime;

    for (;;) {
        curSnap = (char *)cg->snap;

        if (curSnap == NULL) {
            snap = CG_ReadNextSnapshot();
            if (!snap)
                return;

            if (((snapshot_t *)snap)->snapFlags & 2) {
                continue;
            }

            CG_SetInitialSnapshot((snapshot_t *)snap);
            CG_SetNextSnap((snapshot_t *)snap);
            CG_TransitionSnapshot();
            continue;
        }

        nextSnap = (char *)cg->nextSnap;

        if (nextSnap != NULL) {
            CG_SetFrameInterpolation();
        }

        if (nextSnap != NULL && nextSnap != curSnap) {
            curSnap = (char *)cg->snap;
            nextSnap = (char *)cg->nextSnap;
            cgTime = cg->time;

            if (cgTime < ((snapshot_t *)curSnap)->serverTime ||
                cgTime >= ((snapshot_t *)nextSnap)->serverTime) {
                CG_TransitionSnapshot();
                continue;
            }

            return;
        }

        snap = CG_ReadNextSnapshot();
        if (!snap) {
            snapTime = ((snapshot_t *)curSnap)->serverTime;
            if (cg->time < snapTime) {
                cg->time = snapTime;
                cg->bgs.time = snapTime;
            }
            return;
        }

        if (((snapshot_t *)snap)->snapFlags & 2) {
            continue;
        }

        if ((((snapshot_t *)snap)->snapFlags ^ ((snapshot_t *)curSnap)->snapFlags) & 4) {
            CG_SetInitialSnapshot((snapshot_t *)snap);
            CG_SetNextSnap((snapshot_t *)snap);
            CG_TransitionSnapshot();
            continue;
        }

        if (((snapshot_t *)snap)->serverTime < ((snapshot_t *)curSnap)->serverTime) {
            Com_Error(ERR_DROP, (const char *)"\x15"
                                       "CG_ProcessSnapshots: Server time went backwards");
        }

        CG_SetNextSnap((snapshot_t *)snap);
    }
}
