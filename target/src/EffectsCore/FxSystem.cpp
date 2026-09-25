#include "../qcommon/qcommon.h"

struct GfxEntity;
struct Material;

// Only the leading fields are read here.
struct refdef_t
{
	int x;
	int y;
	int width;
	int height;
	float fov_x;
	float fov_y;
	vec3_t vieworg;
	vec3_t viewaxis[3];
};

struct FxCamera
{
	vec3_t vieworg;
	vec4_t frustum[6];
	int numPlanes;
};

class FxArchive
{
public:
	bool IsReading();
	void ArchiveInt( int *value );
};

struct FxHelper
{
	FxHelper();
	void Init();
	void AdjustTime( int intime );
	void WarpTime( int intime );
	void CalcFrustumPlanes( const refdef_t *refdef, float zfar );
	void AdjustCamera( const refdef_t *refdef, float zfar );
	bool CullSphere( const vec3_t worldPos, float radius, int planeCount );
	bool CullSpherePreviousFrame( const vec3_t worldPos, float radius );
	bool CullCylinder( const vec3_t worldPos0, const vec3_t worldPos1, float radius0, float radius1, int planeCount );
	void Trace( trace_t *tr, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int skipEntNum, int flags );
	void AddFxToScene( GfxEntity *ent, XModel *model );
	void SetIgnorePrecacheErrors( bool ignore );
	const char *GetMaterialName( Material *material );
	void Archive( FxArchive *arch );
	int GetMaterialSubimageCount( Material *material );
	bool IsMaterialRefractive( Material *material );
	void AddLightToScene( const vec3_t org, float radius, float red, float green, float blue );
	void Unused();
	void CameraShake( const vec3_t origin, float intensity, int radius, int time );
	int GetSeed();

	int time;
	int mTime;
	int mOldTime;
	int mFrameTime;
	int mTimeFrozen;
	FxCamera mCamera;
	FxCamera mPrevCamera;
	int mSeed;
	float adsZoomFactor;
};

extern FxHelper *theFxHelper;
extern dvar_t *fx_freeze;

void FX_AddScheduledEffects();
void FX_UpdateAllNonBolt();
void FX_UpdateAllBolt();

FxHelper::FxHelper()
{
	mTime = 0;
	mOldTime = 0;
	mFrameTime = 0;
	mTimeFrozen = 0;
}

void FxHelper::Init()
{
	time = 0;
	mTime = 0;
	mOldTime = 0;
	mFrameTime = 0;
	mTimeFrozen = 0;
	mSeed = 0;
}

void FxHelper::AdjustTime( int intime )
{
	if ( fx_freeze->current.boolean )
	{
		mFrameTime = 0;
	}
	else
	{
		if ( !time )
		{
			mFrameTime = 0;
		}
		else
		{
			mFrameTime = intime - time;
			if ( mFrameTime < 0 )
				mFrameTime = 0;
			else if ( mFrameTime > 200 )
				mFrameTime = 200;
		}
		mOldTime = mTime;
		mTime += mFrameTime;
	}
	time = intime;
	mSeed = time;
	mPrevCamera = mCamera;
}

void FxHelper::WarpTime( int intime )
{
	int frameTime;
	const int maxFrameTime = 200;

	if ( fx_freeze->current.boolean )
	{
		mFrameTime = 0;
	}
	else if ( !time )
	{
		mFrameTime = 0;
	}
	else
	{
		mOldTime = mTime;
		frameTime = intime - mTime;
		if ( frameTime < 0 )
		{
			frameTime = 1;
			mTime = intime;
		}
		else
		{
			mFrameTime = 0;
			while ( frameTime > maxFrameTime )
			{
				mFrameTime = maxFrameTime;
				FX_AddScheduledEffects();
				FX_UpdateAllNonBolt();
				FX_UpdateAllBolt();
				mOldTime = mTime;
				mTime += mFrameTime;
				frameTime -= maxFrameTime;
			}
			mFrameTime = frameTime;
			FX_AddScheduledEffects();
			FX_UpdateAllNonBolt();
			FX_UpdateAllBolt();
			mTime += mFrameTime;
		}
	}
	time = intime;
	mSeed = time;
}

void FxHelper::CalcFrustumPlanes( const refdef_t *refdef, float zfar )
{
	int i;
	float halfFov;
	float s;
	float c;

	VectorCopy(refdef->viewaxis[0], mCamera.frustum[0]);
	halfFov = refdef->fov_x * 0.008726646259971648;
	FastSinCos(halfFov, &s, &c);
	VectorScale(refdef->viewaxis[0], s, mCamera.frustum[1]);
	VectorMA(mCamera.frustum[1], c, refdef->viewaxis[1], mCamera.frustum[1]);
	VectorScale(refdef->viewaxis[0], s, mCamera.frustum[2]);
	VectorMA(mCamera.frustum[2], -c, refdef->viewaxis[1], mCamera.frustum[2]);
	halfFov = refdef->fov_y * 0.008726646259971648;
	FastSinCos(halfFov, &s, &c);
	VectorScale(refdef->viewaxis[0], s, mCamera.frustum[3]);
	VectorMA(mCamera.frustum[3], c, refdef->viewaxis[2], mCamera.frustum[3]);
	VectorScale(refdef->viewaxis[0], s, mCamera.frustum[4]);
	VectorMA(mCamera.frustum[4], -c, refdef->viewaxis[2], mCamera.frustum[4]);
	mCamera.numPlanes = 5;
	if ( zfar > 0.0 )
	{
		VectorNegate(refdef->viewaxis[0], mCamera.frustum[5]);
		mCamera.numPlanes = 6;
	}
	for ( i = 0; i < mCamera.numPlanes; i++ )
		mCamera.frustum[i][3] = DotProduct(mCamera.vieworg, mCamera.frustum[i]);
	if ( zfar > 0.0 )
		mCamera.frustum[5][3] = mCamera.frustum[5][3] - zfar;
}

void FxHelper::AdjustCamera( const refdef_t *refdef, float zfar )
{
	VectorCopy(refdef->vieworg, mCamera.vieworg);
	CalcFrustumPlanes(refdef, zfar);
	if ( refdef->fov_x != 80.0f )
		adsZoomFactor = tan(80.0 * 0.5 * (M_PI / 180.0)) / tan(refdef->fov_x * 0.5f * (M_PI / 180.0));
	else
		adsZoomFactor = 1.0f;
}

bool FxHelper::CullSphere( const vec3_t worldPos, float radius, int planeCount )
{
	int i;
	float dist;

	for ( i = 0; i < planeCount; i++ )
	{
		dist = DotProduct(theFxHelper->mCamera.frustum[i], worldPos) - theFxHelper->mCamera.frustum[i][3];
		if ( -radius > dist )
			return true;
	}
	return false;
}

bool FxHelper::CullSpherePreviousFrame( const vec3_t worldPos, float radius )
{
	int i;
	float dist;

	for ( i = 0; i < theFxHelper->mPrevCamera.numPlanes; i++ )
	{
		dist = DotProduct(theFxHelper->mPrevCamera.frustum[i], worldPos) - theFxHelper->mPrevCamera.frustum[i][3];
		if ( -radius > dist )
			return true;
	}
	return false;
}

bool FxHelper::CullCylinder( const vec3_t worldPos0, const vec3_t worldPos1, float radius0, float radius1, int planeCount )
{
	int i;
	float dist;

	for ( i = 0; i < planeCount; i++ )
	{
		dist = DotProduct(theFxHelper->mCamera.frustum[i], worldPos0) - theFxHelper->mCamera.frustum[i][3];
		if ( dist > -radius0 )
			continue;
		dist = DotProduct(theFxHelper->mCamera.frustum[i], worldPos1) - theFxHelper->mCamera.frustum[i][3];
		if ( dist > -radius1 )
			continue;
		return true;
	}
	return false;
}

void FxHelper::Trace( trace_t *tr, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int skipEntNum, int flags )
{
	CM_BoxTrace(tr, start, end, mins, maxs, 0, flags);
	tr->entityNum = tr->fraction != 1.0f ? ENTITYNUM_NONE - 1 : ENTITYNUM_NONE;
}

// Effects are not drawn on the dedicated server.
void FxHelper::AddFxToScene( GfxEntity *ent, XModel *model )
{
}

void FxHelper::SetIgnorePrecacheErrors( bool ignore )
{
}

const char *FxHelper::GetMaterialName( Material *material )
{
	return NULL;
}

void FxHelper::Archive( FxArchive *arch )
{
	arch->ArchiveInt(&mTime);
	arch->ArchiveInt(&mOldTime);
	arch->ArchiveInt(&mFrameTime);
	arch->ArchiveInt(&mTimeFrozen);
	if ( arch->IsReading() )
		time = 0;
}

int FxHelper::GetMaterialSubimageCount( Material *material )
{
	return 0;
}

bool FxHelper::IsMaterialRefractive( Material *material )
{
	return false;
}

void FxHelper::AddLightToScene( const vec3_t org, float radius, float red, float green, float blue )
{
}

// Empty and never called; original name unknown.
void FxHelper::Unused()
{
}

void FxHelper::CameraShake( const vec3_t origin, float intensity, int radius, int time )
{
}

int FxHelper::GetSeed()
{
	int seed;

	seed = mSeed;
	mSeed *= 0x369D035;
	return seed;
}
