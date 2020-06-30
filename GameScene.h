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
//--------------------------------------------------------------------------------------
// DXUT was adapted from the Microsoft DirectX SDK(November 2008)
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// The skybox is free downloaded from :
//   http://en.pudn.com/downloads119/sourcecode/others/detail508412_en.html
//-------------------------------------------------------------------------------------



#ifndef __GAMESCENE_H__
#define __GAMESCENE_H__


#include <d3d9.h>
#include <d3dx9.h>

#include "Terrain.h"

class Terrain;

struct TerrainPara
{
	D3DXVECTOR3			vWindVelocity;
	D3DXVECTOR3			vSunlightDir;
	D3DXCOLOR			cSunColor;
};


struct SkyboxPara
{
	float		fLength;
	float		fWidth;
	float		fHigh;
	float		fCellSize;
	float		fEvolvingSpeed;
	D3DXVECTOR3 vCloudPos;
	WCHAR		szTextureFile[MAX_PATH];
};


class CGameScene
{
public:
	float				m_fLength, m_fWidth, m_fHigh;// actual size of cloud in world space
	float				m_fCellSize; // size of cell in world space.
	UINT				m_uLength, m_uWidth, m_uHigh;
	D3DXVECTOR3			m_vCloudPos; // the coordinates of the left-down corner 
	D3DXVECTOR3			m_vViewpoint;
	D3DXVECTOR3			m_vSunlightDir;
	D3DXCOLOR			m_cSunColor;
	WCHAR				m_szTextureFile[MAX_PATH];
	LPDIRECT3DTEXTURE9	m_pTexture; // the cloud texture
	double				m_fTimeA;
	float				m_fEvolvingSpeed;
	LPDIRECT3DDEVICE9	m_pDevice;
	D3DPRESENT_PARAMETERS md3dPP;
    int                 m_iSun[2];

    void updateTerrain();	
	void updateSkybox();
    void Rotate(float fElapsedTime);


private:

	Terrain*	m_Terrain;
	
public:
	CGameScene();
	~CGameScene();
	bool Setup( LPDIRECT3DDEVICE9 pDevice);
	void onLostDevice();
	void onResetDevice();
	void onDestroyDevice();
	void Render(double fTime, float fElapsedTime);
	void Keydown(WPARAM wParam);
	void MouseMove(int xPos, int yPos, WPARAM wParam);
	
};

#endif // #ifndef __GAMESCENE_H__
