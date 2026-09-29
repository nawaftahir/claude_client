#include "common_types.h"
#include <float.h>
#include "imports.h"
#include "bytematch.h"
#include "cod2_feature_config.h"
#include "pb_public.h"
#include "www_download.h"

extern int qport;
extern bgs_t *bgs;
extern int cl_connectedToPureServer;
extern int fs_checksumFeed;
extern int dvar_modifiedFlags;
extern int g_console_field_width;
extern float g_console_char_height;
extern field_t g_consoleField;
extern LegacyHacks *legacyHacks;
extern int com_frameTime;
extern float com_timescaleValue;
/* dvar globals */
extern const dvar_t *cg_drawCrosshair;
extern const dvar_t *cl_anglespeedkey;
extern const dvar_t *cl_bypassMouseInput;
extern dvar_t *cl_paused;
extern const dvar_t *cl_pitchspeed;
extern const dvar_t *cl_talking;
extern const dvar_t *cl_yawspeed;
extern dvar_t *com_expectedHunkUsage;
extern dvar_t *com_sv_running;
extern dvar_t *net_lanauthorize;
extern dvar_t *nextmap;
extern dvar_t *sv_disableClientConsole;
extern dvar_t *sv_paused;
extern dvar_t *sv_voice;

extern dvar_t *Dvar_RegisterBool(const char *dvarName, bool value, unsigned short flags);
extern int CM_BoxSightTrace( int oldHitNum, const vec3_t start, const vec3_t end, const vec3_t mins, const vec3_t maxs, clipHandle_t model, int brushmask );
extern int I_strnicmp(const char *s0, const char *s1, int n);
extern char *strupr(char *s);
extern void *Hunk_AllocAlignInternal(int size, int alignment);

COD2_ASSERT_FIELD(LegacyHacks, cl_running,                 0x4);
COD2_ASSERT_FIELD(LegacyHacks, cl_stance,                  0x8);
COD2_ASSERT_FIELD(LegacyHacks, cl_downloadSize,            0x10);
COD2_ASSERT_FIELD(LegacyHacks, cl_downloadCount,           0x14);
COD2_ASSERT_FIELD(LegacyHacks, cl_downloadTime,            0x18);
COD2_ASSERT_FIELD(LegacyHacks, cl_downloadName,            0x1c);
COD2_ASSERT_FIELD(LegacyHacks, cl_serverloadmap,           0x5c);
COD2_ASSERT_FIELD(LegacyHacks, cl_serverloadgametype,      0x9c);
COD2_ASSERT_FIELD(LegacyHacks, cl_serverloadwaiting,       0xdc);
COD2_ASSERT_FIELD(LegacyHacks, ui_scriptMenuAllowResponse, 0x4ed);
COD2_ASSERT_FIELD(clientActive_t, timeoutcount, 0x14);

#if COD2_IS_PATCH_13

const dvar_t *cl_punkbuster;
const dvar_t *cl_wwwDownload;

extern void CL_WWWDownload(void);
extern struct clientStatic_t cls;

int CL_ClearStaticDownload(void)
{
    cls.downloadRestart = 0;
    cls.downloadTempName[0] = 0;
    cls.downloadName[0] = 0;
    cls.originalDownloadName[0] = 0;
    cls.wwwDlInProgress = 0;
    cls.wwwDlDisconnected = 0;
    cls.downloadFlags = 0;
    DL_CancelDownload();
    return 0;
}
#endif

extern Bool Sys_IsMainThread(void);
extern int UI_IsFullscreen(void);
extern void IN_Frame(void);
extern void Com_Printf(const char *fmt, ...);
extern const char *FS_LoadedIwdNames(void);
extern const char *FS_ReferencedIwdNames(void);
extern Bool Voice_IsClientTalking(int clientIndex);
extern void Com_SetRecommended(int);
extern void Sys_StartProcess(const char *exeName, int doexit);
extern qboolean UI_SetActiveMenu(int menu);
extern void Sys_ShowIP(void);
extern void Com_Error(errorParm_t code, const char *fmt, ...);
extern void FS_FCloseFile(fileHandle_t f);
extern void MSG_WriteReliableCommandToBuffer(const char *cmd, char *buf, int bufSize);
extern int FS_Write(const void *buffer, int len, int f);
extern void CL_ShutdownCGame(void);
extern qboolean CL_ShutdownUI(void);
extern void Dvar_SetInt(dvar_t *dvar, int value);
extern void Dvar_SetBool(dvar_t *dvar, bool value);
extern void Dvar_SetString(dvar_t *dvar, const char *value);
extern void UI_SetMap(const char *mapname, const char *gametype);
extern void SCR_UpdateScreenInternal(void);
extern void SetScreenScaling(float safeAreaH, float safeAreaV, int vpX, int vpY, float vpW, float vpH);
extern void CalcSplitScreenTextOffset(FontHandle font, float *y);
extern void CalcScreenPlacement(float *x, float *y, float *w, float *h, int horzAlign, int vertAlign);
extern void SND_Init(void);
extern void SND_StopSounds(snd_stopsounds_arg_t);
extern void SND_FadeAllSounds(float, int);
extern void SND_Shutdown(void);
extern void SND_Save(MemoryFile *memFile);
extern void SND_Restore(MemoryFile *memFile);
extern void SND_SaveListeners(struct snd_listener *listeners);
extern void SND_RestoreListeners(struct snd_listener *listeners);
extern void SND_DisconnectListener(void);
extern void CL_InitUI(void);
extern void Sys_LoadingKeepAlive(void);
extern void SCR_Init(void);
extern void SCR_StopCinematic(void);
extern void Con_Init(void);
extern void Con_Close(void);
extern void CL_PlayCinematic_f(void);
extern void CL_Connect_f(void);
extern void CL_GlobalServers_f(void);
extern void CL_Rcon_f(void);
extern void CL_Ping_f(void);
extern void CL_ServerStatus_f(void);
extern void CL_UpdateLevelHunkUsage(void);
extern void SCR_UpdateScreen(void);
extern void CL_CubemapShot_f(void);
extern void Com_WriteLocalizedSoundAliasFiles(void);
extern void UI_OpenMenu_f(void);
extern void UI_CloseMenu_f(void);
extern void CL_InitInput(void);
extern void CL_ShutdownInput(void);
extern void CL_RconInit(void);
extern void CL_ClearMutedList(void);
extern void CL_SetADS(int);
extern void CL_SetCGameTime(void);
extern qboolean CG_IsConsoleCommandName(const char *cmd);
extern void CL_SendCmd(void);
extern void CL_WritePacket(void);
extern void CL_UpdateColor(void);
extern void CL_InitCGame(void);
extern void UI_CloseAll(void);
extern void UI_DrawConnectScreen(void);
extern void Cbuf_Execute(void);
extern void Cbuf_ExecuteText(int, const char *);
extern void Com_DPrintf(const char *fmt, ...);
extern void Com_PrintMessage( print_msg_type_t channel, const char *msg );
extern void Com_Restart(void);
dvar_t *Dvar_RegisterBool(const char *dvarName, bool value, unsigned short flags);
extern dvar_t *Dvar_RegisterInt(const char *dvarName, int value, int min, int max, unsigned short flags);
extern dvar_t *Dvar_RegisterFloat(const char *dvarName, float value, float min, float max, unsigned short flags);
dvar_t *Dvar_RegisterString(const char *dvarName, const char *value, unsigned short flags);
extern bool Dvar_GetBool(const char *dvarName);
extern void Cmd_AddCommand(const char *name, void (*cmd)(void));
extern void Cmd_RemoveCommand(const char *name);
extern void Cmd_SetAutoComplete(const char *cmd, const char *dir, const char *ext);
extern char *Cmd_Args(int start);
extern int atoi(const char *);
extern double atof(const char *);
extern int I_stricmp(const char *a, const char *b);
extern int I_strncmp(const char *a, const char *b, int n);
extern void Info_SetValueForKey(char *s, const char *key, const char *value);
extern const char *Info_ValueForKey( const char *s, const char *key );
extern void Com_Memset(void *dest, const int val, int count);
extern qboolean NET_CompareAdr(netadr_t a, netadr_t b);
extern int NET_CompareBaseAdr(netadr_t a, netadr_t b);
extern const char *NET_AdrToString(netadr_t a);
extern qboolean NET_StringToAdr(const char *s, netadr_t *a);
extern unsigned char NET_OutOfBandPrint(netsrc_t sock, netadr_t adr, const char *msg);
extern unsigned char NET_OutOfBandData(netsrc_t sock, netadr_t adr, unsigned char *data, int len);
extern int Sys_IsLANAddress(netadr_t adr);
extern int Sys_Milliseconds(void);
extern int Sys_MillisecondsRaw(void);
extern void srand(unsigned int seed);
extern int FS_FileExists(const char *path);
extern fileHandle_t FS_FOpenFileWrite(const char *filename);
extern int FS_FOpenFileRead(const char *path, int *file, int uniqueFILE);
extern int FS_Read(void *buffer, int len, int f);
extern qboolean FS_ConditionalRestart(int checksumFeed);
extern void FS_Restart(int checksumFeed);
extern const char *FS_ReferencedIwdPureChecksums(void);
extern int FS_CompareIwds(char *buf, int bufLen, int flag);
extern char *FS_ShiftStr(const char *name, int shift);
extern void MSG_Init(msg_t *msg, byte *data, int length);
extern void MSG_WriteLong(msg_t *msg, int value);
extern void MSG_WriteShort(msg_t *msg, int value);
extern void MSG_WriteByte(msg_t *msg, int value);
extern void MSG_WriteBigString( msg_t *sb, const char *s );
extern void MSG_WriteDeltaEntity(msg_t *msg, entityState_t *from, entityState_t *to, int force);
extern int MSG_ReadLong(msg_t *msg);
extern char *MSG_ReadStringLine(msg_t *msg);
extern char *MSG_ReadBigString(msg_t *msg);
extern void MSG_BeginReading(msg_t *msg);
extern int MSG_WriteBitsCompress(byte *src, byte *dst, int len);
extern void CL_SetServerInfoByAddress(netadr_t from, const char *info, int ping);
extern void Netchan_Setup(netsrc_t sock, netchan_t *chan, netadr_t adr, int qport);
extern int Netchan_Process(netchan_t *chan, msg_t *msg);
extern void CL_Netchan_AddOOBProfilePacket(int size);
extern void CL_Netchan_Decode(byte *data, int len);
extern void CL_Netchan_SendOOBPacket(int len, const void *data, netadr_t to);
extern int CL_CDKeyValidate(const char *cdkey, const char *checksum);
extern void CL_ServerInfoPacket(netadr_t from, msg_t *msg, int time);
extern void CL_ServerStatusResponse(netadr_t from, msg_t *msg);
extern void CL_ServersResponsePacket(netadr_t from, msg_t *msg);
extern const char *CL_GetConfigString(int index);
extern void CL_ParseServerMessage(msg_t *msg);
extern void CL_ArchiveClientState(MemoryFile *memFile);
extern void Cmd_TokenizeString(const char *text);
typedef refexport_t *(*GetRefAPI_t)(int apiVersion, refimport_t *rimp);
extern GetRefAPI_t Sys_LoadRenderer(void);
extern void Sys_UnloadRenderer(void);
extern void MemFile_InitForWriting(MemoryFile *mf, int size, void *buf, bool flag);
extern void MemFile_InitForReading(MemoryFile *mf, int size, void *buf);
extern "C" void *Z_MallocInternal( int size );
extern void *Z_VirtualAllocInternal( unsigned int size );
extern void Z_VirtualFreeInternal(void *ptr);
extern void CG_CalculateFPS(void);
extern int Voice_GetLocalVoiceData(ClientVoicePacket_t *dest);
extern void Voice_Playback(void);
extern void SEH_UpdateLanguageInfo(void);
extern const char *SEH_LocalizeTextMessage(const char *ref, const char *defaultText, msgLocErrType_t errType);
extern char *strchr(const char *, int);
extern float floorf(float);
extern int puts(const char *);

extern refexport_t re;
extern clientConnection_t clc;
extern struct clientStatic_t cls;
extern const dvar_t *cl_nodelta;
extern const dvar_t *cl_noprint;
extern const dvar_t *cl_maxpackets;
extern const dvar_t *cl_packetdup;
extern const dvar_t *cl_shownet;
extern const dvar_t *cl_shownuments;
extern const dvar_t *cl_showSend;
extern const dvar_t *cl_showServerCommands;
extern const dvar_t *cl_showTimeDelta;
extern const dvar_t *cl_freezeDemo;
extern const dvar_t *cl_sensitivity;
extern const dvar_t *cl_freelook;
extern const dvar_t *cl_mouseAccel;
extern const dvar_t *cl_showMouseRate;
extern const dvar_t *m_pitch;
extern const dvar_t *m_yaw;
extern const dvar_t *m_forward;
extern const dvar_t *m_side;
extern const dvar_t *m_filter;
extern const dvar_t *cl_activeAction;
extern const dvar_t *cl_allowDownload;
extern dvar_t *cl_updateavailable;
extern dvar_t *cl_updatefiles;
extern dvar_t *cl_updateoldversion;
extern dvar_t *cl_updateversion;
extern const dvar_t *cl_inGameVideo;
extern dvar_t *fx_enable;
extern dvar_t *fx_draw;
extern dvar_t *fx_cull;
extern dvar_t *fx_sort;
extern const dvar_t *fx_debug;
extern const dvar_t *fx_debugBolt;
extern dvar_t *fx_freeze;
extern const dvar_t *fx_count;
extern dvar_t *fx_visMinTraceDist;
extern dvar_t *fx_profile;
extern const dvar_t *nextdemo;
extern const dvar_t *cl_timeout;
extern const dvar_t *cl_connectTimeout;
extern const dvar_t *cl_avidemo;
extern const dvar_t *cl_forceavidemo;
extern const dvar_t *cl_motdString;
extern const dvar_t *cl_serverStatusResendTime;

#if defined(COD2_X64) || defined(_M_X64) || defined(__x86_64__)
int clc_x64_lastChallenge;
#endif
extern dvar_t *cl_ingame;
extern const dvar_t *name;
extern const dvar_t *cl_voice;
extern clientActive_t clients[1];
extern void Com_ClientDObjClearAllSkel(void);
extern Bool g_waitingForServer;
extern ping_t cl_pinglist[16];
extern unsigned int frame_msec;
extern int old_com_frameTime;
static char szServerIPAddress[128];
static qboolean recursive;
static const dvar_t *input_viewSensitivity;

void CL_SwitchToLocalClient(int clientNum);
Bool CL_GetLocalClientActive(int clientNum);
int CL_GetLocalClientActiveCount(void);
Bool CL_AllLocalClientsDisconnected(void);
Bool CL_AnyLocalClientChallenging(void);
Bool CL_IsRenderingSplitScreen(void);
const char *CL_GetUsernameForLocalClient(int controllerIndex);
void CL_AddReliableCommand(const char *cmd);
void CL_StopRecord_f(void);
void CL_ShutdownDemo(void);
int CL_GetSkelTimeStamp(int localClientNum);
char *CL_AllocSkelMemory(int localClientNum, unsigned int size);
void CL_ResetSkeletonCache(int localClientNum);
void CL_ClearState(void);
void CL_Setenv_f(void);
void CL_Reconnect_f(void);
void CL_OpenedIWDList_f(void);
void CL_ReferencedIWDList_f(void);
void CL_Configstrings_f(void);
void CL_Clientinfo_f(void);
void CL_VoicePacket(msg_t *msg);
Bool CL_IsPlayerTalking(int clientIndex);
void CL_SetupForNewServerMap(const char *pszMapName, const char *pszGametype);
void CL_VoiceTransmit(void);
int Voice_SendVoiceData(void);
void CL_SyncGpu(void);
void CL_SetRecommended_f(void);
void CL_RefPrintf(int print_level, const char *fmt, ...);
void CL_ShutdownRef(void);
void CL_InitRenderer(void);
void CL_StartHunkUsers(void);
int CL_ScaledMilliseconds(void);
void CL_InitRef(void);
void CL_startSingleplayer_f(void);
void CL_StopLogo(void);
void CL_ToggleMenu_f(void);
void CL_OpenScriptMenu_f(void);
void CL_InitOnceForAllClients(void);
void CL_ShutdownDebugData(void);
void CL_GetPing(int n, char *buf, int buflen, int *pingtime);
void CL_ShowIP_f(void);
const char *CL_GetServerIPAddress(void);
void CL_FlushDebugData(qboolean fromServer);
void CL_UpdateDebugData(void);
int CL_TextWidth(const char *text, int maxChars, FontHandle font);
int CL_TextHeight(FontHandle font);
float CL_NormalizedTextScale(FontHandle font, float scale);
void CL_DrawTextPhysical(const char *text, int maxChars, FontHandle font, float x, float y, float xScale, float yScale, const vec_t *color, int style);
void CL_DrawText(const char *text, int maxChars, FontHandle font, float x, float y, int horzAlign, int vertAlign, float xScale, float yScale, const vec_t *color, int style);
void CL_DrawTextPhysicalWithCursor(const char *text, int maxChars, FontHandle font, float x, float y, float xScale, float yScale, const vec_t *color, int style, int cursorPos, int cursor);
void CL_DrawTextWithCursor(const char *text, int maxChars, FontHandle font, float x, float y, int horzAlign, int vertAlign, float xScale, float yScale, const vec_t *color, int style, int cursorPos, int cursor);
int CL_GetKeyCatchers(void);
Bool CL_GetDisplayHUDWithKeycatchUI(void);
FontHandle CL_RegisterFont(const char *fontName, int imageTrack);
void CL_WriteDemoMessage(msg_t *msg, int headerBytes);
void CL_Record_f(void);
void CL_ShutdownHunkUsers(void);
void CL_InitLoad(const char *mapname, const char *gametype);
void CL_DrawLogo(void);
void CL_StopLogoOrCinematic(void);
void CL_AddDebugLine(const vec_t *start, const vec_t *end, const vec_t *color, qboolean depthTest, int duration, qboolean fromServer);
void CL_AddDebugString(const vec_t *xyz, const vec_t *color, float scale, const char *text, qboolean fromServer);
void CL_RequestAuthorization(void);
void CL_CheckForResend(void);
void CL_LocalServers_f(void);
void CL_PlayLogo_f(void);
qboolean CL_UpdateDirtyPings_f(int source);
void CL_RunOncePerClientFrame(int msec);
int Client_SendVoiceData(int bytes, char *enc_buffer);
Bool CL_ConnectionlessPacket(netadr_t from, msg_t *msg, int time);
void CL_UpdateInfoPacket(netadr_t from);
void CL_CheckAutoUpdate(void);
extern qboolean autoupdateChecked;
extern qboolean autoupdateStarted;
short BigShort(short l);
Bool CL_PacketEvent(netadr_t from, msg_t *msg, int time);
void CL_Init(void);
void CL_ShutdownAll(void);
void CL_Frame(void);
void CL_Vid_Restart_f(void);
void CL_Snd_Restart_f(void);
void CL_Disconnect(void);
void CL_Shutdown(void);
void CL_Disconnect_f(void);
void CL_MapLoading(const char *mapname);
void CL_DemoCompleted(void);
void CL_ReadDemoMessage(void);
void CL_PlayDemo_f(void);
void CL_ForwardCommandToServer(const char *string);
void CL_ForwardToServer_f(void);
void CL_DownloadsComplete(void);
void CL_BeginDownload(const char *localName, const char *remoteName);
void CL_NextDownload(void);
void CL_InitDownloads(void);
static void CL_SendPureChecksums(void);

void CL_SwitchToLocalClient(int clientNum)
{

}

Bool CL_GetLocalClientActive(int clientNum)
{
    return 1;
}

int CL_GetLocalClientActiveCount(void)
{
    return 1;
}

Bool CL_AllLocalClientsDisconnected(void)
{
    int client;

    if (!Sys_IsMainThread())
        return 1;
    if (UI_IsFullscreen())
        return 1;
    for (client = 0; client < 1; client++) {
        if (clients[client].active && (&::clc)[client].state >= CA_CONNECTING)
            return 0;
    }
    return 1;
}

Bool CL_AnyLocalClientChallenging(void)
{
    int i;

    for (i = 0; i < 1; i++) {
        if (*(byte *)&clients[i] && (&::clc)[i].state == CA_CHALLENGING)
            return 1;
    }
    return 0;
}

Bool CL_IsRenderingSplitScreen(void)
{
    return 0;
}

const char *CL_GetUsernameForLocalClient(int controllerIndex)
{
    return name->current.string;
}

void CL_AddReliableCommand(const char *cmd)
{
    if (::clc.reliableSequence - ::clc.reliableAcknowledge > 128) {
        Com_Error(ERR_DROP, (const char *)"EXE_ERR_CLIENT_CMD_OVERFLOW");
    }
    ::clc.reliableSequence++;
    MSG_WriteReliableCommandToBuffer(cmd, (char *)::clc.reliableCommands[::clc.reliableSequence & 0x7f], 0x400);
}

void CL_StopRecord_f(void)
{
    clientConnection_t *cc = &::clc;
    clientConnection_t *conn;
    int len;

    if (!cc->demorecording) {
        Com_Printf("Not recording a demo.\n");
        return;
    }

    len = -1;
    FS_Write(&len, 4, cc->demofile);
    conn = &::clc;
    FS_Write(&len, 4, conn->demofile);
    FS_FCloseFile(conn->demofile);
    conn->demofile = 0;
    conn->demorecording = 0;
    Com_Printf("Stopped demo.\n");
}

void CL_ShutdownDemo(void)
{
    clientConnection_t *conn;

    if (!::clc.demofile)
        return;
    FS_FCloseFile(::clc.demofile);
    conn = &::clc;
    conn->demofile = 0;
    conn->demoplaying = 0;
    conn->demorecording = 0;
}

int CL_GetSkelTimeStamp(int localClientNum)
{
    clientActive_t *client = &clients[localClientNum];
    return client->skelTimeStamp;
}

char *CL_AllocSkelMemory(int localClientNum, unsigned int size)
{
    clientActive_t *client = &clients[localClientNum];
    int pos = client->skelMemPos;
    char *buf = (char *)((intptr_t)pos + (intptr_t)client->skelMemoryStart);
    int newPos = pos + ((size + 15) & ~15);
    client->skelMemPos = newPos;
    if (newPos >= 0x3fff1)
        return 0;
    return buf;
}

void CL_ResetSkeletonCache(int localClientNum)
{
    clientActive_t *client = &clients[localClientNum];
    if (!++client->skelTimeStamp)
        ++client->skelTimeStamp;
    client->skelMemoryStart = (char *)(((intptr_t)client->skelMemory + 15) & ~(intptr_t)15);
    client->skelMemPos = 0;
}

void CL_ClearState(void)
{
    clientActive_t *restorePtr;
    Bool cgameInitialized, cgameInitCalled, displayHUD, active;
    int keyCatchers;

    cgameInitialized = clients[0].cgameInitialized;
    cgameInitCalled = clients[0].cgameInitCalled;
    keyCatchers = clients[0].keyCatchers;
    displayHUD = clients[0].displayHUDWithKeycatchUI;
    active = clients[0].active;

    memset(&clients[0], 0, sizeof(clientActive_t));

    restorePtr = clients;
    restorePtr->cgameInitialized = cgameInitialized;
    restorePtr->cgameInitCalled = cgameInitCalled;
    restorePtr->keyCatchers = keyCatchers;
    restorePtr->displayHUDWithKeycatchUI = displayHUD;
    restorePtr->active = active;

    Com_ClientDObjClearAllSkel();
}

extern int Cmd_Argc(void);
extern const char *Cmd_Argv( int arg );
extern void I_strncpyz(char *dest, const char *src, int destsize);
extern void I_strncat(char *dest, int maxlen, const char *src);
extern int putenv(const char *string);
extern char *getenv(const char *name);
void CL_Setenv_f(void)
{
    int argc = Cmd_Argc();

    if (argc > 2) {
        char buffer[1024];
        int i;

        I_strncpyz(buffer, Cmd_Argv(1), sizeof(buffer));
        I_strncat(buffer, sizeof(buffer), "=");

        for (i = 2; i < argc; i++) {
            I_strncat(buffer, sizeof(buffer), Cmd_Argv(i));
            I_strncat(buffer, sizeof(buffer), " ");
        }

        putenv(buffer);
    } else if (argc == 2) {
        char *env = getenv(Cmd_Argv(1));

        if (env) {
            Com_Printf("%s=%s\n", Cmd_Argv(1), env);
        } else {
            Com_Printf("%s undefined\n", Cmd_Argv(1));
        }
    }
}

extern void Cbuf_AddText(const char *text);
extern char *va( const char *format, ... );
void CL_Reconnect_f(void)
{
    char *server = cls.servername;
    int len = strlen(server);

    if (len == 0 || !strcmp(server, "localhost")) {
        Com_Printf("Can't reconnect to localhost.\n");
        return;
    }

    Cbuf_AddText(va("connect %s\n", server));
}

void CL_OpenedIWDList_f(void)
{
    Com_Printf("Opened IWD Names: %s\n", FS_LoadedIwdNames());
}

void CL_ReferencedIWDList_f(void)
{
    Com_Printf("Referenced IWD Names: %s\n", FS_ReferencedIwdNames());
}

void CL_Configstrings_f(void)
{
    int i;
    clientActive_t *c;
    int offset;

    if (::clc.state != 8) {
        Com_Printf("Not connected to a server.\n");
        return;
    }

    c = clients;
    for (i = 0; i < 0x800; i++) {
        offset = c->gameState.stringOffsets[i];
        if (offset)
            Com_Printf("%4i: %s\n", i, c->gameState.stringData + offset);
    }
}

extern char *Dvar_InfoString(int bit);
extern void Info_Print(const char *s);
void CL_Clientinfo_f(void)
{

    Com_Printf("--------- Client Information ---------\n");
    Com_Printf("state: %i\n", ::clc.state);
    Com_Printf("Server: %s\n", cls.servername);
    Com_Printf("User info settings:\n");
    Info_Print(Dvar_InfoString(2));
    Com_Printf("--------------------------------------\n");
}

extern int MSG_ReadByte(msg_t *msg);
extern void MSG_ReadData(msg_t *msg, void *data, int len);
extern Bool CL_IsPlayerMuted(int clientNum);
extern void Voice_IncomingVoiceData(int talker, unsigned char *data, int packetDataSize);
void CL_VoicePacket(msg_t *msg)
{
    byte voiceData[256];
    int numPackets, packet, talker, dataLen;

    numPackets = MSG_ReadByte(msg);
    if (numPackets > 40 || numPackets <= 0)
        return;

    for (packet = 0; packet < numPackets; packet++) {
        talker = MSG_ReadByte(msg);
        dataLen = MSG_ReadByte(msg);

        if (dataLen < 1 || dataLen > 256) {
            Com_Printf("Invalid server voice packet of %i bytes\n", dataLen);
            return;
        }

        MSG_ReadData(msg, voiceData, dataLen);

        if (talker > 63) {
            Com_Printf("Invalid voice packet - talker was %i\n", talker);
            return;
        }

        if (CL_IsPlayerMuted(talker))
            continue;
        if (!cl_voice->current.enabled)
            continue;

        Voice_IncomingVoiceData(talker, voiceData, dataLen);
    }
}

Bool CL_IsPlayerTalking(int clientIndex)
{
    return Voice_IsClientTalking(clientIndex);
}

void CL_SetupForNewServerMap(const char *pszMapName, const char *pszGametype)
{
    LegacyHacks *hacks = (LegacyHacks *)::legacyHacks;

    Com_Printf("Server changing map %s, gametype %s\n", pszMapName, pszGametype);
    I_strncpyz(hacks->cl_serverloadmap, pszMapName, 0x40);
    I_strncpyz(hacks->cl_serverloadgametype, pszGametype, 0x40);
    hacks->cl_serverloadwaiting = 0;

    if (!(com_sv_running)->current.enabled) {
        Dvar_SetInt((dvar_t *)com_expectedHunkUsage, 0);
        g_waitingForServer = 1;
    }

    UI_SetMap(pszMapName, pszGametype);
    SCR_UpdateScreenInternal();
}

extern void CL_WriteVoicePacket(void);
void CL_VoiceTransmit(void)
{
    int voiceLen = clients[0].voicePacketCount;
    clientActive_t *c;

    if (voiceLen <= 0)
        return;

    if (clients[0].serverTime - clients[0].voicePacketLastTransmit > 199 || voiceLen > 9) {
        CL_WriteVoicePacket();
        c = clients;
        c->voicePacketCount = 0;
        c->voicePacketLastTransmit = c->serverTime;
    }
}

extern int Dvar_GetInt(const char *name);
extern Bool IsTalking(void);
int Voice_SendVoiceData(void)
{

    if (!(sv_voice)->current.enabled)
        return 0;
    if (!cl_voice->current.enabled)
        return 0;

    if (Dvar_GetInt("rate") <= 0x1387)
        return 0;

    if (clc.state != 8)
        return 0;

    if ((cl_talking)->current.enabled)
        return 1;
    if (IsTalking())
        return 1;

    {
        int vpc = clients->voicePacketCount;
        if (vpc)
            return 1;
    }

    return 0;
}

void CL_SyncGpu(void)
{
    re.SyncGpu();
}

void CL_SetRecommended_f(void)
{
    Com_SetRecommended(1);
}

void CL_RefPrintf(int print_level, const char *fmt, ...)
{
    char msg[4096];
    __builtin_va_list argptr;

    __builtin_va_start(argptr, fmt);
    vsprintf(msg, fmt, argptr);
    __builtin_va_end(argptr);

    if (print_level == 0) {
        Com_Printf("%s", msg);
    } else if (print_level == 2) {
        Com_Printf("^3%s", msg);
    } else if (print_level == 3) {
        Com_Printf("^1%s", msg);
    } else if (print_level == 1) {
        Com_DPrintf("^1%s", msg);
    }
}

extern void StatMon_Reset(void);
extern void StatMon_Warning(int type, int duration, const char *materialName);
extern void Sys_ShowSplashWindow(void);
extern void Sys_HideSplashWindow(void);
extern void CIN_UploadCinematic(int handle);
extern int CIN_PlayCinematic(const char *name, int x, int y, int w, int h, int bits);
extern e_status CIN_RunCinematic(int handle);
extern void CG_DObjCalcPose(const centity_s *cent, const DObj_s *obj, int *partBits);
extern const char *CL_GetHudMsgIconMaterialName(int iconIndex);
extern refexport_t re;
void CL_ShutdownRef(void)
{
    void (*syncRenderThread)(void) = re.SyncRenderThread;

    if (syncRenderThread)
        syncRenderThread();

    if (re.Shutdown) {
        ((void (*)(int))re.Shutdown)(1);
        memset(&re, 0, sizeof(re));
    }

    StatMon_Reset();
}

void CL_InitRenderer(void)
{
    int fieldWidth;

    re.BeginRegistration(&cls.vidConfig);
    _controlfp(_PC_24, _MCW_PC);
    _controlfp(_MCW_EM, _MCW_EM);
    SetScreenScaling(1.0f, 1.0f, 0, 0, cls.vidConfig.width, cls.vidConfig.height);

    cls.whiteMaterial = re.RegisterMaterial("white", 3, 3);
    cls.consoleMaterial = re.RegisterMaterial("console", 3, 3);
    cls.consoleFont = re.RegisterFont("fonts/consoleFont", 3);

    fieldWidth = cls.vidConfig.width - 0x20;
    g_console_field_width = fieldWidth;

    {
        field_t *cf = (&g_consoleField);
        cf->widthInPixels = fieldWidth;
        *(float *)&cf->charHeight = g_console_char_height;
        cf->fixedSize = 1;
    }

    StatMon_Reset();
}

void CL_StartHunkUsers(void)
{
    LegacyHacks *hacks = (LegacyHacks *)::legacyHacks;

    if (!hacks->cl_running)
        return;

    if (!cls.soundStarted) {
        cls.soundStarted = 1;
        SND_Init();
        Sys_LoadingKeepAlive();
    }

    if (!cls.rendererStarted) {
        cls.rendererStarted = 1;
        CL_InitRenderer();
        Sys_LoadingKeepAlive();
    }

    if (!cls.uiStarted) {
        CL_InitUI();
        Sys_LoadingKeepAlive();
    }

    cls.hunkUsersStarted = 1;
}

int CL_ScaledMilliseconds(void)
{
    return cls.realtime;
}

#ifdef COD2_GFX_DLL

extern void *__stdcall LoadLibraryA(const char *name);
extern void *__stdcall GetProcAddress(void *mod, const char *proc);
extern unsigned long __stdcall GetLastError(void);
void *g_gfxRendererModule;
refexport_t *CL_LoadRendererDll(int apiVersion, refimport_t *ri_in)
{
    const char *name = "gfx_d3d_mp_x86_s.dll";
    void *h;
    refexport_t *(*pGetRefAPI)(int, refimport_t *);
    h = LoadLibraryA(name);
    if (!h) {
        Com_Error(ERR_FATAL, "CL_InitRef: could not load renderer DLL '%s' (err %lu)",
                  name, (unsigned long)GetLastError());
        return NULL;
    }
    pGetRefAPI = (refexport_t * (*)(int, refimport_t *)) GetProcAddress(h, "GetRefAPI");
    if (!pGetRefAPI) {
        Com_Error(ERR_FATAL, "CL_InitRef: '%s' has no GetRefAPI export", name);
        return NULL;
    }
    Com_Printf("Loaded renderer module: %s\n", name);
    g_gfxRendererModule = h;
    return pGetRefAPI(apiVersion, ri_in);
}
#endif

int g_gfxV60DllActive = 0;

void *g_gfxV60AddRefEntity = 0;

#if defined(COD2_GFX_DLL) && defined(COD2_GFX_DLL_RETAIL_V60)
#    include "PC/win32/gfx_dll_v60.h"
#endif

void CL_InitRef(void)
{
    refimport_t ri_local;

    Com_Printf("----- Initializing Renderer ----\n");

    *(void **)&ri_local.Cmd_AddCommand = (void *)imp_Cmd_AddCommand;
    *(void **)&ri_local.Cmd_RemoveCommand = (void *)imp_Cmd_RemoveCommand;
    *(void **)&ri_local.Cmd_Argc = (void *)imp_Cmd_Argc;
    *(void **)&ri_local.Cmd_Argv = (void *)imp_Cmd_Argv;
    *(void **)&ri_local.Cbuf_ExecuteText = (void *)imp_Cbuf_ExecuteText;
    *(void **)&ri_local.Printf = (void *)CL_RefPrintf;
    *(void **)&ri_local.Error = (void *)imp_Com_Error;
    *(void **)&ri_local.Milliseconds = (void *)CL_ScaledMilliseconds;
    *(void **)&ri_local.Hunk_AllocInternal = (void *)imp_Hunk_AllocInternal;
    *(void **)&ri_local.Hunk_AllocateTempMemoryInternal = (void *)imp_Hunk_AllocateTempMemoryInternal;
    *(void **)&ri_local.Z_MallocInternal = (void *)imp_Z_MallocInternal;
    *(void **)&ri_local.Z_FreeInternal = (void *)imp_Z_FreeInternal;
    ri_local.Hunk_AllocAlignInternal = Hunk_AllocAlignInternal;
    *(void **)&ri_local.Z_VirtualReserveInternal = (void *)imp_Z_VirtualReserveInternal;
    *(void **)&ri_local.Z_VirtualCommitInternal = (void *)imp_Z_VirtualCommitInternal;
    *(void **)&ri_local.Z_VirtualDecommitInternal = (void *)imp_Z_VirtualDecommitInternal;
    *(void **)&ri_local.Z_VirtualFreeInternal = (void *)imp_Z_VirtualFreeInternal;
    *(void **)&ri_local.Hunk_FreeTempMemory = (void *)imp_Hunk_FreeTempMemory;
    *(void **)&ri_local.Hunk_AllocateTempMemoryHighInternal = (void *)imp_Hunk_AllocateTempMemoryHighInternal;
    *(void **)&ri_local.Hunk_ClearTempMemory = (void *)imp_Hunk_ClearTempMemory;
    *(void **)&ri_local.Hunk_ClearTempMemoryHigh = (void *)imp_Hunk_ClearTempMemoryHigh;
    *(void **)&ri_local.Sys_DirectXFatalError = (void *)imp_Sys_DirectXFatalError;
    *(void **)&ri_local.Sys_ShowSplashWindow = (void *)Sys_ShowSplashWindow;
    *(void **)&ri_local.Sys_HideSplashWindow = (void *)Sys_HideSplashWindow;
    *(void **)&ri_local.Sys_LoadingKeepAlive = (void *)Sys_LoadingKeepAlive;
    *(void **)&ri_local.FS_ReadFile = (void *)imp_FS_ReadFile;
    *(void **)&ri_local.FS_FreeFile = (void *)imp_FS_FreeFile;
    *(void **)&ri_local.FS_FOpenFileRead = (void *)imp_FS_FOpenFileRead;
    *(void **)&ri_local.FS_WriteFile = (void *)imp_FS_WriteFile;
    *(void **)&ri_local.FS_FreeFileList = (void *)imp_FS_FreeFileList;
    *(void **)&ri_local.FS_ListFiles = (void *)imp_FS_ListFiles;
    *(void **)&ri_local.FS_FileExists = (void *)imp_FS_FileExists;
    *(void **)&ri_local.FS_FOpenFileByMode = (void *)imp_FS_FOpenFileByMode;
    *(void **)&ri_local.FS_FCloseFile = (void *)imp_FS_FCloseFile;
    *(void **)&ri_local.FS_Read = (void *)imp_FS_Read;
    *(void **)&ri_local.FS_Write = (void *)imp_FS_Write;
    *(void **)&ri_local.CM_SaveLump = (void *)imp_CM_SaveLump;
    *(void **)&ri_local.CM_BoxTrace = (void *)imp_CM_BoxTrace;
    ri_local.CM_BoxSightTrace = CM_BoxSightTrace;
    *(void **)&ri_local.Dvar_RegisterBool = (void *)imp_Dvar_RegisterBool;
    *(void **)&ri_local.Dvar_RegisterInt = (void *)imp_Dvar_RegisterInt;
    *(void **)&ri_local.Dvar_RegisterFloat = (void *)imp_Dvar_RegisterFloat;
    *(void **)&ri_local.Dvar_RegisterString = (void *)imp_Dvar_RegisterString;
    *(void **)&ri_local.Dvar_RegisterEnum = (void *)imp_Dvar_RegisterEnum;
    *(void **)&ri_local.Dvar_RegisterColor = (void *)imp_Dvar_RegisterColor;
    *(void **)&ri_local.Dvar_RegisterVec2 = (void *)imp_Dvar_RegisterVec2;
    *(void **)&ri_local.Dvar_RegisterVec3 = (void *)imp_Dvar_RegisterVec3;
    *(void **)&ri_local.Dvar_RegisterVec4 = (void *)imp_Dvar_RegisterVec4;
    *(void **)&ri_local.Dvar_ClearModified = (void *)imp_Dvar_ClearModified;
    *(void **)&ri_local.Dvar_SetModified = (void *)imp_Dvar_SetModified;
    *(void **)&ri_local.Dvar_UpdateEnumDomain = (void *)imp_Dvar_UpdateEnumDomain;
    *(void **)&ri_local.Dvar_UnregisterSystem = (void *)imp_Dvar_UnregisterSystem;
    *(void **)&ri_local.Dvar_ChangeResetValue = (void *)imp_Dvar_ChangeResetValue;
    *(void **)&ri_local.Dvar_IsAtDefaultValue = (void *)imp_Dvar_IsAtDefaultValue;
    *(void **)&ri_local.Dvar_SetBool = (void *)imp_Dvar_SetBool;
    *(void **)&ri_local.Dvar_SetInt = (void *)imp_Dvar_SetInt;
    *(void **)&ri_local.Dvar_SetFloat = (void *)imp_Dvar_SetFloat;
    *(void **)&ri_local.Dvar_SetString = (void *)imp_Dvar_SetString;
    *(void **)&ri_local.Dvar_SetColor = (void *)imp_Dvar_SetColor;
    *(void **)&ri_local.Dvar_SetVec2 = (void *)imp_Dvar_SetVec2;
    *(void **)&ri_local.Dvar_SetVec3 = (void *)imp_Dvar_SetVec3;
    *(void **)&ri_local.Dvar_SetVec4 = (void *)imp_Dvar_SetVec4;
    *(void **)&ri_local.Dvar_SetFromString = (void *)imp_Dvar_SetFromString;
    *(void **)&ri_local.Dvar_SetBoolByName = (void *)imp_Dvar_SetBoolByName;
    *(void **)&ri_local.Dvar_SetIntByName = (void *)imp_Dvar_SetIntByName;
    *(void **)&ri_local.Dvar_SetFloatByName = (void *)imp_Dvar_SetFloatByName;
    *(void **)&ri_local.Dvar_SetStringByName = (void *)imp_Dvar_SetStringByName;
    *(void **)&ri_local.Dvar_SetColorByName = (void *)imp_Dvar_SetColorByName;
    *(void **)&ri_local.Dvar_SetVec2ByName = (void *)imp_Dvar_SetVec2ByName;
    *(void **)&ri_local.Dvar_SetVec3ByName = (void *)imp_Dvar_SetVec3ByName;
    *(void **)&ri_local.Dvar_SetVec4ByName = (void *)imp_Dvar_SetVec4ByName;
    *(void **)&ri_local.Dvar_SetFromStringByName = (void *)imp_Dvar_SetFromStringByName;
    *(void **)&ri_local.Dvar_GetBool = (void *)imp_Dvar_GetBool;
    *(void **)&ri_local.Dvar_GetInt = (void *)imp_Dvar_GetInt;
    *(void **)&ri_local.Dvar_GetFloat = (void *)imp_Dvar_GetFloat;
    *(void **)&ri_local.Dvar_GetString = (void *)imp_Dvar_GetString;
    *(void **)&ri_local.Dvar_GetVariantString = (void *)imp_Dvar_GetVariantString;
    *(void **)&ri_local.Dvar_EnumToString = (void *)imp_Dvar_EnumToString;
    *(void **)&ri_local.Dvar_Reset = (void *)imp_Dvar_Reset;
    *(void **)&ri_local.Com_SaveDvarsToBuffer = (void *)imp_Com_SaveDvarsToBuffer;
    *(void **)&ri_local.Com_LoadDvarsFromBuffer = (void *)imp_Com_LoadDvarsFromBuffer;
    *(void **)&ri_local.Com_GetBsp = (void *)imp_Com_GetBsp;
    *(void **)&ri_local.SEH_ReadCharFromString = (void *)imp_SEH_ReadCharFromString;
    *(void **)&ri_local.CL_UpdateDebugData = (void *)CL_UpdateDebugData;
    *(void **)&ri_local.CL_FlushDebugData = (void *)CL_FlushDebugData;
    *(void **)&ri_local.StatMon_Warning = (void *)StatMon_Warning;
    *(void **)&ri_local.XModelPrecache = (void *)imp_XModelPrecache;
    *(void **)&ri_local.XModelGetSurfaces = (void *)imp_XModelGetSurfaces;
    *(void **)&ri_local.XModelBad = (void *)imp_XModelBad;
    *(void **)&ri_local.Hunk_OverrideDataForFile = (void *)imp_Hunk_OverrideDataForFile;
    *(void **)&ri_local.XModelGetNumLods = (void *)imp_XModelGetNumLods;
    *(void **)&ri_local.XModelSetTestLods = (void *)imp_XModelSetTestLods;
    *(void **)&ri_local.XModelGetLodForDist = (void *)imp_XModelGetLodForDist;
    *(void **)&ri_local.XModelGetLodOutDist = (void *)imp_XModelGetLodOutDist;
    *(void **)&ri_local.XModelGetSurfaceName = (void *)imp_XModelGetSurfaceName;
    *(void **)&ri_local.XModelGetName = (void *)imp_XModelGetName;
    *(void **)&ri_local.XModelGetFlags = (void *)imp_XModelGetFlags;
    *(void **)&ri_local.XModelNumBones = (void *)imp_XModelNumBones;
    *(void **)&ri_local.XModelGetSkins = (void *)imp_XModelGetSkins;
    *(void **)&ri_local.XModelGetMemUsage = (void *)imp_XModelGetMemUsage;
    *(void **)&ri_local.XModelGetLodName = (void *)imp_XModelGetLodName;
    *(void **)&ri_local.XModelGetBasePose = (void *)imp_XModelGetBasePose;
    *(void **)&ri_local.XModelGetBasePoseBone = (void *)imp_XModelGetBasePoseBone;
    *(void **)&ri_local.DObjBad = (void *)imp_DObjBad;
    *(void **)&ri_local.DObjGetBounds = (void *)imp_DObjGetBounds;
    *(void **)&ri_local.DObjGetSurface = (void *)imp_DObjGetSurface;
    *(void **)&ri_local.DObjGetNumModels = (void *)imp_DObjGetNumModels;
    *(void **)&ri_local.DObjGetNumSurfaces = (void *)imp_DObjGetNumSurfaces;
    *(void **)&ri_local.DObjGetSurfaces = (void *)imp_DObjGetSurfaces;
    *(void **)&ri_local.DObjGetModel = (void *)imp_DObjGetModel;
    *(void **)&ri_local.DObjGetSurfaceName = (void *)imp_DObjGetSurfaceName;
    *(void **)&ri_local.DObjCreate = (void *)imp_DObjCreate;
    *(void **)&ri_local.DObjGetAllocSkelSize = (void *)imp_DObjGetAllocSkelSize;
    *(void **)&ri_local.DObjCreateSkel = (void *)imp_DObjCreateSkel;
    *(void **)&ri_local.DObjCalcAnim = (void *)imp_DObjCalcAnim;
    *(void **)&ri_local.DObjCalcSkel = (void *)imp_DObjCalcSkel;
    *(void **)&ri_local.DObjGetRotTransArray = (void *)imp_DObjGetRotTransArray;
    *(void **)&ri_local.DObjSkelAreBonesUpToDate = (void *)imp_DObjSkelAreBonesUpToDate;
    *(void **)&ri_local.DObjGetMatOffset = (void *)imp_DObjGetMatOffset;
    *(void **)&ri_local.DObjNumBones = (void *)imp_DObjNumBones;
    *(void **)&ri_local.DObjGetBoneInfo = (void *)imp_DObjGetBoneInfo;
    *(void **)&ri_local.DObjGetLodForDist = (void *)imp_DObjGetLodForDist;
    *(void **)&ri_local.DObjGetLodOutDist = (void *)imp_DObjGetLodOutDist;
    *(void **)&ri_local.DObjCompleteHierarchyBits = (void *)imp_DObjCompleteHierarchyBits;
    *(void **)&ri_local.DObjSetModel = (void *)imp_DObjSetModel;
    *(void **)&ri_local.CIN_UploadCinematic = (void *)CIN_UploadCinematic;
    *(void **)&ri_local.CIN_PlayCinematic = (void *)CIN_PlayCinematic;
    *(void **)&ri_local.CIN_RunCinematic = (void *)CIN_RunCinematic;
    *(void **)&ri_local.CG_DObjCalcPose = (void *)CG_DObjCalcPose;
    *(void **)&ri_local.CL_GetHudMsgIconMaterialName = (void *)CL_GetHudMsgIconMaterialName;
    {

        extern cplane_t * CM_GetPlaneNum(int planeNum);
        *(void **)&ri_local.CM_GetPlaneNum = (void *)CM_GetPlaneNum;
    }

#if defined(COD2_GFX_DLL) && defined(COD2_GFX_DLL_RETAIL_V60)

    gfxV60_LoadRetailRenderer(&ri_local, &re);
#elif defined(COD2_GFX_DLL)

    {
        extern refimport_t ri;
        extern int g_gfxV60DllActive;
        extern void *g_gfxV60AddRefEntity;
        memcpy(&ri, &ri_local, sizeof(ri_local));
        memcpy(&re, CL_LoadRendererDll(0x3b, &ri_local), sizeof(re));

        g_gfxV60AddRefEntity = GetProcAddress(g_gfxRendererModule, "R_AddRefEntityToScene");
        g_gfxV60DllActive = 1;
    }
#else

    {
        GetRefAPI_t getRefAPI;
        refexport_t *ret;

        getRefAPI = Sys_LoadRenderer();
        if (!getRefAPI)
            Com_Error(ERR_FATAL, "Failed to load renderer.");
        ret = getRefAPI(0x3b, &ri_local);
        Com_Printf("-------------------------------\n");
        if (!ret) {
            Sys_UnloadRenderer();
            Com_Error(ERR_FATAL, "Failed to initialize renderer: version mismatch.");
        }
        memcpy(&re, ret, sizeof(re));
    }
#endif

    Dvar_SetInt((dvar_t *)cl_paused, 0);
}

void CL_startSingleplayer_f(void)
{
    Sys_StartProcess("cod2sp_s.exe", 1);
}

void CL_StopLogo(void)
{
    *(int *)&::clc = 0;
}

void CL_ToggleMenu_f(void)
{
    if (::clc.demoplaying != 0 || legacyHacks->cl_serverloadwaiting != 0) {
        UI_SetActiveMenu(1);
    } else {
        UI_SetActiveMenu(2);
    }
}

void CL_OpenScriptMenu_f(void)
{
    const char *parentMenuName, *menuResponse;
    const char *configStr;
    int menuIndex;

    if (Cmd_Argc() != 3) {
        Com_Printf("USAGE: openscriptmenu <parent menu name> <script menu response>\n");
        Com_Printf("EXAMPLE: openscriptmenu ingame changeweapon\n");
        return;
    }

    if (!legacyHacks->ui_scriptMenuAllowResponse)
        return;
    if (!cls.uiStarted)
        return;

    parentMenuName = Cmd_Argv(1);
    menuResponse = Cmd_Argv(2);

    if (!parentMenuName || !menuResponse)
        return;

    for (menuIndex = 0; menuIndex < 0x20; menuIndex++) {
        configStr = CL_GetConfigString(0x4de + menuIndex);
        if (*configStr && I_stricmp(parentMenuName, configStr) == 0)
            break;
    }
    if (menuIndex == 0x20)
        menuIndex = -1;

    Cbuf_ExecuteText(2, va("cmd mr %i %i %s\n", Dvar_GetInt("sv_serverId"), menuIndex, menuResponse));
}

void CL_InitOnceForAllClients(void)
{
    srand(Sys_MillisecondsRaw());
    Con_Init();
    CL_InitInput();

    cl_noprint = Dvar_RegisterBool("cl_noprint", 0, 0x1000);
    cl_voice = Dvar_RegisterBool("cl_voice", 1, 0x1003);
    cl_timeout = Dvar_RegisterFloat("cl_timeout", 200.0f, 0.0f, 3600.0f, 0x1000);
    cl_connectTimeout = Dvar_RegisterFloat("cl_connectTimeout", 200.0f, 0.0f, 3600.0f, 0x1000);
    cl_shownet = Dvar_RegisterInt("cl_shownet", 0, -2, 4, 0x1000);
    cl_shownuments = Dvar_RegisterBool("cl_shownuments", 0, 0x1000);
    cl_showServerCommands = Dvar_RegisterBool("cl_showServerCommands", 0, 0x1000);
    cl_showSend = Dvar_RegisterBool("cl_showSend", 0, 0x1000);
    cl_showTimeDelta = Dvar_RegisterBool("cl_showTimeDelta", 0, 0x1000);
    cl_freezeDemo = Dvar_RegisterBool("cl_freezeDemo", 0, 0x1000);
    cl_activeAction = Dvar_RegisterString("activeAction", "", 0x1000);
    cl_avidemo = Dvar_RegisterInt("cl_avidemo", 0, 0, 0x7fffffff, 0x1000);
    cl_forceavidemo = Dvar_RegisterBool("cl_forceavidemo", 0, 0x1000);

    cl_yawspeed = Dvar_RegisterFloat("cl_yawspeed", 140.0f, -FLT_MAX, FLT_MAX, 0x1001);
    cl_pitchspeed = Dvar_RegisterFloat("cl_pitchspeed", 140.0f, -FLT_MAX, FLT_MAX, 0x1001);
    cl_anglespeedkey = Dvar_RegisterFloat("cl_anglespeedkey", 1.5f, 0.0f, FLT_MAX, 0x1000);

    cl_maxpackets = Dvar_RegisterInt("cl_maxpackets", 30, 15, 100, 0x1001);
    cl_packetdup = Dvar_RegisterInt("cl_packetdup", 1, 0, 5, 0x1001);
    cl_sensitivity = Dvar_RegisterFloat("sensitivity", 5.0f, 0.01f, 100.0f, 0x1001);
    cl_mouseAccel = Dvar_RegisterFloat("cl_mouseAccel", 0.0f, 0.0f, 100.0f, 0x1001);
    cl_freelook = Dvar_RegisterBool("cl_freelook", 1, 0x1001);
    cl_showMouseRate = Dvar_RegisterBool("cl_showmouserate", 0, 0x1000);
    cl_allowDownload = Dvar_RegisterBool("cl_allowDownload", 1, 0x1001);
#if COD2_IS_PATCH_13
    cl_wwwDownload = Dvar_RegisterBool("cl_wwwDownload", 1, 0x1003);
    cl_punkbuster = Dvar_RegisterBool("cl_punkbuster", 0, 0x1013);
#endif
    cl_talking = Dvar_RegisterBool("cl_talking", 0, 0x1000);
    cl_inGameVideo = Dvar_RegisterBool("r_inGameVideo", 1, 0x1001);
    cl_serverStatusResendTime = Dvar_RegisterInt("cl_serverStatusResendTime", 750, 0, 3600, 0x1000);
    cl_bypassMouseInput = Dvar_RegisterBool("cl_bypassMouseInput", 0, 0x1000);

    m_pitch = Dvar_RegisterFloat("m_pitch", 0.022f, -1.0f, 1.0f, 0x1001);
    m_yaw = Dvar_RegisterFloat("m_yaw", 0.022f, -1.0f, 1.0f, 0x1001);
    m_forward = Dvar_RegisterFloat("m_forward", 0.25f, -1.0f, 1.0f, 0x1001);
    m_side = Dvar_RegisterFloat("m_side", 0.25f, -1.0f, 1.0f, 0x1001);
    m_filter = Dvar_RegisterBool("m_filter", 0, 0x1001);
    input_viewSensitivity = Dvar_RegisterFloat("input_viewSensitivity", 1.0f, 0.0001f, 5.0f, 0x1001);
    cg_drawCrosshair = Dvar_RegisterBool("cg_drawCrosshair", 1, 0x1001);
    cl_motdString = Dvar_RegisterString("cl_motdString", "", 0x1040);
    cl_ingame = Dvar_RegisterBool("cl_ingame", 0, 0x1040);
    Dvar_RegisterInt("cl_maxPing", 800, 20, 2000, 0x1001);
    name = Dvar_RegisterString("name", "Unknown Soldier", 0x1003);
    Dvar_RegisterInt("rate", 5000, 1000, 25000, 0x1003);
    Dvar_RegisterInt("snaps", 20, 1, 30, 0x1003);
    Dvar_RegisterString("password", "", 0x1002);

    fx_enable = Dvar_RegisterBool("fx_enable", 1, 0x1080);
    fx_draw = Dvar_RegisterBool("fx_draw", 1, 0x1080);
    fx_cull = Dvar_RegisterBool("fx_cull", 1, 0x1000);
    fx_sort = Dvar_RegisterBool("fx_sort", 1, 0x1000);
    fx_freeze = Dvar_RegisterBool("fx_freeze", 0, 0x1080);
    fx_debug = Dvar_RegisterBool("fx_debug", 0, 0x1080);
    fx_debugBolt = Dvar_RegisterFloat("fx_debugBolt", 0.0f, 0.0f, 100.0f, 0x1080);
    fx_count = Dvar_RegisterBool("fx_count", 0, 0x1080);
    fx_visMinTraceDist = Dvar_RegisterFloat("fx_visMinTraceDist", 80.0f, 0.0f, 1000.0f, 0x1080);
    fx_profile = Dvar_RegisterBool("fx_profile", 0, 0x1080);
    nextdemo = Dvar_RegisterString("nextdemo", "", 0x1000);
    Dvar_RegisterBool("hud_enable", 1, 0x1001);
    Dvar_RegisterBool("cg_blood", 1, 0x1001);
    cl_updateavailable = Dvar_RegisterBool("cl_updateavailable", 0, 0x1040);
    cl_updatefiles = Dvar_RegisterString("cl_updatefiles", "", 0x1040);
    cl_updateoldversion = Dvar_RegisterString("cl_updateoldversion", "", 0x1040);
    cl_updateversion = Dvar_RegisterString("cl_updateversion", "", 0x1040);
    I_strncpyz(cls.autoupdateServerNames[0], "cod2update.activision.com", 64);
    I_strncpyz(cls.autoupdateServerNames[1], "cod2update2.activision.com", 64);
    I_strncpyz(cls.autoupdateServerNames[2], "cod2update3.activision.com", 64);
    I_strncpyz(cls.autoupdateServerNames[3], "cod2update4.activision.com", 64);
    I_strncpyz(cls.autoupdateServerNames[4], "cod2update5.activision.com", 64);

    Cmd_AddCommand("cmd", CL_ForwardToServer_f);
    Cmd_AddCommand("configstrings", CL_Configstrings_f);
    Cmd_AddCommand("clientinfo", CL_Clientinfo_f);
    Cmd_AddCommand("vid_restart", CL_Vid_Restart_f);
    Cmd_AddCommand("snd_restart", CL_Snd_Restart_f);
    Cmd_AddCommand("disconnect", CL_Disconnect_f);
    Cmd_AddCommand("record", CL_Record_f);
    Cmd_AddCommand("stoprecord", CL_StopRecord_f);
    Cmd_AddCommand("demo", CL_PlayDemo_f);
    Cmd_AddCommand("timedemo", CL_PlayDemo_f);
    Cmd_SetAutoComplete("demo", "demos", "dm_1");
    Cmd_SetAutoComplete("timedemo", "demos", "dm_1");
    Cmd_AddCommand("cinematic", CL_PlayCinematic_f);
    Cmd_SetAutoComplete("cinematic", "video", "roq");
    Cmd_AddCommand("logo", CL_PlayLogo_f);
    Cmd_AddCommand("connect", CL_Connect_f);
    Cmd_AddCommand("reconnect", CL_Reconnect_f);
    Cmd_AddCommand("localservers", CL_LocalServers_f);
    Cmd_AddCommand("globalservers", CL_GlobalServers_f);
    CL_RconInit();
    Cmd_AddCommand("rcon", CL_Rcon_f);
    Cmd_AddCommand("ping", CL_Ping_f);
    Cmd_AddCommand("serverstatus", CL_ServerStatus_f);
    Cmd_AddCommand("toggleMenu", CL_ToggleMenu_f);
    Cmd_AddCommand("setenv", CL_Setenv_f);
    Cmd_AddCommand("showip", CL_ShowIP_f);
    Cmd_AddCommand("fs_openedList", CL_OpenedIWDList_f);
    Cmd_AddCommand("fs_referencedList", CL_ReferencedIWDList_f);
    Cmd_AddCommand("updatehunkusage", CL_UpdateLevelHunkUsage);
    Cmd_AddCommand("updatescreen", SCR_UpdateScreen);
    Cmd_AddCommand("startSingleplayer", CL_startSingleplayer_f);
    Cmd_AddCommand("setRecommended", CL_SetRecommended_f);
    Cmd_AddCommand("cubemapShot", CL_CubemapShot_f);
    Cmd_AddCommand("openScriptMenu", CL_OpenScriptMenu_f);
    Cmd_AddCommand("localizeSoundAliasFiles", Com_WriteLocalizedSoundAliasFiles);
    Cmd_AddCommand("openmenu", UI_OpenMenu_f);
    Cmd_AddCommand("closemenu", UI_CloseMenu_f);

    autoupdateChecked = 0;
    autoupdateStarted = 0;
    CL_CheckAutoUpdate();
    CL_InitRef();
    SCR_Init();
}

extern "C" void Z_FreeInternal( void *ptr );
void CL_UpdateInfoPacket(netadr_t from)
{
    if (cls.autoupdateServer.type == NA_BAD) {
        Com_DPrintf("CL_UpdateInfoPacket:  Auto-Updater has bad address\n");
        return;
    }

    Com_DPrintf("Auto-Updater resolved to %i.%i.%i.%i:%i\n",
                cls.autoupdateServer.ip[0], cls.autoupdateServer.ip[1],
                cls.autoupdateServer.ip[2], cls.autoupdateServer.ip[3],
                BigShort(cls.autoupdateServer.port));

    if (!NET_CompareAdr(from, cls.autoupdateServer)) {
        Com_DPrintf("CL_UpdateInfoPacket:  Received packet from %i.%i.%i.%i:%i\n",
                    from.ip[0], from.ip[1], from.ip[2], from.ip[3],
                    BigShort(from.port));
        return;
    }

    Dvar_SetBool(cl_updateavailable, atoi(Cmd_Argv(1)) != 0);

    if (cl_updateavailable->current.enabled) {
        Dvar_SetString(cl_updatefiles, Cmd_Argv(2));
        Dvar_SetString(cl_updateversion, Cmd_Argv(3));
        Dvar_SetString(cl_updateoldversion, "1.0");
    }
}

void CL_ShutdownDebugData(void)
{
    void (*shutdownDebug)(void);

    if (cls.debug.lines) {
        Z_FreeInternal(cls.debug.lines);
        cls.debug.lines = 0;
    }
    if (cls.debug.lineFromServer) {
        Z_FreeInternal(cls.debug.lineFromServer);
        cls.debug.lineFromServer = 0;
    }
    if (cls.debug.lineDuration) {
        Z_FreeInternal(cls.debug.lineDuration);
        cls.debug.lineDuration = 0;
    }
    if (cls.debug.strings) {
        Z_FreeInternal(cls.debug.strings);
        cls.debug.strings = 0;
    }
    if (cls.debug.stringFromServer) {
        Z_FreeInternal(cls.debug.stringFromServer);
        cls.debug.stringFromServer = 0;
    }

    memset(&cls.debug, 0, sizeof(cls.debug));

    shutdownDebug = re.ShutdownDebug;
    if (shutdownDebug)
        shutdownDebug();
}

void CL_GetPing(int n, char *buf, int buflen, int *pingtime)
{
    const char *str;
    int time;
    int maxPing;

    if (!cl_pinglist[n].adr.port) {
        // empty slot
        buf[0] = '\0';
        *pingtime = 0;
        return;
    }

    str = NET_AdrToString(cl_pinglist[n].adr);
    I_strncpyz(buf, str, buflen);

    time = cl_pinglist[n].time;
    if (!time) {
        // check for timeout
        time = Sys_Milliseconds() - cl_pinglist[n].start;
        maxPing = Dvar_GetInt("cl_maxPing");
        if (maxPing < 100)
            maxPing = 100;
        if (time < maxPing) {
            // not timed out yet
            time = 0;
        }
    }

    CL_SetServerInfoByAddress(cl_pinglist[n].adr, cl_pinglist[n].info, cl_pinglist[n].time);

    *pingtime = time;
}

void CL_UpdateServerInfo(int n)
{
    if (!cl_pinglist[n].adr.port)
        return;

    CL_SetServerInfoByAddress(cl_pinglist[n].adr, cl_pinglist[n].info, cl_pinglist[n].time);
}

void CL_GetPingInfo(int n, char *buf, int buflen)
{
    if (!cl_pinglist[n].adr.port) {
        // empty slot
        if (buflen)
            buf[0] = '\0';
        return;
    }

    I_strncpyz(buf, cl_pinglist[n].info, buflen);
}

void CL_ClearPing(int n)
{
    if (n < 0 || n >= 16)
        return;

    cl_pinglist[n].adr.port = 0;
}

int CL_GetPingQueueCount(void)
{
    int i;
    int count;
    ping_t *pingptr;

    count = 0;
    pingptr = cl_pinglist;

    for (i = 0; i < 16; i++, pingptr++) {
        if (pingptr->adr.port)
            count++;
    }

    return count;
}

void CL_ShowIP_f(void)
{
    Sys_ShowIP();
}

extern int Com_sprintf(char *dest, size_t size, const char *format, ...);
const char *CL_GetServerIPAddress(void)
{
    clientConnection_t *connection = &::clc;
    netadr_t *serverAddress = &connection->serverAddress;

    if (connection->state <= CA_CHALLENGING) {
        memset(szServerIPAddress, 0, 128);
    } else {
        unsigned short port = serverAddress->port;
        port = (port >> 8) | (port << 8);
        Com_sprintf(szServerIPAddress, 128, "%i.%i.%i.%i:%i",
                    serverAddress->ip[0], serverAddress->ip[1],
                    serverAddress->ip[2], serverAddress->ip[3],
                    (int)(short)port);
    }

    return szServerIPAddress;
}

void CL_FlushDebugData(qboolean fromServer)
{
    int i;

    if (!cls.rendererStarted)
        return;

    if (cls.debug.strings) {
        i = 0;
        while (i < cls.debug.numStrings) {
            if (cls.debug.stringFromServer[i] == fromServer) {
                --cls.debug.numStrings;
                cls.debug.stringFromServer[i] = cls.debug.stringFromServer[cls.debug.numStrings];
                memcpy(&cls.debug.strings[i], &cls.debug.strings[cls.debug.numStrings], sizeof(cls.debug.strings[i]));
            } else {
                ++i;
            }
        }

        re.LocateDebugStrings(cls.debug.strings, cls.debug.numStrings, cls.debug.maxStrings);
    }

    if (cls.debug.lines) {
        i = 0;
        while (i < cls.debug.numLines) {
            if (cls.debug.lineFromServer[i] == fromServer) {
                --cls.debug.lineDuration[i];
                if (cls.debug.lineDuration[i] > 0) {
                    ++i;
                } else {
                    --cls.debug.numLines;
                    cls.debug.lineFromServer[i] = cls.debug.lineFromServer[cls.debug.numLines];
                    cls.debug.lineDuration[i] = cls.debug.lineDuration[cls.debug.numLines];
                    memcpy(&cls.debug.lines[i], &cls.debug.lines[cls.debug.numLines], sizeof(cls.debug.lines[i]));
                }
            } else {
                ++i;
            }
        }

        re.LocateDebugLines(cls.debug.lines, cls.debug.numLines, cls.debug.maxLines);
    }
}

void CL_UpdateDebugData(void)
{
    if (!cls.rendererStarted)
        return;

    if (cls.debug.strings) {
        re.LocateDebugStrings(cls.debug.strings, cls.debug.numStrings, cls.debug.maxStrings);
    }

    if (cls.debug.lines) {
        re.LocateDebugLines(cls.debug.lines, cls.debug.numLines, cls.debug.maxLines);
    }
}

int CL_TextWidth(const char *text, int maxChars, FontHandle font)
{
    return re.TextWidth(text, maxChars, font);
}

int CL_TextHeight(FontHandle font)
{
    return re.TextHeight(font);
}

float CL_NormalizedTextScale(FontHandle font, float scale)
{
    return re.NormalizedTextScale(font, scale);
}

void CL_DrawTextPhysical(const char *text, int maxChars, FontHandle font, float x, float y, float xScale, float yScale, const vec_t *color, int style)
{
    re.DrawText(text, maxChars, font, x, y, xScale, yScale, color, style);
}

extern void CalcSplitScreenTextOffset(FontHandle font, float *y);
extern void CalcScreenPlacement(float *x, float *y, float *xScale, float *yScale, int horzAlign, int vertAlign);
void CL_DrawText(const char *text, int maxChars, FontHandle font, float x, float y, int horzAlign, int vertAlign, float xScale, float yScale, const vec_t *color, int style)
{
    CalcSplitScreenTextOffset(font, &y);
    CalcScreenPlacement(&x, &y, &xScale, &yScale, horzAlign, vertAlign);
    re.DrawText(text, maxChars, font, x, y, xScale, yScale, color, style);
}

void CL_DrawTextPhysicalWithCursor(const char *text, int maxChars, FontHandle font, float x, float y, float xScale, float yScale, const vec_t *color, int style, int cursorPos, int cursor)
{
    cursor = (signed char)cursor;
    re.DrawTextWithCursor(text, maxChars, font, x, y, xScale, yScale, color, style, cursorPos, cursor);
}

void CL_DrawTextWithCursor(const char *text, int maxChars, FontHandle font, float x, float y, int horzAlign, int vertAlign, float xScale, float yScale, const vec_t *color, int style, int cursorPos, int cursor)
{
    cursor = (signed char)cursor;
    CalcSplitScreenTextOffset(font, &y);
    CalcScreenPlacement(&x, &y, &xScale, &yScale, horzAlign, vertAlign);
    re.DrawTextWithCursor(text, maxChars, font, x, y, xScale, yScale, color, style, cursorPos, cursor);
}

int CL_GetKeyCatchers(void)
{
    return clients[0].keyCatchers;
}

Bool CL_GetDisplayHUDWithKeycatchUI(void)
{
    return clients[0].displayHUDWithKeycatchUI;
}

FontHandle CL_RegisterFont(const char *fontName, int imageTrack)
{
    return re.RegisterFont(fontName, imageTrack);
}

void CL_WriteDemoMessage(msg_t *msg, int headerBytes)
{
    clientConnection_t *cc = &::clc;
    clientConnection_t *conn;
    int swlen;
    int len;

    swlen = cc->serverMessageSequence;
    FS_Write(&swlen, 4, cc->demofile);

    len = msg->cursize - headerBytes;
    swlen = len;
    conn = &::clc;
    FS_Write(&swlen, 4, conn->demofile);

    FS_Write(msg->data + headerBytes, len, conn->demofile);
}

void CL_Record_f(void)
{
    clientConnection_t *conn = &::clc;
    clientActive_t *active = clients;
    char demoName[64];
    char name[256];
    msg_t buf;
    byte bufData[MAX_MSGLEN];
    byte compressedBuf[MAX_MSGLEN];
    entityState_t nullstate;
    int number;
    int i;
    int compressedSize;
    int len;

    if (Cmd_Argc() > 2) {
        Com_Printf((const char *)"record <demoname>\n");
        return;
    }

    if (conn->demorecording) {
        Com_Printf((const char *)"Already recording.\n");
        return;
    }

    if (conn->state != CA_ACTIVE) {
        Com_Printf((const char *)"You must be in a level to record.\n");
        return;
    }

    if (Cmd_Argc() == 2) {
        I_strncpyz(demoName, Cmd_Argv(1), sizeof(demoName));
        Com_sprintf(name, sizeof(name), (const char *)"demos/%s.dm_%d", demoName, 1);
    } else {
        number = 0;
        for (;;) {
            Com_sprintf(demoName, sizeof(demoName), (const char *)"demo%04i", number);
            Com_sprintf(name, sizeof(name), (const char *)"demos/%s.dm_%d", demoName, 1);
            if (!FS_FileExists(name))
                break;
            if (++number == 10000)
                break;
        }
    }

    Com_Printf((const char *)"recording to %s.\n", name);
    conn->demofile = FS_FOpenFileWrite(name);
    if (!conn->demofile) {
        Com_Printf((const char *)"ERROR: couldn't open.\n");
        return;
    }

    conn->demorecording = 1;
    I_strncpyz(conn->demoName, demoName, sizeof(conn->demoName));
    conn->demowaiting = 1;

    MSG_Init(&buf, bufData, sizeof(bufData));
    MSG_WriteLong(&buf, conn->reliableSequence);
    MSG_WriteByte(&buf, 1);
    MSG_WriteLong(&buf, conn->serverCommandSequence);

    for (i = 0; i < 2048; ++i) {
        int offset = active->gameState.stringOffsets[i];
        if (!offset)
            continue;

        MSG_WriteByte(&buf, 2);
        MSG_WriteShort(&buf, i);
        MSG_WriteBigString(&buf, active->gameState.stringData + offset);
    }

    memset(&nullstate, 0, sizeof(nullstate));
    for (i = 0; i < 1024; ++i) {
        if (!active->entityBaselines[i].number)
            continue;

        MSG_WriteByte(&buf, 3);
        MSG_WriteDeltaEntity(&buf, &nullstate, &active->entityBaselines[i], 1);
    }

    MSG_WriteByte(&buf, 7);
    MSG_WriteLong(&buf, conn->clientNum);
    MSG_WriteLong(&buf, conn->checksumFeed);
    MSG_WriteByte(&buf, 7);

    *(int *)compressedBuf = *(int *)buf.data;
    compressedSize = MSG_WriteBitsCompress(buf.data + 4, compressedBuf + 4, buf.cursize - 4) + 4;

    len = conn->serverMessageSequence;
    FS_Write(&len, 4, conn->demofile);
    len = compressedSize;
    FS_Write(&len, 4, conn->demofile);
    FS_Write(compressedBuf, compressedSize, conn->demofile);
}

void CL_ShutdownHunkUsers(void)
{
    if (!cls.hunkUsersStarted)
        return;
    CL_ShutdownCGame();
    CL_ShutdownUI();
    cls.whiteMaterial = 0;
    cls.consoleMaterial = 0;
    cls.consoleFont = 0;
    cls.hunkUsersStarted = 0;
}

void CL_InitLoad(const char *mapname, const char *gametype)
{
    byte *cc;

    if (!legacyHacks->cl_running)
        return;

    Dvar_SetInt((dvar_t *)com_expectedHunkUsage, 0);
    UI_SetMap(mapname, gametype);

    cc = (byte *)&clc;
    *(int *)cc = (*(int *)cc >= 5) ? 5 : 0;

    SCR_UpdateScreenInternal();
}

void CL_DrawLogo(void)
{
    int time;
    float alpha;
    float color[4];
    float w;
    float h0;
    float h1;

    time = cls.realtime - cls.logo.startTime;

    if (time < cls.logo.fadein) {
        alpha = (float)time / (float)cls.logo.fadein;
    } else if (time > cls.logo.duration - cls.logo.fadeout) {
        alpha = (float)(cls.logo.duration - time) / (float)cls.logo.fadeout;
    } else {
        alpha = 1.0f;
    }

    if (alpha < 0.0f)
        alpha = 0.0f;
    if (alpha > 1.0f)
        alpha = 1.0f;

    color[0] = alpha;
    color[1] = alpha;
    color[2] = alpha;
    color[3] = 1.0f;

    w = (float)cls.vidConfig.width;
    h0 = ((float)cls.vidConfig.height + (float)cls.vidConfig.height) * 0.33333334f;
    h1 = (float)cls.vidConfig.height - h0;

    re.DrawStretchPic(0.0f, 0.0f, w, h0, 0.0f, 0.0f, 1.0f, 1.0f, color, cls.logo.material[0]);
    re.DrawStretchPic(0.0f, h0, w, h1, 0.0f, 0.0f, 1.0f, 1.0f, color, cls.logo.material[1]);

    if (time > cls.logo.duration)
        (&::clc)->state = CA_DISCONNECTED;
}

void CL_StopLogoOrCinematic(void)
{
    if (*(int *)&::clc == 1) {
        SCR_StopCinematic();
    } else {
        *(int *)&::clc = 0;
    }

    SND_StopSounds((snd_stopsounds_arg_t)0);

    if (!clc.state)
        UI_SetActiveMenu(1);
    else
        UI_SetActiveMenu(0);
}

void CL_AddDebugLine(const vec_t *start, const vec_t *end, const vec_t *color, qboolean depthTest, int duration, qboolean fromServer)
{
    int lineIndex;
    trDebugLine_t *line;

    if (!cls.rendererStarted)
        return;

    cls.debug.maxLines = 0x1000;
    lineIndex = cls.debug.numLines;

    if (lineIndex + 1 > cls.debug.maxLines)
        return;

    if (!cls.debug.lines) {
        cls.debug.lines = (trDebugLine_t *)(Z_MallocInternal(0x2c000));
        cls.debug.lineFromServer = (byte *)(Z_MallocInternal(cls.debug.maxLines));
        cls.debug.lineDuration = (int *)(Z_MallocInternal(cls.debug.maxLines * sizeof(*cls.debug.lineDuration)));
        cls.debug.numLines = 0;
        lineIndex = 0;
    }

    line = &cls.debug.lines[lineIndex];
    line->start[0] = start[0];
    line->start[1] = start[1];
    line->start[2] = start[2];
    line->end[0] = end[0];
    line->end[1] = end[1];
    line->end[2] = end[2];
    line->color[0] = color[0];
    line->color[1] = color[1];
    line->color[2] = color[2];
    line->color[3] = color[3];
    line->depthTest = depthTest;

    cls.debug.lineFromServer[lineIndex] = (byte)fromServer;
    cls.debug.lineDuration[lineIndex] = duration;
    cls.debug.numLines++;
}

void CL_AddDebugString(const vec_t *xyz, const vec_t *color, float scale, const char *text, qboolean fromServer)
{
    int stringIndex;
    trDebugString_t *string;

    if (!cls.rendererStarted)
        return;

    cls.debug.maxStrings = 0x100;
    stringIndex = cls.debug.numStrings;

    if (stringIndex + 1 > cls.debug.maxStrings)
        return;

    if (!cls.debug.strings) {
        cls.debug.strings = (trDebugString_t *)(Z_MallocInternal(0x8000));
        cls.debug.stringFromServer = (byte *)(Z_MallocInternal(cls.debug.maxStrings));
        cls.debug.numStrings = stringIndex = 0;
    }

    string = &cls.debug.strings[stringIndex];
    string->xyz[0] = xyz[0];
    string->xyz[1] = xyz[1];
    string->xyz[2] = xyz[2];
    string->color[0] = color[0];
    string->color[1] = color[1];
    string->color[2] = color[2];
    string->color[3] = color[3];
    string->scale = scale;
    strncpy(string->text, text, 0x5f);
    string->text[95] = '\0';

    cls.debug.stringFromServer[cls.debug.numStrings] = (byte)fromServer;
    cls.debug.numStrings++;
}

#if COD2_IS_PATCH_13

int lastUpdateKeyAuthTime;

static const char cl_getKeyAuthorizeFmt_13[] = "getKeyAuthorize %i %s PB %s";

static void CL_BuildMd5StrFromCDKey(char *md5Str)
{
    char nums[64];
    const char *key;
    int len, i, j;
    char *p, *e;

    md5Str[0] = '\0';
    memset(nums, 0, sizeof(nums));

    key = cl_cdkey;
    len = strlen(key);
    if (len > 0) {
        j = 0;
        for (i = 0; i < 32; ++i) {
            unsigned char ch = (unsigned char)key[i];
            if ((ch >= '0' && ch <= '9') ||
                (ch >= 'a' && ch <= 'z') ||
                (ch >= 'A' && ch <= 'Z')) {
                nums[j++] = (char)ch;
            }
        }
        nums[j] = '\0';
    }

    p = nums;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    e = p + strlen(p);
    while (e > p && (e[-1] == ' ' || e[-1] == '\t' || e[-1] == '\n' || e[-1] == '\r'))
        e--;
    *e = '\0';
    if (p != nums)
        memmove(nums, p, (size_t)(e - p) + 1);
    strupr(nums);

#    if COD2_FEATURE_PUNKBUSTER

    md5Str[0] = '\0';
#    endif
}
#endif


void CL_RequestAuthorization(void)
{
    char nums[64];
    const char *cdkey;
    int len;
    int i;
    int j;
    const dvar_t *anonymous;
#if COD2_IS_PATCH_13
    char md5Str[33];

    lastUpdateKeyAuthTime = cls.realtime;
#endif

    if (!CL_CDKeyValidate(cl_cdkey, cl_cdkeychecksum)) {
        Com_Error(ERR_DROP, (const char *)"EXE_ERR_INVALID_CD_KEY");
        return;
    }

    if (!cls.authorizeServer.port) {
        Com_Printf((const char *)"Resolving %s\n", (const char *)"cod2master.activision.com");
        if (!NET_StringToAdr((const char *)"cod2master.activision.com", &cls.authorizeServer)) {
            Com_Printf((const char *)"Couldn't resolve address\n");
            return;
        }

        cls.authorizeServer.port = BigShort(0x50dc);
        Com_Printf((const char *)"%s resolved to %i.%i.%i.%i:%i\n",
                   (const char *)"cod2master.activision.com",
                   cls.authorizeServer.ip[0],
                   cls.authorizeServer.ip[1],
                   cls.authorizeServer.ip[2],
                   cls.authorizeServer.ip[3],
                   BigShort(cls.authorizeServer.port));
    }

    if (cls.authorizeServer.type == NA_BAD)
        return;

    if (Dvar_GetBool((const char *)"fs_restrict")) {
        I_strncpyz(nums, (const char *)"demo", sizeof(nums));
    } else {
        j = 0;
        cdkey = cl_cdkey;
        len = strlen(cdkey);
        if (len > 32)
            len = 32;

        for (i = 0; i < len; ++i) {
            char ch = cdkey[i];
            if ((ch >= '0' && ch <= '9') ||
                (ch >= 'a' && ch <= 'z') ||
                (ch >= 'A' && ch <= 'Z')) {
                nums[j++] = (char)ch;
            }
        }
        nums[j] = '\0';
    }

    anonymous = Dvar_RegisterBool((const char *)"cl_anonymous", 0, 0x101b);
#if COD2_IS_PATCH_13
    CL_BuildMd5StrFromCDKey(md5Str);
    NET_OutOfBandPrint(NS_CLIENT1, cls.authorizeServer,
                       va(cl_getKeyAuthorizeFmt_13, anonymous->current.enabled, nums, md5Str));
#else
    NET_OutOfBandPrint(NS_CLIENT1, cls.authorizeServer,
                       va((const char *)"getKeyAuthorize %i %s", anonymous->current.enabled, nums));
#endif
}

void CL_CheckForResend(void)
{
    int i;
    char info[1024];
    char data[1024];

    if (clc.demoplaying)
        return;

    if (clc.state != CA_CONNECTING && clc.state != CA_CHALLENGING)
        return;

    if (cls.realtime - clc.connectTime < 3000)
        return;

    clc.connectTime = cls.realtime;
    clc.connectPacketCount++;

    switch (clc.state) {
    case CA_CONNECTING:
        if (net_lanauthorize->current.enabled || !Sys_IsLANAddress(clc.serverAddress))
            CL_RequestAuthorization();
        NET_OutOfBandPrint(NS_CLIENT1, clc.serverAddress, "getchallenge");
        break;

    case CA_CHALLENGING:
        I_strncpyz(info, Dvar_InfoString(2), sizeof(info));
        Info_SetValueForKey(info, "protocol", va("%i", 115));
        Info_SetValueForKey(info, "challenge", va("%i", clc.challenge));
        Info_SetValueForKey(info, "qport", va("%i", clc.qport));

        strcpy(data, "connect ");
        data[8] = '"';
        for (i = 0; i < strlen(info); i++)
            data[9 + i] = info[i];
        data[9 + i] = '"';
        data[10 + i] = 0;

        NET_OutOfBandData(NS_CLIENT1, clc.serverAddress, (unsigned char *)data, i + 10);
        dvar_modifiedFlags &= ~2;
        break;

    default:
        Com_Error(ERR_FATAL, "\x15" "CL_CheckForResend: bad clc->state");
    }
}

void CL_LocalServers_f(void)
{
    const char *message;
    int i;
    int j;
    netadr_t to;

    Com_Printf("Scanning for servers on the local network...\n");

    // reset the list, waiting for response
    cls.numlocalservers = 0;
    cls.pingUpdateSource = 0;

    for (i = 0; i < 128; i++) {
        qboolean b = cls.localServers[i].dirty;
        Com_Memset(&cls.localServers[i], 0, sizeof(cls.localServers[i]));
        cls.localServers[i].dirty = b;
    }
    Com_Memset(&to, 0, sizeof(to));

    // the 'xxx' is a challenge echoed back by the server
    message = "\xff\xff\xff\xffgetinfo xxx";

    // send each message twice in case one is dropped
    for (i = 0; i < 2; i++) {
        // broadcast on each server port
        for (j = 0; j < 4; j++) {
            to.port = BigShort((short)(28960 + j));
            to.type = NA_BROADCAST;
            CL_Netchan_SendOOBPacket(strlen(message), message, to);
        }
    }
}

void CL_PlayLogo_f(void)
{
    clientConnection_t *conn;
    const char *name;

    if (Cmd_Argc() != 5) {
        Com_Printf((const char *)"USAGE: logo <image name> <fadein seconds> <full duration seconds> <fadeout seconds>\n");
        return;
    }

    Com_DPrintf((const char *)"CL_PlayLogo_f\n");

    conn = &::clc;
    if (conn->state == CA_CINEMATIC) {
        SCR_StopCinematic();
    } else if (conn->state == CA_LOGO) {
        conn->state = CA_DISCONNECTED;
    } else if (conn->state != CA_DISCONNECTED) {
        return;
    }

    conn->state = CA_LOGO;

    if (cls.uiStarted)
        UI_SetActiveMenu(0);

    SND_StopSounds((snd_stopsounds_arg_t)0);
    SND_FadeAllSounds(1.0f, 0);

    name = Cmd_Argv(1);
    cls.logo.fadein = I_fround((float)atof(Cmd_Argv(2)) * 1000.0f);
    cls.logo.duration = I_fround((float)atof(Cmd_Argv(3)) * 1000.0f);
    cls.logo.fadeout = I_fround((float)atof(Cmd_Argv(4)) * 1000.0f);
    cls.logo.duration += cls.logo.fadein + cls.logo.fadeout;

    cls.logo.material[0] = re.RegisterMaterial(va((const char *)"%s1", name), 0x30, 3);
    cls.logo.material[1] = re.RegisterMaterial(va((const char *)"%s2", name), 0x30, 3);
    cls.logo.startTime = cls.realtime + 100;
}

qboolean CL_UpdateDirtyPings_f(int source)
{
    int slots;
    int i;
    char buff[1024];
    int pingTime;
    int max;
    qboolean status;

    status = 0;

    if ((&::clc)->state != CA_DISCONNECTED)
        return 0;

    if (source < 0 || source > 2)
        return 0;

    cls.pingUpdateSource = source;

    slots = CL_GetPingQueueCount();
    if (slots < 16) {
        serverInfo_t *server = NULL;

        max = (source == 1) ? 20000 : 128;
        switch (source) {
        case 0:
            server = &cls.localServers[0];
            max = cls.numlocalservers;
            break;
        case 1:
            server = &cls.globalServers[0];
            max = cls.numglobalservers;
            break;
        case 2:
            server = &cls.favoriteServers[0];
            max = cls.numfavoriteservers;
            break;
        }
        for (i = 0; i < max; i++) {
            if (server[i].dirty && server[i].ping == -1) {
                int j;

                if (slots >= 16)
                    break;

                for (j = 0; j < 16; j++) {
                    if (!cl_pinglist[j].adr.port)
                        continue;
                    if (NET_CompareAdr(cl_pinglist[j].adr, server[i].adr)) {
                        // already on the list
                        break;
                    }
                }
                if (j >= 16) {
                    status = 1;
                    for (j = 0; j < 16; j++) {
                        if (!cl_pinglist[j].adr.port)
                            break;
                    }
                    cl_pinglist[j].adr = server[i].adr;
                    cl_pinglist[j].start = Sys_Milliseconds();
                    cl_pinglist[j].time = 0;
                    NET_OutOfBandPrint(NS_CLIENT1, cl_pinglist[j].adr, "getinfo xxx");
                    slots++;
                }
            }
        }
    }

    if (slots)
        status = 1;

    for (i = 0; i < 16; i++) {
        if (!cl_pinglist[i].adr.port)
            continue;

        CL_GetPing(i, buff, 1024, &pingTime);
        if (pingTime) {
            CL_ClearPing(i);
            status = 1;
        }
    }

    return status;
}

void CL_RunOncePerClientFrame(int msec)
{

    if (UI_IsFullscreen()) {
        re.SyncGpu();
    }

    IN_Frame();

    if (cl_avidemo->current.integer && msec) {

        if (clc.state == 8 || cl_forceavidemo->current.enabled) {
            Cbuf_ExecuteText(0, "screenshot silent\n");
        }
        float timescale = com_timescaleValue;
        msec = (int)(1000.0f / (float)cl_avidemo->current.integer * timescale);
        if (msec == 0)
            msec = 1;
    }

    if (cls.uiStarted)
        CG_CalculateFPS();

    cls.realFrametime = msec;
    cls.frametime = msec;
    cls.realtime += msec;

    int curFrameTime = com_frameTime;
    unsigned int delta = curFrameTime - old_com_frameTime;
    frame_msec = delta;
    if (delta == 0)
        frame_msec = 1;
    else if (delta > 200)
        frame_msec = 200;
    old_com_frameTime = curFrameTime;
}

int Client_SendVoiceData(int bytes, char *enc_buffer)
{
    clientActive_t *c;
    int voiceIdx, newIdx;

    if (bytes > 0) {
        c = clients;
        voiceIdx = c->voicePacketCount;

        memcpy(c->voicePackets[voiceIdx].data, enc_buffer, bytes);
        c->voicePackets[voiceIdx].dataSize = bytes;

        newIdx = voiceIdx + 1;
        c->voicePacketCount = newIdx;

        if (newIdx > 0) {
            if (c->serverTime - c->voicePacketLastTransmit > 199 || newIdx > 9) {
                CL_WriteVoicePacket();
                c = clients;
                c->voicePacketCount = 0;
                c->voicePacketLastTransmit = c->serverTime;
            }
        }
    }

    return bytes;
}

Bool CL_ConnectionlessPacket(netadr_t from, msg_t *msg, int time)
{
    clientConnection_t *conn = &::clc;
    char *printBuf;
    const char *cmd;
    Bool result = 0;

    LargeLocal printBuf_large_local(MAX_MSGLEN);
    printBuf = (char *)printBuf_large_local.GetBuf();

    MSG_BeginReading(msg);
    MSG_ReadLong(msg);
    CL_Netchan_AddOOBProfilePacket(msg->cursize);

    Cmd_TokenizeString(MSG_ReadStringLine(msg));
    cmd = Cmd_Argv(0);

#if COD2_IS_PATCH_13

    if (I_strnicmp(cmd, "PB_", 3) == 0) {
        PB_HandleClientOobPacket(&from, msg);
        result = 1;
        goto finish;
    }
#endif

    if (I_stricmp(cmd, (const char *)"v") == 0) {
        CL_VoicePacket(msg);
        result = 1;
        goto finish;
    }

    if (I_stricmp(cmd, (const char *)"challengeResponse") == 0) {
        if (conn->state != CA_CONNECTING) {
            Com_Printf((const char *)"Unwanted challenge response received.  Ignored.\n");
            goto finish;
        }

        conn->challenge = atoi(Cmd_Argv(1));
#if defined(COD2_X64) || defined(_M_X64) || defined(__x86_64__)
        clc_x64_lastChallenge = conn->challenge;
#endif
        conn->state = CA_CHALLENGING;
        conn->connectPacketCount = 0;
        conn->connectTime = -99999;
        conn->serverAddress = from;
        Com_DPrintf((const char *)"challenge: %d\n", conn->challenge);
        result = 1;
        goto finish;
    }

    if (I_stricmp(cmd, (const char *)"connectResponse") == 0) {
        if (conn->state > CA_CHALLENGING) {
            Com_Printf((const char *)"Dup connect received.  Ignored.\n");
            goto finish;
        }

        if (conn->state != CA_CHALLENGING) {
            Com_Printf((const char *)"connectResponse packet while not connecting.  Ignored.\n");
            goto finish;
        }

        if (!NET_CompareBaseAdr(from, conn->serverAddress)) {
            const char *expected;
            const char *actual;

            Com_Printf((const char *)"connectResponse from a different address.  Ignored.\n");
            expected = NET_AdrToString(conn->serverAddress);
            actual = NET_AdrToString(from);
            Com_Printf((const char *)"%s should have been %s\n", actual, expected);
            goto finish;
        }

        Netchan_Setup(NS_CLIENT1, &conn->netchan, from, ::qport);
        conn->state = CA_CONNECTED;
        conn->lastPacketSentTime = cls.realtime;
        conn->lastPacketTime = -9999;
        result = 1;
        goto finish;
    }

    if (I_stricmp(cmd, (const char *)"infoResponse") == 0) {
        CL_ServerInfoPacket(from, msg, time);
        result = 1;
        goto finish;
    }

    if (I_stricmp(cmd, (const char *)"statusResponse") == 0) {
        CL_ServerStatusResponse(from, msg);
        result = 1;
        goto finish;
    }

    if (I_stricmp(cmd, (const char *)"disconnect") == 0) {
        if (conn->state && NET_CompareAdr(from, conn->netchan.remoteAddress) &&
            cls.realtime - conn->lastPacketTime > 2999) {
            Com_Error(ERR_DROP, (const char *)"EXE_SERVER_DISCONNECTED");
        }

        result = 1;
        goto finish;
    }

    if (I_stricmp(cmd, (const char *)"echo") == 0) {
        NET_OutOfBandPrint(NS_CLIENT1, from, va((const char *)"%s", Cmd_Argv(1)));
        result = 1;
        goto finish;
    }

    if (I_stricmp(cmd, (const char *)"keyAuthorize") == 0) {
        result = 1;
        goto finish;
    }

    if (I_stricmp(cmd, (const char *)"print") == 0) {
        const char *text = MSG_ReadBigString(msg);

        I_strncpyz(conn->serverMessage, text, sizeof(conn->serverMessage));
        Com_sprintf(printBuf, 0x4000, (const char *)"%s", text);
        Com_PrintMessage((print_msg_type_t)0, printBuf);
        result = 1;
        goto finish;
    }

    if (I_stricmp(cmd, (const char *)"error") == 0) {
        if (conn->state && NET_CompareBaseAdr(from, conn->serverAddress)) {
            Com_Error(ERR_DROP, (const char *)"%s", MSG_ReadBigString(msg));
            result = 1;
        }
        goto finish;
    }

    if (I_stricmp(cmd, (const char *)"updateResponse") == 0) {
        CL_UpdateInfoPacket(from);
        result = 1;
        goto finish;
    }

    if (I_strncmp(cmd, (const char *)"getserversResponse", 18) == 0) {
        CL_ServersResponsePacket(from, msg);
        result = 1;
        goto finish;
    }

    if (I_strncmp(cmd, (const char *)"needcdkey", 9) == 0) {
        I_strncpyz(conn->serverMessage, (const char *)"EXE_AWAITINGCDKEYAUTH", sizeof(conn->serverMessage));
        SEH_LocalizeTextMessage((const char *)"EXE_AWAITINGCDKEYAUTH", (const char *)"need cd key message", LOCMSG_SAFE);
        Com_Printf((const char *)"%s\n", conn->serverMessage);
        CL_RequestAuthorization();
        result = 1;
        goto finish;
    }

    if (I_stricmp(cmd, (const char *)"loadingnewmap") == 0) {
        if (NET_CompareBaseAdr(from, conn->serverAddress)) {
            char mapname[64];
            const char *gametype;

            I_strncpyz(mapname, MSG_ReadStringLine(msg), sizeof(mapname));
            gametype = MSG_ReadStringLine(msg);
            CL_SetupForNewServerMap(mapname, gametype);
            if (conn->state <= CA_CONNECTED)
                conn->state = CA_CONNECTED;
            UI_DrawConnectScreen();
        }

        result = 1;
        goto finish;
    }

finish:
    return result;
}

Bool CL_PacketEvent(netadr_t from, msg_t *msg, int time)
{
    clientConnection_t *conn = &::clc;
    int headerBytes;
    int savedServerMessageSequence;
    int savedReliableAcknowledge;

    if (msg->cursize > 3 && *(int *)msg->data == -1)
        return CL_ConnectionlessPacket(from, msg, time);

    if (*(int *)&::clc <= CA_CHALLENGING)
        return 0;

    if (msg->cursize <= 3) {
        Com_Printf((const char *)"%s: Runt packet\n", NET_AdrToString(from));
        return 1;
    }

    if (!NET_CompareAdr(from, conn->netchan.remoteAddress)) {
        Com_DPrintf((const char *)"%s:sequenced packet without connection\n", NET_AdrToString(from));
        return 0;
    }

    conn->lastPacketTime = cls.realtime;

    if (!Netchan_Process(&conn->netchan, msg))
        return 0;

    headerBytes = msg->readcount;
    savedServerMessageSequence = conn->serverMessageSequence;
    savedReliableAcknowledge = conn->reliableAcknowledge;

    conn->serverMessageSequence = *(int *)msg->data;
    conn->reliableAcknowledge = MSG_ReadLong(msg);
    if (conn->reliableAcknowledge < conn->reliableSequence - 128) {
        conn->reliableAcknowledge = conn->reliableSequence;
        return 0;
    }

    CL_Netchan_Decode(msg->data + msg->readcount, msg->cursize - msg->readcount);
    CL_ParseServerMessage(msg);

    if (msg->overflowed) {
        Com_DPrintf((const char *)"ignoring illegible message");
        conn->serverMessageSequence = savedServerMessageSequence;
        conn->reliableAcknowledge = savedReliableAcknowledge;
        return 0;
    }

    if (conn->demorecording && !conn->demowaiting)
        CL_WriteDemoMessage(msg, headerBytes);

    return 1;
}

extern void Com_ClientDObjClearAllSkel(void);
extern void CL_ClearMutedList(void);
extern void Cbuf_Execute(void);
extern void CL_SetADS(int ads);
extern LegacyHacks *legacyHacks;
void CL_Init(void)
{
    Com_Printf("----- Client Initialization -----\n");

    clientActive_t *c = clients;
    Bool savedCgameInitialized = c->cgameInitialized;
    Bool savedCgameInitCalled = c->cgameInitCalled;
    int savedKeyCatchers = c->keyCatchers;
    Bool savedDisplayHUDWithKeycatchUI = c->displayHUDWithKeycatchUI;
    Bool savedActive = c->active;

    memset(c, 0, sizeof(clientActive_t));

    c->cgameInitialized = savedCgameInitialized;
    c->cgameInitCalled = savedCgameInitCalled;
    c->keyCatchers = savedKeyCatchers;
    c->displayHUDWithKeycatchUI = savedDisplayHUDWithKeycatchUI;
    c->active = savedActive;

    Com_ClientDObjClearAllSkel();
    CL_ClearMutedList();
    c->voicePacketCount = 0;

    clc.state = CA_DISCONNECTED;
    cls.realtime = 0;

    c->active = 1;

    ((byte *)::legacyHacks)[0x5c] = 0;
    ((byte *)::legacyHacks)[0x9c] = 0;
    ((byte *)::legacyHacks)[0xdc] = 0;

    Cbuf_Execute();

    *(int *)((byte *)::legacyHacks + 4) = 1;
    CL_SetADS(0);

    Com_Printf("----- Client Initialization Complete -----\n");
}

void CL_ShutdownAll(void)
{
    void (*syncRenderThread)(void) = re.SyncRenderThread;

    if (syncRenderThread)
        syncRenderThread();

    if (cls.hunkUsersStarted) {
        CL_ShutdownCGame();
        CL_ShutdownUI();
        cls.whiteMaterial = 0;
        cls.consoleMaterial = 0;
        cls.consoleFont = 0;
        cls.hunkUsersStarted = 0;
    }

    if (re.Shutdown)
        ((void (*)(int))re.Shutdown)(0);

    cls.rendererStarted = 0;
}


void CL_CheckUserinfo(void)
{
    if (clc.state < CA_CHALLENGING)
        return;
    if (cl_paused->current.integer)
        return;
    if (dvar_modifiedFlags & 2) {
        dvar_modifiedFlags &= ~2;
        CL_AddReliableCommand(va("userinfo \"%s\"", Dvar_InfoString(2)));
    }
}

void CL_CheckTimeout(void)
{
    if ((!cl_paused->current.integer || !sv_paused->current.integer)
        && clc.state >= CA_CONNECTED && clc.state != CA_CINEMATIC && clc.state != CA_LOGO
        && cls.realtime - clc.connectTime > cl_timeout->current.value * 1000.0f) {
        if (++clients[0].timeoutcount > 5)
            Com_Error(ERR_DROP, "EXE_ERR_SERVER_TIMEOUT");
    } else if ((clc.state == CA_CONNECTING || clc.state == CA_CHALLENGING) && clc.connectTime > 0
        && cls.realtime - clc.connectTime > cl_connectTimeout->current.value * 1000.0f) {
        Com_Error(ERR_DROP, "EXE_ERR_SERVER_TIMEOUT");
    } else {
        clients[0].timeoutcount = 0;
    }
}

void CL_UpdateInGameState(void)
{
    if (clc.state == CA_ACTIVE) {
        if (!cl_ingame->current.enabled)
            Dvar_SetBool(cl_ingame, 1);
    } else if (cl_ingame->current.enabled) {
        Dvar_SetBool(cl_ingame, 0);
    }
}

void CL_Frame(void)
{
    if (!legacyHacks->cl_running)
        return;

    Voice_GetLocalVoiceData((ClientVoicePacket_t *)&clients[0]);
    Voice_Playback();
    CL_UpdateColor();
    CL_CheckUserinfo();
    CL_CheckTimeout();
#if COD2_IS_PATCH_13
    if (DL_InProgress())
        CL_WWWDownload();
#endif
    CL_CheckForResend();
    CL_UpdateInGameState();
    CL_SetCGameTime();
    CL_SendCmd();
}

void CL_Vid_Restart_f(void)
{
    MemoryFile memFile;
    void *clientStateBuf;
    int clientStateBytes;

    if (com_sv_running->current.enabled) {
        Com_Printf("Listen server cannot video restart.\n");
        return;
    }

    clientStateBuf = NULL;
    clientStateBytes = 0;

    if (clients->cgameInitialized) {
        MemFile_InitForWriting(&memFile, 0xa00000, Z_VirtualAllocInternal(0xa00000), 1);
        CL_ArchiveClientState(&memFile);

        clientStateBytes = memFile.bytesUsed;
        clientStateBuf = Z_MallocInternal(clientStateBytes);
        memcpy(clientStateBuf, memFile.buffer, clientStateBytes);
        Z_VirtualFreeInternal(memFile.buffer);
    }

    Dvar_SetInt((dvar_t *)com_expectedHunkUsage, 0);
    g_waitingForServer = 0;
    SND_StopSounds((snd_stopsounds_arg_t)1);

    CL_ShutdownHunkUsers();
    CL_ShutdownRef();
    cls.rendererStarted = 0;

    CL_AddReliableCommand(va((const char *)"vdr"));
    Com_Restart();

    Dvar_RegisterInt((const char *)"loc_language", 0, 0, 13, 0x1021);
    Dvar_RegisterBool((const char *)"loc_translate", 1, 0x1020);
    Dvar_RegisterBool((const char *)"fs_ignoreLocalized", 0, 0x10a0);

    FS_ConditionalRestart(clc.checksumFeed);
    SEH_UpdateLanguageInfo();
    Dvar_SetInt((dvar_t *)cl_paused, 0);
    CL_InitRef();
    CL_StartHunkUsers();

    if (clc.state > CA_CONNECTED && clc.state != CA_CINEMATIC && clc.state != CA_LOGO) {
        CL_InitCGame();
        CL_SendPureChecksums();
    }

    if (clientStateBuf) {
        if (clients->cgameInitialized) {
            MemFile_InitForReading(&memFile, clientStateBytes, clientStateBuf);
            CL_ArchiveClientState(&memFile);
        }
        Z_FreeInternal(clientStateBuf);
    }
}

void CL_Snd_Restart_f(void)
{
    MemoryFile memFile;
    snd_listener listeners;
    int size;
    void *buffer;

    if (com_sv_running->current.enabled) {
        Com_Printf("Listen server cannot sound restart.\n");
        return;
    }

    MemFile_InitForWriting(&memFile, 0xa00000, Z_VirtualAllocInternal(0xa00000), 1);
    SND_Save(&memFile);

    size = memFile.bytesUsed;
    buffer = Z_MallocInternal(size);
    memcpy(buffer, memFile.buffer, size);
    Z_VirtualFreeInternal(memFile.buffer);

    SND_SaveListeners(&listeners);
    SND_Shutdown();
    SND_Init();
    SND_RestoreListeners(&listeners);

    CL_Vid_Restart_f();

    MemFile_InitForReading(&memFile, size, buffer);
    SND_Restore(&memFile);
    Z_FreeInternal(buffer);
}

// retail globals, names from CoD1 (SUPPORTED)
qboolean autoupdateStarted;
char autoupdateFilename[64];

void CL_Disconnect(void)
{
    bool wasConnected;

    if (!legacyHacks->cl_running) {
        return;
    }

    if (clc.demorecording) {
        CL_StopRecord_f();
    }

    if (clc.download) {
        FS_FCloseFile(clc.download);
        clc.download = 0;
    }
    *clc.downloadTempName = *clc.downloadName = 0;
    legacyHacks->cl_downloadName[0] = 0;

    if (clc.demofile) {
        FS_FCloseFile(clc.demofile);
        clc.demofile = 0;
        clc.demoplaying = 0;
        clc.demorecording = 0;
    }

    SCR_StopCinematic();

    // send a disconnect message to the server
    // send it a few times in case one is dropped
    if (clc.state >= CA_CONNECTED && clc.reliableSequence - clc.reliableAcknowledge <= 128) {
        CL_AddReliableCommand("disconnect");
        CL_WritePacket();
        CL_WritePacket();
        CL_WritePacket();
    }

    wasConnected = clc.state >= CA_CONNECTING;

    CL_ClearState();
    CL_ClearMutedList();

    // wipe the client connection
    memset(&clc, 0, sizeof(clc));
    SND_DisconnectListener();
    clc.state = CA_DISCONNECTED;

    if (wasConnected) {
        clients->keyCatchers &= 1;
    }

    if (CL_AllLocalClientsDisconnected()) {
        autoupdateFilename[0] = 0;
        autoupdateStarted = 0;
        Dvar_SetBool(sv_disableClientConsole, 0);

        // not connected to a pure server anymore
        cl_connectedToPureServer = 0;
        fs_checksumFeed = 0;
        bgs = 0;
    }
}

void CL_Shutdown(void)
{
    Com_Printf("----- CL_Shutdown -----\n");

    if (recursive) {
        printf("recursive shutdown\n");
        return;
    }

    recursive = 1;

    CL_Disconnect();

    if (CL_AllLocalClientsDisconnected()) {
        CL_ShutdownDebugData();

        if (cls.hunkUsersStarted) {
            CL_ShutdownCGame();
            CL_ShutdownUI();
            cls.whiteMaterial = 0;
            cls.consoleMaterial = 0;
            cls.consoleFont = 0;
            cls.hunkUsersStarted = 0;
        }

        SND_Shutdown();
        CL_ShutdownRef();
        CL_ShutdownInput();

        Cmd_RemoveCommand("cmd");
        Cmd_RemoveCommand("configstrings");
        Cmd_RemoveCommand("clientinfo");
        Cmd_RemoveCommand("vid_restart");
        Cmd_RemoveCommand("snd_restart");
        Cmd_RemoveCommand("disconnect");
        Cmd_RemoveCommand("record");
        Cmd_RemoveCommand("demo");
        Cmd_RemoveCommand("cinematic");
        Cmd_RemoveCommand("logo");
        Cmd_RemoveCommand("stoprecord");
        Cmd_RemoveCommand("connect");
        Cmd_RemoveCommand("reconnect");
        Cmd_RemoveCommand("localservers");
        Cmd_RemoveCommand("globalservers");
        Cmd_RemoveCommand("rcon");
        Cmd_RemoveCommand("setenv");
        Cmd_RemoveCommand("ping");
        Cmd_RemoveCommand("serverstatus");
        Cmd_RemoveCommand("showip");
        Cmd_RemoveCommand("fs_openedList");
        Cmd_RemoveCommand("fs_referencedList");
        Cmd_RemoveCommand("updatehunkusage");
        Cmd_RemoveCommand("updatescreen");
        Cmd_RemoveCommand("SaveTranslations");
        Cmd_RemoveCommand("SaveNewTranslations");
        Cmd_RemoveCommand("LoadTranslations");
        Cmd_RemoveCommand("startSingleplayer");
        Cmd_RemoveCommand("buyNow");
        Cmd_RemoveCommand("singlePlayLink");
        Cmd_RemoveCommand("setRecommended");
        Cmd_RemoveCommand("cubemapShot");
        Cmd_RemoveCommand("openScriptMenu");
        Cmd_RemoveCommand("openmenu");
        Cmd_RemoveCommand("closemenu");

        memset(&cls, 0, sizeof(cls));
    }

    legacyHacks->cl_running = 0;
    recursive = 0;
    Com_Printf("-----------------------\n");
}

void CL_Disconnect_f(void)
{
    bool wasConnected;

    SCR_StopCinematic();

    wasConnected = clc.state != CA_DISCONNECTED && clc.state != CA_CINEMATIC && clc.state != CA_LOGO;

    CL_Disconnect();

    if (wasConnected && CL_AllLocalClientsDisconnected()) {
        Com_Error(ERR_DISCONNECT, "EXE_DISCONNECTED_FROM_SERVER");
    }
}

void CL_MapLoading(const char *mapname)
{
    LegacyHacks *legacy = (LegacyHacks *)::legacyHacks;
    clientConnection_t *conn = &::clc;
    clientActive_t *active = clients;

    if (!legacy->cl_running)
        return;

    g_waitingForServer = 0;
    Con_Close();
    active->keyCatchers = 0;
    active->displayHUDWithKeycatchUI = 0;

    if (conn->state >= CA_CONNECTED && I_stricmp(cls.servername, (const char *)"localhost") == 0) {
        memset(cls.updateInfoString, 0, sizeof(cls.updateInfoString));
        conn->state = CA_CONNECTED;
        memset(conn->serverMessage, 0, sizeof(conn->serverMessage));
        memset(&active->gameState, 0, sizeof(active->gameState));
        conn->lastPacketSentTime = -9999;
    } else {
        Dvar_SetString((dvar_t *)nextmap, (const char *)"");
        I_strncpyz(cls.servername, (const char *)"localhost", sizeof(cls.servername));
        CL_Disconnect();
        UI_CloseAll();

        conn->state = CA_CHALLENGING;
        ((clientConnection_t *)conn)->connectTime = -3000;
        conn->qport = ::qport;
        NET_StringToAdr(cls.servername, &conn->serverAddress);
        CL_CheckForResend();
    }

    I_strncpyz(legacy->cl_serverloadmap, mapname, 0x40);
    legacy->cl_serverloadwaiting = 0;
    SND_FadeAllSounds(0.0f, 0.0f);
}

void CL_DemoCompleted(void)
{
    clientConnection_t *conn = &::clc;
    connstate_t oldState;

    if (conn->isTimeDemo) {
        int elapsed = Sys_Milliseconds() - conn->timeDemoStart;
        if (elapsed > 0) {
            double seconds = (double)elapsed / 1000.0;
            double fps = (double)conn->timeDemoFrames * 1000.0 / (double)elapsed;
            Com_Printf((const char *)"%i frames, %3.1f seconds: %3.1f fps\n", conn->timeDemoFrames, seconds, fps);
        }
    }

    if (conn->timeDemoLog) {
        FS_FCloseFile(conn->timeDemoLog);
        conn->timeDemoLog = 0;
    }

    SCR_StopCinematic();
    oldState = conn->state;
    CL_Disconnect();

    if (oldState <= CA_LOGO)
        return;

    if (Sys_IsMainThread() && !UI_IsFullscreen() && *(byte *)&clients[0] != 0 && *(int *)&::clc > CA_LOGO)
        return;

    Com_Error(ERR_DISCONNECT, (const char *)"EXE_DISCONNECTED_FROM_SERVER");
}

void CL_ReadDemoMessage(void)
{
    clientConnection_t *conn;
    msg_t buf;
    byte bufData[MAX_MSGLEN];
    int serverMessageSequence;

    if (!::clc.demofile) {
        CL_DemoCompleted();
        return;
    }

    if (FS_Read(&serverMessageSequence, 4, ::clc.demofile) != 4) {
        CL_DemoCompleted();
        return;
    }

    conn = &::clc;
    conn->serverMessageSequence = serverMessageSequence;

    MSG_Init(&buf, bufData, sizeof(bufData));
    if (FS_Read(&buf.cursize, 4, conn->demofile) != 4) {
        CL_DemoCompleted();
        return;
    }

    if (buf.cursize == -1) {
        CL_DemoCompleted();
        return;
    }

    if (buf.cursize > buf.maxsize)
        Com_Error(ERR_DROP, (const char *)"\x15"
                                   "CL_ReadDemoMessage: demoMsglen > MAX_MSGLEN");

    if (FS_Read(buf.data, buf.cursize, conn->demofile) != buf.cursize) {
        Com_Printf((const char *)"Demo file was truncated.\n");
        CL_DemoCompleted();
        return;
    }

    conn->lastPacketTime = cls.realtime;
    buf.readcount = 0;
    conn->reliableAcknowledge = MSG_ReadLong(&buf);

    if (conn->reliableAcknowledge < conn->reliableSequence - 128) {
        conn->reliableAcknowledge = conn->reliableSequence;
        return;
    }

    CL_ParseServerMessage(&buf);
}

void CL_PlayDemo_f(void)
{
    clientConnection_t *conn;
    const char *arg;
    char extension[32];
    char name[256];

    if (Cmd_Argc() != 2) {
        Com_Printf((const char *)"%s <demoname>\n", Cmd_Argv(0));
        return;
    }

    if ((com_sv_running)->current.enabled) {
        Com_Printf((const char *)"listen server cannot play a demo.\n");
        return;
    }

    CL_Disconnect();

    arg = Cmd_Argv(1);
    Com_sprintf(extension, sizeof(extension), (const char *)".dm_%d", 1);
    if (!I_stricmp(arg + strlen(arg) - strlen(extension), extension))
        Com_sprintf(name, sizeof(name), (const char *)"demos/%s", arg);
    else
        Com_sprintf(name, sizeof(name), (const char *)"demos/%s.dm_%d", arg, 1);

    conn = &::clc;
    FS_FOpenFileRead(name, &conn->demofile, 1);
    if (!conn->demofile)
        Com_Error(ERR_DROP, va((const char *)"EXE_ERR_NOT_FOUND\x15%s", name));

    I_strncpyz(conn->demoName, Cmd_Argv(1), sizeof(conn->demoName));
    Con_Close();

    conn->state = CA_CONNECTED;
    conn->demoplaying = 1;
    conn->isTimeDemo = I_stricmp(Cmd_Argv(0), (const char *)"timedemo") == 0;
    I_strncpyz(cls.servername, Cmd_Argv(1), sizeof(cls.servername));

    while (conn->state >= CA_CONNECTED && conn->state < CA_PRIMED)
        CL_ReadDemoMessage();

    conn->firstDemoFrameSkipped = 0;
}

void CL_ForwardCommandToServer(const char *string)
{
    const char *cmd = Cmd_Argv(0);

    if (cmd[0] == '-')
        return;

    if (::clc.demoplaying || ::clc.state < 5 || cmd[0] == '+') {
        Com_Printf((const char *)"Unknown command \"%s\"\n", cmd);
        return;
    }

    if (Cmd_Argc() > 1)
        CL_AddReliableCommand(string);
    else
        CL_AddReliableCommand(cmd);
}

void CL_ForwardToServer_f(void)
{
    if (clc.state != CA_ACTIVE || clc.demoplaying) {
        Com_Printf("Not connected to a server.\n");
        return;
    }

    // don't forward the first argument
    if (Cmd_Argc() > 1) {
        CL_AddReliableCommand(Cmd_Args(1));
    }
}

static void CL_SendPureChecksums(void)
{
    const char *pChecksums;
    char cmd[1024];
    int i;

    pChecksums = FS_ReferencedIwdPureChecksums();
    Com_sprintf(cmd, sizeof(cmd), (const char *)"Va ");
    I_strncat(cmd, sizeof(cmd), pChecksums);

    for (i = 0; i < 2; ++i)
        cmd[i] = (char)(cmd[i] + 13 + i * 2);

    CL_AddReliableCommand(cmd);
}

void CL_DownloadsComplete(void)
{
    char *fn;

    if (autoupdateStarted) {
        if (strlen(autoupdateFilename) > 4) {
            fn = va("%s/%s", FS_ShiftStr("ni]Zm^l", 7), autoupdateFilename);
            Sys_StartProcess(fn, 1);
        }
        autoupdateStarted = 0;
        CL_Disconnect();
        return;
    }

    // if we downloaded files we need to restart the file system
    if (clc.downloadRestart) {
        clc.downloadRestart = 0;

        FS_Restart(clc.checksumFeed);

        // inform the server so we get new gamestate info
        CL_AddReliableCommand("donedl");
        return;
    }

    // let the client game init and load data
    clc.state = CA_LOADING;

    if (!com_sv_running->current.enabled) {
        const char *info;
        char mapname[64];
        char gametype[64];

        info = CL_GetConfigString(0);
        I_strncpyz(mapname, Info_ValueForKey(info, "mapname"), sizeof(mapname));
        I_strncpyz(gametype, Info_ValueForKey(info, "g_gametype"), sizeof(gametype));
        UI_SetMap(mapname, gametype);
        SCR_UpdateScreen();
        CL_ShutdownAll();
        Com_Restart();
        CL_StartHunkUsers();
        SCR_UpdateScreen();
    } else if (clients->cgameInitialized) {
        return;
    }

    Dvar_SetInt(cl_paused, 1);
    CL_InitCGame();

    // set pure checksums
    CL_SendPureChecksums();

    CL_WritePacket();
    CL_WritePacket();
    CL_WritePacket();
}

void CL_BeginDownload(const char *localName, const char *remoteName)
{
    Com_DPrintf("***** CL_BeginDownload *****\nLocalname: %s\nRemotename: %s\n****************************\n", localName, remoteName);
    I_strncpyz(clc.downloadName, localName, sizeof(clc.downloadName));
    Com_sprintf(clc.downloadTempName, sizeof(clc.downloadTempName), "%s.tmp", localName);

    I_strncpyz(legacyHacks->cl_downloadName, remoteName, 0x40);
    legacyHacks->cl_downloadSize = 0;
    legacyHacks->cl_downloadCount = 0;
    legacyHacks->cl_downloadTime = cls.realtime;

    clc.downloadBlock = 0;
    clc.downloadCount = 0;

    CL_AddReliableCommand(va("download %s", remoteName));
}
void CL_NextDownload(void)
{
    char *s;
    char *remoteName, *localName;

    if (*clc.downloadList) {
        s = clc.downloadList;
        if (*s == '@')
            s++;
        remoteName = s;

        if ((s = strchr(s, '@')) == NULL) {
            CL_DownloadsComplete();
            return;
        }

        *s++ = 0;
        localName = s;
        if ((s = strchr(s, '@')) != NULL)
            *s++ = 0;
        else
            s = localName + strlen(localName);

        CL_BeginDownload(localName, remoteName);
        clc.downloadRestart = 1;

        memmove(clc.downloadList, s, strlen(s) + 1);
        return;
    }

    CL_DownloadsComplete();
}

void CL_InitDownloads(void)
{
    char missingfiles[1024];
    const char *dir;

    dir = FS_ShiftStr("ni]Zm^l", 7);

    if (autoupdateStarted && NET_CompareAdr(cls.autoupdateServer, clc.serverAddress)) {
        if (strlen(cl_updatefiles->current.string) > 4) {
            I_strncpyz(autoupdateFilename, cl_updatefiles->current.string, sizeof(autoupdateFilename));
            I_strncpyz(clc.downloadList, va("@%s/%s@%s/%s", dir, cl_updatefiles->current.string, dir, cl_updatefiles->current.string), sizeof(clc.downloadList));
            clc.state = CA_CONNECTED;
            CL_NextDownload();
            return;
        }
    } else if (com_sv_running->current.enabled || !cl_allowDownload->current.enabled) {
        if (FS_CompareIwds(missingfiles, sizeof(missingfiles), 0))
            Com_Printf("\nWARNING: You are missing some files referenced by the server:\n%sYou might not be able to join the game\nGo to the settings menu to turn on autodownload, or get the file elsewhere\n\n", missingfiles);
    } else if (FS_CompareIwds(clc.downloadList, sizeof(clc.downloadList), 1)) {
        Com_Printf("Need iwds: %s\n", clc.downloadList);
        if (*clc.downloadList) {
            clc.state = CA_CONNECTED;
            CL_NextDownload();
            return;
        }
    }

    CL_DownloadsComplete();
}
