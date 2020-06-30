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
// Vertex.h by Frank Luna (C) 2004 All Rights Reserved.
//=============================================================================

#include <d3dx9.h>

#ifndef VERTEX_H
#define VERTEX_H

// Call in constructor and destructor, respectively, of derived application class.
void InitAllVertexDeclarations();
void DestroyAllVertexDeclarations();


//===============================================================
struct VertexPos
{
	VertexPos():pos(0.0f, 0.0f, 0.0f){}
	VertexPos(float x, float y, float z):pos(x,y,z){}
	VertexPos(const D3DXVECTOR3& v):pos(v){}

	D3DXVECTOR3 pos;
	static IDirect3DVertexDeclaration9* Decl;
};

//===============================================================
struct VertexCol
{
	VertexCol():pos(0.0f, 0.0f, 0.0f),col(0x00000000){}
	VertexCol(float x, float y, float z, D3DCOLOR c):pos(x,y,z), col(c){}
	VertexCol(const D3DXVECTOR3& v, D3DCOLOR c):pos(v),col(c){}

	D3DXVECTOR3 pos;
	D3DCOLOR    col;
	static IDirect3DVertexDeclaration9* Decl;
};

//===============================================================
struct VertexPN
{
	VertexPN()
		:pos(0.0f, 0.0f, 0.0f),
		normal(0.0f, 0.0f, 0.0f){}
	VertexPN(float x, float y, float z, 
		float nx, float ny, float nz):pos(x,y,z), normal(nx,ny,nz){}
	VertexPN(const D3DXVECTOR3& v, const D3DXVECTOR3& n)
		:pos(v),normal(n){}

	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	static IDirect3DVertexDeclaration9* Decl;
};

//===============================================================
struct VertexPNT
{
	VertexPNT()
		:pos(0.0f, 0.0f, 0.0f),
		normal(0.0f, 0.0f, 0.0f),
		tex0(0.0f, 0.0f){}
	VertexPNT(float x, float y, float z, 
		float nx, float ny, float nz,
		float u, float v):pos(x,y,z), normal(nx,ny,nz), tex0(u,v){}
	VertexPNT(const D3DXVECTOR3& v, const D3DXVECTOR3& n, const D3DXVECTOR2& uv)
		:pos(v),normal(n), tex0(uv){}

	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 tex0;

	static IDirect3DVertexDeclaration9* Decl;
};

struct Particle
{
  D3DXVECTOR3 initialPos;
  D3DXVECTOR3 initialVelocity;
  float       initialSize; // In pixels.
  float       initialTime;
  float       lifeTime;
  float       mass;
  D3DCOLOR    initialColor;

  Particle(const D3DXVECTOR3& p, const D3DCOLOR& c) : initialPos(p), initialColor(c) {}


  static IDirect3DVertexDeclaration9* Decl;
};
#endif // VERTEX_H
