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
// Terrain.h by Frank Luna (C) 2004 All Rights Reserved.
//=============================================================================

#ifndef TERRAIN_H
#define TERRAIN_H

#include "Heightmap.h"
#include "d3dUtil.h"
#include "Vertex.h"

class Terrain
{
public:
	Terrain(LPDIRECT3DDEVICE9 pDevice, UINT vertRows, UINT vertCols, float dx, float dz, 
		LPCWSTR heightmap, LPCWSTR tex0, LPCWSTR tex1, 
		LPCWSTR tex2, LPCWSTR blendMap, float heightScale, 
		float yOffset);
	~Terrain();

	LPDIRECT3DDEVICE9	m_pDevice;

	DWORD getNumTriangles();
	DWORD getNumVertices();

	float getWidth();
	float getDepth();

	void onLostDevice();
	void onResetDevice();

	// (x, z) relative to terrain's local space.
	float getHeight(float x, float z);
	
	void setDirToSunW(const D3DXVECTOR3& d);

	void draw();

private:
	void buildGeometry();
	void buildSubGridMesh(RECT& R, VertexPNT* gridVerts); 
	void buildEffect();

	struct SubGrid
	{
		ID3DXMesh* mesh;
		AABB box;

		// For sorting.
		bool operator<(const SubGrid& rhs)const;

		const static int NUM_ROWS  = 33;
		const static int NUM_COLS  = 33;
		const static int NUM_TRIS  = (NUM_ROWS-1)*(NUM_COLS-1)*2;
		const static int NUM_VERTS = NUM_ROWS*NUM_COLS;
	};
private:
	Heightmap mHeightmap;
	std::vector<SubGrid> mSubGrids;

	DWORD mVertRows;
	DWORD mVertCols;

	float mWidth;
	float mDepth;

	float mDX;
	float mDZ;

	IDirect3DTexture9* mTex0;
	IDirect3DTexture9* mTex1;
	IDirect3DTexture9* mTex2;
	IDirect3DTexture9* mBlendMap;
	ID3DXEffect*       mFX;
	D3DXHANDLE         mhTech;
	D3DXHANDLE         mhViewProj;
	D3DXHANDLE         mhDirToSunW;
	D3DXHANDLE         mhTex0;
	D3DXHANDLE         mhTex1;
	D3DXHANDLE         mhTex2;
	D3DXHANDLE         mhBlendMap;
};

#endif // TERRAIN_H