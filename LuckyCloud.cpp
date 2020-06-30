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
#include "DXUTgui.h"
#include "SDKmisc.h"

#include "VolumetricCloud.h"
#include "GameScene.h"
#include "DXUTcamera.h"
#include "TaskManager.h"
#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"
#include "tbb/task_scheduler_init.h"

using namespace std;

extern bool     g_Rotate;
extern IDirect3DDevice9* gd3dDevice;
//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
ID3DXFont*              g_pFont = NULL;         // Font for drawing text
ID3DXSprite*            g_pTextSprite = NULL;   // Sprite for batching draw text calls
const unsigned int      gNumCloud = 16;         //In this application, we set the number of cloud as 16.
CVolumetricCloud        g_VolumetricCloud[gNumCloud];
D3DXVECTOR3*            g_pvViewpoint;

std::vector< CVolumetricCloud* >    g_v_pClouds;

CDXUTDialogResourceManager   g_DialogResourceManager;   // Manager for shared resources of dialogs
CDXUTDialog                  g_ParametersHUD;            // Dialog for standard controls
CDXUTDialog                  g_HUD;                     // Dialog for standard controls
CDXUTDialog                  g_DialogLabels;            // Labels within the current scene

TaskManager::JobResult*      g_JobResult = NULL;

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           2
#define IDC_CHANGEDEVICE        3
#define IDC_MULTISAMPLE_TYPE    4
#define IDC_MULTISAMPLE_QUALITY 5
#define IDC_SSE                 6 
#define IDC_FILTERGROUP         7
#define IDC_FILTER_POINT        8
#define IDC_FILTER_LINEAR       9
#define IDC_FILTER_ANISOTROPIC  10
#define IDC_NUMOFTHREAD         11
#define IDC_CELL_SIZE           12
#define IDC_EVOLVING_SPEED      13
#define IDC_CELL_SIZE_STATIC    14
#define IDC_EVOLVING_SPEED_STATIC   15
#define IDC_ROTATION_DEGREES    16
#define IDC_DEGREES             17
#define IDC_APPLYCHANGE         18
#define IDC_PAUSE               19
#define IDC_ROTATE              20
#define IDC_SUNCOLORR_STATIC    21
#define IDC_SUNCOLORR           22

#define IDC_SUNCOLORG_STATIC     23
#define IDC_SUNCOLORG            24

#define IDC_SUNCOLORB_STATIC     25
#define IDC_SUNCOLORB            26
#define IDC_SUNCOLORINTENSITY_STATIC     27
#define IDC_SUNCOLORINTENSITY            28
#define IDC_SUNCOLORUPDATEINTERVAL_STATIC     29
#define IDC_SUNCOLORUPDATEINTERVAL            30

#define IDC_WINDVELOCITY_STATIC     31
#define IDC_WINDVELOCITY            32
#define IDC_ASYNC                   33
#define IDC_PARAMETERS              34


CGameScene          g_GameScene;
bool                g_SSE4On = false;
bool                g_SupportSSE4 = false;
bool                g_EnableAsync = false;
bool                g_PauseEvolving = false;
bool                g_ShowParameters = false;
int                 g_iColorUpdateInterval = 1;

int                 gNumThreads = 4;

struct CloudPosSize
{
    float       x; //pos.x
    float       y; //pos.y
    float       z; //pos.z
    float       l; //length
    float       w; //width
    float       h; //height
};

struct SunColor
{
    float       r;
    float       g;
    float       b;    
};

SunColor    g_SunColor = {1.0f,1.0f,1.0f};
float       g_fSunColorIntensity = 1.4f;
float       g_fWindVelocity = 40.f;

#define CLOUD_POSY 350.f
#define CLOUD_POSY1 450.f
#define CLOUD_POSY2 300.f

float g_CellSize = 12.f;
float g_CloudEvolvingSpeed = 0.8f;

CloudPosSize g_Cloud[]={
    {-800.0f, CLOUD_POSY, 200.0f, 800.f,300.f,80.f},
    {-600.0f, CLOUD_POSY1, 50.0f, 900.f,600.f,100.f },
    {-350.0f, CLOUD_POSY, 300.0f, 400.f,200.f,120.f},
    {-150.0f, CLOUD_POSY2, 0.0f, 400.f,400.f,80.f},
    {-900.0f, CLOUD_POSY2, -400.0f, 600.f,700.f,110.f},
    {-750.0f, CLOUD_POSY, -350.0f, 600.f,200.f,130.f},
    {-300.0f, CLOUD_POSY1, -100.0f, 300.f,200.f,80.f},
    {-250.0f, CLOUD_POSY, -300.0f, 900.f,200.f,70.f},
    {-100.0f, CLOUD_POSY2, -250.0f, 400.f,200.f,140.f},

    {-900.0f, CLOUD_POSY1, -800.0f, 600.f,400.f,80.f},

    {-800.0f, CLOUD_POSY2, -700.0f, 400.f,200.f,90.f},
    {-700.0f, CLOUD_POSY, -680.0f, 600.f,150.f,80.f},
    {-750.0f, CLOUD_POSY1, -800.0f, 400.f,200.f,120.f},
    {-450.0f, CLOUD_POSY, -750.0f, 400.f,600.f,80.f},
    {-225.0f, CLOUD_POSY1,-700.0f, 400.f,250.f,110.f},
    {-200.0f, CLOUD_POSY, -900.0f, 400.f,100.f,80.f},
};


//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
bool    CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
bool    CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext );
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
void    CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext );
void    CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
void    CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );
void    CALLBACK OnLostDevice( void* pUserContext );
void    CALLBACK OnDestroyDevice( void* pUserContext );
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext );

void    InitApp();
void    InitCloud(bool bNeedClean);
void    Rotate();
void    RenderText();

struct UpdateCloud {
    double  m_fTime;
    void operator()( const tbb::blocked_range<int>& range ) const {
        for(int i=range.begin(); i!= range.end(); i++ ) {

                g_VolumetricCloud[i].AdvanceTime(m_fTime, g_iColorUpdateInterval);
        }
    }
     UpdateCloud(double fTime):
     m_fTime(fTime)
     {}
};


UpdateCloud*        g_pUpdateCloud;

static void RunUpdate()
{

    //we can simply use granularity to control the number of work threads, or
    //set the granularity to 1 and control the thread number through Takemanager's init function
    tbb::parallel_for( tbb::blocked_range<int>( 0, gNumCloud, 1),//gNumCloud/gNumThreads),
        *g_pUpdateCloud);
}

bool  CompareViewDistance2( CVolumetricCloud* pCloud1, CVolumetricCloud* pCloud2)
{
    return ( pCloud1->GetViewDistance() > pCloud2->GetViewDistance() );
}

//--------------------------------------------------------------------------------------
// Rejects any devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
                                 D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    // Typically want to skip backbuffer formats that don't support alpha blending
    IDirect3D9* pD3D = DXUTGetD3D9Object(); 
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
        AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
        D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
        return false;


    // Need to support A8R8G8B8 textures
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
        AdapterFormat, 0,
        D3DRTYPE_TEXTURE, D3DFMT_A8R8G8B8 ) ) )
        return false;

    return true;
}


//--------------------------------------------------------------------------------------
// Before a device is created, modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
    assert( DXUT_D3D9_DEVICE == pDeviceSettings->ver );

    HRESULT hr;
    IDirect3D9* pD3D = DXUTGetD3D9Object();
    D3DCAPS9 caps;

    // Turn vsync off
    pDeviceSettings->d3d9.pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    V( pD3D->GetDeviceCaps( pDeviceSettings->d3d9.AdapterOrdinal,
                            pDeviceSettings->d3d9.DeviceType,
                            &caps ) );

    // If device doesn't support HW T&L or doesn't support 1.1 vertex shaders in HW
    // then switch to SWVP.
    if( ( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
         caps.VertexShaderVersion < D3DVS_VERSION(1,1) )
    {
        pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }

     // If the hardware cannot do vertex blending, use software vertex processing.
    if( caps.MaxVertexBlendMatrices < 2 )
        pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

    // If using hardware vertex processing, change to mixed vertex processing
    // so there is a fallback.
    if( pDeviceSettings->d3d9.BehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
        pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_MIXED_VERTEXPROCESSING;

    // Debugging vertex shaders requires either REF or software vertex processing
    // and debugging pixel shaders requires REF.
#ifdef DEBUG_VS
    if( pDeviceSettings->d3d9.DeviceType != D3DDEVTYPE_REF )
    {
        pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
        pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_PUREDEVICE;
        pDeviceSettings->d3d9.BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }
#endif
#ifdef DEBUG_PS
    pDeviceSettings->d3d9.DeviceType = D3DDEVTYPE_REF;
#endif
    // For the first device created if its a REF device, optionally display a warning dialog box
    static bool s_bFirstTime = true;
    if( s_bFirstTime )
    {
        s_bFirstTime = false;
        if( pDeviceSettings->d3d9.DeviceType == D3DDEVTYPE_REF )
            DXUTDisplaySwitchingToREFWarning( pDeviceSettings->ver );
    }

    return true;
}


//--------------------------------------------------------------------------------------
// Create any D3DPOOL_MANAGED resources here 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;

    V_RETURN( g_DialogResourceManager.OnD3D9CreateDevice( pd3dDevice ) );

    // Initialize the font
    V_RETURN( D3DXCreateFont( pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        L"Arial", &g_pFont ) );

    TaskManager::getTaskManager()->init(gNumThreads+1);
    g_pUpdateCloud = new UpdateCloud(0.0);

    g_JobResult = TaskManager::getTaskManager()->getJobResult();

    return S_OK;
}

//--------------------------------------------------------------------------------------
// Create any D3DPOOL_DEFAULT resources here 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, 
                               const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;

    V_RETURN( g_DialogResourceManager.OnD3D9ResetDevice() );

    g_ParametersHUD.SetLocation( pBackBufferSurfaceDesc->Width-250, 10 );
    g_ParametersHUD.SetSize( 250, 250 );

    g_HUD.SetLocation( pBackBufferSurfaceDesc->Width-250, 0 );
    g_HUD.SetSize( 10, 10 );

    if( g_pFont )
        V_RETURN( g_pFont->OnResetDevice() );

    V_RETURN( D3DXCreateSprite( pd3dDevice, &g_pTextSprite ) );

    gd3dDevice = pd3dDevice;

    // Setup the WVP matrix
    {
        // For our world matrix, we will just rotate the object about the y-axis.
        D3DXMATRIXA16 matWorld;

        // Set up the rotation matrix to generate 1 full rotation (2*PI radians) 
        // every 1000 ms. To avoid the loss of precision inherent in very high 
        // floating point numbers, the system time is modulated by the rotation 
        // period before conversion to a radian angle.
        D3DXMatrixIdentity( &matWorld );
        pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

        // Set up our view matrix. A view matrix can be defined given an eye point,
        // a point to lookat, and a direction for which way is up. Here, we set the
        // eye five units back along the z-axis and up three units, look at the
        // origin, and define "up" to be in the y-direction.

        g_pvViewpoint = new D3DXVECTOR3( 150.0f, 100.0f, -120.0f );
        D3DXVECTOR3 vLookatPt( 133.0f, 0.0f, 200.0f );

        D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
        D3DXMATRIXA16 matView;
        D3DXMatrixLookAtLH( &matView, g_pvViewpoint, &vLookatPt, &vUpVec );
        pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

        // For the projection matrix, we set up a perspective transform (which
        // transforms geometry from 3D view space to 2D viewport space, with
        // a perspective divide making objects smaller in the distance). To build
        // a perpsective transform, we need the field of view (1/4 pi is common),
        // the aspect ratio, and the near and far clipping planes (which define at
        // what distances geometry should be no longer be rendered).
        D3DXMATRIXA16 matProj;
        D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/3, 1.0f, 1.0f, 1000.0f );
        pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
    }

    ///////////////////////////////////////////
    //		Cloud
    ///////////////////////////////////////////
    InitCloud(false);
    
    ///////////////////////////////////////////
    //		GameScene:Skybox and Terrain
    ///////////////////////////////////////////
    g_GameScene.Setup(pd3dDevice);
    g_GameScene.onResetDevice();


    return S_OK;
}

//--------------------------------------------------------------------------------------
// Handle updates to the scene
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
    if (g_PauseEvolving) return;
    bool bSubmitNewTask = false;

    if(g_EnableAsync)
    {
        bSubmitNewTask = g_JobResult->isJobDone();
    }
    else
    {
        g_JobResult->waitUntilDone();
        bSubmitNewTask = true;
    }

    if (bSubmitNewTask)   
    {
        g_pUpdateCloud->m_fTime = fTime;
        TaskManager::getTaskManager()->submitJob(
            (TaskManager::JobFunction)RunUpdate, NULL);
    }
}

//--------------------------------------------------------------------------------------
// Render the scene 
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    HRESULT hr;

    // Clear the render target and the zbuffer 
    V( pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0,0,0,255), 1.0f, 0) );

    // Render the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        g_GameScene.Render(fTime, fElapsedTime);
        
        for ( int i=0; i<gNumCloud; ++i )
        {
            g_VolumetricCloud[i].UpdateViewDistance();
        }

        sort(g_v_pClouds.begin(), g_v_pClouds.end(), CompareViewDistance2);

        vector< CVolumetricCloud* >::iterator itCurCP, itEndCP = g_v_pClouds.end();
        for( itCurCP = g_v_pClouds.begin(); itCurCP != itEndCP; ++ itCurCP )	
        {
            (*itCurCP)->Render();
        }

        RenderText();
        V( g_HUD.OnRender( fElapsedTime ) );
        if (g_ShowParameters)
            V( g_ParametersHUD.OnRender( fElapsedTime ) );
        V( pd3dDevice->EndScene() );
    }
}


//--------------------------------------------------------------------------------------
// Handle messages to the application 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, 
                         bool* pbNoFurtherProcessing, void* pUserContext )
{
    *pbNoFurtherProcessing = g_DialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    // Give the dialogs a chance to handle the message first
    *pbNoFurtherProcessing = g_ParametersHUD.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    // Give the dialogs a chance to handle the message first
    *pbNoFurtherProcessing = g_HUD.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    if( uMsg == WM_KEYDOWN )
    {
        // If keyboard input is not enabled, this message should be ignored
        switch( wParam )
        {
        case VK_RIGHT:
        case VK_DOWN:
        case VK_LEFT:
        case VK_UP:
        case 'A':
        case 'W':
        case 'S':
        case 'D':
            g_GameScene.Keydown(wParam);
            break;
        }
    }
    else if ( uMsg == WM_MOUSEMOVE )
    {
        int xPos = LOWORD(lParam); 
        int yPos = HIWORD(lParam);
        g_GameScene.MouseMove(xPos, yPos, static_cast<unsigned>(wParam));
    }
    return 0;
}

//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
    switch( nControlID )
    {
    case IDC_APPLYCHANGE:      InitCloud(true); break;
    case IDC_PAUSE:      g_PauseEvolving = !g_PauseEvolving; break;
    case IDC_ROTATE:      Rotate(); break;
    case IDC_PARAMETERS: g_ShowParameters = !g_ShowParameters; break;
    case IDC_SSE:                   g_SSE4On = ((CDXUTCheckBox*)pControl)->GetChecked(); break;
    case IDC_ASYNC:                 g_EnableAsync = ((CDXUTCheckBox*)pControl)->GetChecked(); break;
    case IDC_CELL_SIZE: 
        g_CellSize = (float) (g_ParametersHUD.GetSlider( IDC_CELL_SIZE )->GetValue() * 1.00f);

        WCHAR sz[100];
        StringCchPrintf( sz, 100, L"Cell size: %0.2f", g_CellSize ); 
        g_ParametersHUD.GetStatic( IDC_CELL_SIZE_STATIC )->SetText( sz );
        break;

    case IDC_EVOLVING_SPEED: 
        g_CloudEvolvingSpeed = (float) (g_ParametersHUD.GetSlider( IDC_EVOLVING_SPEED )->GetValue() * 0.10f);

        StringCchPrintf( sz, 100, L"Evolving speed: %0.2f", g_CloudEvolvingSpeed ); 
        g_ParametersHUD.GetStatic( IDC_EVOLVING_SPEED_STATIC )->SetText( sz );
        
        for (int i = 0; i < gNumCloud; i++)
        {
            g_VolumetricCloud[i].SetEvolvingSpeed((float)(1.0-g_CloudEvolvingSpeed));
        }
        break;

    case IDC_SUNCOLORR: 
        g_SunColor.r = (float) (g_ParametersHUD.GetSlider( IDC_SUNCOLORR )->GetValue()*0.1);

        StringCchPrintf( sz, 100, L"SunColor R: %0.2f", g_SunColor.r ); 
        g_ParametersHUD.GetStatic( IDC_SUNCOLORR_STATIC )->SetText( sz );

        for (int i = 0; i < gNumCloud; i++)
        {
            g_VolumetricCloud[i].SetSunColor(g_SunColor.r, g_SunColor.g, g_SunColor.b, g_fSunColorIntensity);
        }
        break;

    case IDC_SUNCOLORG: 
        g_SunColor.g = (float) (g_ParametersHUD.GetSlider( IDC_SUNCOLORG )->GetValue()*0.1);

        StringCchPrintf( sz, 100, L"SunColor G: %0.2f", g_SunColor.g ); 
        g_ParametersHUD.GetStatic( IDC_SUNCOLORG_STATIC )->SetText( sz );

        for (int i = 0; i < gNumCloud; i++)
        {
            g_VolumetricCloud[i].SetSunColor(g_SunColor.r, g_SunColor.g, g_SunColor.b, g_fSunColorIntensity);
        }
        break;

    case IDC_SUNCOLORB: 
        g_SunColor.b = (float) (g_ParametersHUD.GetSlider( IDC_SUNCOLORB )->GetValue()*0.1);

        StringCchPrintf( sz, 100, L"SunColor B: %0.2f", g_SunColor.b ); 
        g_ParametersHUD.GetStatic( IDC_SUNCOLORB_STATIC )->SetText( sz );

        for (int i = 0; i < gNumCloud; i++)
        {
            g_VolumetricCloud[i].SetSunColor(g_SunColor.r, g_SunColor.g, g_SunColor.b, g_fSunColorIntensity);
        }
        break;

    case IDC_SUNCOLORINTENSITY: 
        g_fSunColorIntensity = (float) (g_ParametersHUD.GetSlider( IDC_SUNCOLORINTENSITY )->GetValue()*0.1);

        StringCchPrintf( sz, 100, L"SunColor Intensity: %0.2f", g_fSunColorIntensity ); 
        g_ParametersHUD.GetStatic( IDC_SUNCOLORINTENSITY_STATIC )->SetText( sz );

        for (int i = 0; i < gNumCloud; i++)
        {
            g_VolumetricCloud[i].SetSunColor(g_SunColor.r, g_SunColor.g, g_SunColor.b, g_fSunColorIntensity);
        }
        break;

    case IDC_WINDVELOCITY: 
        g_fWindVelocity = (float) (g_ParametersHUD.GetSlider( IDC_WINDVELOCITY )->GetValue());

        StringCchPrintf( sz, 100, L"Wind velocity: %0.2f", g_fWindVelocity ); 
        g_ParametersHUD.GetStatic( IDC_WINDVELOCITY_STATIC )->SetText( sz );

        for (int i = 0; i < gNumCloud; i++)
        {
            g_VolumetricCloud[i].SetWindVelocity(-g_fWindVelocity);
        }
        break;

    case IDC_SUNCOLORUPDATEINTERVAL: 
        g_iColorUpdateInterval = (int) (g_ParametersHUD.GetSlider( IDC_SUNCOLORUPDATEINTERVAL )->GetValue());

        StringCchPrintf( sz, 100, L"Illumination frequency %d", g_iColorUpdateInterval ); 
        g_ParametersHUD.GetStatic( IDC_SUNCOLORUPDATEINTERVAL_STATIC )->SetText( sz );

        break;

    case IDC_NUMOFTHREAD:
        DXUTComboBoxItem* pSelectedItem = g_ParametersHUD.GetComboBox( IDC_NUMOFTHREAD)->GetSelectedItem();

        if( 0 == lstrcmp( L"1 thread", pSelectedItem->strText ) )
            gNumThreads = 1;
        else if( 0 == lstrcmp( L"2 thread", pSelectedItem->strText ) )
            gNumThreads = 2;
        else if( 0 == lstrcmp( L"4 thread", pSelectedItem->strText ) )
            gNumThreads = 4;
        else if( 0 == lstrcmp( L"8 thread", pSelectedItem->strText ) )
            gNumThreads = 8;
        TaskManager::getTaskManager()->shutdown();

        TaskManager::getTaskManager()->init(gNumThreads+1);//use gNumThreads to do cloud update

        g_JobResult = TaskManager::getTaskManager()->getJobResult();


    }
}

//--------------------------------------------------------------------------------------
// Release resources created in the OnResetDevice callback here 
//--------------------------------------------------------------------------------------
void CALLBACK OnLostDevice( void* pUserContext )
{
    g_DialogResourceManager.OnD3D9LostDevice();
    if( g_pFont )
        g_pFont->OnLostDevice();

    g_GameScene.onLostDevice();
    SAFE_RELEASE( g_pTextSprite );

    if (g_JobResult != NULL)
    {
        g_JobResult->waitUntilDone();
    }

    for ( int i=0; i<gNumCloud; ++i )
    {
        g_VolumetricCloud[i].Cleanup();
    }
}


//--------------------------------------------------------------------------------------
// Release resources created in the OnCreateDevice callback here
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice( void* pUserContext )
{
    SAFE_RELEASE( g_pFont );
    g_DialogResourceManager.OnD3D9DestroyDevice();

    g_GameScene.onDestroyDevice();

    delete g_pUpdateCloud;
    delete g_pvViewpoint;

    TaskManager::getTaskManager()->shutdown();
}



//--------------------------------------------------------------------------------------
// Initialize everything and go into a render loop
//--------------------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    // Set the callback functions
    DXUTSetCallbackD3D9DeviceAcceptable( IsDeviceAcceptable );
    DXUTSetCallbackD3D9DeviceCreated( OnCreateDevice );
    DXUTSetCallbackD3D9DeviceReset( OnResetDevice );
    DXUTSetCallbackD3D9FrameRender( OnFrameRender );
    DXUTSetCallbackD3D9DeviceLost( OnLostDevice );
    DXUTSetCallbackD3D9DeviceDestroyed( OnDestroyDevice );
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackFrameMove( OnFrameMove );
    DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );
    DXUTSetCursorSettings( true, true );
    
    unsigned RegECX;

    __asm
    { 
        mov eax, 1  // which CPUID feature flag to check
            cpuid
            mov RegECX, ecx
    }

#define SSE41_BIT 0x80000	// ECX[19] Bit 19 is set if SSE4.1 is supported
#define SSE42_BIT 0x100000	// ECX[20] Bit 20 is set if SSE4.2 is supported
    if (RegECX & SSE41_BIT)
        g_SupportSSE4 = true;
    else
        g_SupportSSE4 = false;

    InitApp();

    // Initialize DXUT and create the desired Win32 window and Direct3D device for the application
    DXUTInit( true, true ); // Parse the command line, handle the default hotkeys, and show msgboxes
    DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen
    DXUTCreateWindow( L"LuckyCloud" );

    HRESULT hr = DXUTCreateDevice( true, 800, 600 );
    if( SUCCEEDED( hr ) )
    {
        // Pass control to DXUT for handling the message pump and 
        // dispatching render calls. DXUT will call your FrameMove 
        // and FrameRender callback when there is idle time between handling window messages.
        DXUTMainLoop();
    }

    return DXUTGetExitCode();
}


//--------------------------------------------------------------------------------------
// Initialize the app
//--------------------------------------------------------------------------------------
void InitApp()
{
    g_HUD.Init( &g_DialogResourceManager );
    g_ParametersHUD.Init( &g_DialogResourceManager );
    g_DialogLabels.Init( &g_DialogResourceManager );

    // Title font for comboboxes
    g_ParametersHUD.SetFont( 1, L"Arial", 14, FW_BOLD );
    CDXUTElement* pElement = g_ParametersHUD.GetDefaultElement( DXUT_CONTROL_STATIC, 0 );
    if( pElement )
    {
        pElement->iFont = 1;
        pElement->dwTextFormat = DT_LEFT | DT_BOTTOM;

        // Scene label font
        g_DialogLabels.SetFont( 1, L"Arial", 16, FW_BOLD );
    }

    pElement = g_DialogLabels.GetDefaultElement( DXUT_CONTROL_STATIC, 0 );
    if( pElement )
    {
        pElement->iFont = 1;
        pElement->FontColor.Init( D3DCOLOR_RGBA( 200, 200, 200, 255 ) );
        pElement->dwTextFormat = DT_LEFT | DT_BOTTOM;
    }

    
    // Initialize dialogs
    int iX = 15, iY = 10;

    g_ParametersHUD.SetCallback( OnGUIEvent );  
    iY += 50;

    g_ParametersHUD.AddStatic( -1, L"NumofUpdateThreads", iX, iY += 24, 180, 22 );
    g_ParametersHUD.AddComboBox( IDC_NUMOFTHREAD, iX, iY += 24, 160, 22 );
    g_ParametersHUD.GetComboBox( IDC_NUMOFTHREAD )->AddItem( L"1 thread", NULL );
    g_ParametersHUD.GetComboBox( IDC_NUMOFTHREAD )->AddItem( L"2 thread", NULL );
    g_ParametersHUD.GetComboBox( IDC_NUMOFTHREAD )->AddItem( L"4 thread", NULL );
    g_ParametersHUD.GetComboBox( IDC_NUMOFTHREAD )->AddItem( L"8 thread", NULL );
    g_ParametersHUD.GetComboBox( IDC_NUMOFTHREAD )->SetSelectedByIndex(2);

    g_ParametersHUD.AddCheckBox( IDC_SSE, L"Enable SSE4", iX, iY += 35, 260, 18, false );
    g_ParametersHUD.GetCheckBox( IDC_SSE )->SetEnabled( g_SupportSSE4 );

    g_ParametersHUD.AddCheckBox( IDC_ASYNC, L"Enable Async", iX, iY += 35, 260, 18, false );

    WCHAR sz[100];
    StringCchPrintf( sz, 100, L"Cell size: %0.2f", g_CellSize );
    g_ParametersHUD.AddStatic( IDC_CELL_SIZE_STATIC, sz, iX, iY += 30, 125, 22 );
    g_ParametersHUD.AddSlider( IDC_CELL_SIZE, iX+85, iY + 4, 90, 22, 4, 20, (int) (g_CellSize) );
    g_ParametersHUD.AddButton( IDC_APPLYCHANGE, L"Apply", iX+180, iY+4, 45, 22 );

    StringCchPrintf( sz, 100, L"Evolving speed: %0.2f", (g_CloudEvolvingSpeed) );
    g_ParametersHUD.AddStatic( IDC_EVOLVING_SPEED_STATIC, sz, iX, iY += 30, 125, 22 );
    g_ParametersHUD.AddSlider( IDC_EVOLVING_SPEED, iX+120, iY+4, 100, 22, 0, 10, (int) (10*g_CloudEvolvingSpeed) );

    StringCchPrintf( sz, 100, L"SunColor R: %0.2f", (g_SunColor.r) );
    g_ParametersHUD.AddStatic( IDC_SUNCOLORR_STATIC, sz, iX, iY += 30, 145, 22 );
    g_ParametersHUD.AddSlider( IDC_SUNCOLORR, iX+120, iY +4, 100, 22, 0, 10, (int) (10*g_SunColor.r) );

    StringCchPrintf( sz, 100, L"SunColor G: %0.2f", (g_SunColor.g) );
    g_ParametersHUD.AddStatic( IDC_SUNCOLORG_STATIC, sz, iX, iY += 30, 145, 22 );
    g_ParametersHUD.AddSlider( IDC_SUNCOLORG, iX+120, iY +4, 100, 22, 0, 10, (int) (10*g_SunColor.g) );

    StringCchPrintf( sz, 100, L"SunColor B: %0.2f", (g_SunColor.b) );
    g_ParametersHUD.AddStatic( IDC_SUNCOLORB_STATIC, sz, iX, iY += 30, 145, 22 );
    g_ParametersHUD.AddSlider( IDC_SUNCOLORB, iX+120, iY +4, 100, 22, 0, 10, (int) (10*g_SunColor.b) );

    StringCchPrintf( sz, 100, L"SunColor Intensity: %0.2f", (g_fSunColorIntensity) );
    g_ParametersHUD.AddStatic( IDC_SUNCOLORINTENSITY_STATIC, sz, iX, iY += 30, 145, 22 );
    g_ParametersHUD.AddSlider( IDC_SUNCOLORINTENSITY, iX+130, iY +4, 90, 22, 1, 30, (int) (10*g_fSunColorIntensity) );

    StringCchPrintf( sz, 100, L"Wind velocity: %0.2f", (g_fWindVelocity) );
    g_ParametersHUD.AddStatic( IDC_WINDVELOCITY_STATIC, sz, iX, iY += 30, 145, 22 );
    g_ParametersHUD.AddSlider( IDC_WINDVELOCITY, iX+130, iY +4, 100, 22, 0, 100, (int) (g_fWindVelocity) );

    StringCchPrintf( sz, 100, L"Illumination frequency %d", (g_iColorUpdateInterval) );
    g_ParametersHUD.AddStatic( IDC_SUNCOLORUPDATEINTERVAL_STATIC, sz, iX, iY += 30, 145, 22 );
    g_ParametersHUD.AddSlider( IDC_SUNCOLORUPDATEINTERVAL, iX+140, iY +4, 80, 22, 1, 30, (int) (g_iColorUpdateInterval) );

    g_ParametersHUD.AddButton( IDC_PAUSE, L"Pause/Resume Evolve", iX, iY += 38, 145, 22 );

    g_ParametersHUD.AddButton( IDC_ROTATE, L"Rotate", iX, iY += 28, 145, 22 );

    g_HUD.SetCallback( OnGUIEvent );  

    g_HUD.AddButton( IDC_PARAMETERS, L"Parameters", iX, iY += 40, 145, 22 );

}

void InitCloud(bool bNeedClean)
{
    HRESULT hr;
    
    if (bNeedClean)
    {
        if (g_JobResult != NULL)
        {
            g_JobResult->waitUntilDone();
        }
        for (int i = 0; i < gNumCloud; i++)
            g_VolumetricCloud[i].Cleanup();
    }
    Environment Env;
    Env.cSunColor = D3DXCOLOR( g_SunColor.r, g_SunColor.g, g_SunColor.b, 1.0f );
    Env.fSunColorIntensity = g_fSunColorIntensity;
    Env.vWindVelocity = D3DXVECTOR3( -0.f, 0.f, -g_fWindVelocity );
    Env.vSunlightDir = D3DXVECTOR3( 1.0f, -1.0f, 0.0f );

    CloudProperties Cloud;
    Cloud.fCellSize = g_CellSize;//1.5;
    Cloud.fEvolvingSpeed = (float)(1.0-g_CloudEvolvingSpeed);
    V( DXUTFindDXSDKMediaFileCch( Cloud.szTextureFile, MAX_PATH, L"metaball.dds" ) );

    g_v_pClouds.clear();
    for (int i = 0; i < gNumCloud; i++)
    {
        Cloud.fLength = g_Cloud[i].l;
        Cloud.fWidth  = g_Cloud[i].w;
        Cloud.fHigh   = g_Cloud[i].h;

        Cloud.vCloudPos = D3DXVECTOR3( g_Cloud[i].x, g_Cloud[i].y,g_Cloud[i].z);
        g_VolumetricCloud[i].Setup( gd3dDevice, &Env, &Cloud );
        g_v_pClouds.push_back(&g_VolumetricCloud[i]);
    }
}

void Rotate()
{
    g_Rotate = !g_Rotate;
}

void RenderText()
{

    CDXUTTextHelper txtHelper( g_pFont, g_pTextSprite, 15 );

    // Output statistics
    txtHelper.Begin();
    txtHelper.SetInsertionPos( 5, 5 );
    txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
    txtHelper.DrawTextLine( DXUTGetFrameStats( DXUTIsVsyncEnabled() ) ); 
    txtHelper.DrawTextLine( DXUTGetDeviceStats() );

    txtHelper.End();
}