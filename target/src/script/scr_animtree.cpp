#include "../qcommon/qcommon.h"
#include "script_public.h"

static scrAnimGlob_t scrAnimGlob;
scrAnimPub_t scrAnimPub;

void SetAnimCheck( int bAnimCheck );
void AnimTreeCompileError( const char *msg );
int GetAnimTreeParseProperties();
void Scr_EmitAnimationInternal( char *pos, unsigned int animName, unsigned int names, unsigned int sourcePos );
void Scr_EmitAnimation( char *pos, unsigned int animName, unsigned int sourcePos );
bool AnimTreeParseInternal( unsigned int parentNode, unsigned int names, bool bIncludeParent, bool bLoop, bool bComplete );
void Scr_AnimTreeParse( const char *pos, unsigned int parentNode, unsigned int names );
void *Hunk_AllocXAnimTreePrecache( int size );
int Scr_GetAnimTreeSize( unsigned int parentNode );
void ConnectScriptToAnim( unsigned int names, int index, unsigned int filename, unsigned int name, int treeIndex );
int Scr_GetAnimsIndex( const XAnim *anims );
XAnim *Scr_GetAnims( int index );
void Scr_CheckAnimsDefined( unsigned int names, unsigned int filename );
void Scr_PrecacheAnimationTree( unsigned int parentNode );
unsigned int Scr_UsingTreeInternal( const char *filename, unsigned int *index, int user );
void Scr_UsingTree( const char *filename, unsigned int sourcePos );
bool Scr_LoadAnimTreeInternal( const char *filename, unsigned int parentNode, unsigned int names );
void Scr_LoadAnimTreeAtIndex( int index, void *(*Alloc)(int), int user );
scr_animtree_t Scr_FindAnimTree( const char *filename );
void Scr_FindAnim( const char *filename, const char *animName, scr_anim_s *anim, int user );

static const char *propertyNames[] =
{
	"loopsync",
	"nonloopsync",
	"complete"
};

/*
============
Scr_CreateAnimationTree
============
*/
/*
============
SetAnimCheck
============
*/
void SetAnimCheck( int bAnimCheck )
{
	scrAnimGlob.bAnimCheck = bAnimCheck;
}

/*
============
AnimTreeCompileError
============
*/
void AnimTreeCompileError( const char *msg )
{
	const char *pos = Com_GetLastTokenPos();

	{
#define PP_RETURN goto pp_done_2
#include "../universal/q_parse_endsession.inl"
#undef PP_RETURN
	}
pp_done_2:;
	CompileError(pos - scrAnimGlob.start, "%s", msg);
}

/*
============
GetAnimTreeParseProperties
============
*/
int GetAnimTreeParseProperties()
{
	const char *token;
	int i;
	int flags = 0;

	while ( 1 )
	{
		token = Com_ParseOnLine(&scrAnimGlob.pos);

		if ( !token[0] )
		{
			break;
		}

		for ( i = 0; i < ARRAY_COUNT(propertyNames); i++ )
		{
			if ( !strcasecmp(token, propertyNames[i]) )
			{
				break;
			}
		}

		switch ( i )
		{
		case 0:
			flags |= ANIM_FLAG_LOOPSYNC;
			break;

		case 1:
			flags |= ANIM_FLAG_NONLOOPSYNC;
			break;

		case 2:
			flags |= ANIM_FLAG_COMPLETE;
			break;

		default:
			AnimTreeCompileError("unknown anim property");
			break;
		}
	}

	return flags;
}

/*
============
Scr_EmitAnimationInternal
============
*/
void Scr_EmitAnimationInternal( char *pos, unsigned int animName, unsigned int names, unsigned int sourcePos )
{
	VariableValue tempValue;
	VariableUnion *value;
	unsigned int animId;

	animId = FindVariable(names, animName);

	if ( !animId )
	{
		animId = GetNewVariable(names, animName);
		*(const char **)pos = NULL;
		tempValue.type = VAR_CODEPOS;
		tempValue.u.codePosValue = pos;
		SetVariableValue(animId, &tempValue);
	}
	else
	{
		value = GetVariableValueAddress(animId);
		*(const char **)pos = value->codePosValue;
		value->codePosValue = pos;
	}
}

/*
============
Scr_EmitAnimation
============
*/
void Scr_EmitAnimation( char *pos, unsigned int animName, unsigned int sourcePos )
{
	if ( !scrAnimPub.animTreeNames )
	{
		CompileError(sourcePos, "#using_animtree was not specified");
		return;
	}

	Scr_EmitAnimationInternal(pos, animName, scrAnimPub.animTreeNames, sourcePos);
}

extern scrVarGlob_t scrVarGlob;
extern scrMemTreeGlob_t scrMemTreeGlob;
void MT_FreeIndex( unsigned int nodeNum, int numBytes );
unsigned int FindVariableIndexHash( unsigned int parentId, unsigned int name );
unsigned int GetNewVariableIndexInternal3( unsigned int parentId, unsigned int name, unsigned int index );

// retail inlines these at the first AnimTreeParseInternal sites only
static __forceinline bool Scr_IsIdentifierInl( const char *token )
{
	while ( token[0] )
	{
		if ( !isalnum(token[0]) && token[0] != '_' )
		{
			return false;
		}

		token++;
	}

	return true;
}

#define GET_VARIABLE_INLINE(res, a_parentId, a_name) \
	{ \
			unsigned int parentId_GV = (a_parentId); \
			unsigned int name_GV = (a_name); \
		VariableValueInternal *entry_GV; \
		VariableValueInternal *siblingEntry_GV; \
		unsigned short siblingId_GV; \
		VariableValueInternal *siblingValue_GV; \
		VariableValueInternal *parentValue_GV; \
		unsigned int hash_GV; \
		unsigned int index_GV; \
		hash_GV = ( name_GV + parentId_GV ) % ( VARIABLELIST_CHILD_SIZE - 1 ) + 1; \
		index_GV = FindVariableIndexInternal(name_GV, hash_GV); \
		if ( !index_GV ) \
		{ \
			index_GV = GetNewVariableIndexInternal3(parentId_GV, name_GV, hash_GV); \
			parentValue_GV = &scrVarGlob.variableList[parentId_GV]; \
			siblingId_GV = parentValue_GV->nextSibling; \
			siblingValue_GV = &scrVarGlob.variableList[siblingId_GV]; \
			entry_GV = &scrVarGlob.variableList[index_GV]; \
			siblingEntry_GV = &scrVarGlob.variableList[entry_GV->hash.id]; \
			siblingEntry_GV->nextSibling = siblingId_GV; \
			siblingValue_GV->hash.u.prevSibling = index_GV; \
			entry_GV->hash.u.prevSibling = parentValue_GV->v.next; \
			parentValue_GV->nextSibling = index_GV; \
		} \
		res = scrVarGlob.variableList[index_GV].hash.id; \
	}

#define REMOVE_REF_TO_VALUE_INLINE(a_type, a_u) \
	{ \
			int type_RRTV = (a_type); \
			VariableUnion u_RRTV = (a_u); \
		switch ( type_RRTV ) \
		{ \
		case VAR_POINTER: \
			RemoveRefToObject(u_RRTV.pointerValue); \
			break; \
		case VAR_STRING: \
		case VAR_ISTRING: \
			SL_RemoveRefToString(u_RRTV.stringValue); \
			break; \
		case VAR_VECTOR: \
			if ( *((byte *)u_RRTV.vectorValue - 1) ) \
			{ \
				break; \
			} \
			if ( *((unsigned short *)((byte *)u_RRTV.vectorValue - 4)) ) \
			{ \
				(*((unsigned short *)((byte *)u_RRTV.vectorValue - 4)))--; \
				break; \
			} \
			MT_FreeIndex( (MemoryNode *)((byte *)u_RRTV.vectorValue - 4) - scrMemTreeGlob.nodes, 16 ); \
			break; \
		} \
	}

#define FREE_VALUE_INLINE(a_id) \
	{ \
			unsigned int id_FV = (a_id); \
		VariableValueInternal *entryValue_FV = &scrVarGlob.variableList[id_FV]; \
		REMOVE_REF_TO_VALUE_INLINE(entryValue_FV->w.type & VAR_MASK, entryValue_FV->u.u); \
		FreeVariable(id_FV); \
	}

#define REMOVE_VARIABLE_INLINE(a_parentId, a_name) \
	{ \
			unsigned int parentId_RV = (a_parentId); \
			unsigned int name_RV = (a_name); \
		VariableValueInternal *entryValue_RV; \
		unsigned int index_RV; \
		unsigned int id_RV; \
		index_RV = FindVariableIndexHash(parentId_RV, name_RV); \
		entryValue_RV = &scrVarGlob.variableList[index_RV]; \
		id_RV = entryValue_RV->hash.id; \
		MakeVariableExternal(entryValue_RV, &scrVarGlob.variableList[parentId_RV]); \
		FREE_VALUE_INLINE(id_RV); \
	}

/*
============
AnimTreeParseInternal
============
*/
bool AnimTreeParseInternal( unsigned int parentNode, unsigned int names, bool bIncludeParent, bool bLoop, bool bComplete )
{
	const char *token;
	unsigned int animId;
	unsigned int animName;
	VariableValue tempValue;
	unsigned int childNode;
	int flags;
	bool bResolve;
	bool result;

	tempValue.type = VAR_INTEGER;
	animName = 0;
	animId = 0;
	flags = 0;
	bResolve = false;

	while ( 1 )
	{
		token = Com_Parse(&scrAnimGlob.pos);

		if ( !scrAnimGlob.pos )
		{
			result = true;
			break;
		}

		if ( Scr_IsIdentifierInl(token) )
		{
			if ( bResolve )
			{
				REMOVE_VARIABLE_INLINE(parentNode, animName);
			}

			animName = SL_GetLowercaseString_(token, 2, 4);

			if ( FindVariable(parentNode, animName) )
			{
				AnimTreeCompileError("duplicate animation");
			}

			unsigned int inl_gv1;
			GET_VARIABLE_INLINE(inl_gv1, parentNode, animName);
			animId = inl_gv1;
			bResolve = !bComplete && !FindVariable(names, animName) && !scrAnimGlob.bAnimCheck;
			flags = 0;
			token = Com_ParseOnLine(&scrAnimGlob.pos);

			if ( !token[0] )
			{
				continue;
			}

			if ( Scr_IsIdentifierInl(token) )
			{
				AnimTreeCompileError("FIXME: aliases not yet implemented");
			}

			if ( token[0] != ':' || token[1] )
			{
				AnimTreeCompileError("bad token");
			}

			flags = GetAnimTreeParseProperties();
			token = Com_Parse(&scrAnimGlob.pos);

			if ( token[0] != '{' || token[1] )
			{
				AnimTreeCompileError("properties cannot be applied to primitive animations");
			}
		}

		if ( token[0] == '{' )
		{
			if ( token[1] )
			{
				AnimTreeCompileError("bad token");
			}

			if ( Com_ParseOnLine(&scrAnimGlob.pos)[0] )
			{
				AnimTreeCompileError("token not allowed after '{'");
			}

			if ( !animId )
			{
				AnimTreeCompileError("no animation specified for this block");
			}

			childNode = GetArray(animId);
			if ( AnimTreeParseInternal(childNode, names, !bResolve, flags & ANIM_FLAG_LOOPSYNC, bComplete || ( ( flags & ANIM_FLAG_COMPLETE ) && !bResolve )) )
			{
				AnimTreeCompileError("unexpected end of file");
			}

			if ( GetArraySize(childNode) )
			{
				tempValue.u.intValue = flags;
				SetVariableValue(GetArrayVariable(childNode, 0), &tempValue);
			}
			else
			{
				RemoveVariable(parentNode, animName);
			}

			animId = 0;
			bResolve = false;
			continue;
		}

		if ( token[0] == '}' )
		{
			if ( token[1] )
			{
				AnimTreeCompileError("bad token");
			}

			if ( Com_ParseOnLine(&scrAnimGlob.pos)[0] )
			{
				AnimTreeCompileError("token not allowed after '}'");
			}

			result = false;
			break;
		}

		AnimTreeCompileError("bad token");
	}

	if ( bResolve )
	{
		RemoveVariable(parentNode, animName);
	}

	if ( bIncludeParent && !GetArraySize(parentNode) )
	{
		animName = SL_GetString_(bLoop ? "void_loop" : "void", 0, 4);
		GetVariable(parentNode, animName);
		SL_RemoveRefToString(animName);
	}

	return result;
}

/*
============
Scr_AnimTreeParse
============
*/
void Scr_AnimTreeParse( const char *pos, unsigned int parentNode, unsigned int names )
{
	Com_BeginParseSession("Scr_AnimTreeParse");

	scrAnimGlob.start = scrAnimGlob.pos = pos;

	if ( !AnimTreeParseInternal(parentNode, names, true, false, false) )
	{
		AnimTreeCompileError("bad token");
	}

	{
#define PP_RETURN goto pp_done_3
#include "../universal/q_parse_endsession.inl"
#undef PP_RETURN
	}
pp_done_3:;
}

/*
============
Hunk_AllocXAnimTreePrecache
============
*/
void *Hunk_AllocXAnimTreePrecache( int size )
{
	return Hunk_AllocAlignInternal(size, 4);
}

/*
============
Scr_GetAnimTreeSize
============
*/
int Scr_GetAnimTreeSize( unsigned int parentNode )
{
	unsigned int node, name;
	int size = 0;

	for ( node = FindNextSibling(parentNode); node; node = FindNextSibling(node) )
	{
		name = GetVariableName(node);

		if ( name >= SL_MAX_STRING_INDEX )
		{
			continue;
		}

		if ( GetObjectType(node) == VAR_POINTER )
		{
			size += Scr_GetAnimTreeSize(FindObject(node));
			continue;
		}

		size++;
	}

	if ( size )
	{
		size++;
	}

	return size;
}

/*
============
ConnectScriptToAnim
============
*/
void ConnectScriptToAnim( unsigned int names, int index, unsigned int filename, unsigned int name, int treeIndex )
{
	unsigned int animId;
	VariableUnion *value;
	const char *codePos;
	const char *nextCodePos;
	scr_anim_s anim;

	animId = FindVariable(names, name);

	if ( !animId )
	{
		return;
	}

	value = GetVariableValueAddress(animId);

	if ( !value->codePosValue )
	{
		Com_Error(ERR_DROP, "\x15" "duplicate animation '%s' in 'animtrees/%s.atr'", SL_ConvertToString(name), SL_ConvertToString(filename));
	}

	anim.index = index;
	anim.tree = treeIndex;

	for ( codePos = value->codePosValue; codePos; codePos = nextCodePos )
	{
		nextCodePos = *(const char **)codePos;
		*(const char **)codePos = anim.linkPointer;
	}

	value->codePosValue = NULL;
}

/*
============
Scr_GetAnimsIndex
============
*/
int Scr_GetAnimsIndex( const XAnim *anims )
{
	int i;
	scr_animtree_t *xanim_lookup;

	xanim_lookup = scrAnimPub.xanim_lookup[SCR_XANIM_SERVER];

	for ( i = scrAnimPub.xanim_num[SCR_XANIM_SERVER]; i; i-- )
	{
		if ( xanim_lookup[i].anims == anims )
		{
			break;
		}
	}

	return i;
}

/*
============
Scr_GetAnims
============
*/
XAnim *Scr_GetAnims( int index )
{
	return scrAnimPub.xanim_lookup[SCR_XANIM_SERVER][index].anims;
}

int Scr_CreateAnimationTree( unsigned int parentNode, unsigned int names, XAnim *anims,
                             unsigned int childIndex, const char *parentName, unsigned int parentIndex,
                             unsigned int filename, int treeIndex )
{
	register const char *childName;
	unsigned int nodeRef;
	unsigned int name;
	unsigned int size;
	unsigned int index;
	unsigned short flags;
	unsigned short nodeFlags;
	size = 0;
	for ( nodeRef = FindNextSibling(parentNode); nodeRef; nodeRef = FindNextSibling(nodeRef) )
	{
		name = GetVariableName(nodeRef);
		if ( name >= SL_MAX_STRING_INDEX )
		{
			continue;
		}
		size++;
	}
	index = FindArrayVariable(parentNode, 0);
	if ( index )
	{
		nodeFlags = GetVariableValueAddress(index)->intValue;
	}
	else
	{
		nodeFlags = 0;
	}
	flags = nodeFlags;
	scrVarPub.checksum *= 31;
	scrVarPub.checksum += parentIndex;
	scrVarPub.checksum *= 31;
	scrVarPub.checksum += childIndex;
	scrVarPub.checksum *= 31;
	scrVarPub.checksum += size;
	scrVarPub.checksum *= 31;
	scrVarPub.checksum += flags;
	XAnimBlend(anims, parentIndex, parentName, childIndex, size, flags);
	parentIndex = childIndex;
	childIndex = size + childIndex;
	for ( nodeRef = FindNextSibling(parentNode); nodeRef; nodeRef = FindNextSibling(nodeRef) )
	{
		name = GetVariableName(nodeRef);
		if ( name >= SL_MAX_STRING_INDEX )
		{
			continue;
		}
		ConnectScriptToAnim(names, parentIndex, filename, (unsigned short)name, treeIndex);
		if ( GetObjectType(nodeRef) == VAR_POINTER )
		{
			childName = SL_ConvertToString(name);
			childIndex = Scr_CreateAnimationTree(FindObject(nodeRef), names, anims, childIndex, childName, parentIndex, filename, treeIndex);
		}
		else
		{
			scrVarPub.checksum *= 31;
			scrVarPub.checksum += parentIndex;
			XAnimCreate(anims, parentIndex, SL_ConvertToString(name));
		}
		parentIndex++;
	}
	return childIndex;
}

/*
============
Scr_CheckAnimsDefined
============
*/
void Scr_CheckAnimsDefined( unsigned int names, unsigned int filename )
{
	unsigned int name;
	unsigned int animId;
	VariableUnion *value;
	const char *msg;

	for ( animId = FindNextSibling(names); animId; animId = FindNextSibling(animId) )
	{
		name = GetVariableName(animId);
		value = GetVariableValueAddress(animId);

		if ( !value->codePosValue )
		{
			continue;
		}

		msg = va("animation '%s' not defined in anim tree '%s'", SL_ConvertToString(name), SL_ConvertToString(filename));

		if ( Scr_IsInOpcodeMemory(value->codePosValue) )
		{
			CompileError2(value->codePosValue, "%s", msg);
		}
		else
		{
			Com_Error(ERR_DROP, "\x15%s", msg);
		}
	}
}

/*
============
Scr_PrecacheAnimationTree
============
*/
void Scr_PrecacheAnimationTree( unsigned int parentNode )
{
	unsigned int node, name;

	for ( node = FindNextSibling(parentNode); node; node = FindNextSibling(node) )
	{
		name = GetVariableName(node);

		if ( name >= SL_MAX_STRING_INDEX )
		{
			continue;
		}

		if ( GetObjectType(node) == VAR_POINTER )
		{
			Scr_PrecacheAnimationTree(FindObject(node));
			continue;
		}

		XAnimPrecache(SL_ConvertToString(name), Hunk_AllocXAnimTreePrecache);
	}
}

/*
============
Scr_UsingTreeInternal
============
*/
unsigned int Scr_UsingTreeInternal( const char *filename, unsigned int *index, int user )
{
	unsigned int filenameId;
	unsigned int names;
	unsigned int id;
	unsigned int fileId;
	int i;
	unsigned short *using_xanim_lookup;

	filenameId = Scr_CreateCanonicalFilename(filename);
	id = FindVariable(scrAnimPub.animtrees, filenameId);

	if ( !id )
	{
		id = GetNewVariable(scrAnimPub.animtrees, filenameId);
		fileId = GetObjectA(id);
		++scrAnimPub.xanim_num[user];
		scrAnimGlob.using_xanim_lookup[user][scrAnimPub.xanim_num[user]] = id;
		*index = scrAnimPub.xanim_num[user];
	}
	else
	{
		fileId = FindObject(id);
		*index = 0;
		using_xanim_lookup = scrAnimGlob.using_xanim_lookup[user];

		for ( i = 1; i <= (int)scrAnimPub.xanim_num[user]; ++i )
		{
			if ( using_xanim_lookup[i] == id )
			{
				*index = i;
				break;
			}
		}
	}

	names = GetArray(GetVariable(fileId, ANIMTREE_NAMES));
	SL_RemoveRefToString(filenameId);

	return names;
}

/*
============
Scr_UsingTree
============
*/
void Scr_UsingTree( const char *filename, unsigned int sourcePos )
{
	if ( !Scr_IsIdentifier(filename) )
	{
		CompileError(sourcePos, "bad anim tree name");
		return;
	}

	scrAnimPub.animTreeNames = Scr_UsingTreeInternal(filename, &scrAnimPub.animTreeIndex, SCR_XANIM_SERVER);
}

/*
============
Scr_LoadAnimTreeInternal
============
*/
bool Scr_LoadAnimTreeInternal( const char *filename, unsigned int parentNode, unsigned int names )
{
	char extFilename[MAX_QPATH];
	const char *sourceBuffer;
	const char *oldFilename;
	const char *oldSourceBuf;

	sprintf(extFilename, "animtrees/%s.atr", filename);
	oldSourceBuf = scrParserPub.sourceBuf;
	sourceBuffer = Scr_AddSourceBuffer(NULL, extFilename, NULL, true);

	if ( !sourceBuffer )
	{
		return false;
	}

	oldFilename = scrParserPub.scriptfilename;
	scrParserPub.scriptfilename = extFilename;
	Scr_AnimTreeParse(sourceBuffer, parentNode, names);
	scrParserPub.scriptfilename = oldFilename;
	scrParserPub.sourceBuf = oldSourceBuf;
	Hunk_ClearTempMemoryHighInternal();

	return GetArraySize(parentNode) != 0;
}

/*
============
Scr_LoadAnimTreeAtIndex
============
*/
void Scr_LoadAnimTreeAtIndex( int index, void *(*Alloc)(int), int user )
{
	VariableValue tempValue;
	unsigned int names;
	int size;
	int size2;
	unsigned int filenameId;
	scr_animtree_t animtree;
	unsigned int name;
	unsigned int fileId;
	unsigned int id;

	id = scrAnimGlob.using_xanim_lookup[user][index];
	filenameId = (unsigned short)GetVariableName(id);
	fileId = FindObject(id);

	if ( FindVariable(fileId, ANIMTREE_XANIM) )
	{
		return;
	}

	animtree.anims = NULL;
	names = FindVariable(fileId, ANIMTREE_NAMES);

	if ( !names )
	{
		scrAnimPub.xanim_lookup[user][index] = animtree;
		return;
	}

	names = FindObject(names);
	scrAnimPub.animtree_node = Scr_AllocArray();

	if ( !Scr_LoadAnimTreeInternal(SL_ConvertToString(filenameId), scrAnimPub.animtree_node, names) )
	{
		Com_Error(ERR_DROP, va("unknown anim tree '%s'", SL_ConvertToString(filenameId)));
	}

	size = Scr_GetAnimTreeSize(scrAnimPub.animtree_node);
	animtree.anims = XAnimCreateAnims(SL_ConvertToString(filenameId), size, Alloc);

	name = SL_GetString_("root", 0, 4);
	ConnectScriptToAnim(names, 0, filenameId, name, index);
	SL_RemoveRefToString(name);

	Scr_PrecacheAnimationTree(scrAnimPub.animtree_node);
	size2 = Scr_CreateAnimationTree(scrAnimPub.animtree_node, names, animtree.anims, 1, "root", 0, filenameId, index);
	Scr_CheckAnimsDefined(names, filenameId);

	RemoveVariable(fileId, ANIMTREE_NAMES);
	RemoveRefToObject(scrAnimPub.animtree_node);
	scrAnimPub.animtree_node = 0;

	tempValue.type = VAR_CODEPOS;
	tempValue.u.codePosValue = (const char *)animtree.anims;
	SetVariableValue(GetVariable(fileId, ANIMTREE_XANIM), &tempValue);

	XAnimSetupSyncNodes(animtree.anims);
	scrAnimPub.xanim_lookup[user][index] = animtree;
}

/*
============
Scr_FindAnimTree
============
*/
scr_animtree_t Scr_FindAnimTree( const char *filename )
{
	unsigned int fileId, filenameId, xanimId;
	VariableValue tempValue;
	scr_animtree_t tree;

	filenameId = Scr_CreateCanonicalFilename(filename);
	fileId = FindVariable(scrAnimPub.animtrees, filenameId);

	SL_RemoveRefToString(filenameId);

	tree.anims = NULL;

	if ( !fileId )
	{
		return tree;
	}

	filenameId = (unsigned short)GetVariableName(fileId);
	fileId = FindObject(fileId);
	xanimId = FindVariable(fileId, ANIMTREE_XANIM);

	if ( !xanimId )
	{
		return tree;
	}

	tempValue = Scr_EvalVariable(xanimId);
	tree.anims = (XAnim *)tempValue.u.codePosValue;

	return tree;
}

/*
============
Scr_FindAnim
============
*/
void Scr_FindAnim( const char *filename, const char *animName, scr_anim_s *anim, int user )
{
	unsigned int name;
	unsigned int index;

	name = SL_GetLowercaseString_(animName, 0, 4);
	Scr_EmitAnimationInternal((char *)anim, name, Scr_UsingTreeInternal(filename, &index, user), 0);
	SL_RemoveRefToString(name);
}

