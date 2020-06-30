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
// Terrain.cpp by Frank Luna (C) 2004 All Rights Reserved.
//=============================================================================

//#include "DXApp_PrecompiledHeader.h"
#include "dxut.h"
#include "Terrain.h"
#include "d3dUtil.h"
#include "Camera.h"
#include <algorithm>
#include <list>
#include "SDKmisc.h"

using namespace std;

extern Camera* gCamera;

Terrain::Terrain(LPDIRECT3DDEVICE9 pDevice, UINT vertRows, UINT vertCols, float dx, float dz, 
		LPCWSTR heightmap, LPCWSTR tex0, LPCWSTR tex1, 
		LPCWSTR tex2, LPCWSTR blendMap, float heightScale, 
		float yOffset)
{
	WCHAR		filename[MAX_PATH];

	m_pDevice = pDevice;
	mVertRows = vertRows;
	mVertCols = vertCols;

	mDX = dx;
	mDZ = dz;

	mWidth = (mVertCols-1)*mDX;
	mDepth = (mVertRows-1)*mDZ;

	DXUTFindDXSDKMediaFileCch( filename, MAX_PATH, heightmap );

	mHeightmap.loadRAW(vertRows, vertCols, (LPCWSTR)filename, heightScale, yOffset);

	DXUTFindDXSDKMediaFileCch( filename, MAX_PATH, tex0 );
	D3DXCreateTextureFromFile(m_pDevice, filename, &mTex0);

	DXUTFindDXSDKMediaFileCch( filename, MAX_PATH, tex1 );
	D3DXCreateTextureFromFile(m_pDevice, filename, &mTex1);

	DXUTFindDXSDKMediaFileCch( filename, MAX_PATH, tex2 );
	D3DXCreateTextureFromFile(m_pDevice, filename, &mTex2);

	DXUTFindDXSDKMediaFileCch( filename, MAX_PATH, blendMap );
	D3DXCreateTextureFromFile(m_pDevice, filename, &mBlendMap);

	buildGeometry();
	buildEffect();

	gCamera = new Camera;

    gCamera->pos() = D3DXVECTOR3(0, -0, 0);
    gCamera->setSpeed(100.0f);
}

Terrain::~Terrain()
{
	for(UINT i = 0; i < mSubGrids.size(); ++i)
		SafeRelease(mSubGrids[i].mesh);
	
	delete gCamera;
	SafeRelease(mFX);
	SafeRelease(mTex0);
	SafeRelease(mTex1);
	SafeRelease(mTex2);
	SafeRelease(mBlendMap);
}

DWORD Terrain::getNumTriangles()
{
	return (DWORD)mSubGrids.size()*mSubGrids[0].mesh->GetNumFaces();
}

DWORD Terrain::getNumVertices()
{
	return (DWORD)mSubGrids.size()*mSubGrids[0].mesh->GetNumVertices();
}

float Terrain::getWidth()
{
	return mWidth;
}

float Terrain::getDepth()
{
	return mDepth;
}

void Terrain::onLostDevice()
{
	HRESULT hr;
	V(mFX->OnLostDevice());
}

void Terrain::onResetDevice()
{
	HRESULT hr;
	V(mFX->OnResetDevice());
}

float Terrain::getHeight(float x, float z)
{
	// Transform from terrain local space to "cell" space.
	float c = (x + 0.5f*mWidth) /  mDX;
	float d = (z - 0.5f*mDepth) / -mDZ;

	// Get the row and column we are in.
	int row = (int)floorf(d);
	int col = (int)floorf(c);

	// Grab the heights of the cell we are in.
	// A*--*B
	//  | /|
	//  |/ |
	// C*--*D
	float A = mHeightmap(row, col);
	float B = mHeightmap(row, col+1);
	float C = mHeightmap(row+1, col);
	float D = mHeightmap(row+1, col+1);

	// Where we are relative to the cell.
	float s = c - (float)col;
	float t = d - (float)row;

	// If upper triangle ABC.
	if(t < 1.0f - s)
	{
		float uy = B - A;
		float vy = C - A;
		return A + s*uy + t*vy;
	}
	else // lower triangle DCB.
	{
		float uy = C - D;
		float vy = B - D;
		return D + (1.0f-s)*uy + (1.0f-t)*vy;
	}
}

void Terrain::setDirToSunW(const D3DXVECTOR3& d)
{
	HRESULT hr;
	V(mFX->SetValue(mhDirToSunW, &d, sizeof(D3DXVECTOR3)));
}

// Sort by distance from nearest to farthest from the camera.  In this
// way, we draw objects in front to back order to reduce overdraw 
// (i.e., depth test will prevent them from being processed further.
bool Terrain::SubGrid::operator<(const SubGrid& rhs) const
{
	D3DXVECTOR3 d1 = box.center() - gCamera->pos();
	D3DXVECTOR3 d2 = rhs.box.center() - gCamera->pos();
	return D3DXVec3LengthSq(&d1) < D3DXVec3LengthSq(&d2);
}

void Terrain::draw()
{
	// Frustum cull sub-grids.
	std::list<SubGrid> visibleSubGrids;
	for(UINT i = 0; i < mSubGrids.size(); ++i)
	{
		if( gCamera->isVisible(mSubGrids[i].box) )
			visibleSubGrids.push_back(mSubGrids[i]);
	}

	// Sort front-to-back from camera.
	visibleSubGrids.sort();

	THR(mFX->SetMatrix(mhViewProj, &gCamera->viewProj()));
	THR(mFX->SetTechnique(mhTech));
	UINT numPasses = 0;
	THR(mFX->Begin(&numPasses, 0));
	THR(mFX->BeginPass(0));

	for(std::list<SubGrid>::iterator iter = visibleSubGrids.begin(); iter != visibleSubGrids.end(); ++iter)
		THR(iter->mesh->DrawSubset(0));

	THR(mFX->EndPass());
	THR(mFX->End());
}

void Terrain::buildGeometry()
{
	//===============================================================
	// Create one large mesh for the grid in system memory.

	DWORD numTris  = (mVertRows-1)*(mVertCols-1)*2;
	DWORD numVerts = mVertRows*mVertCols;

	ID3DXMesh* mesh = 0;
	D3DVERTEXELEMENT9 elems[MAX_FVF_DECL_SIZE];
	UINT numElems = 0;
	THR(VertexPNT::Decl->GetDeclaration(elems, &numElems));

	// Use Scratch pool since we are using this mesh purely for some CPU work,
	// which will be used to create the sub-grids that the graphics card
	// will actually draw.
	THR(D3DXCreateMesh(numTris, numVerts, 
		D3DPOOL_SCRATCH|D3DXMESH_32BIT, elems, m_pDevice, &mesh));


	//===============================================================
	// Write the grid vertices and triangles to the mesh.

	VertexPNT* v = 0;
	THR(mesh->LockVertexBuffer(0, (void**)&v));
	
	std::vector<D3DXVECTOR3> verts;
	std::vector<DWORD> indices;
	GenTriGrid(mVertRows, mVertCols, mDX, mDZ, D3DXVECTOR3(0.0f, 0.0f, 0.0f), verts, indices);

	float w = mWidth;
	float d = mDepth;
	for(UINT i = 0; i < mesh->GetNumVertices(); ++i)
	{
		// We store the grid vertices in a linear array, but we can
		// convert the linear array index to an (r, c) matrix index.
		int r = i / mVertCols;
		int c = i % mVertCols;

		v[i].pos   = verts[i];
		v[i].pos.y = mHeightmap(r, c);

		v[i].tex0.x = (v[i].pos.x + (0.5f*w)) / w;
		v[i].tex0.y = (v[i].pos.z - (0.5f*d)) / -d;
	}

	// Write triangle data so we can compute normals.

	DWORD* indexBuffPtr = 0;
	THR(mesh->LockIndexBuffer(0, (void**)&indexBuffPtr));
	for(UINT i = 0; i < mesh->GetNumFaces(); ++i)
	{
		indexBuffPtr[i*3+0] = indices[i*3+0];
		indexBuffPtr[i*3+1] = indices[i*3+1];
		indexBuffPtr[i*3+2] = indices[i*3+2];
	}
	THR(mesh->UnlockIndexBuffer());

	// Compute Vertex Normals.
	THR(D3DXComputeNormals(mesh, 0));

	
	//===============================================================
	// Now break the grid up into subgrid meshes.

	// Find out the number of subgrids we'll have.  For example, if
	// m = 513, n = 257, SUBGRID_VERT_ROWS = SUBGRID_VERT_COLS = 33,
	// then subGridRows = 512/32 = 16 and sibGridCols = 256/32 = 8.
	int subGridRows = (mVertRows-1) / (SubGrid::NUM_ROWS-1);
	int subGridCols = (mVertCols-1) / (SubGrid::NUM_COLS-1);

	for(int r = 0; r < subGridRows; ++r)
	{
		for(int c = 0; c < subGridCols; ++c)
		{
			// Rectangle that indicates (via matrix indices ij) the
			// portion of grid vertices to use for this subgrid.
			RECT R = 
			{
					c * (SubGrid::NUM_COLS-1),
					r * (SubGrid::NUM_ROWS-1),
				(c+1) * (SubGrid::NUM_COLS-1),
				(r+1) * (SubGrid::NUM_ROWS-1)
			};

			buildSubGridMesh(R, v); 
		}
	}

	THR(mesh->UnlockVertexBuffer());

	SafeRelease(mesh); // Done with global mesh.
}

void Terrain::buildSubGridMesh(RECT& R, VertexPNT* gridVerts)
{
	//===============================================================
	// Get indices for subgrid (we don't use the verts here--the verts
	// are given by the parameter gridVerts).

	std::vector<D3DXVECTOR3> tempVerts;
	std::vector<DWORD> tempIndices;
	GenTriGrid(SubGrid::NUM_ROWS, SubGrid::NUM_COLS, mDX, mDZ, 
		D3DXVECTOR3(0.0f, 0.0f, 0.0f), tempVerts, tempIndices);

	ID3DXMesh* subMesh = 0;
	D3DVERTEXELEMENT9 elems[MAX_FVF_DECL_SIZE];
	UINT numElems = 0;
	THR(VertexPNT::Decl->GetDeclaration(elems, &numElems));
	THR(D3DXCreateMesh(SubGrid::NUM_TRIS, SubGrid::NUM_VERTS, 
		D3DXMESH_MANAGED, elems, m_pDevice, &subMesh));


	//===============================================================
	// Build Vertex Buffer.  Copy rectangle of vertices from the
	// grid into the subgrid structure.
	VertexPNT* v = 0;
	THR(subMesh->LockVertexBuffer(0, (void**)&v));
	int k = 0;
	for(int i = R.top; i <= R.bottom; ++i)
	{
		for(int j = R.left; j <= R.right; ++j)
		{
			v[k++] = gridVerts[i*mVertCols+j];
		}
	}

	//===============================================================
	// Compute the bounding box before unlocking the vertex buffer.
	AABB bndBox;
	THR(D3DXComputeBoundingBox((D3DXVECTOR3*)v, subMesh->GetNumVertices(), 
		sizeof(VertexPNT), &bndBox.minPt, &bndBox.maxPt));

	THR(subMesh->UnlockVertexBuffer());


	//===============================================================
	// Build Index and Attribute Buffer.
	WORD* indices  = 0;
	DWORD* attBuff = 0;
	THR(subMesh->LockIndexBuffer(0, (void**)&indices));
	THR(subMesh->LockAttributeBuffer(0, &attBuff));
	for(int i = 0; i < SubGrid::NUM_TRIS; ++i)
	{
		indices[i*3+0] = (WORD)tempIndices[i*3+0];
		indices[i*3+1] = (WORD)tempIndices[i*3+1];
		indices[i*3+2] = (WORD)tempIndices[i*3+2];

		attBuff[i] = 0; // All in subset 0.
	}
	THR(subMesh->UnlockIndexBuffer());
	THR(subMesh->UnlockAttributeBuffer());


	//===============================================================
	// Optimize for the vertex cache and build attribute table.
	DWORD* adj = new DWORD[subMesh->GetNumFaces()*3];
	THR(subMesh->GenerateAdjacency(0.001f, adj));
	THR(subMesh->OptimizeInplace(D3DXMESHOPT_VERTEXCACHE|D3DXMESHOPT_ATTRSORT,
		adj, 0, 0, 0));
	delete[] adj;

	
	//===============================================================
	// Save the mesh and bounding box.
	SubGrid g;
	g.mesh = subMesh;
	g.box  = bndBox;
	mSubGrids.push_back(g);
}

void Terrain::buildEffect()
{
	ID3DXBuffer* errors = 0;
	WCHAR str[MAX_PATH];
	DWORD dwShaderFlags = 0;

	THR( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"Terrain.fx" ) );
	THR(D3DXCreateEffectFromFile(m_pDevice, str,
		0, 0, dwShaderFlags, 0, &mFX, &errors));
	if( errors )
		MessageBox(0, (LPCWSTR)errors->GetBufferPointer(), 0, 0);

	mhTech      = mFX->GetTechniqueByName("TerrainTech");
	mhViewProj  = mFX->GetParameterByName(0, "gViewProj");
	mhDirToSunW = mFX->GetParameterByName(0, "gDirToSunW");
	mhTex0      = mFX->GetParameterByName(0, "gTex0");
	mhTex1      = mFX->GetParameterByName(0, "gTex1");
	mhTex2      = mFX->GetParameterByName(0, "gTex2");
	mhBlendMap  = mFX->GetParameterByName(0, "gBlendMap");

	THR(mFX->SetTexture(mhTex0, mTex0));
	THR(mFX->SetTexture(mhTex1, mTex1));
	THR(mFX->SetTexture(mhTex2, mTex2));
	THR(mFX->SetTexture(mhBlendMap, mBlendMap));
}