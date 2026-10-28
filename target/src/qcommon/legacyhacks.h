#ifndef LEGACYHACKS_H
#define LEGACYHACKS_H

struct LegacyHacks {
    int cg_norender;
    int cl_running;
    int cl_stance;
    int cl_stanceTemp;
    int cl_downloadSize;
    int cl_downloadCount;
    int cl_downloadTime;
    char cl_downloadName[64];
    char cl_serverloadmap[64];
    char cl_serverloadgametype[64];
    unsigned char cl_serverloadwaiting;
    unsigned char sv_killserver;
    char sv_killreason[256];
    char ui_scriptMenu[256];
    int ui_scriptMenuIndex;
    char ui_newScriptMenu[256];
    int ui_newScriptMenuIndex;
    char ui_waitingScriptMenu[256];
    int ui_waitingScriptMenuIndex;
    unsigned char ui_waitingScriptMenuNoMouse;
    unsigned char ui_scriptMenuAllowResponse;
    char ui_savegameName[256];
    char ui_savegameInfo[256];
    unsigned char winnt;
    int persid;
};

#endif
