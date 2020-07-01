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

//-----------------------------------------------------------------------------------------
// Input and Output Structures
//-----------------------------------------------------------------------------------------
struct VS_INPUT
{
    float3 Pos : POSITION0;
    float2 Offset: POSITION1;
    float4 Color : COLOR0;
    float2 Tex: TEXCOORD0;
};


struct PS_INPUT
{
    float4 Pos : POSITION;
    float4 Color : COLOR0;
    float2 Tex: TEXCOORD0;
};


//-----------------------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------------------
float4x4 g_mViewProj;
float4 g_vNormRight;
float4 g_vNormUp;
texture g_txDiffuse;

//-----------------------------------------------------------------------------------------
// Textures and Samplers
//-----------------------------------------------------------------------------------------
sampler	g_samDiffuse =
sampler_state
{
    Texture = (g_txDiffuse);
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};
//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT CloudVS( VS_INPUT input )
{
  PS_INPUT output = (PS_INPUT)0;
	
  //Scale the corner's offsets by the right and up vectors
  output.Pos = float4(input.Pos,1.0f) + input.Offset.x*g_vNormRight + input.Offset.y*g_vNormUp;
  output.Pos.w = 1.0f;
  output.Pos = mul( output.Pos, g_mViewProj );    
  output.Color  = input.Color;
  output.Tex = input.Tex;
  
  return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 CloudPS(PS_INPUT input) : COLOR0
{
	return tex2D( g_samDiffuse, input.Tex ) * input.Color;
}


//--------------------------------------------------------------------------------------
technique RenderCloud
{  
    pass P0 // cloud
    {
        VertexShader = compile vs_2_0 CloudVS();
        PixelShader  = compile ps_2_0 CloudPS();
        
        AlphaBlendEnable = true;
		SrcBlend     = ONE;
		DestBlend    = INVSRCALPHA;
			    
	    // Don't write to depth buffer; that is, particles don't obscure
	    // the objects behind them.
	    ZWriteEnable = false;
    }   
}


