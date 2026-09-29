#include <ctype.h>
#include "common_types.h"
#include "imports.h"
#include "bytematch.h"
extern field_t *chatField;
extern qboolean *chat_team;
extern const vec4_t colorWhite;
extern field_t historyEditLines[];
extern qkey_t *keys;
extern clientActive_t clients[1];
extern field_t g_consoleField;
/* dvar globals */
extern const dvar_t *cg_hudSayPosition;
extern const dvar_t *cl_noprint;

float g_console_char_height = 16.0f;

extern float ceilf(float x);
extern float floorf(float x);


extern void Field_Clear(field_t *field);
extern bool Dvar_GetBool(const char *dvarName);
extern int g_console_field_width;
extern float g_console_char_height;
extern const dvar_t *con_gamemessagetime;
extern const dvar_t *con_boldgamemessagetime;
extern const dvar_t *con_minicontime;
extern const dvar_t *con_miniconlines;
extern const dvar_t *con_restricted;

extern char *CopyStringInternal(const char *in);
extern "C" void Z_FreeInternal( void *ptr );
extern int ColorIndex(unsigned char c);
extern int Cmd_Argc(void);
extern const char *Cmd_Argv( int arg );
extern void Com_Printf(const char *fmt, ...);
extern fileHandle_t FS_FOpenFileWrite(const char *filename);
extern int FS_Write(const void *buffer, int len, int h);
extern void FS_FCloseFile(fileHandle_t h);
extern void I_strncat(char *dest, int size, const char *src);
extern int I_strnicmp(const char *s1, const char *s2, int n);
extern void I_strncpyz(char *dest, const char *src, int destsize);
extern void Cmd_TokenizeString(const char *text);
extern void Dvar_ForEach(void (*callback)(const char *));
extern void Cmd_ForEach(void (*callback)(const char *));
extern void Cmd_AddCommand(const char *name, void (*function)(void));
extern dvar_t *Dvar_RegisterVec4(const char *dvarName, float x, float y, float z, float w, float min, float max, unsigned short flags);
extern dvar_t *Dvar_RegisterFloat(const char *dvarName, float value, float min, float max, unsigned short flags);
extern dvar_t *Dvar_RegisterInt(const char *dvarName, int value, int min, int max, unsigned short flags);
dvar_t *Dvar_RegisterBool(const char *dvarName, bool value, unsigned short flags);
extern dvar_t* Dvar_FindVar(const char* dvarName);
extern bool Dvar_HasLatchedValue(const dvar_t *var);
extern const char *Dvar_DisplayableValue(const dvar_t *dvar);
extern const char *Dvar_DisplayableLatchedValue(const dvar_t *dvar);
extern const char *Dvar_DisplayableResetValue(const dvar_t *dvar);
extern const char *Dvar_GetVariantString(const char *dvarName);
extern const char *Dvar_DomainToString_GetLines(unsigned char type, DvarLimits domain, char *outBuffer, int outBufferLen, int *outLineCount);
extern void CalcScreenPlacement(float *xAdj, float *yAdj, float *xScale, float *yScale, int horzAlign, int vertAlign);
extern void CalcScreenX(float *x, int align);
extern void CalcScreenY(float *y, int align);
extern FontHandle UI_GetFontHandle(int fontEnum, float scale);
extern const char *SEH_SafeTranslateString(const char *str);
extern char *va( const char *format, ... );
extern void CL_DrawText(const char *text, int maxChars, struct Font_s *font, float x, float y, int horzAlign, int vertAlign, float xScale, float yScale, const float *color, int style);
extern void Field_Draw(field_t *field, int x, int y, int width, int horzAlign, int vertAlign);
extern void CL_LookupColor(int colorIndex, float *color);
extern void SCR_DrawSmallStringExt(int x, int y, const char *str, const float *color);
extern void SCR_DrawConsoleString(int x, int y, const short int *text, int len, const float *color);
extern int SEH_PrintStrlen(const char *str);
extern "C" const char *getBuildNumber(void);
extern qboolean Sys_IsMainThread(void);
extern int Sys_Milliseconds(void);
extern void Com_StripExtension(const char *in, char *out);
#ifndef _WIN32
extern void qsort(void *base, int nmemb, int size, int (*compar)(const void *, const void *));
#endif
extern const char **Cmd_GetAutoCompleteFileList(const char *cmd, int *fileCount, int maxCount);
extern void FS_FreeFileList(char **list, int count);
extern int strnicmp(const char *s1, const char *s2, size_t n);
extern void *__DefaultRuneLocale;
extern refexport_t re;
extern clientStatic_t cls;

static struct ConDrawInputGlob conDrawInputGlob;
static struct Console con;
static const dvar_t *con_inputBoxColor;
static const dvar_t *con_inputHintBoxColor;
static const dvar_t *con_outputBarColor;
static const dvar_t *con_outputSliderColor;
static const dvar_t *con_outputWindowColor;
extern int I_stricmp(const char *s1, const char *s2);

extern const vec4_t con_versionColor;
extern const vec4_t con_inputCommandMatchColor;
extern const vec4_t con_inputDvarMatchColor;
extern const vec4_t con_inputDvarValueColor;
extern const vec4_t con_inputDvarInactiveValueColor;
extern const vec4_t con_inputDvarInfoColor;
static int registeredIconMaterialCount;
static const char *hudMsgIconMaterials[256];

static inline __attribute__((always_inline)) clientActive_t *Con_GetClientActive(void)
{
    clientActive_t *cl = clients;

    return cl ? cl : (clientActive_t *)imp_clients;
}

static inline void MsgWnd_Clear(MessageWindow *wnd)
{

    int byteSize = wnd->count * 164;
    memset(wnd->lines, 0, byteSize);
    wnd->current_line = 0;
}

static inline void Con_WriteChar(int colorIdx, int ch)
{
    int row = con.currentLine % con.totallines;
    int offset = row * con.linewidth + con.lineOffset;
    con.textBuffer[offset] = (short)((colorIdx << 8) | ch);
    con.lineOffset++;
}

static inline void Con_ClearAllMessageWindows(void)
{
    MsgWnd_Clear(&con.messageBuffer->gamemsg);
    MsgWnd_Clear(&con.messageBuffer->boldgamemsg);
    MsgWnd_Clear(&con.messageBuffer->minicon);
    MsgWnd_Clear(&con.messageBuffer->subtitle);
}

void CL_RegisterHudMsgIconMaterial(const char *name);
const char *CL_GetHudMsgIconMaterialName(int index);
void Con_ToggleConsole_f(void);
static void Con_ChatModePublic_f(void);
static void Con_ChatModeTeam_f(void);
void Con_Bottom(void);
static void Con_Dump_f(void);
void Con_Shutdown(void);
static void __attribute_regparm__(3) Con_UpdateMessageWindowLine(MessageWindow *msgwnd, qboolean linefeed, int duration);
#ifndef __EMSCRIPTEN__
static inline __attribute__((always_inline)) short Con_FillChar(void)
{
    return (short)((ColorIndex(0x37) << 8) | 0x20);
}
#endif
static void __attribute_regparm__(2) Con_Linefeed(print_msg_type_t type, int duration);
static int CL_ConsolePrint_AddLine(print_msg_type_t type, const char *txt, int duration, int linewidth, int color);
static void Con_UpdateMessageWindowLine_impl(MessageWindow *msgwnd, qboolean linefeed, int duration);
void Con_AutoCompleteFromList(const char **strings, int stringCount, const char *prefix, char *completed, int sizeofCompleted);
static void ConDraw_Box(float x, float y, float w, float h, const vec_t *color);
static int ConDrawInput_CompareStrings(const void *e0, const void *e1);
static void Con_DrawStringOnHUD(int x, int y, int charHeight, int horzAlign, int vertAlign, const short int *string, int length, float alpha, float msgwndScale, qboolean centerText);
void Con_DrawSay(int y);
void Con_ToggleConsoleOutput(void);
static void Con_DrawOuputWindow(void);
void Con_PageUp(void);
void Con_PageDown(void);
void Con_Top(void);
Bool Con_IsActive(void);
static void Con_Clear_f(void);
static void __attribute_regparm__(2) CL_AddConsoleInfoColor(int iFirstInfo, const vec_t *vColor);
static void Con_DrawMessageWindow(MessageWindow *msgwnd, int x, int y, int charHeight, int horzAlign, int vertAlign, float alpha, float msgwndScale, msgwnd_mode_t mode);
void Con_DrawSubtitles(int xPos, int yPos, int charHeight, float alpha, msgwnd_mode_t mode);
void Con_DrawMiniConsole(int xPos, int yPos, float alpha);
void Con_DrawBoldMessages(int xPos, int yPos, float alpha, msgwnd_mode_t mode);
void Con_DrawNotify(int xPos, int yPos, float alpha, msgwnd_mode_t mode);
static void ConDrawInput_IncrMatchCounter(const char *str);
void Con_ClearNotify(void);
void Con_ClearSubtitles(void);
static void ConDrawInput_DvarMatch(const char *str);
const char *Con_TokenizeInput(void);
Bool Con_AnySpaceAfterCommand(void);
static void Con_CheckResize(void);
static void Con_OneTimeInit(void);
void Con_Close(void);
static void ConDrawInput_AutoCompleteArg(const char **strings, int stringCount);
static void ConDrawInput_CmdMatch(const char *str);
void CL_ConsolePrint(print_msg_type_t type, const char *txt, int duration, int linewidth);
void CL_ConsoleFixPosition(void);
static void ConDrawInput_DetailedDvarMatch(const char *str);
static void ConDrawInput_DetailedCmdMatch(const char *str);
static void Con_DrawInput(void);
void Con_DrawConsole(void);
void Con_Init(void);
void CL_DeathMessagePrint(const char *attackerName, const vec_t *attackerColor, const char *victimName, const vec_t *victimColor, const char *iconShader, float iconWidth, float iconHeight, const vec_t *iconColor, int horzFlipIcon);

static inline __attribute__((always_inline)) void CL_RegisterHudMsgIconMaterial_core(const char *name)
{
    int index;
    int count = registeredIconMaterialCount;

    for (index = 0; index < count; index++) {
        if (strcmp(hudMsgIconMaterials[index], name) == 0)
            return;
    }

    hudMsgIconMaterials[count] = CopyStringInternal(name);
    registeredIconMaterialCount++;
}

void CL_RegisterHudMsgIconMaterial(const char *name)
{
    CL_RegisterHudMsgIconMaterial_core(name);
}

const char *CL_GetHudMsgIconMaterialName(int index)
{
    return hudMsgIconMaterials[(unsigned char)index];
}

void Con_ToggleConsole_f(void)
{
    char *field;

    if (con_restricted->current.enabled) {
        if (*(int *)((char *)((void *)keys) + 0x780))
            goto toggle;
        if (!((clients)->keyCatchers & 1))
            return;
    }
toggle:
    field = (char *)&g_consoleField;
    Field_Clear((field_t *)(field));
    ((field_t *)field)->widthInPixels = g_console_field_width;
    ((field_t *)field)->charHeight = g_console_char_height;
    ((field_t *)field)->fixedSize = 1;
    con.outputVisible = 0;
    clients[0].keyCatchers ^= 1;
}

static void Con_ChatModePublic_f(void)
{
    *chat_team = 0;
    Field_Clear(chatField);
    chatField->widthInPixels = 0x24c;
    chatField->charHeight = 10.0f;
    chatField->fixedSize = 0;
    clients->keyCatchers ^= 0x10;
}

static void Con_ChatModeTeam_f(void)
{
    *chat_team = 1;
    Field_Clear(chatField);
    chatField->widthInPixels = 0x21f;
    chatField->charHeight = 10.0f;
    chatField->fixedSize = 0;
    clients->keyCatchers ^= 0x10;
}

void Con_Bottom(void)
{
    con.display = con.currentLine;
}

static void Con_Dump_f(void)
{
    int f;
    char buffer[0x400];
    int lineIndex, linewidth, i, trimLen;
    short *linePtr;

    if (Cmd_Argc() != 2) {
        Com_Printf("usage: condump <filename>\n");
        return;
    }

    Com_Printf("Dumped console text to %s.\n", Cmd_Argv(1));

    f = FS_FOpenFileWrite(Cmd_Argv(1));
    if (!f) {
        Com_Printf("ERROR: couldn't open.\n");
        return;
    }

    linewidth = con.linewidth;
    for (lineIndex = con.currentLine - con.totallines + 1; lineIndex <= con.currentLine; lineIndex++) {

        linePtr = &con.textBuffer[(lineIndex % con.totallines) * linewidth];

        if (linewidth <= 0) {
            if (linewidth == 0)
                continue;
        } else {
            int allSpaces = 1;
            for (i = 0; i < linewidth; i++) {
                if ((linePtr[i] & 0xFF) != 0x20) {
                    allSpaces = 0;
                    break;
                }
            }
            if (allSpaces)
                continue;
        }

        for (i = 0; i < linewidth; i++) {
            buffer[i] = (char)(linePtr[i] & 0xFF);
        }

        trimLen = linewidth - 1;
        while (trimLen >= 0 && buffer[trimLen] == ' ')
            trimLen--;
        buffer[trimLen + 1] = '\0';

        I_strncat(buffer, 0x400, "\n");
        FS_Write(buffer, strlen(buffer), f);
    }

    FS_FCloseFile(f);
}

void Con_Shutdown(void)
{
    while (registeredIconMaterialCount != 0) {
        registeredIconMaterialCount--;
        Z_FreeInternal((void *)hudMsgIconMaterials[registeredIconMaterialCount]);
    }
}

static void Con_UpdateMessageWindowLine_impl(MessageWindow *msgwnd, qboolean linefeed, int duration)
{
    MessageLine *line;
    int serverTime;
    int origLinewidth;
    int copyCount;
    int textOffset;
    int charIndex;
    int i;

    line = &msgwnd->lines[msgwnd->current_line];

    serverTime = (clients)->serverTime;
    line->startTime = serverTime;

    line->endTime = duration + serverTime;

    {
        origLinewidth = con.linewidth;
        copyCount = origLinewidth;

        if (origLinewidth >= 79)
            copyCount = 78;

        textOffset = origLinewidth * (con.currentLine % con.totallines);

        if (copyCount > 0) {
            for (charIndex = 0; charIndex < copyCount; charIndex++) {
                line->textBuffer[charIndex] = con.textBuffer[textOffset + charIndex];
            }
        }

        if (copyCount <= 77) {
            short fillChar = (short)((ColorIndex(0x37) << 8) | 0x20);
            while (copyCount < 78) {
                line->textBuffer[copyCount] = fillChar;
                copyCount++;
            }
        }
    }

    if (!linefeed)
        return;
    if (msgwnd->count <= 0)
        return;

    msgwnd->current_line = (msgwnd->current_line + 1) % msgwnd->count;

    if (msgwnd->padding <= 0)
        return;

    for (i = 0; i < msgwnd->padding; i++) {
        int lineIdx;
        MessageLine *otherLine;
        int endTime;
        int fadeout;
        char *cl_ptr;
        int curTime;

        lineIdx = (i + msgwnd->current_line) % msgwnd->count;
        otherLine = &msgwnd->lines[lineIdx];

        endTime = otherLine->endTime;
        fadeout = msgwnd->fadeout;
        cl_ptr = (char *)((void *)clients);
        curTime = ((clientActive_t *)cl_ptr)->serverTime;

        if (endTime - fadeout > curTime) {

            int lineDuration = endTime - otherLine->startTime;
            otherLine->startTime = fadeout + (curTime - lineDuration);

            otherLine->endTime = (clients)->serverTime + msgwnd->fadeout;
        }
    }
}

#ifndef __EMSCRIPTEN__
static void __attribute_regparm__(3) Con_UpdateMessageWindowLine(MessageWindow *msgwnd, qboolean linefeed, int duration)
{
    Con_UpdateMessageWindowLine_impl(msgwnd, linefeed, duration);
}

static void __attribute_regparm__(2) Con_Linefeed(print_msg_type_t type, int duration)
{
    int cursor;

    if (con.currentLine >= 0) {
        switch (type) {
        case PMSG_CONSOLE:
            Con_UpdateMessageWindowLine(&con.messageBuffer->minicon, 1, duration);
            break;
        case PMSG_GAME:
            Con_UpdateMessageWindowLine(&con.messageBuffer->gamemsg, 1, duration);
            break;
        case PMSG_BOLDGAME:
            Con_UpdateMessageWindowLine(&con.messageBuffer->boldgamemsg, 1, duration);
            break;
        case PMSG_SUBTITLE:
            Con_UpdateMessageWindowLine(&con.messageBuffer->subtitle, 1, duration);
            break;
        default:
            break;
        }
    }

    con.lineOffset = 0;
    if (con.display == con.currentLine)
        con.display++;
    con.currentLine++;

    if (con.linewidth <= 0)
        return;

    for (cursor = 0; cursor < con.linewidth; cursor++) {
        int textIndex = (con.currentLine % con.totallines) * con.linewidth + cursor;
        con.textBuffer[textIndex] = Con_FillChar();
    }
}

void Con_AutoCompleteFromList(const char **strings, int stringCount, const char *prefix, char *completed, int sizeofCompleted)
{
    int prefixLen = strlen(prefix);
    int stringIndex;

    completed[0] = '\0';

    for (stringIndex = 0; stringIndex < stringCount; stringIndex++) {
        const char *string = strings[stringIndex];

        if (strnicmp(prefix, string, prefixLen) != 0)
            continue;

        if (completed[0] == '\0') {

            I_strncpyz(completed, string, sizeofCompleted);
        } else {

            char *p = completed + prefixLen;
            const char *s = string + prefixLen;
            while (*p && *p == *s) {
                p++;
                s++;
            }
            *p = '\0';
        }
    }
}

static void ConDraw_Box(float x, float y, float w, float h, const vec_t *color)
{
    clientStatic_t *cls_ = &cls;
    vec4_t darkColor;

    re.DrawStretchPic(x, y, w, h, 0.0f, 0.0f, 0.0f, 0.0f, color, cls_->whiteMaterial);

    darkColor[0] = color[0] * 0.5f;
    darkColor[1] = color[1] * 0.5f;
    darkColor[2] = color[2] * 0.5f;
    darkColor[3] = color[3];

    re.DrawStretchPic(x, y, 2.0f, h, 0.0f, 0.0f, 0.0f, 0.0f, darkColor, cls_->whiteMaterial);
    re.DrawStretchPic(x + w - 2.0f, y, 2.0f, h, 0.0f, 0.0f, 0.0f, 0.0f, darkColor, cls_->whiteMaterial);
    re.DrawStretchPic(x, y, w, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, darkColor, cls_->whiteMaterial);
    re.DrawStretchPic(x, y + h - 2.0f, w, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, darkColor, cls_->whiteMaterial);
}

static int ConDrawInput_CompareStrings(const void *e0, const void *e1)
{
    return I_stricmp(*(const char **)e0, *(const char **)e1);
}

static void Con_DrawStringOnHUD(int x, int y, int charHeight, int horzAlign, int vertAlign, const short int *string, int length, float alpha, float msgwndScale, qboolean centerText)
{
    vec4_t color;
    FontHandle font;
    float fontScale;
    float xScale;
    float yScale;
    float xAdj;
    float yAdj;

    color[0] = 1.0f;
    color[1] = 1.0f;
    color[2] = 1.0f;
    color[3] = alpha;

    fontScale = (float)charHeight / 48.0f;
    font = (FontHandle)UI_GetFontHandle(centerText ? 4 : 0, fontScale);
    xScale = re.NormalizedTextScale(font, fontScale);

    if (centerText)
        x -= (int)((float)re.ConsoleTextWidth(string, length, font) * xScale * 0.5f);

    xScale *= msgwndScale;
    yScale = xScale;
    xAdj = (float)x;
    yAdj = (float)(y + (int)((float)re.TextHeight(font) * yScale));

    CalcScreenPlacement(&xAdj, &yAdj, &xScale, &yScale, horzAlign, vertAlign);
    re.DrawConsoleText(string, length, font, xAdj, yAdj, xScale, yScale, color, 3);
}

void Con_DrawSay(int y)
{
    clientActive_t *cl = clients;
    const dvar_t *hudSayPosition;
    const char *label;
    const char *string;
    field_t *field;
    FontHandle font;
    float fontScale;
    float normalizedScale;
    int x;
    int fontHeight;
    int textWidth;

    if (!(cl->keyCatchers & 0x10))
        return;

    fontScale = 10.0f / 48.0f;
    font = (FontHandle)UI_GetFontHandle(0, fontScale);

    if (*chat_team)
        label = SEH_SafeTranslateString("EXE_SAYTEAM");
    else
        label = SEH_SafeTranslateString("EXE_SAY");

    string = va("%s: ", label);
    normalizedScale = re.NormalizedTextScale(font, fontScale);

    hudSayPosition = cg_hudSayPosition;
    x = (int)hudSayPosition->current.vector[0];

    fontHeight = re.TextHeight(font);
    CL_DrawText(string, 0x7fffffff, font, (float)x,
                (float)y + (float)fontHeight * normalizedScale,
                1, 1, normalizedScale, normalizedScale,
                (const float *)((void *)&colorWhite), 3);

    textWidth = re.TextWidth(string, 0, font);
    field = ((field_t *)chatField);
    Field_Draw(field, x + (int)((float)textWidth * normalizedScale), y,
               1, 1, 1);
}

void Con_ToggleConsoleOutput(void)
{
    con.outputVisible = (con.outputVisible == 0) ? 1 : 0;
}

static BM_NOINLINE void Con_DrawOuputWindow(void)
{
    float x;
    float y;
    float width;
    float height;
    float innerX;
    float innerY;
    float innerHeight;
    float scrollBarX;
    float scrollBarWidth;
    float textHeight;
    float textY;
    float sliderY;
    float sliderRange;
    float sliderFrac;
    int rowCount;
    int row;
    int rowIndex;
    vec4_t textColor;
    const char *version;

    x = con.screenMin[0];
    y = con.screenMin[1] + 32.0f;
    width = con.screenMax[0] - x;
    height = con.screenMax[1] - con.screenMin[1] - 32.0f;

    ConDraw_Box(x, y, width, height, con_outputWindowColor->current.vector);

    innerX = x + 6.0f;
    innerY = y + 6.0f;
    innerHeight = height - 12.0f;

    version = va("Build %s %s", getBuildNumber(), "win-x86");
    SCR_DrawSmallStringExt((int)innerX, (int)(innerY + innerHeight - 16.0f), version, con_versionColor);

    scrollBarWidth = 10.0f;
    scrollBarX = innerX + width - 22.0f;
    ConDraw_Box(scrollBarX, innerY, scrollBarWidth, innerHeight, con_outputBarColor->current.vector);

    sliderFrac = 0.0f;
    if (con.totallines > 1)
        sliderFrac = 1.0f - ((float)(con.currentLine - con.display) / (float)(con.totallines - 1));
    if (sliderFrac < 0.0f)
        sliderFrac = 0.0f;
    if (sliderFrac > 1.0f)
        sliderFrac = 1.0f;

    sliderRange = innerHeight - scrollBarWidth;
    sliderY = innerY + sliderRange * sliderFrac;
    ConDraw_Box(scrollBarX, sliderY, scrollBarWidth, scrollBarWidth, con_outputSliderColor->current.vector);

    textHeight = innerHeight - 28.0f;
    rowCount = (int)(textHeight * 0.0625f);
    row = con.display;
    if (con.lineOffset < 1)
        row--;

    CL_LookupColor('7', textColor);
    textY = innerY + textHeight;

    for (rowIndex = 0; rowIndex < rowCount && rowIndex != row + 1; rowIndex++) {
        int textRow = row - rowIndex;
        int wrappedRow;
        const short *line;

        if (textRow < 0)
            break;
        if (con.currentLine - textRow >= con.totallines)
            continue;

        textY -= 16.0f;
        wrappedRow = textRow % con.totallines;
        if (wrappedRow < 0)
            wrappedRow += con.totallines;

        line = &con.textBuffer[wrappedRow * con.linewidth];
        SCR_DrawConsoleString((int)innerX, (int)textY, line, con.linewidth, textColor);
    }
}

void Con_PageUp(void)
{
    con.display -= 2;
    if (con.currentLine - con.display >= con.totallines)
        con.display = con.currentLine - con.totallines + 1;
}

void Con_PageDown(void)
{
    con.display += 2;
    if (con.display > con.currentLine)
        con.display = con.currentLine;
}

void Con_Top(void)
{
    con.display = con.totallines;
    if (con.currentLine - con.totallines >= con.totallines)
        con.display = con.currentLine - con.totallines + 1;
}

Bool Con_IsActive(void)
{
    return (*(int *)((char *)((void *)clients) + 4) & 1) != 0;
}

static void Con_Clear_f(void)
{
    int i;
    for (i = 0; i < 65536; i++) {
        con.textBuffer[i] = (ColorIndex(0x37) << 8) | 0x20;
    }
    con.display = con.currentLine;
}

static inline __attribute__((always_inline)) int CL_ClampColorByte(float value)
{
    float v = (float)(int)(value * 255.0f);
    float hi = (v >= 255.0f) ? 255.0f : v;

    if (v > 0.0f)
        return (int)hi;

    return 0;
}

static void __attribute_regparm__(2) CL_AddConsoleInfoColor(int iFirstInfo, const vec_t *vColor)
{
    int lineBase = (con.currentLine % con.totallines) * con.linewidth;
    int offset = con.lineOffset;

    con.textBuffer[lineBase + offset] = (short)((iFirstInfo << 8) | CL_ClampColorByte(vColor[0]));
    offset++;
    con.lineOffset = offset;

    con.textBuffer[lineBase + offset] = (short)(((iFirstInfo + 1) << 8) | CL_ClampColorByte(vColor[1]));
    offset++;
    con.lineOffset = offset;

    con.textBuffer[lineBase + offset] = (short)(((iFirstInfo + 2) << 8) | CL_ClampColorByte(vColor[2]));
    con.lineOffset = offset + 1;
}

static MessageLine *Con_MessageLineForIndex(MessageWindow *msgwnd, int lineIndex)
{
    int index;

    if (!msgwnd || !msgwnd->lines || msgwnd->count <= 0)
        return NULL;

    index = lineIndex % msgwnd->count;
    if (index < 0)
        index += msgwnd->count;

    return &msgwnd->lines[index];
}

static qboolean Con_MessageWindowVisible(void)
{
    clientActive_t *cl = clients;

    if (!cl)
        return 0;

    if (cl->snap.ps.pm_type == 5)
        return 1;

    if (!(cl->keyCatchers & 8))
        return 1;

    return cl->displayHUDWithKeycatchUI != 0;
}

static int Con_MessageLineScrollOffset(MessageWindow *msgwnd, MessageLine *line, int serverTime, int charHeight)
{
    int remaining;

    if (!msgwnd || !line || msgwnd->scrolltime <= 0)
        return 0;

    if (serverTime <= line->endTime)
        return 0;

    remaining = msgwnd->scrolltime - (serverTime - line->endTime);
    if (remaining <= 0)
        return 0;

    return I_fround(((float)remaining / (float)msgwnd->scrolltime) * (float)charHeight);
}

static qboolean Con_MessageLineAlpha(MessageWindow *msgwnd, MessageLine *line, int serverTime, float baseAlpha, float *drawAlpha)
{
    int elapsed;
    int remaining;

    if (!msgwnd || !line || !line->startTime)
        return 0;

    if (line->startTime > serverTime) {
        line->startTime = 0;
        return 0;
    }

    if (serverTime > line->endTime)
        return 0;

    *drawAlpha = baseAlpha;

    elapsed = serverTime - line->startTime;
    if (msgwnd->fadein > 0 && elapsed < msgwnd->fadein) {
        *drawAlpha = ((float)elapsed / (float)msgwnd->fadein) * baseAlpha;
        return 1;
    }

    remaining = line->endTime - serverTime;
    if (msgwnd->fadeout > 0 && remaining < msgwnd->fadeout)
        *drawAlpha = ((float)remaining / (float)msgwnd->fadeout) * baseAlpha;

    return 1;
}

static void Con_DrawMessageWindowTopDown(MessageWindow *msgwnd, int x, int y, int charHeight, int horzAlign, int vertAlign, float alpha, float msgwndScale, qboolean reversed)
{
    clientActive_t *cl = clients;
    int serverTime = cl ? cl->serverTime : 0;
    int lineIndex;
    int endLine;
    int v;

    if (!msgwnd || msgwnd->count <= 0)
        return;

    v = reversed ? y - charHeight : y;
    endLine = msgwnd->current_line + msgwnd->count;

    for (lineIndex = msgwnd->current_line; lineIndex < endLine; lineIndex++) {
        MessageLine *line = Con_MessageLineForIndex(msgwnd, lineIndex);
        float drawAlpha;
        int scrollOffset;

        if (!line)
            continue;

        if (!line->startTime)
            continue;

        if (line->startTime > serverTime) {
            line->startTime = 0;
            continue;
        }

        scrollOffset = Con_MessageLineScrollOffset(msgwnd, line, serverTime, charHeight);
        if (scrollOffset) {
            if (reversed)
                v -= scrollOffset;
            else
                v += scrollOffset;
            continue;
        }

        if (!Con_MessageLineAlpha(msgwnd, line, serverTime, alpha, &drawAlpha))
            continue;

        Con_DrawStringOnHUD(x, v, charHeight, horzAlign, vertAlign,
                            line->textBuffer, 78, drawAlpha, msgwndScale, 0);

        if (reversed)
            v -= charHeight;
        else
            v += charHeight;
    }
}

static void Con_DrawMessageWindowBottomUp(MessageWindow *msgwnd, int x, int y, int charHeight, int horzAlign, int vertAlign, float alpha, float msgwndScale, qboolean centered)
{
    clientActive_t *cl = clients;
    int serverTime = cl ? cl->serverTime : 0;
    int lineIndex;
    int lineHeight;

    if (!msgwnd || msgwnd->count <= 0)
        return;

    lineHeight = charHeight + (centered ? 4 : 0);

    for (lineIndex = msgwnd->current_line + msgwnd->count - 1; lineIndex >= msgwnd->current_line; lineIndex--) {
        MessageLine *line = Con_MessageLineForIndex(msgwnd, lineIndex);
        float drawAlpha;

        if (!line)
            continue;

        if (!Con_MessageLineAlpha(msgwnd, line, serverTime, alpha, &drawAlpha)) {
            if (line->startTime && serverTime >= line->endTime)
                line->startTime = 0;
            continue;
        }

        y -= lineHeight;
        Con_DrawStringOnHUD(x, y, lineHeight, horzAlign, vertAlign,
                            line->textBuffer, 78, drawAlpha, msgwndScale, centered);
    }
}

static void Con_DrawMessageWindow(MessageWindow *msgwnd, int x, int y, int charHeight, int horzAlign, int vertAlign, float alpha, float msgwndScale, msgwnd_mode_t mode)
{
    if (!Con_MessageWindowVisible())
        return;

    switch (mode) {
    case MWM_TOPDOWN:
        Con_DrawMessageWindowTopDown(msgwnd, x, y, charHeight, horzAlign, vertAlign, alpha, msgwndScale, 0);
        break;
    case MWM_TOPDOWN_REVERSED:
        Con_DrawMessageWindowTopDown(msgwnd, x, y, charHeight, horzAlign, vertAlign, alpha, msgwndScale, 1);
        break;
    case MWM_BOTTOMUP:
        Con_DrawMessageWindowBottomUp(msgwnd, x, y, charHeight, horzAlign, vertAlign, alpha, msgwndScale, 0);
        break;
    case MWM_BOTTOMUP_CENTERED:
        Con_DrawMessageWindowBottomUp(msgwnd, x, y, charHeight, horzAlign, vertAlign, alpha, msgwndScale, 1);
        break;
    default:
        break;
    }
}

void Con_DrawSubtitles(int xPos, int yPos, int charHeight, float alpha, msgwnd_mode_t mode)
{
    Con_DrawMessageWindow(&con.messageBuffer->subtitle, xPos, yPos, charHeight, 1, 1, alpha, 1.0f, mode);
}

void Con_DrawMiniConsole(int xPos, int yPos, float alpha)
{
    con.messageBuffer->minicon.count = con_miniconlines->current.integer;
    Con_DrawMessageWindow(&con.messageBuffer->minicon, xPos, yPos, 12, 1, 1, alpha, 1.0f, MWM_TOPDOWN);
}

void Con_DrawBoldMessages(int xPos, int yPos, float alpha, msgwnd_mode_t mode)
{
    Con_DrawMessageWindow(&con.messageBuffer->boldgamemsg, xPos, yPos, 12, 7, 7, alpha, 1.0f, mode);
}

void Con_DrawNotify(int xPos, int yPos, float alpha, msgwnd_mode_t mode)
{
    Con_DrawMessageWindow(&con.messageBuffer->gamemsg, xPos, yPos, 12, 1, 3, alpha, 1.0f, mode);
}

static void ConDrawInput_IncrMatchCounter(const char *str)
{
    if (I_strnicmp(str, conDrawInputGlob.inputText, conDrawInputGlob.inputTextLen) != 0)
        return;

    conDrawInputGlob.matchCount++;

    if (str[conDrawInputGlob.inputTextLen] == '\0')
        conDrawInputGlob.hasExactMatch = 1;
}

void Con_ClearNotify(void)
{
    MsgWnd_Clear(&con.messageBuffer->gamemsg);
    MsgWnd_Clear(&con.messageBuffer->boldgamemsg);
}

void Con_ClearSubtitles(void)
{
    MsgWnd_Clear(&con.messageBuffer->subtitle);
}

static void ConDrawInput_DvarMatch(const char *str)
{
    clientStatic_t *cls_;
    const char *value;

    if (I_strnicmp(str, conDrawInputGlob.inputText, conDrawInputGlob.inputTextLen) != 0)
        return;

    cls_ = &cls;

    re.DrawText(str, 0x18, cls_->consoleFont, conDrawInputGlob.x, conDrawInputGlob.y + conDrawInputGlob.fontHeight, 1.0f, 1.0f, con_inputDvarMatchColor, 0);
    conDrawInputGlob.x += 200.0f;

    value = Dvar_GetVariantString(str);
    re.DrawText(value, 0x28, cls_->consoleFont, conDrawInputGlob.x, conDrawInputGlob.y + conDrawInputGlob.fontHeight, 1.0f, 1.0f, con_inputDvarValueColor, 0);

    conDrawInputGlob.y += conDrawInputGlob.fontHeight;
    conDrawInputGlob.x = conDrawInputGlob.leftX;
}

static inline __attribute__((always_inline)) int isPunctChar(int c)
{
    return isspace(c);
}

const char *Con_TokenizeInput(void)
{
    const char *cmd;
    unsigned char first;

    Cmd_TokenizeString((const char *)&g_consoleField + 0x18);
    cmd = Cmd_Argv(0);

    first = (unsigned char)cmd[0];
    if (first == '\\' || first == '/')
        cmd++;

    while (isPunctChar((signed char)*cmd))
        cmd++;

    return cmd;
}

Bool Con_AnySpaceAfterCommand(void)
{
    const char *p = (const char *)&g_consoleField;
    int offset = 0;

    while (isPunctChar((signed char)p[0x18 + offset]))
        offset++;

    {
        const char *scan = p + 0x18 + offset;
        while (*scan) {
            if (isPunctChar((signed char)*scan))
                return 1;
            scan++;
        }
    }

    return 0;
}

static void Con_ClearMessageWindow(MessageWindow *wnd)
{
    if (!wnd)
        return;
    MsgWnd_Clear(wnd);
}

static void Con_CheckResize(void)
{
    short *tbuf;
    int oldLinewidth;
    int oldTotalLines;
    int lineCount;
    int charCount;
    int i;
    int j;
    int screenWidth;
    int newLinewidth;

    LargeLocal largeLocal(0x20000);
    tbuf = (short *)largeLocal.GetBuf();

    con.screenMin[0] = 4.0f;
    con.screenMin[1] = 4.0f;
    con.screenMax[0] = -4.0f;
    con.screenMax[1] = -4.0f;

    CalcScreenX(&con.screenMin[0], 1);
    CalcScreenY(&con.screenMin[1], 1);
    CalcScreenX(&con.screenMax[0], 3);
    CalcScreenY(&con.screenMax[1], 3);

    con.screenMin[0] = floorf(con.screenMin[0]);
    con.screenMin[1] = floorf(con.screenMin[1]);
    con.screenMax[0] = floorf(con.screenMax[0]);
    con.screenMax[1] = floorf(con.screenMax[1]);

    screenWidth = (int)(con.screenMax[0] - con.screenMin[0]);
    if (screenWidth > 639)
        screenWidth >>= 3;
    else
        screenWidth = 80;

    newLinewidth = screenWidth - 2;
    oldLinewidth = con.linewidth;
    if (newLinewidth == oldLinewidth) {
        return;
    }

    if (newLinewidth <= 0) {
        con.linewidth = 78;
        con.totallines = 0x348;
        for (i = 0; i < 65536; i++)
            con.textBuffer[i] = Con_FillChar();
        con.currentLine = con.totallines - 1;
        con.display = con.currentLine;
        return;
    }

    con.linewidth = newLinewidth;
    oldTotalLines = con.totallines;
    lineCount = 65536 / newLinewidth;
    con.totallines = lineCount;
    if (oldTotalLines < lineCount)
        lineCount = oldTotalLines;

    charCount = oldLinewidth;
    if (charCount > newLinewidth)
        charCount = newLinewidth;

    memcpy(tbuf, con.textBuffer, 0x20000);

    for (i = 0; i < 65536; i++)
        con.textBuffer[i] = Con_FillChar();

    if (lineCount > 0) {
        for (i = 0; i < lineCount; i++) {
            int destBase = (con.totallines - i - 1) * con.linewidth;
            int srcLine = (con.currentLine - i + oldTotalLines) % oldTotalLines;
            int srcBase = srcLine * oldLinewidth;

            if (charCount > 0) {
                for (j = 0; j < charCount; j++)
                    con.textBuffer[destBase + j] = tbuf[srcBase + j];
            }
        }
    }

    MsgWnd_Clear(&con.messageBuffer->gamemsg);
    MsgWnd_Clear(&con.messageBuffer->boldgamemsg);
    MsgWnd_Clear(&con.messageBuffer->minicon);
    MsgWnd_Clear(&con.messageBuffer->subtitle);

    con.currentLine = con.totallines - 1;
    con.display = con.currentLine;
}

static void Con_InitMessageBuffers(void)
{
    MessageBuffer *messageBuffer;

    messageBuffer = con.messageBufferArray;

    messageBuffer->gamemsg.scrolltime = 250;
    messageBuffer->gamemsg.fadein = 250;
    messageBuffer->boldgamemsg.scrolltime = 250;
    messageBuffer->boldgamemsg.fadein = 250;
    messageBuffer->subtitle.scrolltime = 250;
    messageBuffer->subtitle.fadein = 250;

    messageBuffer->gamemsg.lines = messageBuffer->gamemsg_lines;
    messageBuffer->gamemsg.current_line = 0;
    messageBuffer->gamemsg.count = 8;
    messageBuffer->gamemsg.padding = 3;
    messageBuffer->gamemsg.fadeout = 500;

    messageBuffer->boldgamemsg.lines = messageBuffer->boldgamemsg_lines;
    messageBuffer->boldgamemsg.current_line = 0;
    messageBuffer->boldgamemsg.count = 8;
    messageBuffer->boldgamemsg.padding = 3;
    messageBuffer->boldgamemsg.fadeout = 500;

    messageBuffer->subtitle.lines = messageBuffer->subtitle_lines;
    messageBuffer->subtitle.current_line = 0;
    messageBuffer->subtitle.count = 8;
    messageBuffer->subtitle.padding = 3;
    messageBuffer->subtitle.fadeout = 500;

    messageBuffer->minicon.lines = messageBuffer->minicon_lines;
    messageBuffer->minicon.current_line = 0;
    messageBuffer->minicon.count = con_miniconlines->current.integer;
    messageBuffer->minicon.padding = 0;
    messageBuffer->minicon.scrolltime = 0;
    messageBuffer->minicon.fadein = 0;
    messageBuffer->minicon.fadeout = 0;

    con.messageBuffer = messageBuffer;
}

static void Con_OneTimeInit(void)
{
    con_inputBoxColor = Dvar_RegisterVec4("con_inputBoxColor", 0.25f, 0.25f, 0.2f, 1.0f, 0.0f, 1.0f, 0x1001);
    con_inputHintBoxColor = Dvar_RegisterVec4("con_inputHintBoxColor", 0.4f, 0.4f, 0.35f, 1.0f, 0.0f, 1.0f, 0x1001);
    con_outputBarColor = Dvar_RegisterVec4("con_outputBarColor", 1.0f, 1.0f, 0.95f, 0.6f, 0.0f, 1.0f, 0x1001);
    con_outputSliderColor = Dvar_RegisterVec4("con_outputSliderColor", 0.15f, 0.15f, 0.1f, 0.6f, 0.0f, 1.0f, 0x1001);
    con_outputWindowColor = Dvar_RegisterVec4("con_outputWindowColor", 0.35f, 0.35f, 0.3f, 0.75f, 0.0f, 1.0f, 0x1001);

    con_gamemessagetime = Dvar_RegisterFloat("con_gamemessagetime", 5.0f, 0.0f, 3.402823466e38f, 0x1000);
    con_boldgamemessagetime = Dvar_RegisterFloat("con_boldgamemessagetime", 8.0f, 0.0f, 3.402823466e38f, 0x1000);
    con_minicontime = Dvar_RegisterFloat("con_minicontime", 4.0f, 0.0f, 3.402823466e38f, 0x1001);
    con_miniconlines = Dvar_RegisterInt("con_miniconlines", 5, 0, 100, 0x1001);

    Con_InitMessageBuffers();


    con.color[0] = 1.0f;
    con.color[1] = 1.0f;
    con.color[2] = 1.0f;
    con.color[3] = 1.0f;
    con.linewidth = -1;
    Con_CheckResize();
    con.initialized = 1;
}

static int CL_ConsolePrint_AddLine(print_msg_type_t type, const char *txt, int duration, int linewidth, int color)
{
    const char *buf;
    int targetLineWidth;
    int lineBroken;

    {
        int clamp;

        if (type == PMSG_CONSOLE || type == PMSG_LOGFILE)
            clamp = con.linewidth;
        else
            clamp = (con.linewidth >= 78) ? 78 : con.linewidth;

        if (linewidth <= 0 || clamp < linewidth)
            linewidth = clamp;
    }

    if (type == PMSG_GAME || type == PMSG_BOLDGAME) {
        int printLen = SEH_PrintStrlen(txt);
        if (printLen > linewidth) {
            float lines = ceilf((float)printLen / (float)linewidth);
            targetLineWidth = (int)((float)printLen / lines);
        } else {
            targetLineWidth = linewidth;
        }
    } else {
        targetLineWidth = linewidth;
    }

    if (con.prevType != type && con.lineOffset > 0)
        Con_Linefeed(type, duration);

    buf = txt;
    lineBroken = 0;

    while (*buf) {
        unsigned char c = (unsigned char)*buf;

        if (c == '^' && buf[1] != '\0' && buf[1] != '^' && (unsigned char)buf[1] > '/' && (unsigned char)buf[1] <= '9') {
            color = ColorIndex((unsigned char)buf[1]);
            buf += 2;
            continue;
        }

        if (linewidth > 0) {
            if (c > ' ') {
                int wordLen = 0;
                do {
                    wordLen++;
                    if (wordLen == linewidth)
                        break;
                } while ((unsigned char)buf[wordLen] > ' ');

                if (wordLen != linewidth && con.lineOffset + wordLen > linewidth) {
                    Con_Linefeed(type, duration);
                    lineBroken = 1;
                }
            } else if (con.lineOffset > linewidth) {
                Con_Linefeed(type, duration);
                lineBroken = 1;
            }
        }

        buf++;

        if (c == '\n') {
            Con_Linefeed(type, duration);
            continue;
        }

        if (c == '\r') {
            con.lineOffset = 0;
            continue;
        }

        if (con.lineOffset == 0 && c == ' ' && lineBroken)
            continue;

        {
            int textIndex = (con.currentLine % con.totallines) * con.linewidth + con.lineOffset;
            con.textBuffer[textIndex] = (short)((color << 8) | c);
            con.lineOffset++;
        }

        if (con.lineOffset >= linewidth || (con.lineOffset >= targetLineWidth && c == ' ')) {
            Con_Linefeed(type, duration);
            lineBroken = 1;
        }
    }

    if (con.lineOffset > 0) {
        if (type != PMSG_CONSOLE) {
            Con_Linefeed(type, duration);
        } else if (con.currentLine >= 0) {
            Con_UpdateMessageWindowLine(&con.messageBuffer->minicon, 0, duration);
        }
    }

    con.prevType = type;
    return color;
}

void CL_ConsolePrint(print_msg_type_t type, const char *txt, int duration, int linewidth)
{
    const dvar_t *noPrint;
    int color;

    noPrint = cl_noprint;
    if (!noPrint)
        return;
    if (noPrint->current.enabled)
        return;
    if (type == PMSG_LOGFILE)
        return;

    if (!con.initialized)
        Con_OneTimeInit();

    if (!duration) {
        switch (type) {
        case PMSG_CONSOLE:
            duration = I_fround(con_minicontime->current.value * 1000.0f);
            break;
        case PMSG_GAME:
            duration = I_fround(con_gamemessagetime->current.value * 1000.0f);
            break;
        case PMSG_BOLDGAME:
            duration = I_fround(con_boldgamemessagetime->current.value * 1000.0f);
            break;
        case PMSG_SUBTITLE:
            duration = 5000;
        default:
            goto skipClamp;
        }
    }

    if (duration < 0)
        duration = 0;

skipClamp:

    color = ColorIndex(0x37);

    if (type == PMSG_GAME || type == PMSG_BOLDGAME) {
        const char *newline;
        char lineText[0x1000];

        while ((newline = strchr(txt, '\n')) != NULL) {
            unsigned int copyCount = (unsigned int)(newline - txt) + 1;

            if (copyCount > 0xfff)
                copyCount = 0xfff;

            memcpy(lineText, txt, copyCount);
            lineText[copyCount] = '\0';
            color = CL_ConsolePrint_AddLine(type, lineText, duration, linewidth, color);
            txt = newline + 1;
        }
    }

    CL_ConsolePrint_AddLine(type, txt, duration, linewidth, color);
}

void CL_ConsoleFixPosition(void)
{
    CL_ConsolePrint(PMSG_CONSOLE, "\n", 0, 0);
    con.display = con.currentLine - 1;
}

void Con_Close(void)
{
    int *legacyHacks = (int *)::legacyHacks;
    int active = legacyHacks[1];
    if (!active)
        return;

    Field_Clear((&g_consoleField));
    Con_ClearNotify();
    MsgWnd_Clear(&con.messageBuffer->minicon);
    Con_ClearSubtitles();
    clients[0].keyCatchers &= ~1;
}

static inline __attribute__((always_inline)) void ConDrawInput_DrawText(const char *text, int maxChars, const vec_t *color)
{
    FontHandle font = cls.consoleFont;

    re.DrawText(text, maxChars, font, conDrawInputGlob.x, conDrawInputGlob.y + conDrawInputGlob.fontHeight, 1.0f, 1.0f, color, 0);
}

static inline __attribute__((always_inline)) void ConDrawInput_NextLine(void)
{
    conDrawInputGlob.y += conDrawInputGlob.fontHeight;
    conDrawInputGlob.x = conDrawInputGlob.leftX;
}

static void ConDrawInput_Text(const char *str, const vec_t *color)
{
    re.DrawText(str, 0x7fffffff, cls.consoleFont, conDrawInputGlob.x, conDrawInputGlob.y + conDrawInputGlob.fontHeight, 1.0f, 1.0f, color, 0);
}

static void ConDrawInput_TextAndOver(const char *str, const vec_t *color)
{
    ConDrawInput_Text(str, color);
    conDrawInputGlob.x += (float)re.TextWidth(str, 0, cls.consoleFont);
}

static void ConDrawInput_Box(int lines, const vec_t *color)
{
    float x;
    float y;
    float w;
    float h;

    x = conDrawInputGlob.x - 6.0f;
    y = conDrawInputGlob.y - 6.0f;
    w = con.screenMax[0] - con.screenMin[0] - (x - con.screenMin[0]);
    h = (float)lines * conDrawInputGlob.fontHeight + 12.0f;
    ConDraw_Box(x, y, w, h, color);
}

static int ConDrawInput_TextFieldFirstArgChar(void)
{
    int charIndex;

    for (charIndex = 0; isspace(g_consoleField.buffer[charIndex]); ++charIndex)
        ;
    while (!isspace(g_consoleField.buffer[charIndex]))
        ++charIndex;
    while (isspace(g_consoleField.buffer[charIndex]))
        ++charIndex;
    return charIndex;
}

static int ConDrawInput_TextWidth(const char *text)
{
    return re.TextWidth(text, 0, cls.consoleFont);
}

static void ConDrawInput_AutoCompleteArg(const char **stringList, int stringCount)
{
    const char *prefix;
    int prefixLen;
    int matchCount;
    int matchBufferUsed;
    int matchLenMax;
    int matchLen;
    int stringIndex;
    int matchIndex;
    const char *matches[16];
    char matchBuffer[1024];
    float x;
    float y;
    float w;
    float h;

    prefix = Cmd_Argv(1);
    prefixLen = strlen(prefix);
    matchCount = 0;
    matchBufferUsed = 0;
    matchLenMax = 0;
    for (stringIndex = 0; stringIndex < stringCount; ++stringIndex) {
        if (!I_strnicmp(prefix, stringList[stringIndex], prefixLen)) {
            if (matchCount == 16)
                return;
            if (matchBufferUsed + strlen(stringList[stringIndex]) >= 0x400)
                return;
            matches[matchCount] = &matchBuffer[matchBufferUsed];
            Com_StripExtension(stringList[stringIndex], (char *)matches[matchCount]);
            matchBufferUsed += strlen(matches[matchCount]) + 1;
            matchLen = ConDrawInput_TextWidth(matches[matchCount]);
            if (matchLenMax < matchLen)
                matchLenMax = matchLen;
            ++matchCount;
        }
    }
    if (matchCount) {
        qsort(matches, matchCount, sizeof(matches[0]), ConDrawInput_CompareStrings);
        x = (float)re.TextWidth(g_consoleField.buffer, ConDrawInput_TextFieldFirstArgChar(), cls.consoleFont) + conDrawInputGlob.leftX - 6.0f;
        y = con.screenMin[1] + conDrawInputGlob.fontHeight + 6.0f;
        w = (float)matchLenMax + 12.0f;
        h = (float)matchCount * conDrawInputGlob.fontHeight + 12.0f;
        ConDraw_Box(x, y, w, h, con_inputHintBoxColor->current.vector);
        conDrawInputGlob.x = x + 6.0f;
        conDrawInputGlob.y = y + 6.0f;
        conDrawInputGlob.leftX = conDrawInputGlob.x;
        for (matchIndex = 0; matchIndex < matchCount; ++matchIndex) {
            ConDrawInput_Text(matches[matchIndex], con_inputDvarInfoColor);
            conDrawInputGlob.y += conDrawInputGlob.fontHeight;
            conDrawInputGlob.x = conDrawInputGlob.leftX;
        }
    }
}

static void ConDrawInput_CmdMatch(const char *str)
{
    if (I_strnicmp(str, conDrawInputGlob.inputText, conDrawInputGlob.inputTextLen) != 0)
        return;

    ConDrawInput_DrawText(str, 0x7fffffff, con_inputCommandMatchColor);
    ConDrawInput_NextLine();
}

static void ConDrawInput_DetailedDvarMatch(const char *str)
{
    const dvar_t *dvar;
    bool hasLatchedValue;
    int infoLineCount;
    char dvarInfo[1024];

    if (I_strnicmp(str, conDrawInputGlob.inputText, conDrawInputGlob.inputTextLen))
        return;
    if (conDrawInputGlob.hasExactMatch && str[conDrawInputGlob.inputTextLen])
        return;

    dvar = Dvar_FindVar(str);
    hasLatchedValue = Dvar_HasLatchedValue(dvar);
    ConDrawInput_Box(hasLatchedValue ? 3 : 2, con_inputHintBoxColor->current.vector);
    ConDrawInput_DrawText(str, 24, con_inputDvarMatchColor);
    conDrawInputGlob.x += 300.0f;
    ConDrawInput_DrawText(Dvar_DisplayableValue(dvar), 40, con_inputDvarValueColor);
    conDrawInputGlob.y += conDrawInputGlob.fontHeight;
    conDrawInputGlob.x = conDrawInputGlob.leftX;
    if (hasLatchedValue) {
        ConDrawInput_Text("  latched value", con_inputDvarInactiveValueColor);
        conDrawInputGlob.x += 300.0f;
        ConDrawInput_DrawText(Dvar_DisplayableLatchedValue(dvar), 40, con_inputDvarInactiveValueColor);
        conDrawInputGlob.y += conDrawInputGlob.fontHeight;
        conDrawInputGlob.x = conDrawInputGlob.leftX;
    }
    ConDrawInput_Text("  default", con_inputDvarInactiveValueColor);
    conDrawInputGlob.x += 300.0f;
    ConDrawInput_DrawText(Dvar_DisplayableResetValue(dvar), 40, con_inputDvarInactiveValueColor);
    conDrawInputGlob.y += conDrawInputGlob.fontHeight;
    conDrawInputGlob.y += conDrawInputGlob.fontHeight;
    conDrawInputGlob.x = conDrawInputGlob.leftX;
    Dvar_DomainToString_GetLines(dvar->type, dvar->domain, dvarInfo, sizeof(dvarInfo), &infoLineCount);
    ConDrawInput_Box(infoLineCount + 1, con_inputHintBoxColor->current.vector);
    ConDrawInput_Text(dvarInfo, con_inputDvarInfoColor);
    conDrawInputGlob.y += conDrawInputGlob.fontHeight;
    conDrawInputGlob.x = conDrawInputGlob.leftX;
    if (dvar->type == DVAR_TYPE_ENUM && Cmd_Argc() == 2)
        ConDrawInput_AutoCompleteArg(dvar->domain.enumeration.strings, dvar->domain.enumeration.stringCount);
}

static void ConDrawInput_DetailedCmdMatch(const char *str)
{
    int fileCount;
    const char **files;

    if (I_strnicmp(str, conDrawInputGlob.inputText, conDrawInputGlob.inputTextLen))
        return;
    if (conDrawInputGlob.hasExactMatch && str[conDrawInputGlob.inputTextLen])
        return;

    ConDrawInput_Box(1, con_inputHintBoxColor->current.vector);
    ConDrawInput_Text(str, con_inputCommandMatchColor);
    conDrawInputGlob.y += conDrawInputGlob.fontHeight;
    conDrawInputGlob.x = conDrawInputGlob.leftX;
    if (Cmd_Argc() == 2) {
        files = Cmd_GetAutoCompleteFileList(str, &fileCount, 10);
        if (fileCount) {
            ConDrawInput_AutoCompleteArg(files, fileCount);
            FS_FreeFileList((char **)files, 10);
        }
    }
}

static void Con_DrawInput(void)
{
    const char *promptString;
    int matchCount;

    if (!(clients[0].keyCatchers & 1))
        return;
    if (!Sys_IsMainThread())
        return;

    promptString = va("%s: %s> ", "CoD2 MP", COD2_VERSION_SHORT);
    conDrawInputGlob.fontHeight = (float)re.TextHeight(cls.consoleFont);
    conDrawInputGlob.x = con.screenMin[0] + 6.0f;
    conDrawInputGlob.y = con.screenMin[1] + 6.0f;
    conDrawInputGlob.leftX = conDrawInputGlob.x;
    ConDrawInput_Box(1, con_inputBoxColor->current.vector);
    ConDrawInput_TextAndOver(promptString, con_versionColor);
    conDrawInputGlob.leftX = conDrawInputGlob.x;
    g_consoleField.widthInPixels = (int)(con.screenMax[0] - 6.0f - conDrawInputGlob.x);
    Field_Draw(&g_consoleField, (int)conDrawInputGlob.x, (int)conDrawInputGlob.y, 5, 5, 1);
    conDrawInputGlob.y += conDrawInputGlob.fontHeight;
    conDrawInputGlob.x = conDrawInputGlob.leftX;

    conDrawInputGlob.inputText = Con_TokenizeInput();
    conDrawInputGlob.inputTextLen = strlen(conDrawInputGlob.inputText);
    if (!conDrawInputGlob.inputTextLen)
        return;

    conDrawInputGlob.hasExactMatch = 0;
    conDrawInputGlob.matchCount = 0;
    Dvar_ForEach(ConDrawInput_IncrMatchCounter);
    Cmd_ForEach(ConDrawInput_IncrMatchCounter);

    matchCount = conDrawInputGlob.matchCount;
    if (!matchCount)
        return;

    conDrawInputGlob.y += conDrawInputGlob.fontHeight;
    conDrawInputGlob.x = conDrawInputGlob.leftX;
    if (matchCount > 24) {
        promptString = va("%i matches (too many to show)", matchCount);
        ConDrawInput_Box(1, con_inputHintBoxColor->current.vector);
        ConDrawInput_Text(promptString, con_inputDvarMatchColor);
        return;
    }

    if (matchCount == 1 || conDrawInputGlob.hasExactMatch && Con_AnySpaceAfterCommand()) {
        Dvar_ForEach(ConDrawInput_DetailedDvarMatch);
        Cmd_ForEach(ConDrawInput_DetailedCmdMatch);
    } else {
        ConDrawInput_Box(matchCount, con_inputHintBoxColor->current.vector);
        Dvar_ForEach(ConDrawInput_DvarMatch);
        Cmd_ForEach(ConDrawInput_CmdMatch);
    }
}

void Con_DrawConsole(void)
{
    Con_CheckResize();
    if (!((clients)->keyCatchers & 1))
        return;
    if (con.outputVisible)
        Con_DrawOuputWindow();
    Con_DrawInput();
}

void Con_Init(void)
{
    field_t *consoleField;
    field_t *history;
    int i;

    con_restricted = Dvar_RegisterBool("con_restricted", 0, 0x1001);

    consoleField = (&g_consoleField);
    Field_Clear((field_t *)(consoleField));
    consoleField->widthInPixels = g_console_field_width;
    consoleField->charHeight = g_console_char_height;
    consoleField->fixedSize = 1;

    history = (field_t *)((void *)&historyEditLines);
    for (i = 0; i < 32; i++) {
        Field_Clear((field_t *)(&history[i]));
        history[i].widthInPixels = g_console_field_width;
        history[i].charHeight = g_console_char_height;
        history[i].fixedSize = 1;
    }

    Cmd_AddCommand("toggleconsole", Con_ToggleConsole_f);
    Cmd_AddCommand("chatmodepublic", Con_ChatModePublic_f);
    Cmd_AddCommand("chatmodeteam", Con_ChatModeTeam_f);
    Cmd_AddCommand("clear", Con_Clear_f);
    Cmd_AddCommand("condump", Con_Dump_f);

    registeredIconMaterialCount = 0;
    CL_RegisterHudMsgIconMaterial_core("killiconcrush");
    CL_RegisterHudMsgIconMaterial_core("killicondied");
    CL_RegisterHudMsgIconMaterial_core("killiconfalling");
    CL_RegisterHudMsgIconMaterial_core("killiconheadshot");
    CL_RegisterHudMsgIconMaterial_core("killiconmelee");
    CL_RegisterHudMsgIconMaterial_core("killiconsuicide");
}

static void CL_WriteConsoleEncodedByte(int info, int value)
{
    int row;
    int textIndex;

    if (con.linewidth <= 0 || con.totallines <= 0)
        return;

    if (con.lineOffset >= con.linewidth)
        return;

    row = con.currentLine % con.totallines;
    textIndex = row * con.linewidth + con.lineOffset;
    con.textBuffer[textIndex] = (short)((info << 8) | (value & 0xff));
    con.lineOffset++;
}

static unsigned char CL_FindHudMsgIconMaterialIndex(const char *iconShader)
{
    int index;

    for (index = 0; index < registeredIconMaterialCount; index++) {
        if (strcmp(hudMsgIconMaterials[index], iconShader) == 0)
            return index;
    }

    return 0;
}

static const char *CL_SkipConsoleColorCode(const char *text)
{
    unsigned char color;

    if (text[0] != '^')
        return text;

    color = (unsigned char)text[1];
    if (color == '\0' || color == '^' || color <= '/' || color > '9')
        return text;

    return text + 2;
}

static void CL_WriteDeathMessageText(const char *text, int color)
{
    const char *cursor;

    if (!text)
        return;

    for (cursor = text; *cursor && con.lineOffset < con.linewidth;) {
        const char *afterColor = CL_SkipConsoleColorCode(cursor);
        unsigned char ch;

        if (afterColor != cursor) {
            cursor = afterColor;
            continue;
        }

        ch = (unsigned char)*cursor++;
        if (ch == '\n' || ch == '\r')
            continue;

        CL_WriteConsoleEncodedByte(color, ch);
    }
}

static void CL_WriteDeathMessageIcon(const char *iconShader, float iconWidth, float iconHeight, int horzFlipIcon)
{
    int iconMaterialIndex = CL_FindHudMsgIconMaterialIndex(iconShader);
    int widthCode = horzFlipIcon ? 0x13 : 0x10;

    CL_WriteConsoleEncodedByte(widthCode, (int)(iconWidth * 32.0f));
    CL_WriteConsoleEncodedByte(0x11, (int)(iconHeight * 32.0f));
    CL_WriteConsoleEncodedByte(0x12, iconMaterialIndex);
}

void CL_DeathMessagePrint(const char *attackerName, const vec_t *attackerColor, const char *victimName, const vec_t *victimColor, const char *iconShader, float iconWidth, float iconHeight, const vec_t *iconColor, int horzFlipIcon)
{
    const dvar_t *noPrint;
    int duration;
    int defaultColor;

    noPrint = cl_noprint;
    if (noPrint && noPrint->current.enabled)
        return;

    if (!con.initialized)
        Con_OneTimeInit();

    duration = I_fround(con_gamemessagetime->current.value * 1000.0f);
    if (duration < 0)
        duration = 0;

    if (con.lineOffset > 0)
        Con_Linefeed(PMSG_GAME, duration);

    defaultColor = ColorIndex('7');

    if (attackerName && attackerName[0]) {
        CL_AddConsoleInfoColor(0x0a, attackerColor);
        CL_WriteDeathMessageText(attackerName, defaultColor);
        CL_WriteDeathMessageText(" ", defaultColor);
    }

    CL_AddConsoleInfoColor(0x0d, iconColor);
    CL_WriteDeathMessageIcon(iconShader, iconWidth, iconHeight, horzFlipIcon);
    CL_WriteDeathMessageText(" ", defaultColor);

    CL_AddConsoleInfoColor(0x0a, victimColor);
    if (victimName && con.lineOffset + (int)strlen(victimName) + 2 >= con.linewidth) {
        Con_Linefeed(PMSG_GAME, duration);
        CL_AddConsoleInfoColor(0x0a, victimColor);
    }

    CL_WriteDeathMessageText(victimName, defaultColor);
    Con_Linefeed(PMSG_GAME, duration);
    con.prevType = PMSG_GAME;
}
#else
static short Con_FillChar(void)
{
    return (short)((ColorIndex(0x37) << 8) | 0x20);
}

static void Con_ClearMessageWindow(MessageWindow *wnd)
{
    if (!wnd)
        return;
    MsgWnd_Clear(wnd);
}

static void Con_CheckResize(void)
{
    short tbuf[65536];
    int oldLinewidth;
    int oldTotalLines;
    int lineCount;
    int charCount;
    int i;
    int j;
    short fillChar;
    int screenWidth;
    int newLinewidth;

    con.screenMin[0] = 4.0f;
    con.screenMin[1] = 4.0f;
    con.screenMax[0] = -4.0f;
    con.screenMax[1] = -4.0f;

    CalcScreenX(&con.screenMin[0], 1);
    CalcScreenY(&con.screenMin[1], 1);
    CalcScreenX(&con.screenMax[0], 3);
    CalcScreenY(&con.screenMax[1], 3);

    con.screenMin[0] = floorf(con.screenMin[0]);
    con.screenMin[1] = floorf(con.screenMin[1]);
    con.screenMax[0] = floorf(con.screenMax[0]);
    con.screenMax[1] = floorf(con.screenMax[1]);

    screenWidth = (int)(con.screenMax[0] - con.screenMin[0]);
    if (screenWidth > 639)
        screenWidth >>= 3;
    else
        screenWidth = 80;

    newLinewidth = screenWidth - 2;
    oldLinewidth = con.linewidth;
    if (newLinewidth == oldLinewidth)
        return;

    fillChar = Con_FillChar();

    if (newLinewidth <= 0) {
        con.linewidth = 78;
        con.totallines = 0x348;
        for (i = 0; i < 65536; i++)
            con.textBuffer[i] = fillChar;
        con.currentLine = con.totallines - 1;
        con.display = con.currentLine;
        return;
    }

    con.linewidth = newLinewidth;
    oldTotalLines = con.totallines;
    lineCount = 65536 / newLinewidth;
    con.totallines = lineCount;
    if (oldTotalLines < lineCount)
        lineCount = oldTotalLines;

    charCount = oldLinewidth;
    if (charCount > newLinewidth)
        charCount = newLinewidth;

    memcpy(tbuf, con.textBuffer, sizeof(tbuf));

    for (i = 0; i < 65536; i++)
        con.textBuffer[i] = fillChar;

    if (lineCount > 0 && charCount > 0 && oldTotalLines > 0) {
        for (i = 0; i < lineCount; i++) {
            int destBase = (con.totallines - i - 1) * con.linewidth;
            int srcLine = (con.currentLine - i + oldTotalLines) % oldTotalLines;
            int srcBase = srcLine * oldLinewidth;

            for (j = 0; j < charCount; j++)
                con.textBuffer[destBase + j] = tbuf[srcBase + j];
        }
    }

    if (con.messageBuffer) {
        Con_ClearMessageWindow(&con.messageBuffer->gamemsg);
        Con_ClearMessageWindow(&con.messageBuffer->boldgamemsg);
        Con_ClearMessageWindow(&con.messageBuffer->minicon);
        Con_ClearMessageWindow(&con.messageBuffer->subtitle);
    }

    con.currentLine = con.totallines - 1;
    con.display = con.currentLine;
}

static void Con_UpdateMessageWindowLine(MessageWindow *msgwnd, qboolean linefeed, int duration)
{
    Con_UpdateMessageWindowLine_impl(msgwnd, linefeed, duration);
}

static void Con_Linefeed(print_msg_type_t type, int duration)
{
    int cursor;
    short fillChar;

    if (con.currentLine >= 0 && con.messageBuffer) {
        switch (type) {
        case PMSG_CONSOLE:
            Con_UpdateMessageWindowLine(&con.messageBuffer->minicon, 1, duration);
            break;
        case PMSG_GAME:
            Con_UpdateMessageWindowLine(&con.messageBuffer->gamemsg, 1, duration);
            break;
        case PMSG_BOLDGAME:
            Con_UpdateMessageWindowLine(&con.messageBuffer->boldgamemsg, 1, duration);
            break;
        case PMSG_SUBTITLE:
            Con_UpdateMessageWindowLine(&con.messageBuffer->subtitle, 1, duration);
            break;
        default:
            break;
        }
    }

    con.lineOffset = 0;
    if (con.display == con.currentLine)
        con.display++;
    con.currentLine++;

    if (con.linewidth <= 0 || con.totallines <= 0)
        return;

    fillChar = Con_FillChar();
    for (cursor = 0; cursor < con.linewidth; cursor++) {
        int textIndex = (con.currentLine % con.totallines) * con.linewidth + cursor;
        con.textBuffer[textIndex] = fillChar;
    }
}

static void Con_Clear_f(void)
{
    int i;
    short fillChar = Con_FillChar();

    for (i = 0; i < 65536; i++)
        con.textBuffer[i] = fillChar;

    con.display = con.currentLine;
}

static void Con_OneTimeInit(void)
{
    MessageBuffer *messageBuffer;

    con_inputBoxColor = Dvar_RegisterVec4("con_inputBoxColor", 0.25f, 0.25f, 0.2f, 1.0f, 0.0f, 1.0f, 0x1001);
    con_inputHintBoxColor = Dvar_RegisterVec4("con_inputHintBoxColor", 0.4f, 0.4f, 0.35f, 1.0f, 0.0f, 1.0f, 0x1001);
    con_outputBarColor = Dvar_RegisterVec4("con_outputBarColor", 1.0f, 1.0f, 0.95f, 0.6f, 0.0f, 1.0f, 0x1001);
    con_outputSliderColor = Dvar_RegisterVec4("con_outputSliderColor", 0.15f, 0.15f, 0.1f, 0.6f, 0.0f, 1.0f, 0x1001);
    con_outputWindowColor = Dvar_RegisterVec4("con_outputWindowColor", 0.35f, 0.35f, 0.3f, 0.75f, 0.0f, 1.0f, 0x1001);

    con_gamemessagetime = Dvar_RegisterFloat("con_gamemessagetime", 5.0f, 0.0f, 3.402823466e38f, 0x1000);
    con_boldgamemessagetime = Dvar_RegisterFloat("con_boldgamemessagetime", 8.0f, 0.0f, 3.402823466e38f, 0x1000);
    con_minicontime = Dvar_RegisterFloat("con_minicontime", 4.0f, 0.0f, 3.402823466e38f, 0x1001);
    con_miniconlines = Dvar_RegisterInt("con_miniconlines", 5, 0, 100, 0x1001);

    messageBuffer = con.messageBufferArray;
    con.messageBuffer = messageBuffer;

    messageBuffer->gamemsg.lines = messageBuffer->gamemsg_lines;
    messageBuffer->gamemsg.current_line = 0;
    messageBuffer->gamemsg.count = 8;
    messageBuffer->gamemsg.padding = 3;
    messageBuffer->gamemsg.scrolltime = 250;
    messageBuffer->gamemsg.fadein = 250;
    messageBuffer->gamemsg.fadeout = 500;

    messageBuffer->boldgamemsg.lines = messageBuffer->boldgamemsg_lines;
    messageBuffer->boldgamemsg.current_line = 0;
    messageBuffer->boldgamemsg.count = 8;
    messageBuffer->boldgamemsg.padding = 3;
    messageBuffer->boldgamemsg.scrolltime = 250;
    messageBuffer->boldgamemsg.fadein = 250;
    messageBuffer->boldgamemsg.fadeout = 500;

    messageBuffer->subtitle.lines = messageBuffer->subtitle_lines;
    messageBuffer->subtitle.current_line = 0;
    messageBuffer->subtitle.count = 8;
    messageBuffer->subtitle.padding = 3;
    messageBuffer->subtitle.scrolltime = 250;
    messageBuffer->subtitle.fadein = 250;
    messageBuffer->subtitle.fadeout = 500;

    messageBuffer->minicon.lines = messageBuffer->minicon_lines;
    messageBuffer->minicon.current_line = 0;
    messageBuffer->minicon.count = con_miniconlines->current.integer;
    messageBuffer->minicon.padding = 0;
    messageBuffer->minicon.scrolltime = 0;
    messageBuffer->minicon.fadein = 0;
    messageBuffer->minicon.fadeout = 0;

    memcpy(con.color, ((void *)&colorWhite), sizeof(con.color));
    con.linewidth = -1;
    Con_CheckResize();
    con.initialized = 1;
}

static int CL_ConsolePrint_AddLine(print_msg_type_t type, const char *txt, int duration, int linewidth, int color)
{
    const char *buf;
    int targetLineWidth;
    int lineBroken;

    {
        int clamp;

        if (type == PMSG_CONSOLE || type == PMSG_LOGFILE)
            clamp = con.linewidth;
        else
            clamp = (con.linewidth >= 78) ? 78 : con.linewidth;

        if (linewidth <= 0 || clamp < linewidth)
            linewidth = clamp;
    }

    if (type == PMSG_GAME || type == PMSG_BOLDGAME) {
        int printLen = SEH_PrintStrlen(txt);
        if (printLen > linewidth) {
            float lines = ceilf((float)printLen / (float)linewidth);
            targetLineWidth = (int)((float)printLen / lines);
        } else {
            targetLineWidth = linewidth;
        }
    } else {
        targetLineWidth = linewidth;
    }

    if (con.prevType != type && con.lineOffset > 0)
        Con_Linefeed(type, duration);

    buf = txt;
    lineBroken = 0;

    while (*buf) {
        unsigned char c = (unsigned char)*buf;

        if (c == '^' && buf[1] != '\0' && buf[1] != '^' && (unsigned char)buf[1] > '/' && (unsigned char)buf[1] <= '9') {
            color = ColorIndex((unsigned char)buf[1]);
            buf += 2;
            continue;
        }

        if (linewidth > 0) {
            if (c > ' ') {
                int wordLen = 0;
                do {
                    wordLen++;
                    if (wordLen == linewidth)
                        break;
                } while ((unsigned char)buf[wordLen] > ' ');

                if (wordLen != linewidth && con.lineOffset + wordLen > linewidth) {
                    Con_Linefeed(type, duration);
                    lineBroken = 1;
                }
            } else if (con.lineOffset > linewidth) {
                Con_Linefeed(type, duration);
                lineBroken = 1;
            }
        }

        buf++;

        if (c == '\n') {
            Con_Linefeed(type, duration);
            continue;
        }

        if (c == '\r') {
            con.lineOffset = 0;
            continue;
        }

        if (con.lineOffset == 0 && c == ' ' && lineBroken)
            continue;

        {
            int textIndex = (con.currentLine % con.totallines) * con.linewidth + con.lineOffset;
            con.textBuffer[textIndex] = (short)((color << 8) | c);
            con.lineOffset++;
        }

        if (con.lineOffset >= linewidth || (con.lineOffset >= targetLineWidth && c == ' ')) {
            Con_Linefeed(type, duration);
            lineBroken = 1;
        }
    }

    if (con.lineOffset > 0) {
        if (type != PMSG_CONSOLE) {
            Con_Linefeed(type, duration);
        } else if (con.currentLine >= 0) {
            Con_UpdateMessageWindowLine(&con.messageBuffer->minicon, 0, duration);
        }
    }

    con.prevType = type;
    return color;
}

void CL_ConsolePrint(print_msg_type_t type, const char *txt, int duration, int linewidth)
{
    const dvar_t *noPrint;
    int color;

    noPrint = cl_noprint;
    if (!noPrint)
        return;
    if (noPrint->current.enabled)
        return;
    if (type == PMSG_LOGFILE)
        return;

    if (!con.initialized)
        Con_OneTimeInit();

    if (!duration) {
        switch (type) {
        case PMSG_CONSOLE:
            duration = I_fround(con_minicontime->current.value * 1000.0f);
            break;
        case PMSG_GAME:
            duration = I_fround(con_gamemessagetime->current.value * 1000.0f);
            break;
        case PMSG_BOLDGAME:
            duration = I_fround(con_boldgamemessagetime->current.value * 1000.0f);
            break;
        case PMSG_SUBTITLE:
            duration = 5000;
            break;
        default:
            duration = 0;
            break;
        }
    }

    if (duration < 0)
        duration = 0;

    color = ColorIndex(0x37);

    if (type == PMSG_GAME || type == PMSG_BOLDGAME) {
        const char *lineStart = txt;
        const char *newline;
        char lineText[0x1000];

        while ((newline = strchr(lineStart, '\n')) != NULL) {
            int charCount = (int)(newline - lineStart) + 1;
            int copyCount = charCount;

            if (copyCount > 0xfff)
                copyCount = 0xfff;

            memcpy(lineText, lineStart, copyCount);
            lineText[copyCount] = '\0';
            color = CL_ConsolePrint_AddLine(type, lineText, duration, linewidth, color);
            lineStart = newline + 1;
        }

        CL_ConsolePrint_AddLine(type, lineStart, duration, linewidth, color);
        return;
    }

    CL_ConsolePrint_AddLine(type, txt, duration, linewidth, color);
}

void CL_ConsoleFixPosition(void)
{
    CL_ConsolePrint(PMSG_CONSOLE, "\n", 0, 0);
    con.display = con.currentLine - 1;
}

void Con_Init(void)
{
    field_t *consoleField;
    field_t *history;
    int i;

    con_restricted = Dvar_RegisterBool("con_restricted", 0, 0x1001);

    consoleField = (&g_consoleField);
    Field_Clear((field_t *)(consoleField));
    consoleField->widthInPixels = g_console_field_width;
    consoleField->charHeight = g_console_char_height;
    consoleField->fixedSize = 1;

    history = (field_t *)((void *)&historyEditLines);
    for (i = 0; i < 32; i++) {
        Field_Clear((field_t *)(&history[i]));
        history[i].widthInPixels = g_console_field_width;
        history[i].charHeight = g_console_char_height;
        history[i].fixedSize = 1;
    }

    Cmd_AddCommand("toggleconsole", Con_ToggleConsole_f);
    Cmd_AddCommand("chatmodepublic", Con_ChatModePublic_f);
    Cmd_AddCommand("chatmodeteam", Con_ChatModeTeam_f);
    Cmd_AddCommand("clear", Con_Clear_f);
    Cmd_AddCommand("condump", Con_Dump_f);

    registeredIconMaterialCount = 0;
    CL_RegisterHudMsgIconMaterial_core("killiconcrush");
    CL_RegisterHudMsgIconMaterial_core("killicondied");
    CL_RegisterHudMsgIconMaterial_core("killiconfalling");
    CL_RegisterHudMsgIconMaterial_core("killiconheadshot");
    CL_RegisterHudMsgIconMaterial_core("killiconmelee");
    CL_RegisterHudMsgIconMaterial_core("killiconsuicide");
}

void Con_Close(void)
{
    int *legacyHacks = (int *)::legacyHacks;
    if (!legacyHacks[1])
        return;

    Field_Clear((&g_consoleField));
    Con_ClearAllMessageWindows();
    Con_GetClientActive()->keyCatchers &= ~1;
}
#endif

const vec4_t con_versionColor = { 1.0f, 1.0f, 0.0f, 1.0f };

const vec4_t con_inputDvarMatchColor = { 1.0f, 1.0f, 0.8f, 1.0f };

const vec4_t con_inputCommandMatchColor = { 0.8f, 0.8f, 1.0f, 1.0f };

const vec4_t con_inputDvarValueColor = { 1.0f, 1.0f, 1.0f, 1.0f };

const vec4_t con_inputDvarInfoColor = { 0.8f, 0.8f, 1.0f, 1.0f };

const vec4_t con_inputDvarInactiveValueColor = { 0.8f, 0.8f, 0.8f, 1.0f };
