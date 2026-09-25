#include "../qcommon/qcommon.h"
#include "script_public.h"

scrParserGlob_t scrParserGlob;
scrParserPub_t scrParserPub;

unsigned int Scr_GetSourceBuffer( const char *codePos );

/*
==============
Scr_InitOpcodeLookup
==============
*/
void Scr_InitOpcodeLookup()
{
	assert(!scrParserGlob.opcodeLookup);
	assert(!scrParserGlob.sourcePosLookup);
	assert(!scrParserPub.sourceBufferLookup);

	if ( !scrVarPub.developer )
	{
		return;
	}

	scrParserGlob.delayedSourceIndex = -1;

	scrParserGlob.opcodeLookupMaxLen = INITIAL_OPCODE_LOOKUP_LEN;
	scrParserGlob.opcodeLookupLen = 0;
	scrParserGlob.opcodeLookup = (OpcodeLookup *)Z_Malloc( sizeof( *scrParserGlob.opcodeLookup ) * scrParserGlob.opcodeLookupMaxLen );
	memset(scrParserGlob.opcodeLookup, 0, sizeof( *scrParserGlob.opcodeLookup ) * scrParserGlob.opcodeLookupMaxLen);

	scrParserGlob.sourcePosLookupMaxLen = INITIAL_SOURCEPOS_LOOKUP_LEN;
	scrParserGlob.sourcePosLookupLen = 0;
	scrParserGlob.sourcePosLookup = (SourceLookup *)Z_Malloc( sizeof( *scrParserGlob.sourcePosLookup ) * scrParserGlob.sourcePosLookupMaxLen );

	scrParserGlob.currentCodePos = NULL;

	scrParserGlob.currentSourcePosCount = 0;
	scrParserGlob.sourceBufferLookupMaxLen = INITIAL_SOURCEBUFFER_LOOKUP_LEN;

	scrParserPub.sourceBufferLookupLen = 0;
	scrParserPub.sourceBufferLookup = (SourceBufferInfo *)Z_Malloc( sizeof( *scrParserPub.sourceBufferLookup ) * scrParserGlob.sourceBufferLookupMaxLen );
}

/*
==============
Scr_ShutdownOpcodeLookup
==============
*/
void Scr_ShutdownOpcodeLookup()
{
	int i;

	if ( scrParserGlob.opcodeLookup )
	{
		Z_Free( scrParserGlob.opcodeLookup );
		scrParserGlob.opcodeLookup = NULL;
	}

	if ( scrParserGlob.sourcePosLookup )
	{
		Z_Free( scrParserGlob.sourcePosLookup );
		scrParserGlob.sourcePosLookup = NULL;
	}

	if ( scrParserPub.sourceBufferLookup )
	{
		for ( i = 0; i < scrParserPub.sourceBufferLookupLen; i++ )
		{
			Z_Free( scrParserPub.sourceBufferLookup[i].buf );
		}

		Z_Free( scrParserPub.sourceBufferLookup );
		scrParserPub.sourceBufferLookup = NULL;
	}

	if ( scrParserGlob.saveSourceBufferLookup )
	{
		for ( i = 0; i < scrParserGlob.saveSourceBufferLookupLen; i++ )
		{
			if ( scrParserGlob.saveSourceBufferLookup[i].sourceBuf )
			{
				Z_Free( scrParserGlob.saveSourceBufferLookup[i].sourceBuf );
			}
		}

		Z_Free( scrParserGlob.saveSourceBufferLookup );
		scrParserGlob.saveSourceBufferLookup = NULL;
	}
}

/*
==============
AddOpcodePos
==============
*/
void AddOpcodePos( unsigned int sourcePos, int type )
{
	OpcodeLookup *newOpcodeLookup;
	SourceLookup *newSourcePosLookup;
	OpcodeLookup *opcodeLookup;
	SourceLookup *sourcePosLookup;
	int sourcePosLookupIndex;

	if ( !scrVarPub.developer )
	{
		return;
	}

	if ( scrCompilePub.developer_statement == SCR_DEV_IGNORE )
	{
		assert(!scrVarPub.developer_script);
		return;
	}

	if ( !scrCompilePub.allowedBreakpoint )
	{
		type &= ~SOURCE_TYPE_BREAKPOINT;
	}

	assert(scrParserGlob.opcodeLookup);
	assert(scrParserGlob.opcodeLookupMaxLen);
	assert(scrParserGlob.sourcePosLookup);
	assert(scrCompilePub.opcodePos);

	if ( scrParserGlob.opcodeLookupLen >= scrParserGlob.opcodeLookupMaxLen )
	{
		scrParserGlob.opcodeLookupMaxLen *= 2;
		assert(scrParserGlob.opcodeLookupLen < scrParserGlob.opcodeLookupMaxLen);

		newOpcodeLookup = (OpcodeLookup *)Z_Malloc(sizeof(*newOpcodeLookup) * scrParserGlob.opcodeLookupMaxLen);
		memcpy(newOpcodeLookup, scrParserGlob.opcodeLookup, sizeof(*newOpcodeLookup) * scrParserGlob.opcodeLookupLen);

		Z_Free(scrParserGlob.opcodeLookup);
		scrParserGlob.opcodeLookup = newOpcodeLookup;
	}

	if ( scrParserGlob.sourcePosLookupLen >= scrParserGlob.sourcePosLookupMaxLen )
	{
		scrParserGlob.sourcePosLookupMaxLen *= 2;
		assert(scrParserGlob.sourcePosLookupLen < scrParserGlob.sourcePosLookupMaxLen);

		newSourcePosLookup = (SourceLookup *)Z_Malloc(sizeof(*newSourcePosLookup) * scrParserGlob.sourcePosLookupMaxLen);
		memcpy(newSourcePosLookup, scrParserGlob.sourcePosLookup, sizeof(*newSourcePosLookup) * scrParserGlob.sourcePosLookupLen);

		Z_Free(scrParserGlob.sourcePosLookup);
		scrParserGlob.sourcePosLookup = newSourcePosLookup;
	}

	if ( scrParserGlob.currentCodePos == scrCompilePub.opcodePos )
	{
		assert(scrParserGlob.currentSourcePosCount);
		--scrParserGlob.opcodeLookupLen;
		opcodeLookup = &scrParserGlob.opcodeLookup[scrParserGlob.opcodeLookupLen];
		assert(opcodeLookup->sourcePosIndex + scrParserGlob.currentSourcePosCount == scrParserGlob.sourcePosLookupLen);
		assert(opcodeLookup->codePos == (char *)scrParserGlob.currentCodePos);
	}
	else
	{
		scrParserGlob.currentSourcePosCount = 0;
		scrParserGlob.currentCodePos = scrCompilePub.opcodePos;

		opcodeLookup = &scrParserGlob.opcodeLookup[scrParserGlob.opcodeLookupLen];
		opcodeLookup->sourcePosIndex = scrParserGlob.sourcePosLookupLen;
		opcodeLookup->codePos = (const char *)scrParserGlob.currentCodePos;
	}

	sourcePosLookupIndex = opcodeLookup->sourcePosIndex + scrParserGlob.currentSourcePosCount;
	sourcePosLookup = &scrParserGlob.sourcePosLookup[sourcePosLookupIndex];
	sourcePosLookup->sourcePos = sourcePos;

	if ( sourcePos == -1 )
	{
		assert(scrParserGlob.delayedSourceIndex == -1);
		assert(type & SOURCE_TYPE_BREAKPOINT);
		scrParserGlob.delayedSourceIndex = sourcePosLookupIndex;
	}
	else if ( sourcePos == -2 )
	{
		scrParserGlob.threadStartSourceIndex = sourcePosLookupIndex;
	}
	else if ( scrParserGlob.delayedSourceIndex >= 0 && type & SOURCE_TYPE_BREAKPOINT )
	{
		scrParserGlob.sourcePosLookup[scrParserGlob.delayedSourceIndex].sourcePos = sourcePos;
		scrParserGlob.delayedSourceIndex = -1;
	}

	sourcePosLookup->type |= type;

	scrParserGlob.currentSourcePosCount++;
	opcodeLookup->sourcePosCount = scrParserGlob.currentSourcePosCount;

	scrParserGlob.opcodeLookupLen++;
	scrParserGlob.sourcePosLookupLen++;
}

/*
==============
RemoveOpcodePos
==============
*/
void RemoveOpcodePos()
{
	OpcodeLookup *opcodeLookup;

	if ( !scrVarPub.developer )
	{
		return;
	}

	if ( scrCompilePub.developer_statement == SCR_DEV_IGNORE )
	{
		assert(!scrVarPub.developer_script);
		return;
	}

	assert(scrParserGlob.opcodeLookup);
	assert(scrParserGlob.opcodeLookupMaxLen);
	assert(scrParserGlob.sourcePosLookup);
	assert(scrCompilePub.opcodePos);

	assert(scrParserGlob.sourcePosLookupLen);
	scrParserGlob.sourcePosLookupLen--;

	assert(scrParserGlob.opcodeLookupLen);
	scrParserGlob.opcodeLookupLen--;

	assert(scrParserGlob.currentSourcePosCount);
	scrParserGlob.currentSourcePosCount--;

	opcodeLookup = &scrParserGlob.opcodeLookup[scrParserGlob.opcodeLookupLen];

	assert(scrParserGlob.currentCodePos == scrCompilePub.opcodePos);
	assert(opcodeLookup->sourcePosIndex + scrParserGlob.currentSourcePosCount == scrParserGlob.sourcePosLookupLen);
	assert(opcodeLookup->codePos == (char *)scrParserGlob.currentCodePos);

	if ( !scrParserGlob.currentSourcePosCount )
	{
		scrParserGlob.currentCodePos = NULL;
	}

	opcodeLookup->sourcePosCount = scrParserGlob.currentSourcePosCount;
}

/*
==============
AddThreadStartOpcodePos
==============
*/
void AddThreadStartOpcodePos( unsigned int sourcePos )
{
	SourceLookup *sourcePosLookup;

	if ( !scrVarPub.developer )
	{
		return;
	}

	if ( scrCompilePub.developer_statement == SCR_DEV_IGNORE )
	{
		assert(!scrVarPub.developer_script);
		return;
	}

	assert(scrParserGlob.threadStartSourceIndex >= 0);

	sourcePosLookup = &scrParserGlob.sourcePosLookup[scrParserGlob.threadStartSourceIndex];
	sourcePosLookup->sourcePos = sourcePos;

	assert(!sourcePosLookup->type);
	sourcePosLookup->type = SOURCE_TYPE_THREAD_START;

	scrParserGlob.threadStartSourceIndex = -1;
}

/*
==============
Scr_GetOpcodePosOfType
==============
*/
const char *Scr_GetOpcodePosOfType( unsigned int bufferIndex, unsigned int startSourcePos, unsigned int endSourcePos, int type, unsigned int *sourcePos )
{
	unsigned int i;
	int j;
	const char *firstOpcodePos;
	unsigned int firstSourcePos;
	int sourcePosCount;
	SourceLookup *sourcePosLookup;
	const char *opcodePos;
	SourceBufferInfo *sourceBufData;
	SourceBufferInfo *nextSourceBufData;
	const char *startCodePos;
	const char *endCodePos;

	sourceBufData = &scrParserPub.sourceBufferLookup[bufferIndex];
	nextSourceBufData = bufferIndex + 1 < scrParserPub.sourceBufferLookupLen ? &scrParserPub.sourceBufferLookup[bufferIndex + 1] : NULL;
	startCodePos = sourceBufData->codePos;

	if ( !startCodePos )
	{
		*sourcePos = 0;
		return NULL;
	}

	firstOpcodePos = NULL;
	firstSourcePos = 0;
	endCodePos = nextSourceBufData ? nextSourceBufData->codePos - 1 : (const char *)-1;

	for ( i = 0; i < scrParserGlob.opcodeLookupLen; i++ )
	{
		sourcePosCount = scrParserGlob.opcodeLookup[i].sourcePosCount;
		for ( j = 0; j < sourcePosCount; j++ )
		{
			opcodePos = scrParserGlob.opcodeLookup[i].codePos;
			if ( opcodePos < startCodePos )
			{
				continue;
			}
			if ( opcodePos > endCodePos )
			{
				continue;
			}
			sourcePosLookup = &scrParserGlob.sourcePosLookup[scrParserGlob.opcodeLookup[i].sourcePosIndex + j];
			if ( ( sourcePosLookup->type & type ) != type )
			{
				continue;
			}
			if ( sourcePosLookup->sourcePos >= startSourcePos && sourcePosLookup->sourcePos < endSourcePos )
			{
				opcodePos = scrParserGlob.opcodeLookup[i].codePos;
				if ( !firstOpcodePos || opcodePos < firstOpcodePos )
				{
					firstOpcodePos = opcodePos;
					firstSourcePos = sourcePosLookup->sourcePos;
				}
			}
		}
	}

	*sourcePos = firstSourcePos;
	return firstOpcodePos;
}

/*
==============
Scr_GetClosestSourcePosOfType
==============
*/
unsigned int Scr_GetClosestSourcePosOfType( unsigned int bufferIndex, unsigned int sourcePos, int type )
{
	unsigned int i;
	int j;
	int sourcePosCount;
	SourceLookup *sourcePosLookup;
	SourceBufferInfo *sourceBufData;
	const char *opcodePos;
	SourceBufferInfo *nextSourceBufData;
	const char *startCodePos;
	const char *endCodePos;
	unsigned int bestSourcePos;

	bestSourcePos = 0;
	sourceBufData = &scrParserPub.sourceBufferLookup[bufferIndex];
	nextSourceBufData = bufferIndex + 1 < scrParserPub.sourceBufferLookupLen ? &scrParserPub.sourceBufferLookup[bufferIndex + 1] : NULL;
	startCodePos = sourceBufData->codePos;

	if ( !startCodePos )
	{
		return 0;
	}

	endCodePos = nextSourceBufData ? nextSourceBufData->codePos - 1 : (const char *)-1;

	for ( i = 0; i < scrParserGlob.opcodeLookupLen; i++ )
	{
		sourcePosCount = scrParserGlob.opcodeLookup[i].sourcePosCount;
		for ( j = 0; j < sourcePosCount; j++ )
		{
			opcodePos = scrParserGlob.opcodeLookup[i].codePos;
			if ( opcodePos < startCodePos )
			{
				continue;
			}
			if ( opcodePos > endCodePos )
			{
				continue;
			}
			sourcePosLookup = &scrParserGlob.sourcePosLookup[scrParserGlob.opcodeLookup[i].sourcePosIndex + j];
			if ( ( sourcePosLookup->type & type ) != type )
			{
				continue;
			}
			if ( sourcePosLookup->sourcePos >= bestSourcePos && sourcePosLookup->sourcePos <= sourcePos )
			{
				bestSourcePos = sourcePosLookup->sourcePos;
			}
		}
	}

	return bestSourcePos;
}

/*
==============
Scr_GetPrevSourcePosOpcodeLookup
==============
*/
OpcodeLookup* Scr_GetPrevSourcePosOpcodeLookup( const char *codePos )
{
	int low, high, middle;

	assert( Scr_IsInScriptMemory( codePos ) );
	assert( scrParserGlob.opcodeLookup );

	low = 0;
	high = scrParserGlob.opcodeLookupLen - 1;

	while ( low <= high )
	{
		middle = ( low + high ) / 2;

		if ( codePos >= scrParserGlob.opcodeLookup[middle].codePos )
		{
			low = middle + 1;

			if ( low == scrParserGlob.opcodeLookupLen || codePos < scrParserGlob.opcodeLookup[low].codePos )
			{
				return &scrParserGlob.opcodeLookup[middle];
			}
		}
		else
		{
			high = middle - 1;
		}
	}

	return NULL;
}

/*
==============
Scr_GetSourcePosOpcodeLookup
==============
*/
OpcodeLookup* Scr_GetSourcePosOpcodeLookup( const char *codePos )
{
	int low, high, middle;

	assert( Scr_IsInScriptMemory( codePos ) );
	assert( scrParserGlob.opcodeLookup );

	low = 0;
	high = scrParserGlob.opcodeLookupLen - 1;

	while ( low <= high )
	{
		middle = ( low + high ) / 2;

		if ( codePos >= scrParserGlob.opcodeLookup[middle].codePos )
		{
			if ( codePos == scrParserGlob.opcodeLookup[middle].codePos )
			{
				return &scrParserGlob.opcodeLookup[middle];
			}

			low = middle + 1;
		}
		else
		{
			high = middle - 1;
		}
	}

	return NULL;
}

/*
==============
Scr_GetPrevSourcePos
==============
*/
unsigned int Scr_GetPrevSourcePos( const char *codePos, unsigned int index )
{
	return scrParserGlob.sourcePosLookup[ Scr_GetPrevSourcePosOpcodeLookup( codePos )->sourcePosIndex + index ].sourcePos;
}

/*
==============
Scr_GetLineNumInternal
==============
*/
unsigned int Scr_GetLineNumInternal( const char *buf, unsigned int sourcePos, const char **startLine, int *col )
{
	unsigned int lineNum;

	assert(buf);
	*startLine = buf;
	lineNum = 0;

	while ( sourcePos )
	{
		if ( !buf[0] )
		{
			*startLine = buf + 1;
			lineNum++;
		}

		buf++;
		sourcePos--;
	}

	*col = buf - *startLine;
	return lineNum;
}

/*
==============
Scr_GetLineNum
==============
*/
unsigned int Scr_GetLineNum( unsigned int bufferIndex, unsigned int sourcePos )
{
	int col;
	const char *startLine;

	assert( scrVarPub.developer );
	return Scr_GetLineNumInternal( scrParserPub.sourceBufferLookup[bufferIndex].sourceBuf, sourcePos, &startLine, &col );
}

/*
==============
Scr_GetSourcePosOfType
==============
*/
int Scr_GetSourcePosOfType( const char *codePos, int type, Scr_SourcePos_t *pos )
{
	OpcodeLookup *opcodeLookup;
	unsigned int index;

	opcodeLookup = Scr_GetSourcePosOpcodeLookup( codePos );

	if ( opcodeLookup )
	{
		for ( index = 0; index < opcodeLookup->sourcePosCount; index++ )
		{
			if ( ( scrParserGlob.sourcePosLookup[opcodeLookup->sourcePosIndex + index].type & type ) != type )
			{
				continue;
			}

			pos->sourcePos = scrParserGlob.sourcePosLookup[opcodeLookup->sourcePosIndex + index].sourcePos;
			pos->bufferIndex = Scr_GetSourceBuffer(codePos);
			pos->lineNum = Scr_GetLineNum(pos->bufferIndex, pos->sourcePos);

			return 1;
		}
	}

	return 0;
}

/*
==============
Scr_IsDeveloper
==============
*/
unsigned char Scr_IsDeveloper( void )
{
	return scrVarPub.developer;
}

/*
==============
Scr_GetNewSourceBuffer
==============
*/
SourceBufferInfo* Scr_GetNewSourceBuffer()
{
	SourceBufferInfo *newSourceBufferInfo;
	SourceBufferInfo *sourceBuffer;

	assert(scrParserPub.sourceBufferLookup);
	assert(scrParserGlob.sourceBufferLookupMaxLen);

	if ( scrParserPub.sourceBufferLookupLen >= scrParserGlob.sourceBufferLookupMaxLen )
	{
		scrParserGlob.sourceBufferLookupMaxLen *= 2;
		assert(scrParserPub.sourceBufferLookupLen < scrParserGlob.sourceBufferLookupMaxLen);

		newSourceBufferInfo = (SourceBufferInfo *)Z_Malloc(sizeof(*newSourceBufferInfo) * scrParserGlob.sourceBufferLookupMaxLen);
		Com_Memcpy(newSourceBufferInfo, scrParserPub.sourceBufferLookup, sizeof(*newSourceBufferInfo) * scrParserPub.sourceBufferLookupLen);

		Z_Free(scrParserPub.sourceBufferLookup);
		scrParserPub.sourceBufferLookup = newSourceBufferInfo;
	}

	sourceBuffer = &scrParserPub.sourceBufferLookup[scrParserPub.sourceBufferLookupLen];
	scrParserPub.sourceBufferLookupLen++;

	return sourceBuffer;
}

/*
==============
Scr_AddSourceBufferInternal
==============
*/
void Scr_AddSourceBufferInternal( const char *extFilename, const char *codePos, char *sourceBuf, int len, bool doEolFixup, bool archive )
{
	char *buf;
	int l;
	int size;
	char *dest;
	char *b;
	int i;
	char c;
	char *source;
	SourceBufferInfo *newSourceBuffer;

	if ( !scrParserPub.sourceBufferLookup )
	{
		scrParserPub.sourceBuf = NULL;
		return;
	}

	l = strlen( extFilename ) + 1;
	size = l + len + 2;
	buf = (char *)Z_Malloc( size );
	strcpy( buf, extFilename );

	b = sourceBuf ? &buf[l] : NULL;

	source = sourceBuf;
	dest = b;

	if ( doEolFixup )
	{
		for ( i = 0; i <= len; i++ )
		{
			c = *source;
			source++;

			if ( c == 10 || c == 13 && *source != 10 )
			{
				*dest = 0;
			}
			else
			{
				*dest = c;
			}

			dest++;
		}
	}
	else
	{
		for ( i = 0; i <= len; i++ )
		{
			c = *source;
			source++;

			*dest = c;
			dest++;
		}
	}

	newSourceBuffer = Scr_GetNewSourceBuffer();

	newSourceBuffer->codePos = codePos;
	newSourceBuffer->buf = buf;
	newSourceBuffer->sourceBuf = b;
	newSourceBuffer->len = len;
	newSourceBuffer->sortedIndex = -1;
	newSourceBuffer->archive = archive;

	if ( b )
	{
		scrParserPub.sourceBuf = b;
	}
}

/*
==============
Scr_ReadFile
==============
*/
char* Scr_ReadFile( const char *filename, const char *extFilename, const char *codePos, bool archive )
{
	char *buf;
	int len;
	fileHandle_t file;

	len = FS_FOpenFileByMode( extFilename, &file, FS_READ );

	if ( len < 0 )
	{
		Scr_AddSourceBufferInternal( extFilename, codePos, NULL, -1, true, archive );
		return NULL;
	}

	buf = (char *)Hunk_AllocateTempMemoryHighInternal( len + 1 );

	FS_Read( buf, len, file );
	buf[len] = 0;

	FS_FCloseFile( file );
	Scr_AddSourceBufferInternal( extFilename, codePos, buf, len, true, archive );

	return buf;

}

/*
==============
Scr_AddSourceBuffer
==============
*/
char* Scr_AddSourceBuffer( const char *filename, const char *extFilename, const char *codePos, bool archive )
{
	char *dest;
	int i;
	char c;
	char *sourceBuf;
	int len;
	char *source;
	SaveSourceBufferInfo *saveSourceBuffer;

	if ( archive && scrParserGlob.saveSourceBufferLookup )
	{
		assert(scrParserGlob.saveSourceBufferLookupLen > 0);
		--scrParserGlob.saveSourceBufferLookupLen;
		saveSourceBuffer = &scrParserGlob.saveSourceBufferLookup[scrParserGlob.saveSourceBufferLookupLen];

		len = saveSourceBuffer->len;
		assert(len >= -1);

		if ( len < 0 )
		{
			sourceBuf = NULL;
		}
		else
		{
			sourceBuf = (char *)Hunk_AllocateTempMemoryHighInternal( len + 1 );
			source = saveSourceBuffer->sourceBuf;
			dest = sourceBuf;

			for ( i = 0; i < len; i++ )
			{
				c = *source;
				source++;

				*dest = c ? c : 10;
				dest++;
			}

			*dest = 0;

			if ( saveSourceBuffer->sourceBuf )
			{
				Z_Free( scrParserGlob.saveSourceBufferLookup[scrParserGlob.saveSourceBufferLookupLen].sourceBuf );
			}
		}
	}
	else
	{
		return Scr_ReadFile( filename, extFilename, codePos, archive );
	}

	Scr_AddSourceBufferInternal( extFilename, codePos, sourceBuf, len, true, archive );
	return sourceBuf;
}

/*
==============
Scr_GetLineInfo
==============
*/
unsigned int Scr_GetLineInfo( const char *buf, unsigned int sourcePos, int *col, char *line )
{
	int len;
	int i;
	char c;
	unsigned int lineNum;
	const char *startLine;

	lineNum = Scr_GetLineNumInternal(buf, sourcePos, &startLine, col);

	len = strlen(startLine);

	if ( len > 1023 )
		len = 1023;

	for ( i = 0; i <= len; i++ )
	{
		c = startLine[i];
		line[i] = c != '\t' ? c : ' ';
	}

	if ( line[len - 1] == '\r' )
		line[len - 1] = 0;

	return lineNum;
}

/*
==============
Scr_PrintSourcePos
==============
*/
void Scr_PrintSourcePos( conChannel_t channel, const char *filename, const char *buf, unsigned int sourcePos )
{
	char line[MAX_STRING_CHARS];
	unsigned int lineNum;
	int col;
	int i;

	assert(filename);
	lineNum = Scr_GetLineInfo(buf, sourcePos, &col, line);

	Com_PrintMessage(channel, va("(file '%s'%s, line %d)\n", filename, scrParserGlob.saveSourceBufferLookup ? " (savegame)" : "", lineNum + 1));
	Com_PrintMessage(channel, va("%s\n", line));

	for ( i = 0; i < col; i++ )
	{
		Com_PrintMessage(channel, " ");
	}

	Com_PrintMessage(channel, "*\n");
}

/*
==============
Scr_GetSourcePos
==============
*/
int Scr_GetSourcePos( unsigned int bufferIndex, unsigned int sourcePos, char *outBuf, int outBufLen )
{
	char line[MAX_STRING_CHARS];
	int lineNum;
	int col;

	assert(scrVarPub.developer);
	lineNum = Scr_GetLineInfo(scrParserPub.sourceBufferLookup[bufferIndex].sourceBuf, sourcePos, &col, line);
	Com_sprintf( outBuf, outBufLen, "%s // %s%s, line %d", line, scrParserPub.sourceBufferLookup[bufferIndex].buf, scrParserGlob.saveSourceBufferLookup ? " (savegame)" : "", lineNum + 1);

	return lineNum;
}

/*
==============
Scr_GetSourceBuffer
==============
*/
unsigned int Scr_GetSourceBuffer( const char *codePos )
{
	int bufferIndex;

	assert( Scr_IsInScriptMemory( codePos ) );
	assert( scrParserPub.sourceBufferLookupLen > 0 );

	for ( bufferIndex = scrParserPub.sourceBufferLookupLen - 1; bufferIndex > 0; bufferIndex-- )
	{
		if ( !scrParserPub.sourceBufferLookup[bufferIndex].codePos )
		{
			continue;
		}

		if ( scrParserPub.sourceBufferLookup[bufferIndex].codePos <= codePos )
		{
			break;
		}
	}

	return bufferIndex;
}

/*
==============
Scr_PrintPrevCodePos
==============
*/
void Scr_PrintPrevCodePos( conChannel_t channel, const char *codePos, unsigned int index )
{
	unsigned int bufferIndex;

	if ( !codePos )
	{
		Com_PrintMessage(channel, "<frozen thread>\n");
	}
	else if ( codePos != &g_EndPos )
	{
		if ( !scrVarPub.developer )
		{
			if ( Scr_IsInOpcodeMemory( codePos - 1 ) )
			{
				Com_PrintMessage(channel, va("@ %d\n", codePos - scrVarPub.programBuffer));
				return;
			}
		}
		else if ( scrVarPub.programBuffer && Scr_IsInOpcodeMemory(codePos) )
		{
			bufferIndex = Scr_GetSourceBuffer( codePos - 1 );
			Scr_PrintSourcePos( channel, scrParserPub.sourceBufferLookup[bufferIndex].buf, scrParserPub.sourceBufferLookup[bufferIndex].sourceBuf, Scr_GetPrevSourcePos( codePos - 1, index ) );
			return;
		}

		Com_PrintMessage( channel, va("%s\n\n", codePos) );
	}
	else
	{
		Com_PrintMessage(channel, "<removed thread>\n");
	}
}

/*
==============
Scr_GetCodePos
==============
*/
void Scr_GetCodePos( const char *codePos, unsigned int index, char *outBuf, int outBufLen )
{
	Scr_SourcePos_t pos;

	if ( !scrVarPub.developer )
	{
		assert( Scr_IsInScriptMemory( codePos ) );
		Com_sprintf(outBuf, outBufLen, "@ %d", codePos - scrVarPub.programBuffer);
		return;
	}

	Scr_GetSourcePosOfType(codePos, SOURCE_TYPE_THREAD_START, &pos);
	Scr_GetSourcePos(pos.bufferIndex, pos.sourcePos, outBuf, outBufLen);
}

/*
==============
Scr_GetFileAndLine
==============
*/
void Scr_GetFileAndLine( const char *codePos, char **filename, int *linenum )
{
	OpcodeLookup *opcodeLookup;
	unsigned int sourcePos;
	unsigned int bufferIndex;

	assert( Scr_IsInScriptMemory( codePos ) );
	opcodeLookup = Scr_GetPrevSourcePosOpcodeLookup(codePos);

	if ( opcodeLookup )
	{
		sourcePos = scrParserGlob.sourcePosLookup[opcodeLookup->sourcePosIndex].sourcePos;
		bufferIndex = Scr_GetSourceBuffer(codePos);

		*linenum = Scr_GetLineNum(bufferIndex, sourcePos) + 1;
		*filename = scrParserPub.sourceBufferLookup[bufferIndex].buf;
	}
	else
	{
		*linenum = 0;
		*filename = "";
	}
}

/*
==============
CompileError
==============
*/
void CompileError( unsigned int sourcePos, const char *format, ... )
{
	va_list argptr;
	char text[MAX_STRING_CHARS];

	va_start( argptr, format );
	vsprintf( text, format, argptr );
	va_end( argptr );

	if ( scrVarPub.evaluate )
	{
		if ( !scrVarPub.error_message )
		{
			scrVarPub.error_message = va("%s", text);
		}
	}
	else
	{
		Com_Printf("\n");
		Com_Printf("******* script compile error *******\n");

		if ( !scrVarPub.developer )
		{
			Com_Printf("%s\n", text);
		}
		else
		{
			Com_Printf("%s: ", text);
			Scr_PrintSourcePos(CON_CHANNEL_DONT_FILTER, scrParserPub.scriptfilename, scrParserPub.sourceBuf, sourcePos);
		}

		Com_Printf("************************************\n");
		Com_Error(ERR_SCRIPT_DROP, "\x15" "script compile error\n(see console for details)");
	}
}

/*
==============
CompileError2
==============
*/
void CompileError2( const char *codePos, const char *msg, ... )
{
	va_list argptr;
	char text[MAX_STRING_CHARS];

	assert( !scrVarPub.evaluate );
	assert( Scr_IsInScriptMemory( codePos ) );

	Com_Printf("\n");
	Com_Printf("******* script compile error *******\n");

	va_start( argptr, msg );
	vsprintf( text, msg, argptr );
	va_end( argptr );

	Com_Printf("%s: ", text);
	Scr_PrintPrevCodePos(CON_CHANNEL_DONT_FILTER, codePos, 0);

	Com_Printf("************************************\n");
	Com_Error(ERR_SCRIPT_DROP, "\x15" "script compile error\n(see console for details)");
}

/*
==============
RuntimeErrorInternal
==============
*/
void RuntimeErrorInternal( conChannel_t channel, const char *codePos, unsigned int index, const char *msg )
{
	int i;
	function_frame_t *frame;

	assert( Scr_IsInScriptMemory( codePos ) );
	Com_PrintMessage(channel, va("\n******* script runtime error *******\n%s: ", msg));
	Scr_PrintPrevCodePos(channel, codePos, index);

	if ( scrVmPub.function_count )
	{
		for ( i = scrVmPub.function_count - 1; i > 0; i-- )
		{
			Com_PrintMessage(channel, "called from:\n");
			frame = &scrVmPub.function_frame_start[i];
			Scr_PrintPrevCodePos(channel, frame->fs.pos, frame->fs.localId == 0);
		}

		Com_PrintMessage(channel, "started from:\n");
		Scr_PrintPrevCodePos(channel, scrVmPub.function_frame_start[0].fs.pos, 1);
	}

	Com_PrintMessage(channel, "************************************\n");
}

/*
==============
RuntimeError
==============
*/
void RuntimeError( const char *codePos, unsigned int index, const char *msg, const char *dialogMessage )
{
	bool abort_on_error;

	if ( scrVarPub.developer || scrVmPub.terminal_error )
	{
		if ( scrVmPub.debugCode )
		{
			Com_Printf("%s\n", msg);

			if ( scrVmPub.terminal_error )
			{
				goto error;
			}
		}
		else
		{
			abort_on_error = false;

			if ( scrVmPub.abort_on_error || scrVmPub.terminal_error )
			{
				abort_on_error = true;
			}

			RuntimeErrorInternal(abort_on_error ? CON_CHANNEL_DONT_FILTER : CON_CHANNEL_LOGFILEONLY, codePos, index, msg);

			if ( !abort_on_error )
			{
				return;
			}
error:
			Com_Error(scrVmPub.terminal_error ? ERR_SCRIPT_DROP : ERR_SCRIPT, "\x15" "script runtime error\n(see console for details)\n%s%s%s", msg, dialogMessage ? "\n" : "", dialogMessage ? dialogMessage : "");
		}
	}
}

/*
==============
node1_
==============
*/
sval_u node1_( sval_u val1 )
{
	return val1;
}

/*
==============
node_pos
==============
*/
sval_u node_pos( unsigned int sourcePos )
{
	sval_u result;

	result.sourcePosValue = sourcePos;
	return result;
}

/*
==============
node0
==============
*/
sval_u node0( int type )
{
	sval_u result;

	result.node = (sval_u *)Hunk_AllocateTempMemoryHighInternal( 1 * sizeof( sval_u ) );
	result.node[0].type = type;
	return result;
}

/*
==============
node1
==============
*/
sval_u node1( int type, sval_u val1 )
{
	sval_u result;

	result.node = (sval_u *)Hunk_AllocateTempMemoryHighInternal( 2 * sizeof( sval_u ) );
	result.node[0].type = type;
	result.node[1] = val1;
	return result;
}

/*
==============
node2
==============
*/
sval_u node2( int type, sval_u val1, sval_u val2 )
{
	sval_u result;

	result.node = (sval_u *)Hunk_AllocateTempMemoryHighInternal( 3 * sizeof( sval_u ) );
	result.node[0].type = type;
	result.node[1] = val1;
	result.node[2] = val2;
	return result;
}

/*
==============
node2_
==============
*/
sval_u node2_( sval_u val1, sval_u val2 )
{
	sval_u result;

	result.node = (sval_u *)Hunk_AllocateTempMemoryHighInternal( 2 * sizeof( sval_u ) );
	result.node[0] = val1;
	result.node[1] = val2;
	return result;
}

/*
==============
node3
==============
*/
sval_u node3( int type, sval_u val1, sval_u val2, sval_u val3 )
{
	sval_u result;

	result.node = (sval_u *)Hunk_AllocateTempMemoryHighInternal( 4 * sizeof( sval_u ) );
	result.node[0].type = type;
	result.node[1] = val1;
	result.node[2] = val2;
	result.node[3] = val3;
	return result;
}

/*
==============
node3_
==============
*/
sval_u node3_( sval_u val1, sval_u val2, sval_u val3 )
{
	sval_u result;

	result.node = (sval_u *)Hunk_AllocateTempMemoryHighInternal( 3 * sizeof( sval_u ) );
	result.node[0] = val1;
	result.node[1] = val2;
	result.node[2] = val3;
	return result;
}

/*
==============
node4
==============
*/
sval_u node4( int type, sval_u val1, sval_u val2, sval_u val3, sval_u val4 )
{
	sval_u result;

	result.node = (sval_u *)Hunk_AllocateTempMemoryHighInternal( 5 * sizeof( sval_u ) );
	result.node[0].type = type;
	result.node[1] = val1;
	result.node[2] = val2;
	result.node[3] = val3;
	result.node[4] = val4;
	return result;
}

/*
==============
node4_
==============
*/
sval_u node4_( sval_u val1, sval_u val2, sval_u val3, sval_u val4 )
{
	sval_u result;

	result.node = (sval_u *)Hunk_AllocateTempMemoryHighInternal( 4 * sizeof( sval_u ) );
	result.node[0] = val1;
	result.node[1] = val2;
	result.node[2] = val3;
	result.node[3] = val4;
	return result;
}

/*
==============
node5
==============
*/
sval_u node5( int type, sval_u val1, sval_u val2, sval_u val3, sval_u val4, sval_u val5 )
{
	sval_u result;

	result.node = (sval_u *)Hunk_AllocateTempMemoryHighInternal( 6 * sizeof( sval_u ) );
	result.node[0].type = type;
	result.node[1] = val1;
	result.node[2] = val2;
	result.node[3] = val3;
	result.node[4] = val4;
	result.node[5] = val5;
	return result;
}

/*
==============
node6
==============
*/
sval_u node6( int type, sval_u val1, sval_u val2, sval_u val3, sval_u val4, sval_u val5, sval_u val6 )
{
	sval_u result;

	result.node = (sval_u *)Hunk_AllocateTempMemoryHighInternal( 7 * sizeof( sval_u ) );
	result.node[0].type = type;
	result.node[1] = val1;
	result.node[2] = val2;
	result.node[3] = val3;
	result.node[4] = val4;
	result.node[5] = val5;
	result.node[6] = val6;
	return result;
}

/*
==============
node7
==============
*/
sval_u node7( int type, sval_u val1, sval_u val2, sval_u val3, sval_u val4, sval_u val5, sval_u val6, sval_u val7 )
{
	sval_u result;

	result.node = (sval_u *)Hunk_AllocateTempMemoryHighInternal( 8 * sizeof( sval_u ) );
	result.node[0].type = type;
	result.node[1] = val1;
	result.node[2] = val2;
	result.node[3] = val3;
	result.node[4] = val4;
	result.node[5] = val5;
	result.node[6] = val6;
	result.node[7] = val7;
	return result;
}

/*
==============
node8
==============
*/
sval_u node8( int type, sval_u val1, sval_u val2, sval_u val3, sval_u val4, sval_u val5, sval_u val6, sval_u val7, sval_u val8 )
{
	sval_u result;

	result.node = (sval_u *)Hunk_AllocateTempMemoryHighInternal( 9 * sizeof( sval_u ) );
	result.node[0].type = type;
	result.node[1] = val1;
	result.node[2] = val2;
	result.node[3] = val3;
	result.node[4] = val4;
	result.node[5] = val5;
	result.node[6] = val6;
	result.node[7] = val7;
	result.node[8] = val8;
	return result;
}

/*
==============
linked_list_end
==============
*/
sval_u linked_list_end( sval_u val1 )
{
	sval_u list;
	sval_u *node;

	node = (sval_u *)Hunk_AllocateTempMemoryHighInternal( 2 * sizeof( sval_u ) );
	node[0] = val1;
	node[1].node = NULL;
	list.node = (sval_u *)Hunk_AllocateTempMemoryHighInternal( 2 * sizeof( sval_u ) );
	list.node[0].node = node;
	list.node[1].node = node;
	return list;
}

/*
==============
prepend_node
==============
*/
sval_u prepend_node( sval_u val1, sval_u list )
{
	sval_u *node;

	node = (sval_u *)Hunk_AllocateTempMemoryHighInternal( 2 * sizeof( sval_u ) );
	node[0] = val1;
	node[1].node = list.node[0].node;
	list.node[0].node = node;
	return list;
}

/*
==============
append_node
==============
*/
sval_u append_node( sval_u list, sval_u val1 )
{
	sval_u *node;

	node = (sval_u *)Hunk_AllocateTempMemoryHighInternal( 2 * sizeof( sval_u ) );
	node[0] = val1;
	node[1].node = NULL;
	list.node[1].node[1].node = node;
	list.node[1].node = node;
	return list;
}

/*
==============
concat_list
==============
*/
sval_u concat_list( sval_u list1, sval_u list2 )
{
	list1.node[1].node[1].node = list2.node[0].node;
	list1.node[1].node = list2.node[1].node;
	return list1;
}
