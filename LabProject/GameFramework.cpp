#include "stdafx.h"
#include "GameFramework.h"

//DeferredContextを利用するスレッドを作ります。
UINT WINAPI DeferredContextThreadProc(LPVOID lpParameter)
{
	RENDERINGTHREADINFO *pRenderingThreadInfo = (RENDERINGTHREADINFO*)lpParameter;
	ID3D11DeviceContext *pd3dDeferredContext = pRenderingThreadInfo->m_pd3dDeferredContext;

	while (1){
		WaitForSingleObject(pRenderingThreadInfo->m_hRenderingBeginEvent, INFINITE);
		if (pRenderingThreadInfo->m_nRenderingThreadID == 0)
			pd3dDeferredContext->ClearDepthStencilView(pRenderingThreadInfo->m_pd3dDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
		pRenderingThreadInfo->m_pPlayer->UpdateShaderVariables(pd3dDeferredContext);
		pRenderingThreadInfo->m_pScene->Render(pd3dDeferredContext, pRenderingThreadInfo->m_nRenderingThreadID, pRenderingThreadInfo->m_pPlayer->GetCamera());
		pd3dDeferredContext->FinishCommandList(true, &pRenderingThreadInfo->m_pd3dCommandList);
		SetEvent(pRenderingThreadInfo->m_hRenderingEndEvent);
	}
}

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
	InitializeWorkerThreads();
	InitializeNetworkState();
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
	D3D11_FEATURE_DATA_THREADING d3dThreading;
	m_pd3dDevice->CheckFeatureSupport(D3D11_FEATURE_THREADING, &d3dThreading, sizeof(d3dThreading));
	if (!(d3dThreading.DriverCommandLists && d3dThreading.DriverConcurrentCreates))
		return false;
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

//初期化
void CGameFramework::InitializePhysxEngine()
{
	m_pPxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_PxDefaultAllocatorCallback, m_PxDefaultErrorCallback);
	PxTolerancesScale PxScale = PxTolerancesScale();
	PxScale.length /= 100;
	PxScale.mass /= 100;
	PxScale.speed /= 100;
	m_pPxPhysicsSDK = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pPxFoundation, PxScale, false);
	if (m_pPxPhysicsSDK == NULL){
		cout << "PhysicsSDK Initialize Failed" << endl;
	}
	else{
		cout << "PhysicsSDK Initialize Succeed" << endl;
	}

	PxInitExtensions(*m_pPxPhysicsSDK);
	PxSceneDesc sceneDesc(m_pPxPhysicsSDK->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);

	if (!sceneDesc.cpuDispatcher)
	{
		PxDefaultCpuDispatcher* pCpuDispatcher = PxDefaultCpuDispatcherCreate(1);
		sceneDesc.cpuDispatcher = pCpuDispatcher;
	}

	if (!sceneDesc.filterShader)
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;

	m_pPxScene = m_pPxPhysicsSDK->createScene(sceneDesc);
	m_pPxControllerManager = PxCreateControllerManager(*m_pPxScene);

	// Physx Visual Debuggerに連結
	if (NULL == m_pPxPhysicsSDK->getPvdConnectionManager())
	{
		cout << "PVD Connect Failed" << endl;
		return;
	}
	PxVisualDebuggerConnectionFlags connectionFlags = PxVisualDebuggerExt::getAllConnectionFlags();
	m_pPxPhysicsSDK->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_SCENEQUERIES, true);
	m_pPVDConnection = PxVisualDebuggerExt::createConnection(m_pPxPhysicsSDK->getPvdConnectionManager(), "127.0.0.1", 5425, 1000, connectionFlags);
}
void CGameFramework::InitializeFbxManager()
{
	if (m_pFbxSdkManager == NULL)
	{
		m_pFbxSdkManager = FbxManager::Create();

		FbxIOSettings* pIOsettings = FbxIOSettings::Create(m_pFbxSdkManager, IOSROOT);
		m_pFbxSdkManager->SetIOSettings(pIOsettings);
	}
}
void CGameFramework::InitializeWorkerThreads()
{
	m_nRenderThreads = MAX_THREAD;
	m_pRenderingThreadInfo = new RENDERINGTHREADINFO[m_nRenderThreads];
	m_hRenderingEndEvents = new HANDLE[m_nRenderThreads];
	for (int i = 0; i < m_nRenderThreads; ++i)
	{
		m_pRenderingThreadInfo[i].m_nRenderingThreadID = i;
		m_pRenderingThreadInfo[i].m_pPlayer = m_pPlayer;
		m_pRenderingThreadInfo[i].m_pScene = m_pScene;
		m_pRenderingThreadInfo[i].m_pd3dCommandList = NULL;
		m_pRenderingThreadInfo[i].m_hRenderingBeginEvent = CreateEvent(NULL, false, false, NULL);
		m_pRenderingThreadInfo[i].m_hRenderingEndEvent = CreateEvent(NULL, false, false, NULL);
		m_pRenderingThreadInfo[i].m_pd3dDepthStencilView = m_pd3dDepthStencilView;
		m_hRenderingEndEvents[i] = m_pRenderingThreadInfo[i].m_hRenderingEndEvent;
		m_pd3dDevice->CreateDeferredContext(0, &m_pRenderingThreadInfo[i].m_pd3dDeferredContext);
		m_pRenderingThreadInfo[i].m_pPlayer->GetCamera()->SetViewport(m_pRenderingThreadInfo[i].m_pd3dDeferredContext, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		m_pRenderingThreadInfo[i].m_pd3dDeferredContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, m_pd3dDepthStencilView);

		m_pRenderingThreadInfo[i].m_hRenderingThread = (HANDLE) ::_beginthreadex(NULL, 0, DeferredContextThreadProc, &m_pRenderingThreadInfo[i], CREATE_SUSPENDED, NULL);
		ResumeThread(m_pRenderingThreadInfo[i].m_hRenderingThread);
	}
}
void CGameFramework::InitializeNetworkState(){
	m_pNetworkClient = new CNetworkClient("127.0.0.1", 9000);
	m_pNetworkClient->Init();
	InitializeCriticalSection(&cs);
	int optval = 3000;	// TimeOut
	SetSocketOption(*m_pNetworkClient->GetTCPSocket(), SOL_SOCKET, SO_RCVTIMEO, (char *)&optval, sizeof(int));

	char Login[100];
	Network_Packet PlayerPacket;
	while (1){
		//サーバーからの応答を待ちます。
		m_pNetworkClient->TCPReceive(m_pNetworkClient->GetTCPSocket(), Login);
		if (Login[0] == TCP_CLIENT_LOGIN){
			memcpy(&PlayerPacket, &Login[2], Login[1]);
			m_pPlayer->SetName(PlayerPacket.m_ID);
			break;
		}
	}

	optval = 0;
	SetSocketOption(*m_pNetworkClient->GetTCPSocket(), SOL_SOCKET, SO_RCVTIMEO, (char *)&optval, sizeof(int));

	//TCP,UDPデータを送信、受信するスレッドを作ります。
	m_hUDPThread = (HANDLE) ::_beginthreadex(NULL, 0, ThreadProcUDP, this, CREATE_SUSPENDED, NULL);
	m_hTCPThread = (HANDLE) ::_beginthreadex(NULL, 0, ThreadProcTCP, this, CREATE_SUSPENDED, NULL);
	ResumeThread(m_hUDPThread);
	ResumeThread(m_hTCPThread);
}
void CGameFramework::BuildObjects()
{
	m_pPlayer = new CPlayer();

	m_pScene = new CScene(m_pPxPhysicsSDK, m_pPxScene, m_pPlayer);

	if (m_pScene){
		m_pScene->BuildObjects(m_pd3dDevice, m_pPxPhysicsSDK, m_pPxScene, m_pPxControllerManager, m_pFbxSdkManager);
	}
	CCamera *pCamera = m_pPlayer->GetCamera();
	pCamera->SetViewport(m_pd3dImmediateDeviceContext, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	pCamera->GenerateViewMatrix();
}

//終了する時の処理
void CGameFramework::OnDestroy()
{
	ReleaseObjects();
	ShutDownFbxManager();
	ShutDownPhysxEngine();
	ReleaseNetworkState();
	DeleteCriticalSection(&cs);
	for (int i = 0; i < m_nRenderThreads; ++i)
	{
		m_pRenderingThreadInfo[i].m_pd3dDeferredContext->Release();
		CloseHandle(m_pRenderingThreadInfo[i].m_hRenderingBeginEvent);
		CloseHandle(m_pRenderingThreadInfo[i].m_hRenderingEndEvent);
		CloseHandle(m_pRenderingThreadInfo[i].m_hRenderingThread);
	}

	if (m_pd3dImmediateDeviceContext) m_pd3dImmediateDeviceContext->ClearState();
	if (m_pd3dRenderTargetView) m_pd3dRenderTargetView->Release();
	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
	if (m_pd3dDepthStencilView) m_pd3dDepthStencilView->Release();
	if (m_pDXGISwapChain) m_pDXGISwapChain->Release();
	if (m_pd3dImmediateDeviceContext) m_pd3dImmediateDeviceContext->Release();
	if (m_pd3dDevice) m_pd3dDevice->Release();

}
void CGameFramework::ReleaseObjects()
{
	if (m_pScene) m_pScene->ReleaseObjects();
	if (m_pScene) delete m_pScene;
}
void CGameFramework::ReleaseNetworkState()
{
	m_pNetworkClient->TCPSendInt(m_pNetworkClient->GetTCPSocket(), TCP_CLIENT_DISCONNECT, 1);
	m_pNetworkClient->Release();
}
void CGameFramework::ShutDownPhysxEngine()
{
	if (m_pPVDConnection) m_pPVDConnection->release();
	if (m_pPxControllerManager) m_pPxControllerManager->release();
	if (m_pPxScene) m_pPxScene->release();
	if (m_pPxFoundation) m_pPxFoundation->release();
	if (m_pPxPhysicsSDK) m_pPxPhysicsSDK->release();
}
void CGameFramework::ShutDownFbxManager()
{
	if (m_pFbxSdkManager)
	{
		m_pFbxSdkManager->Destroy();
	}
}

//キー入力の処理
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
		switch (wParam)
		{
		case VK_SPACE:
			break;
		case 'F':
			int Index = 5;//rand() % 80 + 2;
			m_pScene->GetInstancingShader()->AddObject(m_pPxPhysicsSDK, m_pPxScene, Index, 0, 1, m_pPlayer->GetPosition() - m_pPlayer->GetUp(), false, m_pPlayer->GetUp() * -100);
			Network_Throw_Packet pkt;
			strncpy(pkt.m_ID, m_pPlayer->GetName().c_str(), sizeof(pkt.m_ID));
			pkt.m_ID[9] = 0;
			pkt.m_iIndex = Index;
			m_pNetworkClient->TCPSendData(m_pNetworkClient->GetTCPSocket(), TCP_CLIENT_THROWOBJECT, pkt);
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
			m_pPlayer->ChangeCamera(m_pd3dDevice, (wParam - VK_F1 + 1), m_GameTimer.GetTimeElapsed());
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

		if (m_pPlayer)
		{
			CCamera *pCamera = m_pPlayer->GetCamera();
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
void CGameFramework::ProcessInput()
{
	DWORD dwForward = 0, dwBackward = 0, dwLeft = 0, dwRight = 0, dwUp = 0, dwDown = 0;
	float cxDelta = 0.0f, cyDelta = 0.0f;

	static UCHAR pKeyBuffer[256];
	POINT ptCursorPos;

	if (GetKeyboardState(pKeyBuffer))
	{
		if (pKeyBuffer['W'] & 0xF0) dwForward |= DIR_FORWARD;
		if (pKeyBuffer['S'] & 0xF0) dwBackward |= DIR_BACKWARD;
		if (pKeyBuffer['A'] & 0xF0) dwLeft |= DIR_LEFT;
		if (pKeyBuffer['D'] & 0xF0) dwRight |= DIR_RIGHT;
		if (pKeyBuffer['Q'] & 0xF0) dwUp |= DIR_UP;
		if (pKeyBuffer['Z'] & 0xF0) dwDown |= DIR_DOWN;
	}
	
	if (GetCapture() == m_hWnd)
	{
		SetCursor(NULL);
		GetCursorPos(&ptCursorPos);
		cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
		SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
	}

	if ((dwForward != 0) || (dwBackward != 0) || (dwLeft != 0) || (dwRight != 0) || (dwUp != 0) || (dwDown != 0) || cxDelta || cyDelta)
	{

		if (dwForward == DIR_FORWARD)
			m_pPlayer->Move(dwForward, 10.0f, m_GameTimer.GetTimeElapsed());
		if (dwBackward == DIR_BACKWARD)
			m_pPlayer->Move(dwBackward, 10.0f, m_GameTimer.GetTimeElapsed());
		if (dwLeft == DIR_LEFT)
			m_pPlayer->Move(dwLeft, 10.0f, m_GameTimer.GetTimeElapsed());
		if (dwRight == DIR_RIGHT)
			m_pPlayer->Move(dwRight, 10.0f, m_GameTimer.GetTimeElapsed());
		if (dwUp == DIR_UP)
			m_pPlayer->Move(dwUp, 10.0f, m_GameTimer.GetTimeElapsed());
		if (dwDown == DIR_DOWN)
			m_pPlayer->Move(dwDown, 10.0f, m_GameTimer.GetTimeElapsed());
		if (pKeyBuffer[VK_RBUTTON] & 0xF0)
			m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
		Network_Packet pkt;
		strncpy(pkt.m_ID, m_pPlayer->GetName().c_str(), sizeof(pkt.m_ID));
		pkt.m_ID[9] = 0;
		pkt.m_d3dxvPosition = m_pPlayer->GetPosition();
		pkt.m_d3dxvRotation = m_pPlayer->GetRotation();
		m_pNetworkClient->TCPSendData(m_pNetworkClient->GetTCPSocket(), TCP_CLIENT_TRANSFORM, pkt);
	}
	if ((dwForward != 0) || (dwBackward != 0) || (dwLeft != 0) || (dwRight != 0) || (dwUp != 0) || (dwDown != 0))
	{
		CAnimationController* pAnimationController = m_pPlayer->GetAnimationController();
		if (pAnimationController->GetCurrentAnimationName() != "Run")
		{
			pAnimationController->Play("Run");
			m_pNetworkClient->TCPSendInt(m_pNetworkClient->GetTCPSocket(), TCP_CLIENT_MOVE, 1);
		}
	}
	else{
		CAnimationController* pAnimationController = m_pPlayer->GetAnimationController();
		if (pAnimationController->GetCurrentAnimationName() != "Idle")
		{
			pAnimationController->Play("Idle");
			m_pNetworkClient->TCPSendInt(m_pNetworkClient->GetTCPSocket(), TCP_CLIENT_MOVE, 0);
		}
	}
}

//シーンアップデート、計算処理
void CGameFramework::AnimateObjects()
{
	if (m_pScene) m_pScene->AnimateObjects(m_GameTimer.GetTimeElapsed(), m_pPxScene);
}
void CGameFramework::FrameAdvance()
{

	m_GameTimer.Tick(120);

	EnterCriticalSection(&cs);
	ProcessInput();
	LeaveCriticalSection(&cs);

	if (m_pPxScene){
		m_pPxScene->simulate(m_GameTimer.GetTimeElapsed());
		m_pPxScene->fetchResults(true);
	}
	EnterCriticalSection(&cs);
	AnimateObjects();
	LeaveCriticalSection(&cs);
	float fClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	if (m_pd3dRenderTargetView) m_pd3dImmediateDeviceContext->ClearRenderTargetView(m_pd3dRenderTargetView, fClearColor);
	for (int i = 0; i < m_nRenderThreads; ++i)
	{
		SetEvent(m_pRenderingThreadInfo[i].m_hRenderingBeginEvent);
	}
	WaitForMultipleObjects(m_nRenderThreads, m_hRenderingEndEvents, true, INFINITE);
	if (m_pPlayer)
		m_pPlayer->UpdateShaderVariables(m_pd3dImmediateDeviceContext);
	for (int i = 0; i < m_nRenderThreads; ++i)
	{
		m_pd3dImmediateDeviceContext->ExecuteCommandList(m_pRenderingThreadInfo[i].m_pd3dCommandList, true);
		m_pRenderingThreadInfo[i].m_pd3dCommandList->Release();
	}

	m_pDXGISwapChain->Present(0, 0);

	m_GameTimer.GetFrameRate(m_pszBuffer + 12, 37);
	::SetWindowText(m_hWnd, m_pszBuffer);
}

UINT WINAPI CGameFramework::ThreadProcTCP(LPVOID arg)
{

	CGameFramework* m_pGameFramework = (CGameFramework*)arg;
	return m_pGameFramework->ThreadProcTCP(m_pGameFramework);
}
UINT WINAPI CGameFramework::ThreadProcUDP(LPVOID arg)
{

	CGameFramework* m_pGameFramework = (CGameFramework*)arg;
	return m_pGameFramework->ThreadProcUDP(m_pGameFramework);
}

//実際のTCPスレッドです。サーバーから情報を受信
UINT CGameFramework::ThreadProcTCP(CGameFramework* _GameFramework)
{
	CGameFramework *pGameFramework = _GameFramework;
	int recv;
	SOCKET *TCP_Sock = m_pNetworkClient->GetTCPSocket();
	BOOL optval = TRUE;
	setsockopt(*TCP_Sock, IPPROTO_TCP, TCP_NODELAY, (char *)&optval, sizeof(optval));
	char Data[50];
	while (1){
		memset(Data, 0, sizeof(char) * 50);
		recv = m_pNetworkClient->TCPReceive(TCP_Sock, Data);
		if (Data != NULL){
			switch (Data[0]){//最初のcharはプロトコル,その後のcharはデータの長さ、その後はデータそのもの
			case TCP_NULL:
				break;
			case TCP_CLIENT_THROWOBJECT:
				Network_Throw_Packet network_throw_packet;
				memcpy(&network_throw_packet, &Data[2], Data[1]);
				for (int nPlayers = 0; nPlayers < m_vPlayers.size(); ++nPlayers)
				{
					if (m_vPlayers[nPlayers].first == network_throw_packet.m_ID)
					{
						EnterCriticalSection(&cs);
						m_pScene->GetInstancingShader()->AddObject(m_pPxPhysicsSDK, m_pPxScene, network_throw_packet.m_iIndex, 0, 1, m_vPlayers[nPlayers].second->GetPosition() - m_vPlayers[nPlayers].second->GetUp(), false, m_vPlayers[nPlayers].second->GetUp() * -100);
						LeaveCriticalSection(&cs);
					}
				}
				break;
			}
		}
		Sleep(10);
	}
}
//実際のUDPスレッドです。サーバーから情報を受信
UINT CGameFramework::ThreadProcUDP(CGameFramework* _GameFramework)
{
	CGameFramework *pGameFramework = _GameFramework;
	CNetworkClient *pNetworkClient = pGameFramework->m_pNetworkClient;
	CInstancingShader *pInstancingShader = m_pScene->GetInstancingShader();
	SOCKET *UDP_Sock = pNetworkClient->GetUDPSocket();

	Network_Packet packet;
	char Data[50] = { 0, };
	while (1){
		int recv = pNetworkClient->UDPReceive(UDP_Sock, Data);
		if (!recv)
			break;
		if (Data != NULL){
			memcpy(&packet, &Data[2], Data[1]);//最初のcharはプロトコル,その後のcharはデータの長さ、その後はデータそのもの
			switch (Data[0]){
			case UDP_SERVER_PLAYER:
				int nPlayers = 0;
				for (nPlayers = 0; nPlayers < m_vPlayers.size(); ++nPlayers){
					if (m_vPlayers[nPlayers].first == packet.m_ID)
					{
						EnterCriticalSection(&cs);
						m_vPlayers[nPlayers].second->SetPosition(packet.m_d3dxvPosition);
						m_vPlayers[nPlayers].second->SetRotation(packet.m_d3dxvRotation);
						if (m_vPlayers[nPlayers].second->GetAnimationController()->GetCurrentAnimationName() != "Run" && packet.m_isMoving)
						{
							m_vPlayers[nPlayers].second->GetAnimationController()->Play("Run");
						}
						else if (m_vPlayers[nPlayers].second->GetAnimationController()->GetCurrentAnimationName() != "Idle" && !packet.m_isMoving)
						{
							m_vPlayers[nPlayers].second->GetAnimationController()->Play("Idle");
						}
						LeaveCriticalSection(&cs);
						break;
					}
				}
				if (nPlayers == m_vPlayers.size() && strcmp(packet.m_ID, m_pPlayer->GetName().c_str()))//登録していない情報が来た場合、その情報で新たなプレイヤーを作ります。
				{
					CCharacterObject *pCharacterObject = pInstancingShader->AddCharacter(m_pPxPhysicsSDK, m_pPxScene, m_pPxControllerManager, 1, 0, 4, packet.m_d3dxvPosition);
					pCharacterObject->SetRotation(packet.m_d3dxvRotation);
					pCharacterObject->GetAnimationController()->Play("Idle");
					m_vPlayers.push_back(make_pair(packet.m_ID, pCharacterObject));
				}
				break;
			}
		}

		Sleep(10);
	}

	return 0;
}

