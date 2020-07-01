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
// The skybox is free downloaded form :
//   http://en.pudn.com/downloads119/sourcecode/others/detail508412_en.html
//-------------------------------------------------------------------------------------





matrix  g_mWorldView;
matrix  g_mProj;
texture g_txScene;

sampler2D g_samSkyBox =
sampler_state
{
    Texture = <g_txScene>;
    AddressU = CLAMP;
    AddressV = CLAMP;
    MinFilter = Point;
    MagFilter = Linear;
    MipFilter = Linear;
};


//-----------------------------------------------------------------------------
// Renders skybox.  Used only for geometry with a 3D texcoord.
void RenderSkyBoxVS( float4 Pos : POSITION,
                     float3 Tex : TEXCOORD0,
                     out float4 oPos : POSITION,
                     out float3 oTex : TEXCOORD0,
                     out float3 oViewPos : TEXCOORD1 )
{
    oViewPos = mul( Pos, g_mWorldView );
    oPos = mul( float4( oViewPos, 1.0f ), g_mProj );

    oTex = Tex;
}


void RenderSkyBoxPS( float3 Tex : TEXCOORD0,
                     float3 ViewPos : TEXCOORD1,
                     out float4 oColor : COLOR0,
                     out float4 oNormal : COLOR1,
                     out float4 oPos : COLOR2 )
{
    //
    // Output pixel color
    //
    oColor = tex2D( g_samSkyBox, Tex );
    oColor.a = 0.8;

    //
    // Output normal has (0, 0, -1) always
    //
    oNormal = float4( 0.0f, 0.0f, -1.0f, 1.0f );

    //
    // Output view position
    //
    oPos = float4( ViewPos, 1.0f );
}

//-----------------------------------------------------------------------------
// Technique: RenderSkyBox
// Desc: Renders using 3D texture coordinates to sample from a cube texture.
//-----------------------------------------------------------------------------
technique RenderSkyBox
{
    pass p0
    {
        VertexShader = compile vs_2_0 RenderSkyBoxVS();
        PixelShader = compile ps_2_0 RenderSkyBoxPS();
        ZEnable = true;
    }
}
