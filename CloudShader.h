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



#ifndef __CloudShader_H__
#define __CloudShader_H__

#pragma warning( push, 3 )
#pragma warning(disable:4786 4788)
#include <vector>
#pragma warning( pop )
#pragma warning(disable:4786 4788)

#include "CloudParticle.h"

class CVolumetricCloud;


struct PARTICLE_RENDERSTRUCT{
    D3DXVECTOR3 vecPos; // Coordinates of particle
    D3DXVECTOR2 vecOffset; // Vertex coordinate offsets
    float u, v; // Texture coordinates
    DWORD Diffuse; // Diffuse color of particle
};

class CCloudShader{
private:
    LPDIRECT3DDEVICE9		m_pDevice;
    CVolumetricCloud 		*m_pVolumetricCloud; 
    
    float					m_fParticleSize; // Size of particle
    LPDIRECT3DVERTEXBUFFER9 m_pVB;      //Vertex buffer
    IDirect3DIndexBuffer9*	m_pIB;
    ID3DXEffect*            m_pEffect;  // D3DX effect interface
    LPDIRECT3DTEXTURE9		m_pTexture; // the cloud texture
    
    IDirect3DVertexDeclaration9* m_Decl;
    
    int						m_iNumParticlesPerBuffer;

public:
    CCloudShader();
    ~CCloudShader();
    bool Setup( CVolumetricCloud* VolumetricCloud, float fParticleSize, int iNumParticlesPerBuffer = 32);	
    void Cleanup();
    inline void SetParticleSize(float fSize){ m_fParticleSize = fSize;}	
    void SetParticleRS();
    void UnSetParticleRS();
    void Render();
    static inline DWORD FToW(float f){return *((DWORD*)&f);}
};


#endif // #ifndef __CloudShader_H__


