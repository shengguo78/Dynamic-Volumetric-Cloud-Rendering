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
// Vertex.cpp by Frank Luna (C) 2004 All Rights Reserved.
//=============================================================================

#include "DXUT.h"
#include "Vertex.h"
#include "d3dUtil.h"

using namespace std;

// Initialize static variables.
IDirect3DVertexDeclaration9* VertexPos::Decl = 0;
IDirect3DVertexDeclaration9* VertexCol::Decl = 0;
IDirect3DVertexDeclaration9* VertexPN::Decl  = 0;
IDirect3DVertexDeclaration9* VertexPNT::Decl = 0;
IDirect3DVertexDeclaration9* Particle::Decl  = 0;

void InitAllVertexDeclarations()
{
	//===============================================================
	// VertexPos

	D3DVERTEXELEMENT9 VertexPosElements[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		D3DDECL_END()
	};	
	THR(gd3dDevice->CreateVertexDeclaration(VertexPosElements, &VertexPos::Decl));

	//===============================================================
	// VertexCol

	D3DVERTEXELEMENT9 VertexColElements[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
		D3DDECL_END()
	};	
	THR(gd3dDevice->CreateVertexDeclaration(VertexColElements, &VertexCol::Decl));

	//===============================================================
	// VertexPN

	D3DVERTEXELEMENT9 VertexPNElements[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		D3DDECL_END()
	};	
	THR(gd3dDevice->CreateVertexDeclaration(VertexPNElements, &VertexPN::Decl));

	//===============================================================
	// VertexPNT

	D3DVERTEXELEMENT9 VertexPNTElements[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};	
	THR(gd3dDevice->CreateVertexDeclaration(VertexPNTElements, &VertexPNT::Decl));


  //===============================================================
  // Particle

  D3DVERTEXELEMENT9 ParticleElements[] = 
  {
    {0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
    {0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
    {0, 24, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
    {0, 28, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2},
    {0, 32, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3},
    {0, 36, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4},
    {0, 40, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
    D3DDECL_END()
  };	
  THR(gd3dDevice->CreateVertexDeclaration(ParticleElements, &Particle::Decl));
}

void DestroyAllVertexDeclarations()
{
	SafeRelease(VertexPos::Decl);
	SafeRelease(VertexCol::Decl);
	SafeRelease(VertexPN::Decl);
	SafeRelease(VertexPNT::Decl);
	SafeRelease(Particle::Decl);
}