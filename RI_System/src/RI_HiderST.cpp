//==================================================================
/// RI_HiderST.cpp
///
/// Created by Davide Pasca - 2009/02/08
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_Primitive.h"
#include "RI_HiderST.h"
#include "RI_Transform.h"

static const u_int	BUCKET_SIZE = 128;

//==================================================================
#define MAX_SAMP_DATA_PER_PIXEL	1024

//==================================================================
namespace RI
{

//==================================================================
static void MakeCube( const Bound &b, Vec3f out_box[8] )
{
	out_box[0] = Vec3f( b.mBox[0][0], b.mBox[0][1], b.mBox[0][2] );
	out_box[1] = Vec3f( b.mBox[1][0], b.mBox[0][1], b.mBox[0][2] );
	out_box[2] = Vec3f( b.mBox[0][0], b.mBox[1][1], b.mBox[0][2] );
	out_box[3] = Vec3f( b.mBox[1][0], b.mBox[1][1], b.mBox[0][2] );
	out_box[4] = Vec3f( b.mBox[0][0], b.mBox[0][1], b.mBox[1][2] );
	out_box[5] = Vec3f( b.mBox[1][0], b.mBox[0][1], b.mBox[1][2] );
	out_box[6] = Vec3f( b.mBox[0][0], b.mBox[1][1], b.mBox[1][2] );
	out_box[7] = Vec3f( b.mBox[1][0], b.mBox[1][1], b.mBox[1][2] );
}

//==================================================================
/// Hider
//==================================================================
Hider::Hider( const Params &params ) :
	mParams(params),
	mpGlobalSyms(NULL)
{
}

//==================================================================
Hider::~Hider()
{
	for (size_t i=0; i < mpBuckets.size(); ++i)
		DDELETE( mpBuckets[i] );
}

//==================================================================
void Hider::WorldBegin(
					const Options &opt,
					const Matrix44 &mtxWorldCamera )
{
	mOptions = opt;

	mMtxWorldCamera	= mtxWorldCamera;
	mMtxWorldProj	= mMtxWorldCamera * opt.mMtxCamProj;
	
	mFinalBuff.Setup( opt.mXRes, opt.mYRes );
	mFinalBuff.Clear();
	
#if 0
	// DNEW instead ?
	mpBuckets.push_back(
			new Bucket( 0, 0, opt.mXRes, opt.mYRes ) );
#else
	for (int y=0; y < opt.mYRes; y += BUCKET_SIZE)
	{
		int	y2 = y + BUCKET_SIZE;
		y2 = DMIN( y2, opt.mYRes );

		for (int x=0; x < opt.mXRes; x += BUCKET_SIZE)
		{
			int	x2 = x + BUCKET_SIZE;
			x2 = DMIN( x2, opt.mXRes );

			if ( mParams.mDbgOnlyBucketAtX == -1 ||
				 mParams.mDbgOnlyBucketAtX >= x &&
				 mParams.mDbgOnlyBucketAtY >= y &&
				 mParams.mDbgOnlyBucketAtX < x2 &&
				 mParams.mDbgOnlyBucketAtY < y2 )
			{
				mpBuckets.push_back(
						DNEW HiderBucket( x, y, x2, y2 ) );
			}
		}
	}
#endif
}

//==================================================================
void Hider::Insert( PrimitiveBase *pPrim )
{
	mpPrims.push_back( pPrim->Borrow() );
}

//==================================================================
void Hider::InsertSimple(	
					SimplePrimitiveBase		*pSimplePrim,
					ComplexPrimitiveBase	&srcPrim
					)
{
	pSimplePrim->CopyStates( srcPrim );

	mpPrims.push_back( pSimplePrim->Borrow() );
}

//==================================================================
void Hider::InsertSplitted(	
				SimplePrimitiveBase	*pDesPrim,
				SimplePrimitiveBase	&srcPrim
				)
{
	pDesPrim->CopyStates( srcPrim );

	pDesPrim->mSplitCnt += 1;

	mpPrims.push_back( pDesPrim->Borrow() );
}

//==================================================================
void Hider::InsertForDicing( SimplePrimitiveBase *pPrim )
{
	Bound	bound;
	if NOT( pPrim->MakeBound( bound ) )
	{
		DASSERT( 0 );
		return;
	}

	const Matrix44 &mtxLocalWorld = pPrim->mpTransform->GetMatrix();

	float bound2df[4];
	if NOT( makeRasterBound( bound, mtxLocalWorld, bound2df ) )
	{
		return;
	}

	int minX = (int)floorf( bound2df[0] );
	int minY = (int)floorf( bound2df[1] );
	int maxX =  (int)ceilf( bound2df[2] );
	int maxY =  (int)ceilf( bound2df[3] );

	for (size_t i=0; i < mpBuckets.size(); ++i)
	{
		if ( mpBuckets[i]->Intersects( minX, minY, maxX, maxY ) )
		{
			mpBuckets[i]->mpPrims.push_back( (SimplePrimitiveBase *)pPrim->Borrow() );
		}
	}

}

//==================================================================
void Hider::WorldEnd()
{
	if ( mParams.mDbgShowBuckets )
	{
		float borderCol[] = { 1, 0, 0 };
		for (size_t bi=0; bi < mpBuckets.size(); ++bi)
		{
			const HiderBucket	*pBucket = mpBuckets[bi];

			mFinalBuff.DrawHLine( pBucket->mX1, pBucket->mY1, pBucket->mX2, borderCol );
			mFinalBuff.DrawHLine( pBucket->mX1, pBucket->mY2, pBucket->mX2, borderCol );

			mFinalBuff.DrawVLine( pBucket->mX1, pBucket->mY1, pBucket->mY2, borderCol );
			mFinalBuff.DrawVLine( pBucket->mX2, pBucket->mY1, pBucket->mY2, borderCol );
		}
	}

	for (size_t i=0; i < mpBuckets.size(); ++i)
		DDELETE( mpBuckets[i] );

	mpBuckets.clear();
}

//==================================================================
bool Hider::makeRasterBound(
						const Bound &b,
						const Matrix44 &mtxLocalWorld,
						float out_bound2d[4] ) const
{
	Vec3f	boxVerts[8];
	MakeCube( b, boxVerts );

	float destHalfWd	= (float)mFinalBuff.mWd * 0.5f;
	float destHalfHe	= (float)mFinalBuff.mHe * 0.5f;

	float minX =  FLT_MAX;
	float minY =  FLT_MAX;
	float maxX = -FLT_MAX;
	float maxY = -FLT_MAX;

	Matrix44	mtxLocalProj = mtxLocalWorld * mMtxWorldProj;

	static const U32	CCODE_X1 = 1;
	static const U32	CCODE_X2 = 2;
	static const U32	CCODE_Y1 = 4;
	static const U32	CCODE_Y2 = 8;
	static const U32	CCODE_Z1 = 16;
	static const U32	CCODE_Z2 = 32;

	U32 orCode = 0;
	U32 andCode = 0xff;

	for (size_t i=0; i < 8; ++i)
	{
		Vec4f	Pproj = V4__V3W1_Mul_M44<float>( boxVerts[i], mtxLocalProj );

		float	x = Pproj.x();
		float	y = Pproj.y();
		float	z = Pproj.z();
		float	w = Pproj.w();
		
		U32 code = 0;
		if ( x < -w )	code |= CCODE_X1;
		if ( x >  w )	code |= CCODE_X2;
		if ( y < -w )	code |= CCODE_Y1;
		if ( y >  w )	code |= CCODE_Y2;
		if ( z < -w )	code |= CCODE_Z1;
		if ( z >  w )	code |= CCODE_Z2;

		orCode |= code;
		andCode &= code;

		if ( w > 0 )
		{
			float	oow = 1.0f / w;

			float	winX = destHalfWd + destHalfWd * x * oow;
			float	winY = destHalfHe - destHalfHe * y * oow;

			minX = DMIN( minX, winX );
			minY = DMIN( minY, winY );
			maxX = DMAX( maxX, winX );
			maxY = DMAX( maxY, winY );
		}
		else
		{
			// $$$ this shouldn't happen ..once proper
			// front plane clipping is implemented 8)
			//DASSERT( 0 );
			return false;
		}
	}

	if ( andCode )
		return false;

	out_bound2d[0] = minX;
	out_bound2d[1] = minY;
	out_bound2d[2] = maxX;
	out_bound2d[3] = maxY;

	return minX < maxX && minY < maxY;
}

//==================================================================
float Hider::RasterEstimate( const Bound &b, const Matrix44 &mtxLocalWorld ) const
{
	if NOT( b.IsValid() )
	{
		return MP_GRID_MAX_SIZE / 4;
	}

	float	bound2d[4];

	bool valid = makeRasterBound( b, mtxLocalWorld, bound2d );

	if ( valid )
	{
		float	squareArea = (bound2d[3] - bound2d[1]) * (bound2d[2] - bound2d[0]);
		return squareArea * 2;
	}
	else
		return 0.0f;	// invalid or zero area...
}

/*
//==================================================================
void Hider::pointsTo2D( Point2 *pDes, const Point3 *pSrc, u_int n )
{
	float destHalfWd	= (float)mDestBuff.mWd * 0.5f;
	float destHalfHe	= (float)mDestBuff.mHe * 0.5f;

	for (size_t i=0; i < n; ++i)
	{
		Vec4f	Pproj = MultiplyV3W1M( pSrc[i], mMtxCamProj );
		
		float	oow = 1.0f / Pproj.w;
		
		pDes[i].Set( 
				destHalfWd + destHalfWd * Pproj.x * oow,
				destHalfHe - destHalfHe * Pproj.y * oow
				);
	}
}
*/

//==================================================================
void Hider::Bust(
				ShadedGrid		&shadGrid,
				const WorkGrid	&workGrid,
				float			destOffX,
				float			destOffY,
				u_int			screenWd,
				u_int			screenHe ) const
{
	SlScalar screenCx  = SlScalar( screenWd * 0.5f );
	SlScalar screenCy  = SlScalar( screenHe * 0.5f );
	SlScalar screenHWd = SlScalar( screenWd * 0.5f );
	SlScalar screenHHe = SlScalar( screenHe * 0.5f );

	const SlVec3	*pPointsWS	= (const SlVec3 *)workGrid.mpPointsWS;

	const SlColor	*pOi = (const SlColor *)workGrid.mSymbolIs.FindSymbolIData( "Oi" );
	const SlColor	*pCi = (const SlColor *)workGrid.mSymbolIs.FindSymbolIData( "Ci" );

	size_t	sampleIdx = 0;

	static const SlScalar	one( 1 );

	size_t	blocksN = RI_GET_SIMD_BLOCKS( workGrid.mPointsN );
	for (size_t blkIdx=0; blkIdx < blocksN; ++blkIdx)
	{
		SlVec4		Pproj = V4__V3W1_Mul_M44<SlScalar>( pPointsWS[ blkIdx ], mMtxWorldProj );
		SlScalar	oow = one / Pproj.w();

		// TODO: should replace pPointsWS with pPointsCS... ..to avoid this and also because
		// perhaps P should indeed be in "current" space (camera)
		SlVec3		PtCS = V3__V3W1_Mul_M44<SlScalar>( pPointsWS[ blkIdx ], mMtxWorldCamera );

		shadGrid.mpPointsCS[ blkIdx ]			= PtCS;
		//shadGrid.mpPointsCloseCS[ blkIdx ]	= 0;

		shadGrid.mpPosWin[ blkIdx ][0] =  Pproj.x() * screenHWd * oow + screenCx;
		shadGrid.mpPosWin[ blkIdx ][1] = -Pproj.y() * screenHHe * oow + screenCy;

		shadGrid.mpCi[ blkIdx ] = pCi[ blkIdx ];
		shadGrid.mpOi[ blkIdx ] = pOi[ blkIdx ];
	}
}

//==================================================================
void Hider::HideCountBegin(
					DVec<u_int> &out_pixelsSamplesCount,
					HiderBucket &buck )
{
	size_t	pixelsN = buck.GetWd() * buck.GetHe();

	out_pixelsSamplesCount.resize( pixelsN );

	for (size_t i=0; i < pixelsN; ++i)
		out_pixelsSamplesCount[i] = 0;
}

//==================================================================
void Hider::HideCountGrid(
					DVec<u_int>			&inout_pixelsSamplesCount,
					HiderBucket			&buck,
					const ShadedGrid	&shadGrid )
{
	size_t	blocksN = RI_GET_SIMD_BLOCKS( shadGrid.mPointsN );

	int	buckWd = buck.GetWd();
	int	buckHe = buck.GetHe();

	for (size_t blkIdx=0; blkIdx < blocksN; ++blkIdx)
	{
		for (size_t	itmIdx=0; itmIdx < RI_SIMD_BLK_LEN; ++itmIdx)
		{
			int	x = (int)(shadGrid.mpPosWin[ blkIdx ][0][ itmIdx ]) - buck.mX1;
			int	y = (int)(shadGrid.mpPosWin[ blkIdx ][1][ itmIdx ]) - buck.mY1;

			if ( x >= 0 && y >= 0 && x < buckWd && y < buckHe )
			{
				inout_pixelsSamplesCount[ y * buckWd + x ] += 1;
			}
		}
	}
}

//==================================================================
void Hider::HideCountEnd(
					DVec<HiderPixel>		&out_pixels,
					DVec<HiderSampleData>	&out_sampData,
					HiderBucket				&buck,
					const DVec<u_int>		&pixelsSamplesCount )
{
	size_t	totSamples = 0;
	size_t	pixelsN = pixelsSamplesCount.size();
	for (size_t i=0; i < pixelsN; ++i)
		totSamples += pixelsSamplesCount[i];

	out_pixels.resize( pixelsN );
	out_sampData.resize( totSamples );

	size_t	sampIdx = 0;
	for (size_t i=0; i < pixelsN; ++i)
	{
		out_pixels[i].mX			= 0;
		out_pixels[i].mY			= 0;
		out_pixels[i].mpSampCoords	= NULL;
		out_pixels[i].mSampDataN	= pixelsSamplesCount[i];

		if ( out_pixels[i].mSampDataN )
			out_pixels[i].mpSampData = &out_sampData[ sampIdx ];
		else
			out_pixels[i].mpSampData = NULL;

		sampIdx += pixelsSamplesCount[i];
	}
}

//==================================================================
void Hider::HideAddSamplesSetup(
				DVec<u_int>			&out_pixelsSampsIdxs,
				HiderBucket			&buck )
{
	size_t	pixelsN = buck.GetWd() * buck.GetHe();

	out_pixelsSampsIdxs.resize( pixelsN );

	for (size_t i=0; i < pixelsN; ++i)
		out_pixelsSampsIdxs[i] = 0;
}

//==================================================================
void Hider::HideAddSamples(
					DVec<HiderPixel>	&pixels,
					DVec<u_int>			&pixelsSampsIdxs,
					HiderBucket			&buck,
					const ShadedGrid	&shadGrid )
{
	size_t	blocksN = RI_GET_SIMD_BLOCKS( shadGrid.mPointsN );

	int	buckWd = buck.GetWd();
	int	buckHe = buck.GetHe();

	for (size_t blkIdx=0; blkIdx < blocksN; ++blkIdx)
	{
		for (size_t	itmIdx=0; itmIdx < RI_SIMD_BLK_LEN; ++itmIdx)
		{
			int	x = (int)(shadGrid.mpPosWin[ blkIdx ][0][ itmIdx ]) - buck.mX1;
			int	y = (int)(shadGrid.mpPosWin[ blkIdx ][1][ itmIdx ]) - buck.mY1;

			if ( x >= 0 && y >= 0 && x < buckWd && y < buckHe )
			{
				float	rgb[3];

				rgb[0] = shadGrid.mpCi[blkIdx][0][itmIdx];
				rgb[1] = shadGrid.mpCi[blkIdx][1][itmIdx];
				rgb[2] = shadGrid.mpCi[blkIdx][2][itmIdx];

				size_t		pixIdx = buckWd * y + x;
				size_t		sampIdx = pixelsSampsIdxs[ pixIdx ]++;

				HiderSampleData	*pSampData = pixels[ pixIdx ].mpSampData + sampIdx;

				pSampData->mDepth = shadGrid.mpPointsCS[blkIdx][2][itmIdx];

				pSampData->mCi[0] = shadGrid.mpCi[blkIdx][0][itmIdx];
				pSampData->mCi[1] = shadGrid.mpCi[blkIdx][1][itmIdx];
				pSampData->mCi[2] = shadGrid.mpCi[blkIdx][2][itmIdx];

				pSampData->mOi[0] = shadGrid.mpOi[blkIdx][0][itmIdx];
				pSampData->mOi[1] = shadGrid.mpOi[blkIdx][1][itmIdx];
				pSampData->mOi[2] = shadGrid.mpOi[blkIdx][2][itmIdx];
			}
		}
	}
}

//==================================================================
static void sortSampData( HiderSampleData *pSampDataListSort[], HiderSampleData *pData, u_int dataN )
{
	if ( dataN == 1 )
	{
		pSampDataListSort[0] = pData;
		return;
	}

	// (sad ?) insert sort
	{
		int doneDataN = 0;

		for (u_int i=0; i < dataN; ++i)
		{
			float	depthI = pData[i].mDepth;

			int j=0;
			for (; j < doneDataN; ++j)
			{
				if ( depthI < pSampDataListSort[j]->mDepth )
				{
					for (int k=doneDataN-1; k >= j; --k)
						pSampDataListSort[k+1] = pSampDataListSort[k];

					break;
				}
			}

			pSampDataListSort[j] = &pData[i];
			doneDataN += 1;
		}
	}
}

//==================================================================
void Hider::Hide(
				DVec<HiderPixel>	&pixels,
				HiderBucket			&buck )
{
	u_int	buckWd = buck.GetWd();
	u_int	buckHe = buck.GetHe();

	size_t	pixIdx = 0;
	for (u_int y=0; y < buckHe; ++y)
	{
		for (u_int x=0; x < buckWd; ++x, ++pixIdx)
		{
			u_int			dataN = pixels[pixIdx].mSampDataN;

			if NOT( dataN )
				continue;

			DASSERT( dataN <= MAX_SAMP_DATA_PER_PIXEL );
			dataN = DMIN( dataN, MAX_SAMP_DATA_PER_PIXEL );

			HiderSampleData	*pData = pixels[pixIdx].mpSampData;

			HiderSampleData *pSampDataListSort[ MAX_SAMP_DATA_PER_PIXEL ];

			sortSampData( pSampDataListSort, pData, dataN );

			static Vec3<float>	one( 1 );

			if ( dataN > 0 )
			{
				Vec3<float>	accCol( 0.f );

				for (int i=(int)dataN-1; i >= 0; --i)
				{
					Vec3<float>	col = Vec3<float>( pSampDataListSort[i]->mCi );
					Vec3<float>	opa = Vec3<float>( pSampDataListSort[i]->mOi );

					accCol = accCol * (one - opa) + col;
				}

				buck.mCBuff.SetSample( x, y, &accCol.x() );
			}
		}
	}
}

//==================================================================
size_t Hider::GetOutputBucketMemSize( size_t buckIdx ) const
{
	const HiderBucket	&buck = *mpBuckets[ buckIdx ];

	u_int	wd = (u_int)(buck.mX2 - buck.mX1);
	u_int	he = (u_int)(buck.mY2 - buck.mY1);

	return sizeof(float) * NCOLS * wd * he;
}

//==================================================================
void Hider::CopyOutputBucket( size_t buckIdx, float *pDest, size_t destMaxSize ) const
{
	const HiderBucket	&buck = *mpBuckets[ buckIdx ];

	u_int	wd	= (u_int)(buck.mX2 - buck.mX1);
	u_int	he	= (u_int)(buck.mY2 - buck.mY1);

	const float *pSrc = GetOutputData( buck.mX1, buck.mY1 );

	size_t wdStride = wd * NCOLS;
	size_t srcStride = GetOutputDataStride();

	U8	*pDestEnd = (U8 *)pDest + destMaxSize;

	for (u_int y=0; y < he; ++y)
	{
		memcpy( pDest, pSrc, sizeof(float) * wdStride );
		pDest += wdStride;
		pSrc += srcStride;
	}

	DASSERT( (U8*)pDest == pDestEnd );
}

//==================================================================
void Hider::StoreOutputBucket( size_t buckIdx, const float *pSrc, size_t srcSize )
{
	const HiderBucket	&buck = *mpBuckets[ buckIdx ];

	u_int	wd	= (u_int)(buck.mX2 - buck.mX1);
	u_int	he	= (u_int)(buck.mY2 - buck.mY1);

	float *pDest = GetOutputDataRW( buck.mX1, buck.mY1 );
	size_t desStride = GetOutputDataStride();

	size_t wdStride = wd * NCOLS;

	U8	*pSrcEnd = (U8 *)pSrc + srcSize;

	for (u_int y=0; y < he; ++y)
	{
		memcpy( pDest, pSrc, sizeof(float) * wdStride );

		pSrc	+= wdStride;
		pDest	+= desStride;
	}

	DASSERT( (U8*)pSrc == pSrcEnd );
}

//==================================================================
}
