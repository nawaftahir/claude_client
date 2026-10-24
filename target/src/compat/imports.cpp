// Engine functions the client passes to the renderer by pointer
#include "../qcommon/qcommon.h"
#include "../qcommon/cm_public.h"
#include "../qcommon/cmd.h"
#include "../universal/com_memory.h"
#include "../universal/com_files.h"
#include "../universal/dvar.h"
#include "../xanim/xanim_public.h"
#include "../stringed/stringed_public.h"
void CM_SaveLump( int lumpType, const void *data, int len, int *checksum );
qboolean Com_LoadDvarsFromBuffer(const char **dvarnames, int numDvars, const char *buffer, const char *filename);
qboolean Com_SaveDvarsToBuffer(const char **dvarnames, int numDvars, char *buffer, int bufsize);
int DObjBad( const DObj *obj );
void DObjCompleteHierarchyBits(DObj_s *obj, int *partBits);
void DObjGetBoneInfo( const DObj *obj, XBoneInfo **boneInfo );
int DObjGetMatOffset( const DObj_s *obj, int modelIndex );
int DObjGetNumModels( const DObj_s *obj );
void Dvar_ChangeResetValue(dvar_t *dvar, DvarValue value);
void Dvar_SetFromString(dvar_t *dvar, const char *string);
void Dvar_SetModified(dvar_t *dvar);
void Dvar_SetVec4ByName(const char *dvarName, float x, float y, float z, float w);
void Dvar_UpdateEnumDomain(const dvar_t *dvar, const char **stringTable);
unsigned int SEH_ReadCharFromString( const char **text, int *isTrailingPunctuation );
int XModelGetMemUsage(const XModel *model);
const trXSkin_t *XModelGetSkins(const XModel *model);
const char *XModelGetSurfaceName(const XModel *model, int surfIndex, int lod);
const char *Dvar_EnumToString(const dvar_t *dvar);
int DObjGetSurfaceList( const DObj *obj, DObjSurfRef *list, unsigned int *surfPartBits, const char *lods );
float DObjGetMaxLodOutDist(const DObj_s *obj);
void *DObjGetModelSurface( const DObj *obj, int modelIndex, int surfIndex, int lod );
int DObjGetModelLodForDist( const DObj_s *obj, int modelIndex, float dist );
void XModelSetLodOverride( int lod, float dist );
const char *XModelGetLodName( const XModel *model, int lod );
int DObjGetSurfaceCount( const DObj *obj, const char *lods );
const char *DObjGetModelSurfaceName( const DObj *obj, int modelIndex, int surfIndex, int lod );
void DObjSetModel(DObj_s *obj, XModel *model);
void Sys_DirectXFatalError( void );
void Dvar_ClearFlagsFromAll(int flags);
char *XModelGetBasePoseOffset(const XModel *model, unsigned int offset);
void DB_EnumXAssets(XAssetType type, void (*func)(XAssetHeader, void *), void *inData, bool includeOverride);
void FX_UpdateAndDraw();
/* Dead pointers (keep the callees address-taken); parked at the end of .data */
