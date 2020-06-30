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
// D3DUtil.h by Frank Luna (C) 2004 All Rights Reserved.
//=============================================================================


#ifndef D3DUTIL_H
#define D3DUTIL_H

// Enable extra D3D debugging in debug builds if using the debug DirectX runtime.  
// This makes D3D objects work well in the debugger watch window, but slows down 
// performance slightly.
#if defined(DEBUG) | defined(_DEBUG)
#ifndef D3D_DEBUG_INFO
#define D3D_DEBUG_INFO
#endif
#endif

#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr.h>
#include <string>
#include <sstream>
#include <vector>

//===============================================================
// Globals for convenient access.
class D3DApp;
extern D3DApp* gd3dApp;
extern IDirect3DDevice9* gd3dDevice;


//===============================================================
// Clean up

#define SafeRelease(X) { if(0 != X) { X->Release(); X = 0; } }


//===============================================================
// Geometry generation.

void GenTriGrid(int numVertRows, int numVertCols, 
	float dx, float dz, const D3DXVECTOR3& center, 
	std::vector<D3DXVECTOR3>& verts, std::vector<DWORD>& indices);

//===============================================================
// Colors and Materials


//const float EPSILON  = 0.001f;

const D3DXCOLOR WHITE(1.0f, 1.0f, 1.0f, 1.0f);
const D3DXCOLOR BLACK(0.0f, 0.0f, 0.0f, 1.0f);
const D3DXCOLOR RED(1.0f, 0.0f, 0.0f, 1.0f);
const D3DXCOLOR GREEN(0.0f, 1.0f, 0.0f, 1.0f);
const D3DXCOLOR BLUE(0.0f, 0.0f, 1.0f, 1.0f);

struct Mtrl
{
  Mtrl()
    :ambient(WHITE), diffuse(WHITE), spec(WHITE), specPower(8.0f){}
  Mtrl(const D3DXCOLOR& a, const D3DXCOLOR& d, 
    const D3DXCOLOR& s, float power)
    :ambient(a), diffuse(d), spec(s), specPower(power){}

  D3DXCOLOR ambient;
  D3DXCOLOR diffuse;
  D3DXCOLOR spec;
  float specPower;
};

//===============================================================
// Bounding Volumes

struct AABB 
{
  // Initialize to an infinitely small bounding box.
  AABB()
    : minPt(FLT_MAX, FLT_MAX, FLT_MAX),
    maxPt(-FLT_MAX, -FLT_MAX, -FLT_MAX){}

  D3DXVECTOR3 center()const
  {
    return (minPt+maxPt)*0.5f;
  }

  D3DXVECTOR3 extent()const
  {
    return (maxPt-minPt)*0.5f;
  }

  void xform(const D3DXMATRIX& M, AABB& out)
  {
    // Convert to center/extent representation.
    D3DXVECTOR3 c = center();
    D3DXVECTOR3 e = extent();

    // Transform center in usual way.
    D3DXVec3TransformCoord(&c, &c, &M);

    // Transform extent.
    D3DXMATRIX absM;
    D3DXMatrixIdentity(&absM);
    absM(0,0) = fabsf(M(0,0)); absM(0,1) = fabsf(M(0,1)); absM(0,2) = fabsf(M(0,2));
    absM(1,0) = fabsf(M(1,0)); absM(1,1) = fabsf(M(1,1)); absM(1,2) = fabsf(M(1,2));
    absM(2,0) = fabsf(M(2,0)); absM(2,1) = fabsf(M(2,1)); absM(2,2) = fabsf(M(2,2));
    D3DXVec3TransformNormal(&e, &e, &absM);

    // Convert back to AABB representation.
    out.minPt = c - e;
    out.maxPt = c + e;
  }

  D3DXVECTOR3 minPt;
  D3DXVECTOR3 maxPt;
};

struct BoundingSphere 
{
  BoundingSphere()
    : pos(0.0f, 0.0f, 0.0f), radius(0.0f){}

  D3DXVECTOR3 pos;
  float radius;
};

//===============================================================
// Debug

#if defined(DEBUG) | defined(_DEBUG)
	#ifndef THR
	#define THR(x)                                      \
	{                                                  \
		HRESULT hr = x;                                \
		if(FAILED(hr))                                 \
		{                                              \
			DXTrace(__FILE__, __LINE__, hr, L#x, TRUE); \
		}                                              \
	}
	#endif

#else
	#ifndef THR
	#define THR(x) x;
	#endif
#endif 

#endif // D3DUTIL_H