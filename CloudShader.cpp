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



#include "DXUT.h"
#include "SDKmisc.h"
#include "CloudShader.h"
#include "VolumetricCloud.h"
#include "Camera.h"

using namespace std;

extern Camera*  gCamera;
extern bool     g_Rotate;

CCloudShader::CCloudShader()
{
}
	
CCloudShader::~CCloudShader()
{
}

bool CCloudShader::Setup( CVolumetricCloud* pVolumetricCloud, float fParticleSize, int iNumParticlesPerBuffer )
{
	HRESULT hr;
	m_pVolumetricCloud = pVolumetricCloud;
	m_pDevice = pVolumetricCloud->m_pDevice;
	
	m_fParticleSize = fParticleSize;
	m_iNumParticlesPerBuffer = iNumParticlesPerBuffer;
	
    DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE;
    #ifdef DEBUG_VS
        dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
    #endif
    #ifdef DEBUG_PS
        dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
    #endif

    // Read the D3DX effect file
    WCHAR str[MAX_PATH];
    V( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"Particle.fx" ) );
    V( D3DXCreateEffectFromFile( m_pDevice, str, NULL, NULL, dwShaderFlags, NULL, &m_pEffect, NULL ) );
	m_pEffect->SetTechnique( "RenderCloud" );

    // Create the mesh texture from a file
    V( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, m_pVolumetricCloud->m_szTextureFile ) );
	V( D3DXCreateTextureFromFile( m_pDevice, str, &m_pTexture));
    V( m_pEffect->SetTexture( "g_txDiffuse", m_pTexture) );


	D3DVERTEXELEMENT9 Particle_Decl[ ] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION, 1 },
		{ 0, 20, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 28, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_COLOR, 0 },
		D3DDECL_END()
	};

	if(FAILED( m_pDevice->CreateVertexDeclaration(Particle_Decl, &m_Decl)))
	{
		return false;
	}

	if(FAILED( m_pDevice->CreateVertexBuffer(
			4*m_iNumParticlesPerBuffer*sizeof(PARTICLE_RENDERSTRUCT),
			D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY,0,
			D3DPOOL_DEFAULT,&m_pVB,NULL)))
	{
		return false;
	}

	if( FAILED( m_pDevice->CreateIndexBuffer( 6*m_iNumParticlesPerBuffer * sizeof(short),\
											 D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,D3DPOOL_DEFAULT, &m_pIB, 0)))
    {
        return false;
    }

	unsigned short *IBPtr;
	m_pIB->Lock(0, 0, (void**)&IBPtr, 0);
	for(int i=0;i<m_iNumParticlesPerBuffer;i++) { // # particles
		IBPtr[i*6+0] = (unsigned short)i * 4 + 0;
		IBPtr[i*6+1] = (unsigned short)i * 4 + 1;
		IBPtr[i*6+2] = (unsigned short)i * 4 + 2;
		IBPtr[i*6+3] = (unsigned short)i * 4 + 3;
		IBPtr[i*6+4] = (unsigned short)i * 4 + 2;
		IBPtr[i*6+5] = (unsigned short)i * 4 + 1;
	}
	m_pIB->Unlock();

	return true;

}


void CCloudShader::Cleanup(){
	if(m_pVB)
	{
		m_pVB->Release();
		m_pVB = NULL;
	}
	if(m_pIB)
	{
		m_pIB->Release();
		m_pIB = NULL;

	}
	
	if(m_Decl)
	{
		m_Decl->Release();
		m_Decl = NULL;
	}
	if(m_pEffect)
	{
		m_pEffect->Release();
		m_pEffect = NULL;
	}
	if(m_pTexture)
	{
		m_pTexture->Release();
		m_pTexture = NULL;
	}
}

void CCloudShader::Render()
{
    HRESULT hr;
    UINT iPass, cPasses;	

    D3DXMATRIXA16 mView = gCamera->view();

    D3DXMATRIXA16 mProj = gCamera->proj();


	D3DXMATRIX matViewProj = mView * mProj;
	m_pEffect->SetMatrix( "g_mViewProj", & matViewProj );

	// Get normalized right/up vectors from view transformation
	D3DXVECTOR4 vecRight(mView._11, mView._21, mView._31, 0.0f);
    D3DXVECTOR4 vecUp(mView._12, mView._22, mView._32, 0.0f);
    
	// Right vector is 1st columnn
	D3DXVec4Normalize(&vecRight, &vecRight);
	// Up vector is 2nd column
	D3DXVec4Normalize(&vecUp, &vecUp);

	m_pEffect->SetVector("g_vNormRight",&vecRight );
	m_pEffect->SetVector("g_vNormUp",&vecUp );


	m_pDevice->SetVertexDeclaration(m_Decl);
	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(PARTICLE_RENDERSTRUCT));
	m_pDevice->SetIndices(m_pIB);

    //Number of particles filled in
	int uNumInBlock=0;

	PARTICLE_RENDERSTRUCT* pVBData=NULL;
	m_pVB->Lock(0, 0, (void**)&pVBData, D3DLOCK_DISCARD);

    //Render thread, get data from last buffer
	CParticleEnumerator Enumerator( & m_pVolumetricCloud->m_ParticlePool );
	CloudParticle *pCurParticle = Enumerator.NextParticleFromLastBuffer();
	while( pCurParticle )
	{
		if( pCurParticle->m_bVisible )
		{
			pVBData[0].vecPos = *( pCurParticle->GetPositionFromLastBuffer() );
			pVBData[0].vecOffset = D3DXVECTOR2(-m_fParticleSize,m_fParticleSize);
			pVBData[0].Diffuse = pCurParticle->m_cScatteringColor;
			pVBData[0].u = 0.0f; pVBData[0].v = 0.0f;
			
			pVBData[1].vecPos =  *( pCurParticle->GetPositionFromLastBuffer() );
			pVBData[1].vecOffset = D3DXVECTOR2(m_fParticleSize, m_fParticleSize);
			pVBData[1].Diffuse = pCurParticle->m_cScatteringColor;
			pVBData[1].u = 1.0f; pVBData[1].v = 0.0f;

			pVBData[2].vecPos =  *( pCurParticle->GetPositionFromLastBuffer() );
			pVBData[2].vecOffset = D3DXVECTOR2(-m_fParticleSize, -m_fParticleSize);
			pVBData[2].Diffuse = pCurParticle->m_cScatteringColor;
			pVBData[2].u = 0.0f; pVBData[2].v = 1.0f;

			pVBData[3].vecPos =  *( pCurParticle->GetPositionFromLastBuffer() );
			pVBData[3].vecOffset = D3DXVECTOR2(m_fParticleSize, -m_fParticleSize);
			pVBData[3].Diffuse = pCurParticle->m_cScatteringColor;
			pVBData[3].u = 1.0f; pVBData[3].v = 1.0f;

			//// Go to next four vertices
			pVBData += 4;
			// Increase vertex count and flush buffer if full
			uNumInBlock++;
			if(uNumInBlock >= m_iNumParticlesPerBuffer) 
			{
				// Unlock buffer and render polygons
				m_pVB->Unlock();

				// Apply the technique contained in the effect 
				V( m_pEffect->Begin(&cPasses, 0) );

				for (iPass = 0; iPass < cPasses; iPass++)
				{
					V( m_pEffect->BeginPass(iPass) );

					m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,4*m_iNumParticlesPerBuffer,0,2*m_iNumParticlesPerBuffer);
		            
					V( m_pEffect->EndPass() );
				}
				V( m_pEffect->End() );

				// Lock vertex buffer again
				m_pVB->Lock(0, 0, (void**)&pVBData, D3DLOCK_DISCARD);
				// Clear vertex count
				uNumInBlock=0;
			}
		}
		// Go to next particle
		pCurParticle = Enumerator.NextParticleFromLastBuffer();
	}

	// Unlock vertex buffer
	m_pVB->Unlock();
	// Render any polygons left
	if(uNumInBlock)
	{

		// Apply the technique contained in the effect 
		V( m_pEffect->Begin(&cPasses, 0) );

		for (iPass = 0; iPass < cPasses; iPass++)
		{
			V( m_pEffect->BeginPass(iPass) );

			m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,4*uNumInBlock,0,2*uNumInBlock);
            
			V( m_pEffect->EndPass() );
		}
		V( m_pEffect->End() );
	}
}




	
			
			










