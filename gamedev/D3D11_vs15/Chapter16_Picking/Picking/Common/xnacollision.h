//-------------------------------------------------------------------------------------
// XNACollision.h
//  
// An opimtized collision library based on XNAMath
//  
// Microsoft XNA Developer Connection
// Copyright (c) Microsoft Corporation. All rights reserved.
//-------------------------------------------------------------------------------------

#pragma once

#ifndef _XNA_COLLISION_H_
#define _XNA_COLLISION_H_

#define _XM_NO_INTRINSICS_

#include <DirectXMath.h>
#include <DirectXPackedVector.h>

#if !defined(XMASSERT)
#if defined(_PREFAST_)
#define XMASSERT(Expression) __analysis_assume((Expression))
#elif defined(XMDEBUG) // !_PREFAST_
#define XMASSERT(Expression) ((VOID)((Expression) || (XMAssert(#Expression, __FILE__, __LINE__), 0)))
#else // !XMDEBUG
#define XMASSERT(Expression) ((VOID)0)
#endif // !XMDEBUG
#endif // !XMASSERT

namespace XNA
{
	using namespace DirectX;
//-----------------------------------------------------------------------------
// Bounding volumes structures.
//
// The bounding volume structures are setup for near minimum size because there
// are likely to be many of them, and memory bandwidth and space will be at a
// premium relative to CPU cycles on Xbox 360.
//-----------------------------------------------------------------------------

#pragma warning(push)
#pragma warning(disable: 4324)

DECLSPEC_ALIGN(16) struct Sphere
{
    DirectX::XMFLOAT3 Center;            // Center of the sphere.
    FLOAT Radius;               // Radius of the sphere.
};

DECLSPEC_ALIGN(16) struct AxisAlignedBox
{
    DirectX::XMFLOAT3 Center;            // Center of the box.
    DirectX::XMFLOAT3 Extents;           // Distance from the center to each side.
};

DECLSPEC_ALIGN(16) struct OrientedBox
{
    DirectX::XMFLOAT3 Center;            // Center of the box.
    DirectX::XMFLOAT3 Extents;           // Distance from the center to each side.
    DirectX::XMFLOAT4 Orientation;       // Unit quaternion representing rotation (box -> world).
};

DECLSPEC_ALIGN(16) struct Frustum
{
    DirectX::XMFLOAT3 Origin;            // Origin of the frustum (and projection).
    DirectX::XMFLOAT4 Orientation;       // Unit quaternion representing rotation.

    FLOAT RightSlope;           // Positive X slope (X/Z).
    FLOAT LeftSlope;            // Negative X slope.
    FLOAT TopSlope;             // Positive Y slope (Y/Z).
    FLOAT BottomSlope;          // Negative Y slope.
    FLOAT Near, Far;            // Z of the near plane and far plane.
};

#pragma warning(pop)

//-----------------------------------------------------------------------------
// Bounding volume construction.
//-----------------------------------------------------------------------------
VOID ComputeBoundingSphereFromPoints( Sphere* pOut, UINT Count, const DirectX::XMFLOAT3* pPoints, UINT Stride );
VOID ComputeBoundingAxisAlignedBoxFromPoints( AxisAlignedBox* pOut, UINT Count, const DirectX::XMFLOAT3* pPoints, UINT Stride );
VOID ComputeBoundingOrientedBoxFromPoints( OrientedBox* pOut, UINT Count, const DirectX::XMFLOAT3* pPoints, UINT Stride );
VOID ComputeFrustumFromProjection( Frustum* pOut, XMMATRIX* pProjection );
VOID ComputePlanesFromFrustum( const Frustum* pVolume, XMVECTOR* pPlane0, XMVECTOR* pPlane1, XMVECTOR* pPlane2,
                               XMVECTOR* pPlane3, XMVECTOR* pPlane4, XMVECTOR* pPlane5 );



//-----------------------------------------------------------------------------
// Bounding volume transforms.
//-----------------------------------------------------------------------------
VOID TransformSphere( Sphere* pOut, const Sphere* pIn, FLOAT Scale, DirectX::FXMVECTOR Rotation, DirectX::FXMVECTOR Translation );
VOID TransformAxisAlignedBox( AxisAlignedBox* pOut, const AxisAlignedBox* pIn, FLOAT Scale, DirectX::FXMVECTOR Rotation,
                              DirectX::FXMVECTOR Translation );
VOID TransformOrientedBox( OrientedBox* pOut, const OrientedBox* pIn, FLOAT Scale, DirectX::FXMVECTOR Rotation,
                           DirectX::FXMVECTOR Translation );
VOID TransformFrustum( Frustum* pOut, const Frustum* pIn, FLOAT Scale, DirectX::FXMVECTOR Rotation, DirectX::FXMVECTOR Translation );



//-----------------------------------------------------------------------------
// Intersection testing routines.
//-----------------------------------------------------------------------------
BOOL IntersectPointSphere( DirectX::FXMVECTOR Point, const Sphere* pVolume );
BOOL IntersectPointAxisAlignedBox( DirectX::FXMVECTOR Point, const AxisAlignedBox* pVolume );
BOOL IntersectPointOrientedBox( DirectX::FXMVECTOR Point, const OrientedBox* pVolume );
BOOL IntersectPointFrustum( DirectX::FXMVECTOR Point, const Frustum* pVolume );
BOOL IntersectRayTriangle( DirectX::FXMVECTOR Origin, DirectX::FXMVECTOR Direction, DirectX::FXMVECTOR V0, CXMVECTOR V1, CXMVECTOR V2,
                           FLOAT* pDist );
BOOL IntersectRaySphere( DirectX::FXMVECTOR Origin, DirectX::FXMVECTOR Direction, const Sphere* pVolume, FLOAT* pDist );
BOOL IntersectRayAxisAlignedBox( DirectX::FXMVECTOR Origin, DirectX::FXMVECTOR Direction, const AxisAlignedBox* pVolume, FLOAT* pDist );
BOOL IntersectRayOrientedBox( DirectX::FXMVECTOR Origin, DirectX::FXMVECTOR Direction, const OrientedBox* pVolume, FLOAT* pDist );
BOOL IntersectTriangleTriangle( DirectX::FXMVECTOR A0, DirectX::FXMVECTOR A1, DirectX::FXMVECTOR A2, CXMVECTOR B0, CXMVECTOR B1, CXMVECTOR B2 );
BOOL IntersectTriangleSphere( DirectX::FXMVECTOR V0, DirectX::FXMVECTOR V1, DirectX::FXMVECTOR V2, const Sphere* pVolume );
BOOL IntersectTriangleAxisAlignedBox( DirectX::FXMVECTOR V0, DirectX::FXMVECTOR V1, DirectX::FXMVECTOR V2, const AxisAlignedBox* pVolume );
BOOL IntersectTriangleOrientedBox( DirectX::FXMVECTOR V0, DirectX::FXMVECTOR V1, DirectX::FXMVECTOR V2, const OrientedBox* pVolume );
BOOL IntersectSphereSphere( const Sphere* pVolumeA, const Sphere* pVolumeB );
BOOL IntersectSphereAxisAlignedBox( const Sphere* pVolumeA, const AxisAlignedBox* pVolumeB );
BOOL IntersectSphereOrientedBox( const Sphere* pVolumeA, const OrientedBox* pVolumeB );
BOOL IntersectAxisAlignedBoxAxisAlignedBox( const AxisAlignedBox* pVolumeA, const AxisAlignedBox* pVolumeB );
BOOL IntersectAxisAlignedBoxOrientedBox( const AxisAlignedBox* pVolumeA, const OrientedBox* pVolumeB );
BOOL IntersectOrientedBoxOrientedBox( const OrientedBox* pVolumeA, const OrientedBox* pVolumeB );



//-----------------------------------------------------------------------------
// Frustum intersection testing routines.
// Return values: 0 = no intersection, 
//                1 = intersection, 
//                2 = A is completely inside B
//-----------------------------------------------------------------------------
INT IntersectTriangleFrustum( DirectX::FXMVECTOR V0, DirectX::FXMVECTOR V1, DirectX::FXMVECTOR V2, const Frustum* pVolume );
INT IntersectSphereFrustum( const Sphere* pVolumeA, const Frustum* pVolumeB );
INT IntersectAxisAlignedBoxFrustum( const AxisAlignedBox* pVolumeA, const Frustum* pVolumeB );
INT IntersectOrientedBoxFrustum( const OrientedBox* pVolumeA, const Frustum* pVolumeB );
INT IntersectFrustumFrustum( const Frustum* pVolumeA, const Frustum* pVolumeB );




//-----------------------------------------------------------------------------
// Test vs six planes (usually forming a frustum) intersection routines.
// The intended use for these routines is for fast culling to a view frustum.  
// When the volume being tested against a view frustum is small relative to the
// view frustum it is usually either inside all six planes of the frustum or 
// outside one of the planes of the frustum. If neither of these cases is true
// then it may or may not be intersecting the frustum. Outside a plane is 
// defined as being on the positive side of the plane (and inside negative).
// Return values: 0 = volume is outside one of the planes (no intersection),
//                1 = not completely inside or completely outside (intersecting),
//                2 = volume is inside all the planes (completely inside)
//-----------------------------------------------------------------------------
INT IntersectTriangle6Planes( DirectX::FXMVECTOR V0, DirectX::FXMVECTOR V1, DirectX::FXMVECTOR V2, CXMVECTOR Plane0, CXMVECTOR Plane1,
                              CXMVECTOR Plane2, CXMVECTOR Plane3, CXMVECTOR Plane4, CXMVECTOR Plane5 );
INT IntersectSphere6Planes( const Sphere* pVolume, DirectX::FXMVECTOR Plane0, DirectX::FXMVECTOR Plane1, DirectX::FXMVECTOR Plane2,
                            CXMVECTOR Plane3, CXMVECTOR Plane4, CXMVECTOR Plane5 );
INT IntersectAxisAlignedBox6Planes( const AxisAlignedBox* pVolume, DirectX::FXMVECTOR Plane0, DirectX::FXMVECTOR Plane1,
                                    DirectX::FXMVECTOR Plane2, CXMVECTOR Plane3, CXMVECTOR Plane4, CXMVECTOR Plane5 );
INT IntersectOrientedBox6Planes( const OrientedBox* pVolume, DirectX::FXMVECTOR Plane0, DirectX::FXMVECTOR Plane1, DirectX::FXMVECTOR Plane2,
                                 CXMVECTOR Plane3, CXMVECTOR Plane4, CXMVECTOR Plane5 );
INT IntersectFrustum6Planes( const Frustum* pVolume, DirectX::FXMVECTOR Plane0, DirectX::FXMVECTOR Plane1, DirectX::FXMVECTOR Plane2,
                             CXMVECTOR Plane3, CXMVECTOR Plane4, CXMVECTOR Plane5 );


//-----------------------------------------------------------------------------
// Volume vs plane intersection testing routines.
// Return values: 0 = volume is outside the plane (on the positive sideof the plane),
//                1 = volume intersects the plane,
//                2 = volume is inside the plane (on the negative side of the plane) 
//-----------------------------------------------------------------------------
INT IntersectTrianglePlane( DirectX::FXMVECTOR V0, DirectX::FXMVECTOR V1, DirectX::FXMVECTOR V2, CXMVECTOR Plane );
INT IntersectSpherePlane( const Sphere* pVolume, DirectX::FXMVECTOR Plane );
INT IntersectAxisAlignedBoxPlane( const AxisAlignedBox* pVolume, DirectX::FXMVECTOR Plane );
INT IntersectOrientedBoxPlane( const OrientedBox* pVolume, DirectX::FXMVECTOR Plane );
INT IntersectFrustumPlane( const Frustum* pVolume, DirectX::FXMVECTOR Plane );

}; // namespace

#endif