// PunkBuster server glue (1.2c). Source file and type names from the Mac 1.3 STABS
// (PC/punkbuster/pbsv.cpp, pbsv.h, pbcommon.h).

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <unistd.h>

struct Pb_Sv_Client_s;

// engine side
void set_sv_punkbuster(char *value);
void Cmd_ExecuteString(const char *text);
void PB_DropClient(int clientNum, char *reason);
int Pb_Q_maxclients();
int Pb_Q_client(int clientNum, Pb_Sv_Client_s *c);
int Pb_Q_stats(int clientNum, char *data);
const char *Dvar_GetVariantString(const char *dvarName);
void PbMsgToScreen(char *prefix, char *msg);
void Com_Printf(const char *fmt, ...);
void SV_SendPbPacket(int datalen, char *data, int clientNum);
void Sys_PBSendUdpPacket(char *addr, unsigned short port, int datalen, char *data);
void PBdvar_set(const char *var_name, const char *value);

char *PbSvGameCommand(char *Cmd, char *Result);
char *PbSvGameQuery(int Qtype, char *Data);
char *PbSvGameMsg(char *Msg, int Type);
char *PbSvSendToClient(int DataLen, char *Data, int clientIndex);
char *PbSvSendToAddrPort(char *addr, unsigned short port, int DataLen, char *Data);

struct stPbSv;

typedef char *(*tdPbGameCommand)(char *, char *);
typedef char *(*tdPbGameQuery)(int, char *);
typedef char *(*tdPbGameMsg)(char *, int);
typedef char *(*tdPbSendToClient)(int, char *, int);
typedef char *(*tdPbAddSvEvent)(stPbSv *, int, int, int, char *, int);
typedef char *(*tdPbProcessPbEvents)(stPbSv *, int);
typedef char *(*tdPbSendToAddrPort)(char *, unsigned short, int, char *);
typedef char *(*tdPbPassConnectString)(stPbSv *, char *, char *);
typedef char *(*tdPbAuthClient)(stPbSv *, char *, int, char *);
typedef void (*tdPbTrapConsole)(stPbSv *, char *, int);

inline char *itoa(int val, char *str, int radix)
{
	char buf[35];
	unsigned long u;
	unsigned long p;
	unsigned int d;

	if ( !str )
		return 0;
	strcpy(str, "0");
	if ( val && radix > 1 && radix <= 36 )
	{
		u = val;
		p = 34;
		buf[p] = 0;
		if ( val < 0 && radix == 10 )
			u = -val;
		while ( u )
		{
			d = u % radix;
			buf[--p] = d < 10 ? d + '0' : d + 'a' - 10;
			u /= radix;
		}
		if ( val < 0 && radix == 10 )
			buf[--p] = '-';
		strcpy(str, &buf[p]);
	}
	return str;
}

// name unknown (no Mac symbol)
inline void pbChmod_813cbd4(char *path)
{
	chmod(path, 0777);
}

// name unknown (no Mac symbol)
inline int pbCopyFile_813cf52(char *fromFn, char *toFn, int maxSize)
{
	FILE *fs;
	int res;
	FILE *ft;
	long siz;
	char *b;
	int r;
	int w;

	fs = fopen(fromFn, "rb");
	res = 0;
	if ( fs )
	{
		ft = fopen(toFn, "wb");
		if ( ft )
		{
			fseek(fs, 0, SEEK_END);
			siz = ftell(fs);
			if ( siz > 0 && ( !maxSize || siz < maxSize ) )
			{
				b = new char[siz];
				if ( b )
				{
					fseek(fs, 0, SEEK_SET);
					r = fread(b, 1, siz, fs);
					w = fwrite(b, 1, r, ft);
					delete b;
					if ( w == siz )
						res = 1;
				}
			}
			fclose(ft);
		}
		fclose(fs);
	}
	return res;
}

struct stPbSv
{
	unsigned long m_svId;
	void *m_Md5;
	void *m_SvInstance;
	void *m_ClInstance;
	void *m_AgInstance;
	char m_msgPrefix[32];
	char m_cwd[257];
	int m_ReloadServer;
	tdPbGameCommand m_GameCommand;
	tdPbGameQuery m_GameQuery;
	tdPbGameMsg m_GameMsg;
	tdPbSendToClient m_SendToClient;
	tdPbAddSvEvent m_AddPbEvent;
	tdPbProcessPbEvents m_ProcessPbEvents;
	tdPbSendToAddrPort m_SendToAddrPort;
	tdPbPassConnectString m_PassConnectString;
	tdPbAuthClient m_AuthClient;
	tdPbTrapConsole m_TrapConsole;
	void *m_Agent;

	void getBasePath(char *path)
	{
		if ( !m_GameQuery )
			return;
		m_GameQuery(103, strcpy(path, "fs_basepath"));
	}

	void copyIfNotExists(char *fn, char *basepath)
	{
		char fromFn[511];
		char toFn[511];
		FILE *f;

		strcpy(toFn, m_cwd);
		strcat(toFn, fn);
		f = fopen(toFn, "rb");
		if ( f )
		{
			fclose(f);
		}
		else
		{
			strcpy(fromFn, basepath);
			strcat(fromFn, fn);
			pbCopyFile_813cf52(fromFn, toFn, 0);
		}
	}

	void getHomePath()
	{
		if ( !m_GameQuery )
			return;
		m_GameQuery(103, strcpy(m_cwd, "fs_homepath"));
		if ( !*m_cwd )
			getcwd(m_cwd, 251);
		// *"/" (not '/'): retail compares the promoted int
		if ( *m_cwd && m_cwd[strlen(m_cwd) - 1] != *"/" )
			strcat(m_cwd, "/");
		strcat(m_cwd, "pb/");
	}

	char *makefn(char *buf, char *fn)
	{
		char basepath[260];

		if ( !*m_cwd )
		{
			getHomePath();
			getBasePath(basepath);
			if ( basepath[strlen(basepath) - 1] != *"/" )
				strcat(basepath, "/");
			strcat(basepath, "pb/");
			if ( strcasecmp(basepath, m_cwd) && *basepath && *m_cwd )
			{
				mkdir(m_cwd, 0777);
				copyIfNotExists("pbsv.so", basepath);
				copyIfNotExists("pbcl.so", basepath);
				copyIfNotExists("pbag.so", basepath);
			}
		}
		strcpy(buf, m_cwd);
		strcat(buf, fn);
		return buf;
	}

	void UnloadClientDll()
	{
		if ( m_ClInstance )
			dlclose(m_ClInstance);
		m_ClInstance = 0;
	}

	void UnloadAgentDll()
	{
		m_Agent = 0;
		if ( m_AgInstance )
			dlclose(m_AgInstance);
		m_AgInstance = 0;
	}

	void uninitialize()
	{
		m_GameCommand = 0;
		m_GameQuery = 0;
		m_GameMsg = 0;
		m_SendToClient = 0;
	}

	void initialize()
	{
		uninitialize();
		m_GameCommand = PbSvGameCommand;
		m_GameQuery = PbSvGameQuery;
		m_GameMsg = PbSvGameMsg;
		m_SendToClient = PbSvSendToClient;
		m_SendToAddrPort = PbSvSendToAddrPort;
	}

	void UnloadServerDll()
	{
		m_Md5 = 0;
		m_ProcessPbEvents = 0;
		m_AddPbEvent = 0;
		m_PassConnectString = 0;
		m_AuthClient = 0;
		m_TrapConsole = 0;
		if ( m_SvInstance )
			dlclose(m_SvInstance);
		m_SvInstance = 0;
	}

	char *LoadServerDll()
	{
		char fn[512];
		char extrafn[512];
		FILE *f;

		if ( m_SvInstance )
			return NULL;
		UnloadServerDll();
		f = fopen(makefn(fn, "pbsvnew.so"), "rb");
		if ( f )
		{
			fclose(f);
			pbChmod_813cbd4(makefn(fn, "pbsvold.so"));
			remove(makefn(fn, "pbsvold.so"));
			rename(makefn(fn, "pbsv.so"), makefn(extrafn, "pbsvold.so"));
			pbChmod_813cbd4(makefn(fn, "pbsv.so"));
			remove(makefn(fn, "pbsv.so"));
			rename(makefn(fn, "pbsvnew.so"), makefn(extrafn, "pbsv.so"));
		}
		m_SvInstance = dlopen(makefn(fn, "pbsv.so"), RTLD_LAZY);
		if ( !m_SvInstance )
			return "PB Error: Server DLL Load Failure";
		m_ProcessPbEvents = (tdPbProcessPbEvents)dlsym(m_SvInstance, "sa");
		m_AddPbEvent = (tdPbAddSvEvent)dlsym(m_SvInstance, "sb");
		if ( !m_ProcessPbEvents || !m_AddPbEvent )
		{
			UnloadServerDll();
			return "PB Error: Server DLL Get Procedure Failure";
		}
		m_ReloadServer = 0;
		return NULL;
	}

	char *AddPbEvent(int type, int clientIndex, int datalen, char *data, int retry)
	{
		if ( !m_GameCommand )
			return NULL;
		if ( m_ReloadServer || !m_SvInstance )
		{
			if ( m_SvInstance )
			{
				UnloadServerDll();
				return NULL;
			}
			char *res = LoadServerDll();
			if ( res )
			{
				if ( type == 0x71 || type == 0x72 )
					return NULL;
				return res;
			}
		}
		return m_AddPbEvent(this, type, clientIndex, datalen, data, retry);
	}

	char *ProcessPbEvents(int n)
	{
		if ( !m_GameCommand )
			return NULL;
		if ( !m_SvInstance )
		{
			if ( m_ReloadServer )
				AddPbEvent(16, -1, 0, "", 0);
			return NULL;
		}
		if ( m_ReloadServer )
		{
			UnloadServerDll();
			return NULL;
		}
		return m_ProcessPbEvents(this, n);
	}

	stPbSv()
	{
		m_svId = 0x357afe24;
		strcpy(m_msgPrefix, "PunkBuster Server");
		m_SvInstance = 0;
		m_ReloadServer = 1;
		uninitialize();
		m_Md5 = 0;
		m_AddPbEvent = 0;
		m_ProcessPbEvents = 0;
		m_SendToAddrPort = 0;
		m_PassConnectString = 0;
		m_AuthClient = 0;
		m_TrapConsole = 0;
	}

	~stPbSv()
	{
		UnloadServerDll();
		UnloadClientDll();
		UnloadAgentDll();
	}
};

stPbSv pbsv;
char *g_ConsoleCaptureBuf;
long g_ConsoleCaptureBufLen;

void PbSvAddEvent(int event, int clientIndex, int datalen, char *data)
{
	pbsv.AddPbEvent(event, clientIndex, datalen, data, 0);
}

// name unknown: queues "<a> <b>" as PB event 15 for a client
void PbSvEvent15_813be28(int clientIndex, char *a, char *b)
{
	char buf[2050];

	if ( strlen(a) + strlen(b) > 2048 )
		return;
	strcpy(buf, a);
	strcat(buf, " ");
	strcat(buf, b);
	PbSvAddEvent(15, clientIndex, strlen(buf), buf);
}

void PbServerInitialize()
{
	pbsv.initialize();
	PbSvAddEvent(16, -1, 0, "");
	if ( !pbsv.m_AddPbEvent )
		set_sv_punkbuster("0");
}

void PbServerProcessEvents()
{
	pbsv.ProcessPbEvents(0);
}

void PbServerForceProcess()
{
	pbsv.ProcessPbEvents(-1);
}

void PbServerCompleteCommand(char *buf, int buflen)
{
	pbsv.AddPbEvent(0x33, -1, buflen, buf, 0);
}

void PbPassConnectString(char *fromAddr, char *connectString)
{
	if ( !pbsv.m_PassConnectString )
		return;
	pbsv.m_PassConnectString(&pbsv, fromAddr, connectString);
}

char *PbAuthClient(char *fromAddr, int cl_pb, char *cl_guid)
{
	if ( !pbsv.m_AuthClient )
		return NULL;
	return pbsv.m_AuthClient(&pbsv, fromAddr, cl_pb, cl_guid);
}

// name unknown: PB event 0x71
void PbSvEvent71_813c004()
{
	pbsv.AddPbEvent(0x71, -1, 0, 0, 0);
}

void EnablePbSv()
{
	pbsv.AddPbEvent(0x75, -1, 0, 0, 0);
}

void DisablePbSv()
{
	pbsv.AddPbEvent(0x76, -1, 0, 0, 0);
}

void PbCaptureConsoleOutput(char *msg, int msglen)
{
	int len;

	if ( pbsv.m_TrapConsole )
		pbsv.m_TrapConsole(&pbsv, msg, msglen);
	if ( !g_ConsoleCaptureBuf )
		return;
	len = strlen(g_ConsoleCaptureBuf);
	if ( len + (int)strlen(msg) >= g_ConsoleCaptureBufLen )
		return;
	strcpy(g_ConsoleCaptureBuf + len, msg);
}

char *PbSvGameCommand(char *Cmd, char *Result)
{
	if ( !strcasecmp(Cmd, "set_sv_punkbuster") )
	{
		set_sv_punkbuster(Result);
	}
	else if ( !strcasecmp(Cmd, "ConCapBufLen") )
	{
		g_ConsoleCaptureBufLen = (long)Result;
	}
	else if ( !strcasecmp(Cmd, "ConCapBuf") )
	{
		g_ConsoleCaptureBuf = Result;
	}
	else if ( !strcasecmp(Cmd, "Cmd_Exec") )
	{
		int pbCmd = !strncasecmp(Result, "pb_", 3);
		Cmd_ExecuteString(Result);
		if ( pbCmd )
			PbServerForceProcess();
	}
	else
	{
		char *arg1 = Result;
		char *end;
		char hold;

		while ( *arg1 == ' ' )
			arg1++;
		while ( *arg1 && *arg1 != ' ' )
			arg1++;
		end = arg1;
		while ( *arg1 == ' ' )
			arg1++;
		if ( !strcasecmp(Cmd, "DropClient") )
		{
			PB_DropClient(atoi(Result), arg1);
		}
		else if ( !strcasecmp(Cmd, "Cvar_Set") || !strcasecmp(Cmd, "Dvar_Set") )
		{
			hold = *end;
			*end = 0;
			PBdvar_set(Result, arg1);
			*end = hold;
		}
	}
	return 0;
}

char *PbSvGameQuery(int Qtype, char *Data)
{
	int clientIndex;

	Data[255] = 0;
	switch ( Qtype )
	{
	case 101:
		itoa(Pb_Q_maxclients(), Data, 10);
		break;
	case 102:
		clientIndex = atoi(Data);
		if ( !Pb_Q_client(clientIndex, (Pb_Sv_Client_s *)Data) )
			return "PB Error: Query Failed";
		break;
	case 103:
		strncpy(Data, Dvar_GetVariantString(Data), 255);
		break;
	case 114:
		clientIndex = atoi(Data);
		if ( !Pb_Q_stats(clientIndex, Data) )
			return "PB Error: Query Failed";
		break;
	}
	return NULL;
}

char *PbSvGameMsg(char *Msg, int Type)
{
	if ( strncasecmp(pbsv.m_msgPrefix, "[skipnotify]", 12) )
		PbMsgToScreen(pbsv.m_msgPrefix, Msg);
	else
		Com_Printf("%s: %s\n", pbsv.m_msgPrefix + 12, Msg);
	return 0;
}

char *PbSvSendToClient(int DataLen, char *Data, int clientIndex)
{
	SV_SendPbPacket(DataLen, Data, clientIndex);
	return 0;
}

char *PbSvSendToAddrPort(char *addr, unsigned short port, int DataLen, char *Data)
{
	Sys_PBSendUdpPacket(addr, port, DataLen, Data);
	return 0;
}
