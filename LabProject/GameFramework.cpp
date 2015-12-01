//-----------------------------------------------------------------------------
// File: CGameFramework.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "GameFramework.h"

CGameFramework::CGameFramework()
{

	m_pd3dDevice = NULL;
	m_pDXGISwapChain = NULL;
	m_pd3dRenderTargetView = NULL;
	m_pd3dDepthStencilBuffer = NULL;
	m_pd3dDepthStencilView = NULL;
	m_pd3dImmediateDeviceContext = NULL;

	m_pScene = NULL;


	_tcscpy_s(m_pszBuffer, _T("LapProject ("));

	srand(timeGetTime());
}

CGameFramework::~CGameFramework()
{
}

bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;
	InitializePhysxEngine();
	InitializeFbxManager();
	if (!CreateDirect3DDisplay()) return(false); 

	BuildObjects(); 

	return(true);
}

bool CGameFramework::CreateDirect3DDisplay()
{
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	m_nWndClientWidth = rcClient.right - rcClient.left;
	m_nWndClientHeight = rcClient.bottom - rcClient.top;

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
	dxgiSwapChainDesc.BufferCount = 1;
	dxgiSwapChainDesc.BufferDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.BufferDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.OutputWindow = m_hWnd;
	dxgiSwapChainDesc.SampleDesc.Count = 1;
	dxgiSwapChainDesc.SampleDesc.Quality = 0;
	dxgiSwapChainDesc.Windowed = TRUE;
	dxgiSwapChainDesc.Flags = 0;

	UINT dwCreateDeviceFlags = 0;
#ifdef _DEBUG
	dwCreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE d3dDriverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};
	UINT nDriverTypes = sizeof(d3dDriverTypes) / sizeof(D3D_DRIVER_TYPE);

	D3D_FEATURE_LEVEL pd3dFeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	UINT nFeatureLevels = sizeof(pd3dFeatureLevels) / sizeof(D3D_FEATURE_LEVEL);

	D3D_DRIVER_TYPE nd3dDriverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL nd3dFeatureLevel = D3D_FEATURE_LEVEL_11_0;
	HRESULT hResult = S_OK;
	for (UINT i = 0; i < nDriverTypes; i++)
	{
		nd3dDriverType = d3dDriverTypes[i];
		if (SUCCEEDED(hResult = D3D11CreateDeviceAndSwapChain(NULL, nd3dDriverType, NULL, dwCreateDeviceFlags, pd3dFeatureLevels, nFeatureLevels, D3D11_SDK_VERSION, &dxgiSwapChainDesc, &m_pDXGISwapChain, &m_pd3dDevice, &nd3dFeatureLevel, &m_pd3dImmediateDeviceContext))) break;
	}

	if (!CreateRenderTargetDepthStencilView()) return(false);

	return(true);
}

bool CGameFramework::CreateRenderTargetDepthStencilView()
{
	HRESULT hResult = S_OK;

	ID3D11Texture2D *pd3dBackBuffer;
	if (FAILED(hResult = m_pDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pd3dBackBuffer))) return(false);
	if (FAILED(hResult = m_pd3dDevice->CreateRenderTargetView(pd3dBackBuffer, NULL, &m_pd3dRenderTargetView))) return(false);
	if (pd3dBackBuffer) pd3dBackBuffer->Release();

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC d3dDepthStencilBufferDesc;
	ZeroMemory(&d3dDepthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));
	d3dDepthStencilBufferDesc.Width = m_nWndClientWidth;
	d3dDepthStencilBufferDesc.Height = m_nWndClientHeight;
	d3dDepthStencilBufferDesc.MipLevels = 1;
	d3dDepthStencilBufferDesc.ArraySize = 1;
	d3dDepthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dDepthStencilBufferDesc.SampleDesc.Count = 1;
	d3dDepthStencilBufferDesc.SampleDesc.Quality = 0;
	d3dDepthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dDepthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	d3dDepthStencilBufferDesc.CPUAccessFlags = 0;
	d3dDepthStencilBufferDesc.MiscFlags = 0;
	if (FAILED(hResult = m_pd3dDevice->CreateTexture2D(&d3dDepthStencilBufferDesc, NULL, &m_pd3dDepthStencilBuffer))) return(false);

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC d3dDepthStencilViewDesc;
	ZeroMemory(&d3dDepthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	d3dDepthStencilViewDesc.Format = d3dDepthStencilBufferDesc.Format;
	d3dDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	d3dDepthStencilViewDesc.Texture2D.MipSlice = 0;
	if (FAILED(hResult = m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, &d3dDepthStencilViewDesc, &m_pd3dDepthStencilView))) return(false);

	m_pd3dImmediateDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, m_pd3dDepthStencilView);

	return(true);
}

void CGameFramework::OnDestroy()
{
	ReleaseObjects();
	ShutDownFbxManager();
	ShutDownPhysxEngine();
	
	if (m_pd3dImmediateDeviceContext) m_pd3dImmediateDeviceContext->ClearState();
	if (m_pd3dRenderTargetView) m_pd3dRenderTargetView->Release();
	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();	
	if (m_pd3dDepthStencilView) m_pd3dDepthStencilView->Release();	
	if (m_pDXGISwapChain) m_pDXGISwapChain->Release();
	if (m_pd3dImmediateDeviceContext) m_pd3dImmediateDeviceContext->Release();
	if (m_pd3dDevice) m_pd3dDevice->Release();
}

void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_pScene) m_pScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		SetCapture(hWnd);
		GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
}

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_pScene) m_pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_SPACE:
			break;
		}
		break;
	case WM_KEYUP:
		switch (wParam) 
		{
		case VK_SPACE:
			break;
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		case VK_RETURN:
			break;
		case VK_F1:
		case VK_F2:
		case VK_F3:
			m_ppPlayers[0]->ChangeCamera(m_pd3dDevice, (wParam-VK_F1+1), m_GameTimer.GetTimeElapsed());
			break;
		case VK_F9:
			{
				BOOL bFullScreenState = FALSE;
				m_pDXGISwapChain->GetFullscreenState(&bFullScreenState, NULL);
				if (!bFullScreenState)
				{
					DXGI_MODE_DESC dxgiTargetParameters;
					dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
					dxgiTargetParameters.Width = m_nWndClientWidth;
					dxgiTargetParameters.Height = m_nWndClientHeight;
					dxgiTargetParameters.RefreshRate.Numerator = 0;
					dxgiTargetParameters.RefreshRate.Denominator = 0;
					dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
					dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
					m_pDXGISwapChain->ResizeTarget(&dxgiTargetParameters);
				}

				//m_pPxScene->sl
				m_pDXGISwapChain->SetFullscreenState(!bFullScreenState, NULL);
				break;
			}
		case VK_F10:
			break;
		default:
			break;
		} 
		break;
	default:
		break;
	}
}

LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
		{
			m_nWndClientWidth = LOWORD(lParam);
			m_nWndClientHeight = HIWORD(lParam);

			m_pd3dImmediateDeviceContext->OMSetRenderTargets(0, NULL, NULL);

			if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
			if (m_pd3dRenderTargetView) m_pd3dRenderTargetView->Release();
			if (m_pd3dDepthStencilView) m_pd3dDepthStencilView->Release();

			m_pDXGISwapChain->ResizeBuffers(1, 0/*m_nWndClientWidth*/, 0/*m_nWndClientHeight*/, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

			CreateRenderTargetDepthStencilView();

			if (m_ppPlayers)
			{
				CCamera *pCamera = m_ppPlayers[0]->GetCamera();
				pCamera->SetViewport(m_pd3dImmediateDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f);
			}
			break;
		}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return(0);
}

void CGameFramework::BuildObjects()
{
	m_nPlayers = 1;
	m_ppPlayers = new CPlayer*[m_nPlayers];

	CGamePlayer *pGamePlayer = new CGamePlayer(m_pd3dDevice);
	pGamePlayer->ChangeCamera(m_pd3dDevice, FIRST_PERSON_CAMERA, m_GameTimer.GetTimeElapsed());

	CCamera *pCamera = pGamePlayer->GetCamera();
	pCamera->SetViewport(m_pd3dImmediateDeviceContext, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 90.0f);
	pCamera->GenerateViewMatrix();

	pGamePlayer->SetPosition(D3DXVECTOR3(0.0f, 0.0f, -50.0f));

	m_ppPlayers[0] = pGamePlayer;

	m_pScene = new CScene(m_pPxPhysicsSDK, m_pPxScene, m_ppPlayers);

	if (m_pScene){
		m_pScene->BuildObjects(m_pd3dDevice, m_pPxPhysicsSDK, m_pPxScene, m_pFbxSdkManager);
	}
}

void CGameFramework::ReleaseObjects()
{
	if (m_pScene) m_pScene->ReleaseObjects();
	if (m_pScene) delete m_pScene;

	if (m_ppPlayers)
	{
		for (int j = 0; j < m_nPlayers; j++) delete m_ppPlayers[j];
		delete [] m_ppPlayers;
	}
}

void CGameFramework::ProcessInput()
{
	long double Dist=0;
	DWORD dwForward=0, dwBackward=0, dwLeft=0, dwRight=0, dwUp=0, dwDown=0;
	bool bProcessedByScene = false;
	if (m_pScene) bProcessedByScene = m_pScene->ProcessInput();
	if (!bProcessedByScene)
	{
		static UCHAR pKeyBuffer[256];

		if (GetKeyboardState(pKeyBuffer))
		{
			if (pKeyBuffer['W'] & 0xF0) dwForward |= DIR_FORWARD;
			if (pKeyBuffer['S'] & 0xF0) dwBackward |= DIR_BACKWARD;
			if (pKeyBuffer['A'] & 0xF0) dwLeft |= DIR_LEFT;
			if (pKeyBuffer['D'] & 0xF0) dwRight |= DIR_RIGHT;
			if (pKeyBuffer['Q'] & 0xF0) {dwUp |= DIR_UP;}
			if (pKeyBuffer['Z'] & 0xF0) dwDown |= DIR_DOWN;
		}	    
		float cxDelta = 0.0f, cyDelta = 0.0f;
		POINT ptCursorPos;
		if (GetCapture() == m_hWnd)
		{
			SetCursor(NULL);
			GetCursorPos(&ptCursorPos);
			cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
			cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
			SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		} 
		if ((dwForward != 0) || (dwBackward != 0) || (dwLeft != 0) || (dwRight != 0) || (dwUp != 0) || (dwDown != 0) ||(cxDelta != 0.0f) || (cyDelta != 0.0f))
		{
			if (cxDelta || cyDelta) 
			{
				if (pKeyBuffer[VK_RBUTTON] & 0xF0)
					m_ppPlayers[0]->Rotate(cyDelta, cxDelta, 0.0f);
			}
			if(dwForward==DIR_FORWARD)
				m_ppPlayers[0]->Move(dwForward, 300.0f * m_GameTimer.GetTimeElapsed(),false);
			if(dwBackward==DIR_BACKWARD)
				m_ppPlayers[0]->Move(dwBackward, 300.0f * m_GameTimer.GetTimeElapsed(),false);
			if(dwLeft==DIR_LEFT)
				m_ppPlayers[0]->Move(dwLeft, 300.0f * m_GameTimer.GetTimeElapsed(),false);
			if(dwRight==DIR_RIGHT)
				m_ppPlayers[0]->Move(dwRight, 300.0f * m_GameTimer.GetTimeElapsed(),false);
			if(dwUp==DIR_UP)
				m_ppPlayers[0]->Move(dwUp, 300.0f * m_GameTimer.GetTimeElapsed(),false);
			if(dwDown==DIR_DOWN)
				m_ppPlayers[0]->Move(dwDown, 300.0f * m_GameTimer.GetTimeElapsed(),false);
		}
		m_ppPlayers[0]->Update(m_GameTimer.GetTimeElapsed());
	}
}

void CGameFramework::AnimateObjects()
{
	if (m_pScene) m_pScene->AnimateObjects(m_GameTimer.GetTimeElapsed(),m_pPxScene);
}

void CGameFramework::FrameAdvance()
{    

	m_GameTimer.Tick();

	ProcessInput();

	if(m_pPxScene){
		m_pPxScene->simulate(m_GameTimer.GetTimeElapsed());
		m_pPxScene->fetchResults(true);
	}
	AnimateObjects();

	float fClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; 
	if (m_pd3dRenderTargetView) m_pd3dImmediateDeviceContext->ClearRenderTargetView(m_pd3dRenderTargetView, fClearColor);
	if (m_pd3dDepthStencilView) m_pd3dImmediateDeviceContext->ClearDepthStencilView(m_pd3dDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	for (int i = 0; i < m_nPlayers; ++i)
	{
		if (m_ppPlayers[i]) {
			m_ppPlayers[i]->UpdateShaderVariables(m_pd3dImmediateDeviceContext);
			m_ppPlayers[i]->Render(m_pd3dImmediateDeviceContext);
		}		
	}
	if (m_pScene)
		m_pScene->Render(m_pd3dImmediateDeviceContext, m_ppPlayers[0]->GetCamera());



	m_pDXGISwapChain->Present(0, 0);

	m_GameTimer.GetFrameRate(m_pszBuffer+12, 37);
	::SetWindowText(m_hWnd, m_pszBuffer);
}

void CGameFramework::InitializePhysxEngine()
{
	m_pPxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION,m_PxDefaultAllocatorCallback,m_PxDefaultErrorCallback);
	m_pPxPhysicsSDK = PxCreatePhysics(PX_PHYSICS_VERSION,*m_pPxFoundation,PxTolerancesScale());
	if(m_pPxPhysicsSDK == NULL){
		cout << "PhysicsSDK Initialize Failed" << endl;
	}
	else{
		cout << "PhysicsSDK Initialize Succeed" << endl;
	}

	PxInitExtensions(*m_pPxPhysicsSDK);

	PxSceneDesc sceneDesc(m_pPxPhysicsSDK->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f,-98,0.0f);

	if(!sceneDesc.cpuDispatcher)
	{
		PxDefaultCpuDispatcher* pCpuDispatcher = PxDefaultCpuDispatcherCreate(1);
		sceneDesc.cpuDispatcher = pCpuDispatcher;
	}	
	if(!sceneDesc.filterShader)
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	m_pPxScene = m_pPxPhysicsSDK->createScene(sceneDesc);
}

void CGameFramework::ShutDownPhysxEngine()
{
	if(m_pPxScene) m_pPxScene->release();
	if(m_pPxPhysicsSDK) m_pPxPhysicsSDK->release();
	if(m_pPxFoundation) m_pPxFoundation->release();

}

void CGameFramework::InitializeFbxManager()
{
	if(m_pFbxSdkManager == NULL) 
    {
       m_pFbxSdkManager = FbxManager::Create(); // fbxManager를 만든다.

       FbxIOSettings* pIOsettings = FbxIOSettings::Create(m_pFbxSdkManager, IOSROOT ); // FbxIO를 셋팅한다.
       m_pFbxSdkManager->SetIOSettings(pIOsettings);
    }
}

void CGameFramework::ShutDownFbxManager()
{
	if(m_pFbxSdkManager)
	{
		m_pFbxSdkManager->Destroy();
	}
}
