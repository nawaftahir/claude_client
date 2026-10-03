#include "common_types.h"
#include "imports.h"
#include <ctype.h>
#include <string.h>
extern const dvar_t *cl_bypassMouseInput;
extern dvar_t *com_sv_running;
extern dvar_t *sv_disableClientConsole;
extern clientActive_t clients[1];
extern clientConnection_t clc;
extern int dvar_modifiedFlags;
extern int g_console_field_width;
extern float g_console_char_height;

extern keyname_t keynames[99];
extern keyname_t keynames_localized[100];

extern clientStatic_t cls;
extern refexport_t re;

extern void ReplaceStringInternal(const char **str, const char *in);
extern void Cmd_AddCommand(const char *name, void (*func)(void));
extern int Cmd_Argc(void);
extern const char *Cmd_Argv( int arg );
extern void I_strncat(char *dest, int maxlen, const char *src);
extern int I_stricmp(const char *s0, const char *s1);
extern bool I_isdigit( int c );
extern int I_strnicmp(const char *s0, const char *s1, int n);
extern void I_strncpyz(char *dest, const char *src, int destsize);
extern void Com_Printf(const char *fmt, ...);
extern void FS_Printf(fileHandle_t f, const char *fmt, ...);
extern int SEH_GetCurrentLanguage(void);
extern void CL_SwitchToLocalClient(int localClientNum);
extern "C" void Z_FreeInternal( void *ptr );
extern void UI_KeyEvent(int key, int down);
extern void Field_Clear(field_t *edit);
extern char *Sys_GetClipboardData(void);
extern void Cbuf_AddText(const char *text);
extern int Com_sprintf(char *dest, size_t size, const char *format, ...);
extern void CL_AddReliableCommand(const char *cmd);
extern void Con_ToggleConsole_f(void);
extern void Con_ToggleConsoleOutput(void);
extern Bool Con_IsActive(void);
extern void Con_PageUp(void);
extern void Con_PageDown(void);
extern void Con_Top(void);
extern void Con_Bottom(void);
extern cmd_function_t *Cmd_FindCommand(const char *cmdName);
extern char **FS_ListFiles( const char *path, const char *extension, FsListBehavior behavior, int *numfiles, int allocType );
extern void FS_FreeFileList(char **list, int allocType);
extern void Com_StripExtension(const char *in, char *out);
extern dvar_s *Dvar_FindVar(const char *dvarName);
extern void Con_AutoCompleteFromList(const char **strings, int stringCount, const char *prefix, char *completed, int sizeofCompleted);
extern const char *Con_TokenizeInput(void);
extern Bool Con_AnySpaceAfterCommand(void);
extern qboolean UI_SetActiveMenu(int menu);
extern qboolean UI_checkKeyExec(int key);
extern int CG_KeyInterceptEvent(int key, qboolean down);
extern void CL_StopLogoOrCinematic(void);
extern void CL_Disconnect(void);
extern const dvar_t *nextdemo;
extern LegacyHacks *legacyHacks;
int stricmp(const char *str1, const char *str2);
extern void Dvar_SetString(dvar_t *dvar, const char *value);
extern int I_strncmp(const char *s0, const char *s1, int n);
extern void Cmd_ForEach(void (*callback)(const char *));
extern void Dvar_ForEach(void (*callback)(const char *));
extern void SCR_UpdateScreen(void);
extern Bool CL_AllLocalClientsDisconnected(void);

extern FontHandle UI_GetFontHandle(int fontEnum, float scale);
extern void CL_DrawTextWithCursor(const char *text, int maxChars, FontHandle font, float x, float y, int horzAlign, int vertAlign, float xScale, float yScale, const vec_t *color, int style, int cursorPos, char cursor);
extern float GetRealWidthFromVirtualWidth(float w);
extern float GetRealHeightFromVirtualHeight(float h);
extern float GetVirtualWidthFromRealWidth(float w);
extern float GetVirtualHeightFromRealHeight(float h);
extern int SEH_PrintStrlen(const char *str);

extern PlayerKeyState playerKeys[1];
extern field_t *chatField;
extern qboolean *chat_team;
extern qboolean *key_overstrikeMode;
extern qboolean *anykeydown;
extern qkey_t *keys;
extern field_t historyEditLines[32];
extern field_t g_consoleField;
extern int nextHistoryLine;
extern int historyLine;
/* migrated from the ILP32 data blob to typed C (x64 port Stage 2). */
static char *frenchNumberKeysMap[10] = {
    "\xff" "fd",
    "&",
    "\xff" "fd",
    "\"",
    "'",
    "(",
    "-",
    "\xff" "fd",
    "_",
    "\xff" "fd",
};

#define COMMAND_HISTORY 32

static char tinystr[5];
static const char *completionString;
static Bool hasExactMatch;
static char shortestMatch[1024];
static int matchCount;

enum {
    K_ENTER = 0x0d,
    K_TAB = 0x09,
    K_ESCAPE = 0x1b,
    K_UPARROW = 0x9a,
    K_DOWNARROW = 0x9b,
    K_RIGHTARROW = 0x9d,
    K_LEFTARROW = 0x9c,
    K_CTRL = 0x9f,
    K_SHIFT = 0xa0,
    K_INS = 0xa1,
    K_DEL = 0xa2,
    K_PGDN = 0xa3,
    K_PGUP = 0xa4,
    K_HOME = 0xa5,
    K_END = 0xa6,
    K_KP_UPARROW = 0xb7,
    K_KP_DOWNARROW = 0xbd,
    K_KP_ENTER = 0xbf,
    K_KP_INS = 0xc0,
    K_MWHEELDOWN = 0xcd,
    K_MWHEELUP = 0xce
};

qboolean Key_GetOverstrikeMode(void);
void Field_AdjustScroll(field_t *edit);
void Key_SetOverstrikeMode(qboolean state);
static void FindMatches(const char *s);
static void PrintMatches(const char *s);
static void keyConcatArgs(void);
int Key_StringToKeynum(const char *str);
char *Key_KeynumToString(int keynum, qboolean translate);
void Key_SetBinding(int keynum, const char *binding);
char *Key_GetBinding(int keynum);
void CL_InitKeyCommands(void);
void Key_Shutdown(void);
void Field_Draw(field_t *edit, int x, int y, int horzAlign, int vertAlign);
void Key_Unbind_f(void);
void Key_Unbindall_f(void);
void Key_Bind_f(void);
void Field_CharEvent(field_t *edit, int ch);
void CL_CharEvent(int key);
void Field_KeyDownEvent(field_t *edit, int key);
void Message_Key(int key);
void Console_Key(int key);
void Key_WriteBindings(fileHandle_t f);
void Key_Bindlist_f(void);
void CL_KeyEvent(int key, const qboolean down, const unsigned int time);
void Key_ClearStates(void);

#if COD2_FEATURE_GAMEPAD
extern void CL_Gamepad_NoteKbmActive(void);
#endif

qboolean Key_GetOverstrikeMode(void)
{
    return *key_overstrikeMode;
}

void Field_AdjustScroll(field_t *edit)
{
	float fontScale;
	float lineWidth;
	FontHandle font;
	float actualScale;
	float len;
	int strLen;

	fontScale = edit->charHeight * 0.020833334f;
	lineWidth = (float)edit->widthInPixels;
	if ( edit->fixedSize )
	{
		fontScale = GetVirtualWidthFromRealWidth(fontScale);
		lineWidth = GetVirtualHeightFromRealHeight(lineWidth);
		font = cls.consoleFont;
	}
	else
	{
		font = UI_GetFontHandle(0, fontScale);
	}
	actualScale = re.NormalizedTextScale(font, fontScale);
	if ( (float)re.TextWidth(edit->buffer, 0, font) * actualScale < lineWidth )
	{
		edit->scroll = 0;
		edit->drawWidth = SEH_PrintStrlen(edit->buffer);
		return;
	}
	len = 0.0f;
	while ( len < lineWidth && edit->scroll > 0 )
	{
		len = (float)re.TextWidth(&edit->buffer[edit->scroll - 1], 0, font) * actualScale;
		if ( len >= lineWidth )
			break;
		edit->scroll--;
	}
	do
	{
		len = (float)re.TextWidth(&edit->buffer[edit->scroll], 0, font) * actualScale
			- (float)re.TextWidth(&edit->buffer[edit->cursor], 0, font) * actualScale;
		if ( len < 0.0f || len >= lineWidth )
		{
			if ( len < 0.0f )
			{
				if ( !edit->scroll )
					len = 0.0f;
				else
					edit->scroll--;
			}
			else
			{
				edit->scroll++;
			}
		}
	}
	while ( len < 0.0f || len >= lineWidth );
	strLen = strlen(&edit->buffer[edit->scroll]);
	edit->drawWidth = edit->cursor - edit->scroll;
	len = 0.0f;
	while ( len < lineWidth && edit->drawWidth < strLen )
	{
		len = (float)re.TextWidth(&edit->buffer[edit->scroll], edit->drawWidth + 1, font) * actualScale;
		if ( len < lineWidth )
			edit->drawWidth++;
	}
}

void Key_SetOverstrikeMode(qboolean state)
{
    *key_overstrikeMode = state;
}

void FindMatches(const char *s)
{
	int i;

	if ( I_strnicmp(s, completionString, strlen(completionString)) )
		return;

	matchCount++;
	if ( matchCount == 1 )
	{
		I_strncpyz(shortestMatch, s, sizeof(shortestMatch));
		hasExactMatch = 1;
		return;
	}

	for ( i = 0; s[i]; i++ )
	{
		if ( tolower(shortestMatch[i]) != tolower(s[i]) )
			break;
	}

	if ( s[i] && (!hasExactMatch || shortestMatch[i]) )
		hasExactMatch = 0;
	else
		hasExactMatch = 1;

	shortestMatch[i] = 0;
}

static void PrintMatches(const char *s)
{
    if (I_strnicmp(s, shortestMatch, (int)strlen(shortestMatch)) == 0) {
        Com_Printf("    %s\n", s);
    }
}

static void keyConcatArgs(void)
{
    int i;
    const char *arg;

    for (i = 1; i < Cmd_Argc(); i++) {
        I_strncat(g_consoleField.buffer, sizeof(g_consoleField.buffer), " ");
        arg = Cmd_Argv(i);
        while (*arg) {
            if (*arg == ' ') {
                I_strncat(g_consoleField.buffer, sizeof(g_consoleField.buffer), "\"");
                break;
            }
            arg++;
        }
        I_strncat(g_consoleField.buffer, sizeof(g_consoleField.buffer), Cmd_Argv(i));
        if (*arg == ' ') {
            I_strncat(g_consoleField.buffer, sizeof(g_consoleField.buffer), "\"");
        }
    }
}

int Key_StringToKeynum(const char *str)
{
    keyname_t *name;

    if (str == NULL || *str == '\0') {
        return -1;
    }

    if (str[1] == '\0') {
        return (signed char)str[0];
    }

    if (str[0] == '0' && str[1] == 'x' && strlen(str) == 4) {
        int n1, n2;

        n1 = str[2];
        if (I_isdigit(n1)) {
            n1 -= '0';
        } else if (n1 >= 'a' && n1 <= 'f') {
            n1 = n1 - 'a' + 10;
        } else {
            n1 = 0;
        }

        n2 = str[3];
        if (I_isdigit(n2)) {
            n2 -= '0';
        } else if (n2 >= 'a' && n2 <= 'f') {
            n2 = n2 - 'a' + 10;
        } else {
            n2 = 0;
        }

        return n1 * 16 + n2;
    }

    for (name = keynames; name->name; ++name) {
        if (I_stricmp(str, name->name) == 0) {
            return name->keynum;
        }
    }

    return -1;
}

char *Key_KeynumToString(int keynum, qboolean translate)
{
    keyname_t *name;
    int upperNibble;
    int lowerNibble;

    if (keynum == -1) {
        return "<KEY NOT FOUND>";
    }

    if (keynum < 0 || keynum > 0xff) {
        return "<OUT OF RANGE>";
    }

    if (translate && SEH_GetCurrentLanguage() == 1 && keynum >= '0' && keynum <= '9') {
        return frenchNumberKeysMap[keynum - '0'];
    }

    if (keynum > 0x20 && keynum < 0x7f && keynum != '"') {
        tinystr[0] = toupper(keynum);
        tinystr[1] = '\0';
        if (keynum != ';' || translate) {
            return tinystr;
        }
    }

    name = translate ? keynames_localized : keynames;
    while (name->name) {
        if (keynum == name->keynum) {
            return name->name;
        }
        ++name;
    }

    upperNibble = keynum >> 4;
    lowerNibble = keynum & 0xf;
    tinystr[0] = '0';
    tinystr[1] = 'x';
    tinystr[2] = upperNibble > 9 ? (char)(upperNibble + 'a' - 10) : (char)(upperNibble + '0');
    tinystr[3] = lowerNibble > 9 ? (char)(lowerNibble + 'a' - 10) : (char)(lowerNibble + '0');
    tinystr[4] = '\0';
    return tinystr;
}

void Key_SetBinding(int keynum, const char *binding)
{
    if (keynum == -1)
        return;
    ReplaceStringInternal((const char **)&keys[keynum].binding, binding);
    dvar_modifiedFlags |= 1;
}

char *Key_GetBinding(int keynum)
{
    if (keynum == -1)
        return (char *)"";
    return keys[keynum].binding;
}

void CL_InitKeyCommands(void)
{
    Cmd_AddCommand("bind", Key_Bind_f);
    Cmd_AddCommand("unbind", Key_Unbind_f);
    Cmd_AddCommand("unbindall", Key_Unbindall_f);
    Cmd_AddCommand("bindlist", Key_Bindlist_f);
}

void Key_Shutdown(void)
{
    int keyIndex;

    CL_SwitchToLocalClient(0);

    for (keyIndex = 0; keyIndex < 256; ++keyIndex) {
        char *binding = keys[keyIndex].binding;
        if (binding != NULL) {
            Z_FreeInternal(binding);
            keys[keyIndex].binding = NULL;
        }
    }
}

void Field_Draw(field_t *edit, int x, int y, int horzAlign, int vertAlign)
{
    vec4_t vColor;
    char str[0x400];
    int cursorPos;
    FontHandle font;
    int fontStyle;
    float xScale;
    float yScale;
    float xAdj;
    float yAdj;
    char cursorChar;
    float scale;

    vColor[0] = 1.0f;
    vColor[1] = 1.0f;
    vColor[2] = 1.0f;
    vColor[3] = 1.0f;

    I_strncpyz(str, edit->buffer + edit->scroll, 0x100 - edit->scroll);

    cursorPos = edit->cursor - edit->scroll;

    if (edit->fixedSize) {
        font = cls.consoleFont;
        fontStyle = 0;
        xScale = 1.0f;
        yScale = 1.0f;
        cursorChar = *key_overstrikeMode ? '_' : '|';
    } else {
        scale = edit->charHeight / 48.0f;
        font = UI_GetFontHandle(0, scale);
        scale = re.NormalizedTextScale(font, scale);
        if (vertAlign == 5) {
            xScale = GetRealWidthFromVirtualWidth(scale);
            yScale = GetRealHeightFromVirtualHeight(scale);
        } else {
            xScale = scale;
            yScale = scale;
        }
        fontStyle = 3;
        cursorChar = *key_overstrikeMode ? '_' : '|';
    }

    xAdj = (float)x;
    yAdj = (float)y + (float)re.TextHeight(font) * yScale;

    if (!edit->drawWidth)
        edit->drawWidth = 0x100;

    CL_DrawTextWithCursor(str, edit->drawWidth, font,
                          xAdj, yAdj, horzAlign, vertAlign,
                          xScale, yScale,
                          vColor, fontStyle, cursorPos, cursorChar);
}

void Key_Unbind_f(void)
{
    int keynum;

    if (Cmd_Argc() != 2) {
        Com_Printf("unbind <key> : remove commands from a key\n");
        return;
    }

    keynum = Key_StringToKeynum(Cmd_Argv(1));
    if (keynum == -1) {
        Com_Printf("\"%s\" isn't a valid key\n", Cmd_Argv(1));
        return;
    }

    Key_SetBinding(keynum, "");
}

void Key_Unbindall_f(void)
{
    int keynum;

    for (keynum = 0; keynum < 256; ++keynum) {
        if (keys[keynum].binding != NULL && keynum != -1) {
            ReplaceStringInternal((const char **)&keys[keynum].binding, "");
            dvar_modifiedFlags |= 1;
        }
    }
}

void Key_Bind_f(void)
{
    bool hasSpace;
    int keynum;
    int argIndex;
    const char *binding;
    int argc;
    char cmd[1024];

    argc = Cmd_Argc();
    if (Cmd_Argc() < 2) {
        Com_Printf("bind <key> [command] : attach a command to a key\n");
        return;
    }

    keynum = Key_StringToKeynum(Cmd_Argv(1));
    if (keynum == -1) {
        Com_Printf("\"%s\" isn't a valid key\n", Cmd_Argv(1));
        return;
    }

    keynum = tolower(keynum);
    if (argc == 2) {
        binding = keys[keynum].binding;
        if (binding) {
            Com_Printf("\"%s\" = \"%s\"\n", Cmd_Argv(1), binding);
        } else {
            Com_Printf("\"%s\" is not bound\n", Cmd_Argv(1));
        }
        return;
    }

    cmd[0] = '\0';
    for (argIndex = 2; argIndex < argc; ++argIndex) {
        hasSpace = argc > 3 && strchr(Cmd_Argv(argIndex), ' ');
        if (hasSpace) {
            I_strncat(cmd, sizeof(cmd), "\"");
        }

        I_strncat(cmd, sizeof(cmd), Cmd_Argv(argIndex));

        if (hasSpace) {
            I_strncat(cmd, sizeof(cmd), "\"");
        }

        if (argIndex != argc - 1) {
            I_strncat(cmd, sizeof(cmd), " ");
        }
    }

    Key_SetBinding(keynum, cmd);
}

void Field_PasteClipboard(field_t *edit)
{
    char *cbd;
    int len;
    int i;

    cbd = Sys_GetClipboardData();
    if (!cbd)
        return;

    len = (int)strlen(cbd);
    for (i = 0; i < len; ++i) {
        Field_CharEvent(edit, cbd[i]);
    }

    Z_FreeInternal(cbd);
}

void Field_CharEvent(field_t *edit, int ch)
{
    int len = (int)strlen(edit->buffer);

    if (ch == 0x16) {
        Field_PasteClipboard(edit);
        Field_AdjustScroll(edit);
        return;
    }

    if (ch == 3) {
        Field_Clear(edit);
        Field_AdjustScroll(edit);
        return;
    }

    if (ch == 8) {
        if (edit->cursor > 0) {
            memmove(edit->buffer + edit->cursor - 1,
                    edit->buffer + edit->cursor,
                    (size_t)(len - edit->cursor + 1));
            edit->cursor--;
        }
        Field_AdjustScroll(edit);
        return;
    }

    if (ch == 1) {
        edit->cursor = 0;
        edit->scroll = 0;
        return;
    }

    if (ch == 5) {
        edit->cursor = len;
        Field_AdjustScroll(edit);
        return;
    }

    if (ch < 32)
        return;

    if (*key_overstrikeMode) {
        if (edit->cursor == 255)
            return;

        edit->buffer[edit->cursor] = (char)ch;
        edit->cursor++;
    } else {
        if (len == 255)
            return;

        memmove(edit->buffer + edit->cursor + 1,
                edit->buffer + edit->cursor,
                (size_t)(len - edit->cursor + 1));

        edit->buffer[edit->cursor] = (char)ch;
        edit->cursor++;
    }

    if (edit->cursor == len + 1)
        edit->buffer[edit->cursor] = 0;

    Field_AdjustScroll(edit);
}

void CL_CharEvent(int key)
{
    int keyCatchers;

    if (key == '`' || key == '~')
        return;

    keyCatchers = (clients)->keyCatchers;

    if (keyCatchers & 1) {
        Field_CharEvent(&g_consoleField, key);
        return;
    }
    if (keyCatchers & 8) {
        UI_KeyEvent(key | 0x400, 1);
        return;
    }
    if (keyCatchers & 0x10) {
        Field_CharEvent((field_t *)chatField, key);
        return;
    }

    if (!*((int *)&::clc)) {
        Field_CharEvent(&g_consoleField, key);
        return;
    }
}

void Field_KeyDownEvent(field_t *edit, int key)
{
	int len;

	len = strlen(edit->buffer);
	if ( (key == K_INS || key == K_KP_INS) && keys[K_SHIFT].down )
	{
		Field_PasteClipboard(edit);
	}
	else if ( key == K_DEL )
	{
		if ( edit->cursor < len )
			memmove(&edit->buffer[edit->cursor], &edit->buffer[edit->cursor + 1], len - edit->cursor);
	}
	else if ( key == K_RIGHTARROW )
	{
		if ( edit->cursor < len )
			edit->cursor++;
		if ( keys[K_CTRL].down )
		{
			while ( edit->cursor < len && isalnum(edit->buffer[edit->cursor]) )
				edit->cursor++;
			while ( edit->cursor < len && !isalnum(edit->buffer[edit->cursor]) )
				edit->cursor++;
		}
	}
	else if ( key == K_LEFTARROW )
	{
		if ( edit->cursor > 0 )
			edit->cursor--;
		if ( keys[K_CTRL].down )
		{
			while ( edit->cursor > 0 && isalnum(edit->buffer[edit->cursor - 1]) )
				edit->cursor--;
		}
		if ( edit->cursor < edit->scroll )
			edit->scroll = edit->cursor;
	}
	else if ( key == K_HOME || tolower(key) == 'a' && keys[K_CTRL].down )
	{
		edit->cursor = 0;
	}
	else if ( key == K_END || tolower(key) == 'e' && keys[K_CTRL].down )
	{
		edit->cursor = len;
	}
	else if ( key == K_INS )
	{
		Key_SetOverstrikeMode(!Key_GetOverstrikeMode());
	}
	if ( cls.uiStarted )
		Field_AdjustScroll(edit);
}

void Message_Key(int key)
{
    if (key == K_ESCAPE) {
        (clients)->keyCatchers &= ~0x10;
        Field_Clear(chatField);
        return;
    }

    if (key == K_ENTER || key == K_KP_ENTER) {
        if (chatField->buffer[0]) {
            if ((&::clc)->state == 8) {
                char buffer[1024];

                if (*chat_team) {
                    Com_sprintf(buffer, sizeof(buffer),
                                "say_team \"\x15%s\"\n", chatField->buffer);
                } else {
                    Com_sprintf(buffer, sizeof(buffer),
                                "say \"\x15%s\"\n", chatField->buffer);
                }
                CL_AddReliableCommand(buffer);
            }
        }
        (clients)->keyCatchers &= ~0x10;
        Field_Clear(chatField);
        return;
    }

    Field_KeyDownEvent(chatField, key);
}

static void ConcatRemaining(const char *src, const char *start)
{
	const char *str;

	str = strstr(src, start);
	if ( !str )
	{
		keyConcatArgs();
		return;
	}

	str += strlen(start);
	I_strncat(g_consoleField.buffer, sizeof(g_consoleField.buffer), str);
}

static void ReplaceConsoleInputArgument(int replaceCount, const char *replacement)
{
	int cmdLineLen;
	int replaceStart;

	if ( !*replacement )
		return;

	cmdLineLen = strlen(g_consoleField.buffer);
	while ( cmdLineLen && isspace(g_consoleField.buffer[cmdLineLen - 1]) )
		cmdLineLen--;

	replaceStart = cmdLineLen - replaceCount;
	I_strncpyz(&g_consoleField.buffer[replaceStart], replacement, sizeof(g_consoleField.buffer) - replaceStart);
}

static void CompleteCmdArgument(void)
{
	const char *cmdName;
	const char *prefix;
	cmd_function_t *cmd;
	char **files;
	int fileCount;
	char arg[256];

	cmdName = Con_TokenizeInput();
	prefix = Cmd_Argv(1);
	if ( !*prefix )
		return;

	fileCount = 0;
	cmd = Cmd_FindCommand(cmdName);
	if ( !cmd->autoCompleteDir || !cmd->autoCompleteExt )
		return;

	files = FS_ListFiles(cmd->autoCompleteDir, cmd->autoCompleteExt, FS_LIST_PURE_ONLY, &fileCount, 10);
	if ( !fileCount )
		return;

	Con_AutoCompleteFromList((const char **)files, fileCount, prefix, arg, sizeof(arg));
	FS_FreeFileList(files, 10);
	Com_StripExtension(arg, arg);
	ReplaceConsoleInputArgument(strlen(prefix), arg);
}

static void CompleteDvarArgument(void)
{
	const dvar_s *dvar;
	const char *prefix;
	char value[256];

	dvar = Dvar_FindVar(Con_TokenizeInput());
	if ( dvar->type != DVAR_TYPE_ENUM )
		return;

	prefix = Cmd_Argv(1);
	if ( !*prefix )
		return;

	Con_AutoCompleteFromList(dvar->domain.enumeration.strings, dvar->domain.enumeration.stringCount, prefix, value, sizeof(value));
	ReplaceConsoleInputArgument(strlen(prefix), value);
}

static void CompleteCommand(void)
{
	field_t temp;
	int matchLenAfterCmds;
	int matchLenAfterDvars;

	completionString = Con_TokenizeInput();
	shortestMatch[0] = 0;
	matchCount = 0;
	if ( !*completionString )
		return;

	Cmd_ForEach(FindMatches);
	matchLenAfterCmds = strlen(shortestMatch);
	Dvar_ForEach(FindMatches);
	matchLenAfterDvars = strlen(shortestMatch);
	if ( !matchCount )
		return;

	temp = g_consoleField;

	if ( matchCount == 1 || hasExactMatch && Con_AnySpaceAfterCommand() )
	{
		Com_sprintf(g_consoleField.buffer, sizeof(g_consoleField.buffer), "\\%s", shortestMatch);
		if ( Cmd_Argc() == 1 )
		{
			I_strncat(g_consoleField.buffer, sizeof(g_consoleField.buffer), " ");
		}
		else
		{
			ConcatRemaining(temp.buffer, completionString);
			if ( Cmd_Argc() == 2 )
			{
				if ( matchLenAfterCmds == matchLenAfterDvars )
					CompleteCmdArgument();
				else
					CompleteDvarArgument();
			}
		}
		g_consoleField.cursor = strlen(g_consoleField.buffer);
		Field_AdjustScroll(&g_consoleField);
		return;
	}

	Com_sprintf(g_consoleField.buffer, sizeof(g_consoleField.buffer), "\\%s", shortestMatch);
	g_consoleField.cursor = strlen(g_consoleField.buffer);
	ConcatRemaining(temp.buffer, completionString);
	Field_AdjustScroll(&g_consoleField);

	Com_Printf("]%s\n", g_consoleField.buffer);
	Cmd_ForEach(PrintMatches);
	Dvar_ForEach(PrintMatches);
}

bool Console_IsRconCmd(const char *commandString)
{
    const char *rcon = "rcon";

    return I_strncmp(commandString, rcon, strlen(rcon)) == 0;
}

void Console_Key(int key)
{
    // ctrl-L clears screen
    if (key == 'l' && keys[K_CTRL].down) {
        Cbuf_AddText("clear\n");
        return;
    }

    // enter finishes the line
    if (key == K_ENTER || key == K_KP_ENTER) {
        // if not in the game explicitly prepend a slash if needed
        if (::clc.state != CA_ACTIVE && g_consoleField.buffer[0] != '\\' && g_consoleField.buffer[0] != '/') {
            char temp[1024];

            I_strncpyz(temp, g_consoleField.buffer, sizeof(temp));
            Com_sprintf(g_consoleField.buffer, sizeof(g_consoleField.buffer), "\\%s", temp);
            g_consoleField.cursor++;
        }

        Com_Printf("]%s\n", g_consoleField.buffer);

        // leading slash is an explicit command
        if (g_consoleField.buffer[0] == '\\' || g_consoleField.buffer[0] == '/') {
            Cbuf_AddText(g_consoleField.buffer + 1);
            Cbuf_AddText("\n");
        } else {
            // other text will be chat messages
            if (!g_consoleField.buffer[0]) {
                return;
            }
            if (!Console_IsRconCmd(g_consoleField.buffer)) {
                Cbuf_AddText("cmd say ");
                Cbuf_AddText(g_consoleField.buffer);
                Cbuf_AddText("\n");
            }
        }

        // copy line to history buffer
        if (g_consoleField.buffer[0]) {
            historyEditLines[nextHistoryLine % COMMAND_HISTORY] = g_consoleField;
            nextHistoryLine++;
            historyLine = nextHistoryLine;
        }

        Field_Clear(&g_consoleField);
        g_consoleField.widthInPixels = g_console_field_width;
        g_consoleField.charHeight = g_console_char_height;
        g_consoleField.fixedSize = 1;

        if (CL_AllLocalClientsDisconnected()) {
            SCR_UpdateScreen(); // the command may take some time
        }
        return;
    }

    // command completion
    if (key == K_TAB) {
        CompleteCommand();
        return;
    }

    // command history (ctrl-p ctrl-n for unix style)
    if ((key == K_MWHEELUP && keys[K_SHIFT].down) || key == K_UPARROW || key == K_KP_UPARROW
        || (tolower(key) == 'p' && keys[K_CTRL].down)) {
        if (nextHistoryLine - historyLine < COMMAND_HISTORY && historyLine > 0) {
            historyLine--;
        }
        g_consoleField = historyEditLines[historyLine % COMMAND_HISTORY];
        Field_AdjustScroll(&g_consoleField);
        return;
    }

    if ((key == K_MWHEELDOWN && keys[K_SHIFT].down) || key == K_DOWNARROW || key == K_KP_DOWNARROW
        || (tolower(key) == 'n' && keys[K_CTRL].down)) {
        if (historyLine == nextHistoryLine) {
            return;
        }
        historyLine++;
        g_consoleField = historyEditLines[historyLine % COMMAND_HISTORY];
        Field_AdjustScroll(&g_consoleField);
        return;
    }

    // console scrolling
    if (key == K_PGUP) {
        Con_PageUp();
        return;
    }

    if (key == K_PGDN) {
        Con_PageDown();
        return;
    }

    if (key == K_MWHEELUP) {
        Con_PageUp();
        // hold <ctrl> to accelerate scrolling
        if (keys[K_CTRL].down) {
            Con_PageUp();
            Con_PageUp();
        }
        return;
    }

    if (key == K_MWHEELDOWN) {
        Con_PageDown();
        if (keys[K_CTRL].down) {
            Con_PageDown();
            Con_PageDown();
        }
        return;
    }

    // ctrl-home = top of console
    if (key == K_HOME && keys[K_CTRL].down) {
        Con_Top();
        return;
    }

    // ctrl-end = bottom of console
    if (key == K_END && keys[K_CTRL].down) {
        Con_Bottom();
        return;
    }

    // pass to the normal editline routine
    Field_KeyDownEvent(&g_consoleField, key);
}

void Key_WriteBindings(fileHandle_t f)
{
    int keynum;

    FS_Printf(f, "unbindall\n");

    for (keynum = 0; keynum < 256; ++keynum) {
        const char *binding;
        const char *scan;

        binding = keys[keynum].binding;
        if (binding == NULL || *binding == '\0') {
            continue;
        }

        FS_Printf(f, "bind %s \"", Key_KeynumToString(keynum, 0));
        for (scan = keys[keynum].binding; *scan; ++scan) {
            if (*scan == '"') {
                FS_Printf(f, "\\\"");
            } else {
                FS_Printf(f, "%c", *scan);
            }
        }
        FS_Printf(f, "\"\n");
    }
}

void Key_Bindlist_f(void)
{
    int keynum;

    for (keynum = 0; keynum < 256; keynum++) {
        if (keys[keynum].binding && keys[keynum].binding[0]) {
            Com_Printf("%s \"%s\"\n", Key_KeynumToString(keynum, 0), keys[keynum].binding);
        }
    }
}

// retail 0x40acf0
void CL_KeyEvent(int key, const qboolean down, const unsigned int time)
{
    char *kb;
    char cmd[1024];
    qboolean bypassMenu;

    bypassMenu = 0;

    // update auto-repeat status and BUTTON_ANY status
    keys[key].down = down;
    if (down) {
        keys[key].repeats++;
        if (keys[key].repeats == 1)
            anykeydown++;
    } else {
        keys[key].repeats = 0;
        anykeydown--;
    }

    if (key != '`' && key != '~' && !(clients->keyCatchers & 1) && keys[key].binding
        && !stricmp(keys[key].binding, "devgui")) {
        if (down && keys[key].repeats == 1) {
            Cbuf_AddText(keys[key].binding);
            return;
        }
    }

    // only a few keys repeat
    if (down && keys[key].repeats > 1) {
        if (!(clients->keyCatchers & (1 | 0x10))) {
            if (!(clients->keyCatchers & 8))
                return;
            switch (key) {
            case 0x9a:
            case 0x9b:
            case 0xa3:
            case 0xa4:
                break;
            default:
                return;
            }
        }
        if (key == '`' || key == '~' || key == 0x1b)
            return;
    }

    // console key is hardcoded, so the user can never unbind it
    if (key == '`' || key == '~') {
        if (!down)
            return;
        if (!(clients->keyCatchers & 1) && !com_sv_running->current.enabled
            && sv_disableClientConsole->current.enabled)
            return;
        if (keys[0xa0].down) {
            if (!(clients->keyCatchers & 1))
                Con_ToggleConsole_f();
            Con_ToggleConsoleOutput();
        } else {
            Con_ToggleConsole_f();
        }
        return;
    }

    // any key during a demo or cinematic acts like escape
    if (down && (key < 0x80 || key == 0xc8) && (clc.demoplaying || clc.state == CA_CINEMATIC || clc.state == CA_LOGO)
        && !clients->keyCatchers) {
        Dvar_SetString((dvar_t *)nextdemo, "");
        key = 0x1b;
    }

    // escape is always handled special
    if (key == 0x1b && down) {
        if (clients->keyCatchers & 0x10) {
            Message_Key(key);
            return;
        }
        if (!(clients->keyCatchers & 8)) {
            switch (clc.state) {
            case CA_ACTIVE:
                if (!clc.demoplaying && !legacyHacks->cl_serverloadwaiting) {
                    UI_SetActiveMenu(2);
                    return;
                }
                break;
            case CA_CINEMATIC:
            case CA_LOGO:
                CL_StopLogoOrCinematic();
                return;
            case CA_CONNECTING:
            case CA_CHALLENGING:
            case CA_CONNECTED:
                CL_Disconnect();
                if (com_sv_running->current.enabled)
                    legacyHacks->sv_killserver = 1;
                return;
            default:
                if (!cls.uiStarted)
                    return;
                break;
            }
            UI_SetActiveMenu(1);
            return;
        }
        UI_KeyEvent(key, down);
        return;
    }

    if (clients->cgameInitialized && CG_KeyInterceptEvent(key, down))
        return;

    // key up events only perform actions if the game key binding is
    // a button command (leading + sign)
    if (!down) {
        kb = keys[key].binding;
        if (kb && kb[0] == '+') {
            Com_sprintf(cmd, sizeof(cmd), "-%s %i %i\n", kb + 1, key, time);
            Cbuf_AddText(cmd);
        }
        if ((clients->keyCatchers & 8) && cls.uiStarted)
            UI_KeyEvent(key, down);
        return;
    }

    if (cl_bypassMouseInput && cl_bypassMouseInput->current.enabled) {
        if (key == 0xc8 || key == 0xc9 || key == 0xca || !UI_checkKeyExec(key))
            bypassMenu = 1;
    }

    // distribute the key down event to the apropriate handler
    if (clients->keyCatchers & 1) {
        Console_Key(key);
        return;
    }
    if ((clients->keyCatchers & 8) && !bypassMenu) {
        UI_KeyEvent(key, down);
        return;
    }
    if (clients->keyCatchers & 0x10) {
        Message_Key(key);
        return;
    }
    if (clc.state == CA_DISCONNECTED) {
        Console_Key(key);
        return;
    }

    // send the bound action
    kb = keys[key].binding;
    if (!kb) {
        if (key >= 0xc8)
            Com_Printf("%s is unbound, use controls menu to set.\n", Key_KeynumToString(key, 0));
    } else if (kb[0] == '+') {
        // button commands add keynum and time as parms
        Com_sprintf(cmd, sizeof(cmd), "%s %i %i\n", kb, key, time);
        Cbuf_AddText(cmd);
    } else {
        Cbuf_AddText(kb);
        Cbuf_AddText("\n");
    }
}

void Key_ClearStates(void)
{
    int i;
    *anykeydown = 0;

    for (i = 0; i < 256; i++) {
        if (keys[i].down) {
            CL_KeyEvent(i, 0, 0);
        }
        keys[i].down = 0;
        keys[i].repeats = 0;
    }
}

static keyname_t keynames[99] = {
    { (char *)&str_002a6bf0, 0x9 },
    { (char *)&str_002a6bf4, 0xd },
    { (char *)&str_002a6bfc, 0x1b },
    { (char *)&str_002a6c04, 0x20 },
    { (char *)&str_002a6c0c, 0x7f },
    { (char *)&str_002a6c18, 0x9a },
    { (char *)&str_002a6c20, 0x9b },
    { (char *)&str_002a6c2c, 0x9c },
    { (char *)&str_002a6c38, 0x9d },
    { (char *)&str_002a6c44, 0x9e },
    { (char *)&str_002a6c48, 0x9f },
    { (char *)&str_002a6c50, 0xa0 },
    { (char *)&str_002a6c58, 0x97 },
    { (char *)&str_002a6c64, 0xa7 },
    { (char *)&str_002a6c68, 0xa8 },
    { (char *)&str_002a6c6c, 0xa9 },
    { (char *)&str_002a6c70, 0xaa },
    { (char *)&str_002a6c74, 0xab },
    { (char *)&str_002a6c78, 0xac },
    { (char *)&str_002a6c7c, 0xad },
    { (char *)&str_002a6c80, 0xae },
    { (char *)&str_002a6c84, 0xaf },
    { (char *)&str_002a6c88, 0xb0 },
    { (char *)&str_002a6c8c, 0xb1 },
    { (char *)&str_002a6c90, 0xb2 },
    { (char *)&str_002a6c94, 0xb3 },
    { (char *)&str_002a6c98, 0xb4 },
    { (char *)&str_002a6c9c, 0xb5 },
    { (char *)&str_002a6ca0, 0xa1 },
    { (char *)&str_002a6ca4, 0xa2 },
    { (char *)&str_002a6ca8, 0xa3 },
    { (char *)&str_002a6cb0, 0xa4 },
    { (char *)&str_002a6cb8, 0xa5 },
    { (char *)&str_002a6cc0, 0xa6 },
    { (char *)&str_002a6cc4, 0xc8 },
    { (char *)&str_002a6ccc, 0xc9 },
    { (char *)&str_002a6cd4, 0xca },
    { (char *)&str_002a6cdc, 0xcb },
    { (char *)&str_002a6ce4, 0xcc },
    { (char *)&str_002a6cec, 0xce },
    { (char *)&str_002a6cf8, 0xcd },
    { (char *)&str_002a6d04, 0xcf },
    { (char *)&str_002a6d0c, 0xd0 },
    { (char *)&str_002a6d14, 0xd1 },
    { (char *)&str_002a6d1c, 0xd2 },
    { (char *)&str_002a6d24, 0xd3 },
    { (char *)&str_002a6d2c, 0xd4 },
    { (char *)&str_002a6d34, 0xd5 },
    { (char *)&str_002a6d3c, 0xd6 },
    { (char *)&str_002a6d44, 0xd7 },
    { (char *)&str_002a6d4c, 0xd8 },
    { (char *)&str_002a6d54, 0xd9 },
    { (char *)&str_002a6d5c, 0xda },
    { (char *)&str_002a6d64, 0xdb },
    { (char *)&str_002a6d6c, 0xdc },
    { (char *)&str_002a6d74, 0xdd },
    { (char *)&str_002a6d7c, 0xde },
    { (char *)&str_002a6d84, 0xb6 },
    { (char *)&str_002a6d8c, 0xb7 },
    { (char *)&str_002a6d98, 0xb8 },
    { (char *)&str_002a6da0, 0xb9 },
    { (char *)&str_002a6db0, 0xba },
    { (char *)&str_002a6db8, 0xbb },
    { (char *)&str_002a6dc8, 0xbc },
    { (char *)&str_002a6dd0, 0xbd },
    { (char *)&str_002a6de0, 0xbe },
    { (char *)&str_002a6de8, 0xbf },
    { (char *)&str_002a6df4, 0xc0 },
    { (char *)&str_002a6dfc, 0xc1 },
    { (char *)&str_002a6e04, 0xc2 },
    { (char *)&str_002a6e10, 0xc3 },
    { (char *)&str_002a6e1c, 0xc4 },
    { (char *)&str_002a6e24, 0xc5 },
    { (char *)&str_002a6e30, 0xc6 },
    { (char *)&str_002a6e38, 0xc7 },
    { (char *)&str_002a6e44, 0x99 },
    { (char *)&str_002a6e4c, 0x3b },
    { (char *)&str_002a6e58, 0x96 },
    { (char *)&str_002a6e60, 0x80 },
    { (char *)&str_002a6e64, 0x81 },
    { (char *)&str_002a6e68, 0x82 },
    { (char *)&str_002a6e6c, 0x83 },
    { (char *)&str_002a6e70, 0x84 },
    { (char *)&str_002a6e74, 0x85 },
    { (char *)&str_002a6e78, 0x86 },
    { (char *)&str_002a6e7c, 0x87 },
    { (char *)&str_002a6e80, 0x88 },
    { (char *)&str_002a6e84, 0x89 },
    { (char *)&str_002a6e88, 0x8a },
    { (char *)&str_002a6e8c, 0x8b },
    { (char *)&str_002a6e90, 0x8c },
    { (char *)&str_002a6e94, 0x8d },
    { (char *)&str_002a6e98, 0x8e },
    { (char *)&str_002a6e9c, 0x8f },
    { (char *)&str_002a6ea0, 0x90 },
    { (char *)&str_002a6ea4, 0x91 },
    { (char *)&str_002a6ea8, 0x92 },
    { (char *)&str_002a6eac, 0x93 },
    { 0, 0x0 }
};
static keyname_t keynames_localized[100] = {
    { (char *)&str_002a6808, 0x9 },
    { (char *)&str_002a6810, 0xd },
    { (char *)&str_002a681c, 0x1b },
    { (char *)&str_002a6828, 0x20 },
    { (char *)&str_002a6834, 0x7f },
    { (char *)&str_002a6844, 0x9a },
    { (char *)&str_002a6850, 0x9b },
    { (char *)&str_002a6860, 0x9c },
    { (char *)&str_002a6870, 0x9d },
    { (char *)&str_002a6880, 0x9e },
    { (char *)&str_002a6888, 0x9f },
    { (char *)&str_002a6894, 0xa0 },
    { (char *)&str_002a68a0, 0x97 },
    { (char *)&str_002a68b0, 0xa7 },
    { (char *)&str_002a68b8, 0xa8 },
    { (char *)&str_002a68c0, 0xa9 },
    { (char *)&str_002a68c8, 0xaa },
    { (char *)&str_002a68d0, 0xab },
    { (char *)&str_002a68d8, 0xac },
    { (char *)&str_002a68e0, 0xad },
    { (char *)&str_002a68e8, 0xae },
    { (char *)&str_002a68f0, 0xaf },
    { (char *)&str_002a68f8, 0xb0 },
    { (char *)&str_002a6900, 0xb1 },
    { (char *)&str_002a6908, 0xb2 },
    { (char *)&str_002a6910, 0xb3 },
    { (char *)&str_002a6918, 0xb4 },
    { (char *)&str_002a6920, 0xb5 },
    { (char *)&str_002a6928, 0xa1 },
    { (char *)&str_002a6930, 0xa2 },
    { (char *)&str_002a6938, 0xa3 },
    { (char *)&str_002a6944, 0xa4 },
    { (char *)&str_002a6950, 0xa5 },
    { (char *)&str_002a695c, 0xa6 },
    { (char *)&str_002a6964, 0xc8 },
    { (char *)&str_002a6970, 0xc9 },
    { (char *)&str_002a697c, 0xca },
    { (char *)&str_002a6988, 0xcb },
    { (char *)&str_002a6994, 0xcc },
    { (char *)&str_002a69a0, 0xce },
    { (char *)&str_002a69b0, 0xcd },
    { (char *)&str_002a69c0, 0xcf },
    { (char *)&str_002a69cc, 0xd0 },
    { (char *)&str_002a69d8, 0xd1 },
    { (char *)&str_002a69e4, 0xd2 },
    { (char *)&str_002a69f0, 0xd3 },
    { (char *)&str_002a69fc, 0xd4 },
    { (char *)&str_002a6a08, 0xd5 },
    { (char *)&str_002a6a14, 0xd6 },
    { (char *)&str_002a6a20, 0xd7 },
    { (char *)&str_002a6a2c, 0xd8 },
    { (char *)&str_002a6a38, 0xd9 },
    { (char *)&str_002a6a44, 0xda },
    { (char *)&str_002a6a50, 0xdb },
    { (char *)&str_002a6a5c, 0xdc },
    { (char *)&str_002a6a68, 0xdd },
    { (char *)&str_002a6a74, 0xde },
    { (char *)&str_002a6a80, 0xb6 },
    { (char *)&str_002a6a8c, 0xb7 },
    { (char *)&str_002a6a9c, 0xb8 },
    { (char *)&str_002a6aa8, 0xb9 },
    { (char *)&str_002a6abc, 0xba },
    { (char *)&str_002a6ac8, 0xbb },
    { (char *)&str_002a6adc, 0xbc },
    { (char *)&str_002a6ae8, 0xbd },
    { (char *)&str_002a6afc, 0xbe },
    { (char *)&str_002a6b08, 0xbf },
    { (char *)&str_002a6b18, 0xc0 },
    { (char *)&str_002a6b24, 0xc1 },
    { (char *)&str_002a6b30, 0xc2 },
    { (char *)&str_002a6b40, 0xc3 },
    { (char *)&str_002a6b50, 0xc4 },
    { (char *)&str_002a6b5c, 0xc5 },
    { (char *)&str_002a6b6c, 0xc6 },
    { (char *)&str_002a6b78, 0xc7 },
    { (char *)&str_002a6b88, 0x99 },
    { (char *)&str_002a6b94, 0x3b },
    { (char *)&str_002a6ba4, 0x96 },
    { (char *)&str_002a6bb0, 0x80 },
    { (char *)&str_002a6bb4, 0x81 },
    { (char *)&str_002a6bb8, 0x82 },
    { (char *)&str_002a67f0, 0x83 },
    { (char *)&str_002a6bbc, 0x84 },
    { (char *)&str_002a6bc0, 0x85 },
    { (char *)&str_002a6bc4, 0x86 },
    { (char *)&str_002a6bc8, 0x87 },
    { (char *)&str_002a6804, 0x88 },
    { (char *)&str_002a67fc, 0x89 },
    { (char *)&str_002a67f4, 0x8a },
    { (char *)&str_002a6bcc, 0x8b },
    { (char *)&str_002a6bd0, 0x8c },
    { (char *)&str_002a6bd4, 0x8d },
    { (char *)&str_002a6bd8, 0x8e },
    { (char *)&str_002a6bdc, 0x8f },
    { (char *)&str_002a6be0, 0x90 },
    { (char *)&str_002a6be4, 0x91 },
    { (char *)&str_002a6be8, 0x92 },
    { (char *)&str_002a6bec, 0x93 },
    { 0, 0x0 },
    { 0, 0x0 }
};

const char str_002a67f0[] = "\xff" "fd";
const char str_002a67f4[] = "\xff" "fd";
const char str_002a67fc[] = "\xff" "fd";
const char str_002a6804[] = "\xff" "fd";
const char str_002a6808[] = "KEY_TAB";
const char str_002a6810[] = "KEY_ENTER";
const char str_002a681c[] = "KEY_ESCAPE";
const char str_002a6828[] = "KEY_SPACE";
const char str_002a6834[] = "KEY_BACKSPACE";
const char str_002a6844[] = "KEY_UPARROW";
const char str_002a6850[] = "KEY_DOWNARROW";
const char str_002a6860[] = "KEY_LEFTARROW";
const char str_002a6870[] = "KEY_RIGHTARROW";
const char str_002a6880[] = "KEY_ALT";
const char str_002a6888[] = "KEY_CTRL";
const char str_002a6894[] = "KEY_SHIFT";
const char str_002a68a0[] = "KEY_CAPSLOCK";
const char str_002a68b0[] = "KEY_F1";
const char str_002a68b8[] = "KEY_F2";
const char str_002a68c0[] = "KEY_F3";
const char str_002a68c8[] = "KEY_F4";
const char str_002a68d0[] = "KEY_F5";
const char str_002a68d8[] = "KEY_F6";
const char str_002a68e0[] = "KEY_F7";
const char str_002a68e8[] = "KEY_F8";
const char str_002a68f0[] = "KEY_F9";
const char str_002a68f8[] = "KEY_F10";
const char str_002a6900[] = "KEY_F11";
const char str_002a6908[] = "KEY_F12";
const char str_002a6910[] = "KEY_F13";
const char str_002a6918[] = "KEY_F14";
const char str_002a6920[] = "KEY_F15";
const char str_002a6928[] = "KEY_INS";
const char str_002a6930[] = "KEY_DEL";
const char str_002a6938[] = "KEY_PGDN";
const char str_002a6944[] = "KEY_PGUP";
const char str_002a6950[] = "KEY_HOME";
const char str_002a695c[] = "KEY_END";
const char str_002a6964[] = "KEY_MOUSE1";
const char str_002a6970[] = "KEY_MOUSE2";
const char str_002a697c[] = "KEY_MOUSE3";
const char str_002a6988[] = "KEY_MOUSE4";
const char str_002a6994[] = "KEY_MOUSE5";
const char str_002a69a0[] = "KEY_MWHEELUP";
const char str_002a69b0[] = "KEY_MWHEELDOWN";
const char str_002a69c0[] = "KEY_AUX1";
const char str_002a69cc[] = "KEY_AUX2";
const char str_002a69d8[] = "KEY_AUX3";
const char str_002a69e4[] = "KEY_AUX4";
const char str_002a69f0[] = "KEY_AUX5";
const char str_002a69fc[] = "KEY_AUX6";
const char str_002a6a08[] = "KEY_AUX7";
const char str_002a6a14[] = "KEY_AUX8";
const char str_002a6a20[] = "KEY_AUX9";
const char str_002a6a2c[] = "KEY_AUX10";
const char str_002a6a38[] = "KEY_AUX11";
const char str_002a6a44[] = "KEY_AUX12";
const char str_002a6a50[] = "KEY_AUX13";
const char str_002a6a5c[] = "KEY_AUX14";
const char str_002a6a68[] = "KEY_AUX15";
const char str_002a6a74[] = "KEY_AUX16";
const char str_002a6a80[] = "KEY_KP_HOME";
const char str_002a6a8c[] = "KEY_KP_UPARROW";
const char str_002a6a9c[] = "KEY_KP_PGUP";
const char str_002a6aa8[] = "KEY_KP_LEFTARROW";
const char str_002a6abc[] = "KEY_KP_5";
const char str_002a6ac8[] = "KEY_KP_RIGHTARROW";
const char str_002a6adc[] = "KEY_KP_END";
const char str_002a6ae8[] = "KEY_KP_DOWNARROW";
const char str_002a6afc[] = "KEY_KP_PGDN";
const char str_002a6b08[] = "KEY_KP_ENTER";
const char str_002a6b18[] = "KEY_KP_INS";
const char str_002a6b24[] = "KEY_KP_DEL";
const char str_002a6b30[] = "KEY_KP_SLASH";
const char str_002a6b40[] = "KEY_KP_MINUS";
const char str_002a6b50[] = "KEY_KP_PLUS";
const char str_002a6b5c[] = "KEY_KP_NUMLOCK";
const char str_002a6b6c[] = "KEY_KP_STAR";
const char str_002a6b78[] = "KEY_KP_EQUALS";
const char str_002a6b88[] = "KEY_PAUSE";
const char str_002a6b94[] = "KEY_SEMICOLON";
const char str_002a6ba4[] = "KEY_COMMAND";
const char str_002a6bb0[] = "\xff" "fd";
const char str_002a6bb4[] = "\xff" "fd";
const char str_002a6bb8[] = "\xff" "fd";
const char str_002a6bbc[] = "\xff" "fd";
const char str_002a6bc0[] = "\xff" "fd";
const char str_002a6bc4[] = "\xff" "fd";
const char str_002a6bc8[] = "\xff" "fd";
const char str_002a6bcc[] = "\xff" "fd";
const char str_002a6bd0[] = "\xff" "fd";
const char str_002a6bd4[] = "\xff" "fd";
const char str_002a6bd8[] = "\xff" "fd";
const char str_002a6bdc[] = "\xff" "fd";
const char str_002a6be0[] = "\xff" "fd";
const char str_002a6be4[] = "\xff" "fd";
const char str_002a6be8[] = "\xff" "fd";
const char str_002a6bec[] = "\xff" "fd";
const char str_002a6bf0[] = "TAB";
const char str_002a6bf4[] = "ENTER";
const char str_002a6bfc[] = "ESCAPE";
const char str_002a6c04[] = "SPACE";
const char str_002a6c0c[] = "BACKSPACE";
const char str_002a6c18[] = "UPARROW";
const char str_002a6c20[] = "DOWNARROW";
const char str_002a6c2c[] = "LEFTARROW";
const char str_002a6c38[] = "RIGHTARROW";
const char str_002a6c44[] = "ALT";
const char str_002a6c48[] = "CTRL";
const char str_002a6c50[] = "SHIFT";
const char str_002a6c58[] = "CAPSLOCK";
const char str_002a6c64[] = "F1";
const char str_002a6c68[] = "F2";
const char str_002a6c6c[] = "F3";
const char str_002a6c70[] = "F4";
const char str_002a6c74[] = "F5";
const char str_002a6c78[] = "F6";
const char str_002a6c7c[] = "F7";
const char str_002a6c80[] = "F8";
const char str_002a6c84[] = "F9";
const char str_002a6c88[] = "F10";
const char str_002a6c8c[] = "F11";
const char str_002a6c90[] = "F12";
const char str_002a6c94[] = "F13";
const char str_002a6c98[] = "F14";
const char str_002a6c9c[] = "F15";
const char str_002a6ca0[] = "INS";
const char str_002a6ca4[] = "DEL";
const char str_002a6ca8[] = "PGDN";
const char str_002a6cb0[] = "PGUP";
const char str_002a6cb8[] = "HOME";
const char str_002a6cc0[] = "END";
const char str_002a6cc4[] = "MOUSE1";
const char str_002a6ccc[] = "MOUSE2";
const char str_002a6cd4[] = "MOUSE3";
const char str_002a6cdc[] = "MOUSE4";
const char str_002a6ce4[] = "MOUSE5";
const char str_002a6cec[] = "MWHEELUP";
const char str_002a6cf8[] = "MWHEELDOWN";
const char str_002a6d04[] = "AUX1";
const char str_002a6d0c[] = "AUX2";
const char str_002a6d14[] = "AUX3";
const char str_002a6d1c[] = "AUX4";
const char str_002a6d24[] = "AUX5";
const char str_002a6d2c[] = "AUX6";
const char str_002a6d34[] = "AUX7";
const char str_002a6d3c[] = "AUX8";
const char str_002a6d44[] = "AUX9";
const char str_002a6d4c[] = "AUX10";
const char str_002a6d54[] = "AUX11";
const char str_002a6d5c[] = "AUX12";
const char str_002a6d64[] = "AUX13";
const char str_002a6d6c[] = "AUX14";
const char str_002a6d74[] = "AUX15";
const char str_002a6d7c[] = "AUX16";
const char str_002a6d84[] = "KP_HOME";
const char str_002a6d8c[] = "KP_UPARROW";
const char str_002a6d98[] = "KP_PGUP";
const char str_002a6da0[] = "KP_LEFTARROW";
const char str_002a6db0[] = "KP_5";
const char str_002a6db8[] = "KP_RIGHTARROW";
const char str_002a6dc8[] = "KP_END";
const char str_002a6dd0[] = "KP_DOWNARROW";
const char str_002a6de0[] = "KP_PGDN";
const char str_002a6de8[] = "KP_ENTER";
const char str_002a6df4[] = "KP_INS";
const char str_002a6dfc[] = "KP_DEL";
const char str_002a6e04[] = "KP_SLASH";
const char str_002a6e10[] = "KP_MINUS";
const char str_002a6e1c[] = "KP_PLUS";
const char str_002a6e24[] = "KP_NUMLOCK";
const char str_002a6e30[] = "KP_STAR";
const char str_002a6e38[] = "KP_EQUALS";
const char str_002a6e44[] = "PAUSE";
const char str_002a6e4c[] = "SEMICOLON";
const char str_002a6e58[] = "COMMAND";
const char str_002a6e60[] = "181";
const char str_002a6e64[] = "191";
const char str_002a6e68[] = "223";
const char str_002a6e6c[] = "224";
const char str_002a6e70[] = "225";
const char str_002a6e74[] = "228";
const char str_002a6e78[] = "229";
const char str_002a6e7c[] = "230";
const char str_002a6e80[] = "231";
const char str_002a6e84[] = "232";
const char str_002a6e88[] = "233";
const char str_002a6e8c[] = "236";
const char str_002a6e90[] = "241";
const char str_002a6e94[] = "242";
const char str_002a6e98[] = "243";
const char str_002a6e9c[] = "246";
const char str_002a6ea0[] = "248";
const char str_002a6ea4[] = "249";
const char str_002a6ea8[] = "250";
const char str_002a6eac[] = "252";
