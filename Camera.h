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
// Camera.h by Frank Luna (C) 2004 All Rights Reserved.
//=============================================================================

#ifndef CAMERA_H
#define CAMERA_H

#include <d3dx9.h>
#include "d3dUtil.h"
#include "DXUTcamera.h"

// Forward declaration.
class Terrain;

class Camera
{
public:
	Camera();

	const D3DXMATRIX& view() const;
	const D3DXMATRIX& proj() const;
	const D3DXMATRIX& viewProj() const;

	const D3DXVECTOR3& right() const;
	const D3DXVECTOR3& up() const;
	const D3DXVECTOR3& look() const;

	D3DXVECTOR3& pos();

	void lookAt(D3DXVECTOR3& pos, D3DXVECTOR3& target, D3DXVECTOR3& up);
	void setLens(float fov, float aspect, float nearZ, float farZ);
	void setSpeed(float s);

	// Box coordinates should be relative to world space.
	bool isVisible(const AABB& box)const;

	void update(float dt, Terrain* terrain, float offsetHeight, int dir);	
  void fly(int x, int y);

protected:
	void buildView();
	void buildWorldFrustumPlanes();

protected:
	D3DXMATRIX m_ViewMatrix;
	D3DXMATRIX m_ProjectionMatrix;
	D3DXMATRIX m_ViewProjMatrix;

	// Relative to world space.
	D3DXVECTOR3 m_PosWorld;
	D3DXVECTOR3 m_RightWorld;
	D3DXVECTOR3 m_UpWorld;
	D3DXVECTOR3 m_LookAtWorld;

	float m_Speed;

	// Frustum Planes
	D3DXPLANE m_FrustumPlanes[6]; // [0] = near
	                             // [1] = far
	                             // [2] = left
	                             // [3] = right
	                             // [4] = top
	                             // [5] = bottom
};

#endif // CAMERA_H