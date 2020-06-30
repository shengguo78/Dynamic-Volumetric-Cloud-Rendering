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

//

#include "DXUT.h"
#include "Camera.h"

#include "GameScene.h"
#include "SDKmesh.h"
#include "SDKmisc.h"

using namespace std;
//#pragma warning( disable : 4996 )

IDirect3DDevice9* gd3dDevice = 0;
extern Camera*  gCamera;
extern bool     g_Rotate;
CDXUTXFileMesh			g_Skybox;                // Skybox mesh
ID3DXEffect*            g_pEffect = NULL;        // D3DX effect interface
D3DXHANDLE            g_hTRenderSkyBox;        // Handle to RenderSkyBox technique


CGameScene::CGameScene()
: m_fLength(0.0f)
{
	
}

CGameScene::~CGameScene()
{
}

void CGameScene::onLostDevice()
{
	g_Skybox.InvalidateDeviceObjects();
	if( g_pEffect )
        g_pEffect->OnLostDevice();
	
	m_Terrain->onLostDevice();
	delete m_Terrain;
	DestroyAllVertexDeclarations();
    SAFE_RELEASE( g_pEffect );
}

void CGameScene::onDestroyDevice()
{
	g_Skybox.Destroy();
	SAFE_RELEASE( g_pEffect );
}

void CGameScene::onResetDevice()
{
	HRESULT hr;
	if( g_pEffect )
    {
        V( g_pEffect->OnResetDevice() );
        g_pEffect->SetMatrix( "g_mProj", &gCamera->proj());
    }

    g_Skybox.RestoreDeviceObjects( m_pDevice );
	gCamera->setLens(D3DX_PI * 0.25f, 1.3333334f, 0.1f, 20000000.0f);
	
	m_Terrain->onResetDevice();
}

void CGameScene::updateTerrain()
{
}

void CGameScene::updateSkybox()
{
}

bool CGameScene::Setup(LPDIRECT3DDEVICE9 pDevice)
{
	HRESULT hr;
	m_pDevice = pDevice;

	gd3dDevice = pDevice;

	WCHAR str[MAX_PATH];
	DWORD dwShaderFlags = 0;
    V( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"Scene.fx" ) );

	V( D3DXCreateEffectFromFile( m_pDevice, str, NULL, NULL, dwShaderFlags, 
                                        NULL, &g_pEffect, NULL ) );

    g_hTRenderSkyBox = g_pEffect->GetTechniqueByName( "RenderSkyBox" );

    if( FAILED( g_Skybox.Create( m_pDevice, L"alley_skybox.x" ) ) )
        return false;

	InitAllVertexDeclarations();

	m_Terrain = new Terrain(m_pDevice,513, 513, 4.0f, 4.0f, 
    L"coastMountain513.raw",  
	L"grass.dds",
    L"dirt.dds",
    L"rock.dds",
    L"blend_coastal.dds",
    1.0f, 0.0f);

	D3DXVECTOR3 toSun(1.0f, 1.0f, 1.0f);
	D3DXVec3Normalize(&toSun, &toSun);
	m_Terrain->setDirToSunW(toSun);


	gCamera->pos() = D3DXVECTOR3(160.0f, 125.0f,-782.0f);
    gCamera->setSpeed(200.0f);

	return true;
}

void CGameScene::Render(double fTime, float fElapsedTime)
{
	HRESULT hr;
	UINT cPass, p;
	LPD3DXMESH pMeshObj;
    D3DXMATRIXA16 mWorldView;


	// Render the skybox as if the camera is at center
    Rotate(fElapsedTime);
    
    
	D3DXMatrixIdentity(&mWorldView);
	D3DXMATRIXA16 mView = gCamera->view();
    /*mView._41 = mView._41 *0;
	mView._42 = mView._42 *0;
	mView._43 = mView._43 *0;*/

    D3DXMatrixScaling( &mWorldView, 1.f, 1.f, 1.f );
    D3DXMatrixMultiply( &mWorldView, &mWorldView, &mView );
    V( g_pEffect->SetMatrix( "g_mWorldView", &mWorldView ) );
    V( g_pEffect->SetMatrix( "g_mProj", &gCamera->proj()));

    pMeshObj = g_Skybox.GetMesh();

    V( g_pEffect->SetTechnique( g_hTRenderSkyBox ) );
    V( g_pEffect->Begin( &cPass, 0 ) );
    for(p = 0; p < cPass; ++p )
    {

        V( g_pEffect->BeginPass( p ) );

        for( DWORD m = 0; m < 6; ++m )
        {
            V( g_pEffect->SetTexture( "g_txScene", g_Skybox.m_pTextures[m] ) );
            V( g_pEffect->CommitChanges() );
            V( pMeshObj->DrawSubset( m ) );
        }

        V( g_pEffect->EndPass() );
    }
    V( g_pEffect->End() );


 	//draw the terrain
     gCamera->update(.01f, m_Terrain, 2.5f,0);
     m_Terrain->draw(); 
}

void CGameScene::Keydown(WPARAM wParam)
{
	switch (wParam) 
	  { 
	  case 'A': // Process the A ARROW key. 
	  case VK_LEFT: // Process the LEFT ARROW key. 
		gCamera->update(.01f, m_Terrain, 2.5f,2);
		break; 

	  case 'D': // Process the D ARROW key. 
	  case VK_RIGHT: // Process the RIGHT ARROW key. 
		gCamera->update(.01f, m_Terrain, 2.5f,4);
		break; 

	  case 'W': // Process the W ARROW key. 
	  case VK_UP:  // Process the UP ARROW key. 
		gCamera->update(.01f, m_Terrain, 2.5f,1);
		break; 

	  case 'S': // Process the S ARROW key. 
	  case VK_DOWN: // Process the DOWN ARROW key. 
		gCamera->update(.01f, m_Terrain, 2.5f,3);
		break; 
 
	  }

}

void CGameScene::MouseMove(int xPos, int yPos, WPARAM wParam)
{
	static int xlast = -1, yLast = -1;
	bool bNoMovement = (xlast == xPos) && (yLast == yPos);
	if ( bNoMovement )
    {
        return;
    }
	else if ( (MK_LBUTTON) & wParam )
    {
		unsigned int flags = static_cast<unsigned>(wParam);
		if (( -1 != xlast ) && ( 0 != (flags & MK_LBUTTON) ))
		{
			gCamera->fly(xPos-xlast,yPos-yLast);
		}
	}
    xlast = xPos;
    yLast = yPos;
}


void CGameScene::Rotate(float fElapsedTime)
{
    if (g_Rotate)
    {

        D3DXVECTOR3 vEyePt(0.0f, 350.0f, 800.0f);
        D3DXVECTOR3 vLookatPt( 0.0f, 350.0f, 0.0f );
        D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );

        

        static float angle = 0.0;
        angle += 0.001f*fElapsedTime;
        if (angle >=4)
            angle = 0.0;
    
        vEyePt.x = -800.0f*sin(angle * 90);
        vEyePt.z = 800.0f*cos(angle * 90);
        gCamera->lookAt(vEyePt, vLookatPt, vUpVec);
    }
}