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
// D3DUtil.cpp by Frank Luna (C) 2004 All Rights Reserved.
//=============================================================================

#include "DXUT.h"
#include "d3dUtil.h"
#include "Vertex.h"

using namespace std;

void GenTriGrid(int numVertRows, int numVertCols,
				float dx, float dz, 
				const D3DXVECTOR3& center, 
				std::vector<D3DXVECTOR3>& verts,
				std::vector<DWORD>& indices)
{
	int numVertices = numVertRows*numVertCols;
	int numCellRows = numVertRows-1;
	int numCellCols = numVertCols-1;

	int numTris = numCellRows*numCellCols*2;

	float width = (float)numCellCols * dx;
	float depth = (float)numCellRows * dz;

	//===========================================
	// Build vertices.

	// We first build the grid geometry centered about the origin and on
	// the xz-plane, row-by-row and in a top-down fashion.  We then translate
	// the grid vertices so that they are centered about the specified 
	// parameter 'center'.

	verts.resize( numVertices );

	// Offsets to translate grid from quadrant 4 to center of 
	// coordinate system.
	float xOffset = -width * 0.5f; 
	float zOffset =  depth * 0.5f;

	int k = 0;
	for(float i = 0; i < numVertRows; ++i)
	{
		for(float j = 0; j < numVertCols; ++j)
		{
			// Negate the depth coordinate to put in quadrant four.  
			// Then offset to center about coordinate system.
			verts[k].x =  j * dx + xOffset;
			verts[k].z = -i * dz + zOffset;
			verts[k].y =  0.0f;

			// Translate so that the center of the grid is at the
			// specified 'center' parameter.
			D3DXMATRIX T;
			D3DXMatrixTranslation(&T, center.x, center.y, center.z);
			D3DXVec3TransformCoord(&verts[k], &verts[k], &T);
			
			++k; // Next vertex
		}
	}

	//===========================================
	// Build indices.

	indices.resize(numTris * 3);
	 
	// Generate indices for each quad.
	k = 0;
	for(DWORD i = 0; i < (DWORD)numCellRows; ++i)
	{
		for(DWORD j = 0; j < (DWORD)numCellCols; ++j)
		{
			indices[k]     =   i   * numVertCols + j;
			indices[k + 1] =   i   * numVertCols + j + 1;
			indices[k + 2] = (i+1) * numVertCols + j;
					
			indices[k + 3] = (i+1) * numVertCols + j;
			indices[k + 4] =   i   * numVertCols + j + 1;
			indices[k + 5] = (i+1) * numVertCols + j + 1;

			// next quad
			k += 6;
		}
	}
}

void LoadXFile(
	LPCWSTR filename, 
	ID3DXMesh** meshOut,
	std::vector<Mtrl>& mtrls, 
	std::vector<IDirect3DTexture9*>& texs)
{
	// Step 1: Load the .x file from file into a system memory mesh.

	ID3DXMesh* meshSys      = 0;
	ID3DXBuffer* adjBuffer  = 0;
	ID3DXBuffer* mtrlBuffer = 0;
	DWORD numMtrls          = 0;

	THR(D3DXLoadMeshFromX(filename, D3DXMESH_SYSTEMMEM, gd3dDevice,
		&adjBuffer,	&mtrlBuffer, 0, &numMtrls, &meshSys));


	// Step 2: Find out if the mesh already has normal info?

	D3DVERTEXELEMENT9 elems[MAX_FVF_DECL_SIZE];
	THR(meshSys->GetDeclaration(elems));
	
	bool hasNormals = false;
	for(int i = 0; i < MAX_FVF_DECL_SIZE; ++i)
	{
		// Did we reach D3DDECL_END() {0xFF,0,D3DDECLTYPE_UNUSED, 0,0,0}?
		if(elems[i].Stream == 0xff )
			break;

		if( elems[i].Type == D3DDECLTYPE_FLOAT3 &&
			elems[i].Usage == D3DDECLUSAGE_NORMAL &&
			elems[i].UsageIndex == 0 )
		{
			hasNormals = true;
			break;
		}
	}


	// Step 3: Change vertex format to VertexPNT.

	D3DVERTEXELEMENT9 elements[64];
	UINT numElements = 0;
	VertexPNT::Decl->GetDeclaration(elements, &numElements);

	ID3DXMesh* temp = 0;
	THR(meshSys->CloneMesh(D3DXMESH_SYSTEMMEM, 
		elements, gd3dDevice, &temp));
	SafeRelease(meshSys);
	meshSys = temp;


	// Step 4: If the mesh did not have normals, generate them.

	if( hasNormals == false)
		THR(D3DXComputeNormals(meshSys, 0));


	// Step 5: Optimize the mesh.

	THR(meshSys->Optimize(D3DXMESH_MANAGED | 
		D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE, 
		(DWORD*)adjBuffer->GetBufferPointer(), 0, 0, 0, meshOut));
	SafeRelease(meshSys); // Done w/ system mesh.
	SafeRelease(adjBuffer); // Done with buffer.

	// Step 6: Extract the materials and load the textures.

	if( mtrlBuffer != 0 && numMtrls != 0 )
	{
		D3DXMATERIAL* d3dxmtrls = (D3DXMATERIAL*)mtrlBuffer->GetBufferPointer();

		for(DWORD i = 0; i < numMtrls; ++i)
		{
			// Save the ith material.  Note that the MatD3D property does not have an ambient
			// value set when its loaded, so just set it to the diffuse value.
			Mtrl m;
			m.ambient   = d3dxmtrls[i].MatD3D.Diffuse;
			m.diffuse   = d3dxmtrls[i].MatD3D.Diffuse;
			m.spec      = d3dxmtrls[i].MatD3D.Specular;
			m.specPower = d3dxmtrls[i].MatD3D.Power;
			mtrls.push_back( m );

			// Check if the ith material has an associative texture
			if( d3dxmtrls[i].pTextureFilename != 0 )
			{
				// Yes, load the texture for the ith subset
				IDirect3DTexture9* tex = 0;
				LPCWSTR texFN = (LPCWSTR)d3dxmtrls[i].pTextureFilename;
				THR(D3DXCreateTextureFromFile(gd3dDevice, texFN, &tex));

				// Save the loaded texture
				texs.push_back( tex );
			}
			else
			{
				// No texture for the ith subset
				texs.push_back( 0 );
			}
		}
	}
	SafeRelease(mtrlBuffer); // done w/ buffer
}

float GetRandomFloat(float a, float b)
{
	if( a >= b ) // bad input
		return a;

	// Get random float in [0, 1] interval.
	float f = (rand()%10001) * 0.0001f;

	return (f*(b-a))+a;
}

void GetRandomVec(D3DXVECTOR3& out)
{
	out.x = GetRandomFloat(-1.0f, 1.0f);
	out.y = GetRandomFloat(-1.0f, 1.0f);
	out.z = GetRandomFloat(-1.0f, 1.0f);

	// Project onto unit sphere.
	D3DXVec3Normalize(&out, &out);
}