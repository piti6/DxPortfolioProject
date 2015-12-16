#pragma once

#include "Timer.h"
#include "Scene.h"
#include "NetworkClient.h"

//マルチスレッドレンダリングに使う情報を持っている構造体です。
struct RENDERINGTHREADINFO
{
	int m_nRenderingThreadID;
	HANDLE m_hRenderingThread;
	HANDLE m_hRenderingBeginEvent;
	HANDLE m_hRenderingEndEvent;

	ID3D11DeviceContext *m_pd3dDeferredContext;
	ID3D11CommandList *m_pd3dCommandList;
	ID3D11DepthStencilView *m_pd3dDepthStencilView;

	CScene *m_pScene;
	CPlayer *m_pPlayer;
};

//全体的なゲームのフレームワークです。
class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	/////////////// Input Callbacks ///////////////

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	/////////////// DirectX SDK Member Function ///////////////

	bool CreateRenderTargetDepthStencilView();
	bool CreateDirect3DDisplay();

	void BuildObjects();
	void ReleaseObjects();

	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	void InitializeWorkerThreads();

	/////////////// Physx SDK Member Function ///////////////

	void InitializePhysxEngine();
	void ShutDownPhysxEngine();

	/////////////// FBX SDK Member Function ///////////////

	void InitializeFbxManager();
	void ShutDownFbxManager();

	/////////////// Network Function ///////////////

	void InitializeNetworkState();
	void ReleaseNetworkState();

	static UINT WINAPI ThreadProcTCP(LPVOID arg);
	static UINT WINAPI ThreadProcUDP(LPVOID arg);
	UINT ThreadProcTCP(CGameFramework* _GameFramework);
	UINT ThreadProcUDP(CGameFramework* _GameFramework);

private:

	/////////////// Windows Member Variables ///////////////

	HINSTANCE						m_hInstance;
	HWND							m_hWnd;
	HMENU							m_hMenu;

	int								m_nWndClientWidth;
	int								m_nWndClientHeight;

	POINT							m_ptOldCursorPos;
	_TCHAR							m_pszBuffer[50];

	/////////////// DirectX SDK Member Variables ///////////////

	ID3D11Device					*m_pd3dDevice;
	IDXGISwapChain					*m_pDXGISwapChain;
	ID3D11RenderTargetView			*m_pd3dRenderTargetView;
	ID3D11Texture2D					*m_pd3dDepthStencilBuffer;
	ID3D11DepthStencilView			*m_pd3dDepthStencilView;
	ID3D11DeviceContext				*m_pd3dImmediateDeviceContext;

	/////////////// Threading ///////////////

	int								m_nRenderThreads;
	RENDERINGTHREADINFO				*m_pRenderingThreadInfo;
	HANDLE							*m_hRenderingEndEvents;

	/////////////// Network ///////////////

	HANDLE							m_hUDPThread;
	HANDLE							m_hTCPThread;
	CNetworkClient					*m_pNetworkClient;
	CRITICAL_SECTION				cs;

	/////////////// Physx SDK Member Variables ///////////////

	PxPhysics						*m_pPxPhysicsSDK;
	PxDefaultErrorCallback			m_PxDefaultErrorCallback;
	PxDefaultAllocator				m_PxDefaultAllocatorCallback;
	PxScene							*m_pPxScene;
	PxFoundation					*m_pPxFoundation;
	PxVisualDebuggerConnection		*m_pPVDConnection;
	PxControllerManager				*m_pPxControllerManager;

	/////////////// FBX SDK Member Variables ///////////////

	FbxManager						*m_pFbxSdkManager;

	/////////////// User Defined Member Variables ///////////////

	CGameTimer								m_GameTimer;
	CScene									*m_pScene;
	CPlayer									*m_pPlayer;
	vector<pair<string, CCharacterObject*>>	m_vPlayers;
};
