//-------------------------------------------------------------------------------------
//
// Copyright 2009 Intel Corporation
// All Rights Reserved
//
// Permission is granted to use, copy, distribute and prepare derivative works of this
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  THIS SOFTWARE IS PROVIDED "AS IS."
// INTEL SPECIFICALLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, AND ALL LIABILITY,
// INCLUDING CONSEQUENTIAL AND OTHER INDIRECT DAMAGES, FOR THE USE OF THIS SOFTWARE,
// INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PROPRIETARY RIGHTS, AND INCLUDING THE
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.
//
//=============================================================================
// Camera.cpp by Frank Luna (C) 2004 All Rights Reserved.
//=============================================================================


#include "DXUT.h"
//#include "dxstdafx.h"
#include "Camera.h"
#include "d3dUtil.h"
#include "Terrain.h"

using namespace std;

Camera* gCamera;
bool    g_Rotate = false;

Camera::Camera()
{
	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjectionMatrix);
	D3DXMatrixIdentity(&m_ViewProjMatrix);

	m_PosWorld   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_RightWorld = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_UpWorld    = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_LookAtWorld  = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	// Client should adjust to a value that makes sense for application's
	// unit scale, and the object the camera is attached to--e.g., car, jet,
	// human walking, etc.
	m_Speed  = 50.0f;
}

const D3DXMATRIX& Camera::view() const
{
	return m_ViewMatrix;
}

const D3DXMATRIX& Camera::proj() const
{
	return m_ProjectionMatrix;
}

const D3DXMATRIX& Camera::viewProj() const
{
	return m_ViewProjMatrix;
}

const D3DXVECTOR3& Camera::right() const
{
	return m_RightWorld;
}

const D3DXVECTOR3& Camera::up() const
{
	return m_UpWorld;
}

const D3DXVECTOR3& Camera::look() const
{
	return m_LookAtWorld;
}

D3DXVECTOR3& Camera::pos()
{
	return m_PosWorld;
}

void Camera::lookAt(D3DXVECTOR3& pos, D3DXVECTOR3& target, D3DXVECTOR3& up)
{
	D3DXVECTOR3 L = target - pos;
	D3DXVec3Normalize(&L, &L);

	D3DXVECTOR3 R;
	D3DXVec3Cross(&R, &up, &L);
	D3DXVec3Normalize(&R, &R);

	D3DXVECTOR3 U;
	D3DXVec3Cross(&U, &L, &R);
	D3DXVec3Normalize(&U, &U);

	m_PosWorld   = pos;
	m_RightWorld = R;
	m_UpWorld    = U;
	m_LookAtWorld  = L;

	buildView();
	buildWorldFrustumPlanes();

	m_ViewProjMatrix = m_ViewMatrix * m_ProjectionMatrix;
}

void Camera::setLens(float fov, float aspect, float nearZ, float farZ)
{
	D3DXMatrixPerspectiveFovLH(&m_ProjectionMatrix, fov, aspect, nearZ, farZ);
	buildWorldFrustumPlanes();
	m_ViewProjMatrix = m_ViewMatrix * m_ProjectionMatrix;
}

void Camera::setSpeed(float s)
{
	m_Speed = s;
}

bool Camera::isVisible(const AABB& box)const
{
	// Test assumes frustum planes face inward.

	D3DXVECTOR3 P;
	D3DXVECTOR3 Q;

	//      N  *Q                    *P
	//      | /                     /
	//      |/                     /
	// -----/----- Plane     -----/----- Plane    
	//     /                     / |
	//    /                     /  |
	//   *P                    *Q  N
	//
	// PQ forms diagonal most closely aligned with plane normal.

	// For each frustum plane, find the box diagonal (there are four main
	// diagonals that intersect the box center point) that points in the
	// same direction as the normal along each axis (i.e., the diagonal 
	// that is most aligned with the plane normal).  Then test if the box
	// is in front of the plane or not.
	for(int i = 0; i < 6; ++i)
	{
		// For each coordinate axis x, y, z...
		for(int j = 0; j < 3; ++j)
		{
			// Make PQ point in the same direction as the plane normal on this axis.
			if( m_FrustumPlanes[i][j] >= 0.0f )
			{
				P[j] = box.minPt[j];
				Q[j] = box.maxPt[j];
			}
			else 
			{
				P[j] = box.maxPt[j];
				Q[j] = box.minPt[j];
			}
		}

		// If box is in negative half space, it is behind the plane, and thus, completely
		// outside the frustum.  Note that because PQ points roughly in the direction of the 
		// plane normal, we can deduce that if Q is outside then P is also outside--thus we
		// only need to test Q.
		if( D3DXPlaneDotCoord(&m_FrustumPlanes[i], &Q) < 0.0f  ) // outside
			return false;
	}
	return true;
}

void Camera::update(float dt, Terrain* terrain, float offsetHeight,int key)
{
  static bool bLockToGround = false;
	// Find the net direction the camera is traveling in (since the
	// camera could be running and strafing).
	D3DXVECTOR3 dir(0.0f, 0.0f, 0.0f);
	if( key == 1 ) // up
		dir += m_LookAtWorld;
	if( key == 3 ) // down
		dir -= m_LookAtWorld;
	if( key == 4 )
		dir += m_RightWorld;
	if( key == 2 )
		dir -= m_RightWorld;
 // if( gDInput->keyDown(DIK_G) )
 //   bLockToGround = !bLockToGround;

	// Move at m_Speed along net direction.
	D3DXVec3Normalize(&dir, &dir);
	D3DXVECTOR3 newPos = m_PosWorld + dir*m_Speed*dt;

	if( terrain != 0  && bLockToGround)
	{
		// New position might not be on terrain, so project the
		// point onto the terrain.
		newPos.y = terrain->getHeight(newPos.x, newPos.z) + offsetHeight;

		// Now the difference of the new position and old (current) 
		// position approximates a tangent vector on the terrain.
		D3DXVECTOR3 tangent = newPos - m_PosWorld;
		D3DXVec3Normalize(&tangent, &tangent);

		// Now move camera along tangent vector.
		m_PosWorld += tangent*m_Speed*dt;

		// After update, there may be errors in the camera height since our
		// tangent is only an approximation.  So force camera to correct height,
		// and offset by the specified amount so that camera does not sit
		// exactly on terrain, but instead, slightly above it.
    m_PosWorld.y = terrain->getHeight(m_PosWorld.x, m_PosWorld.z) + offsetHeight;

	}
	else
	{
		m_PosWorld = newPos;
    float y = terrain->getHeight(m_PosWorld.x, m_PosWorld.z) + offsetHeight;
    if ( y > m_PosWorld.y )
      m_PosWorld.y = y;

	}

	// Rebuild the view matrix to reflect changes.
	buildView();
	buildWorldFrustumPlanes();

	m_ViewProjMatrix = m_ViewMatrix * m_ProjectionMatrix;
}     

void  Camera::fly(int x, int y)
{
  // We rotate at a fixed speed.
  float pitch  = y / 200.0f;
  float yAngle = x / 200.0f;

  // Rotate camera's look and up vectors around the camera's right vector.
  D3DXMATRIX R;
  D3DXMatrixRotationAxis(&R, &m_RightWorld, pitch);
  D3DXVec3TransformCoord(&m_LookAtWorld, &m_LookAtWorld, &R);
  D3DXVec3TransformCoord(&m_UpWorld, &m_UpWorld, &R);


  // Rotate camera axes about the world's y-axis.
  D3DXMatrixRotationY(&R, yAngle);
  D3DXVec3TransformCoord(&m_RightWorld, &m_RightWorld, &R);
  D3DXVec3TransformCoord(&m_UpWorld, &m_UpWorld, &R);
  D3DXVec3TransformCoord(&m_LookAtWorld, &m_LookAtWorld, &R);


  // Rebuild the view matrix to reflect changes.
  buildView();
  buildWorldFrustumPlanes();

  m_ViewProjMatrix = m_ViewMatrix * m_ProjectionMatrix;
}




void Camera::buildView()
{
	// Keep camera's axes orthogonal to each other and of unit length.
	D3DXVec3Normalize(&m_LookAtWorld, &m_LookAtWorld);

	D3DXVec3Cross(&m_UpWorld, &m_LookAtWorld, &m_RightWorld);
	D3DXVec3Normalize(&m_UpWorld, &m_UpWorld);

	D3DXVec3Cross(&m_RightWorld, &m_UpWorld, &m_LookAtWorld);
	D3DXVec3Normalize(&m_RightWorld, &m_RightWorld);

	// Fill in the view matrix entries.

	float x = -D3DXVec3Dot(&m_PosWorld, &m_RightWorld);
	float y = -D3DXVec3Dot(&m_PosWorld, &m_UpWorld);
	float z = -D3DXVec3Dot(&m_PosWorld, &m_LookAtWorld);

	m_ViewMatrix(0,0) = m_RightWorld.x; 
	m_ViewMatrix(1,0) = m_RightWorld.y; 
	m_ViewMatrix(2,0) = m_RightWorld.z; 
	m_ViewMatrix(3,0) = x;   

	m_ViewMatrix(0,1) = m_UpWorld.x;
	m_ViewMatrix(1,1) = m_UpWorld.y;
	m_ViewMatrix(2,1) = m_UpWorld.z;
	m_ViewMatrix(3,1) = y;  

	m_ViewMatrix(0,2) = m_LookAtWorld.x; 
	m_ViewMatrix(1,2) = m_LookAtWorld.y; 
	m_ViewMatrix(2,2) = m_LookAtWorld.z; 
	m_ViewMatrix(3,2) = z;   

	m_ViewMatrix(0,3) = 0.0f;
	m_ViewMatrix(1,3) = 0.0f;
	m_ViewMatrix(2,3) = 0.0f;
	m_ViewMatrix(3,3) = 1.0f;
}

void Camera::buildWorldFrustumPlanes()
{
	// Note: Extract the frustum planes in world space.

	D3DXMATRIX VP = m_ViewMatrix * m_ProjectionMatrix;
	//D3DXMatrixMultiply(&VP, &m_ViewMatrix, &m_ProjectionMatrix);

	D3DXVECTOR4 col0(VP(0,0), VP(1,0), VP(2,0), VP(3,0));
	D3DXVECTOR4 col1(VP(0,1), VP(1,1), VP(2,1), VP(3,1));
	D3DXVECTOR4 col2(VP(0,2), VP(1,2), VP(2,2), VP(3,2));
	D3DXVECTOR4 col3(VP(0,3), VP(1,3), VP(2,3), VP(3,3));

	// Planes face inward.
	m_FrustumPlanes[0] = (D3DXPLANE)(col2);        // near
	m_FrustumPlanes[1] = (D3DXPLANE)(col3 - col2); // far
	m_FrustumPlanes[2] = (D3DXPLANE)(col3 + col0); // left
	m_FrustumPlanes[3] = (D3DXPLANE)(col3 - col0); // right
	m_FrustumPlanes[4] = (D3DXPLANE)(col3 - col1); // top
	m_FrustumPlanes[5] = (D3DXPLANE)(col3 + col1); // bottom

	for(int i = 0; i < 6; i++)
		D3DXPlaneNormalize(&m_FrustumPlanes[i], &m_FrustumPlanes[i]);
}
 