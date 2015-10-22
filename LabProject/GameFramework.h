#pragma once

#include "Timer.h"
#include "Player.h"
#include "Scene.h"

class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	bool CreateRenderTargetDepthStencilView();
	bool CreateDirect3DDisplay();
    
    void BuildObjects();
    void ReleaseObjects();

    void ProcessInput();
    void AnimateObjects();
    void FrameAdvance();

	
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void InitializePhysxEngine();
	void ShutDownPhysxEngine();

private:   
/*						Windows Member Variables					*/
	HINSTANCE						m_hInstance;
	HWND							m_hWnd; 
	HMENU							m_hMenu;

	int								m_nWndClientWidth;
	int								m_nWndClientHeight;
        
	POINT							m_ptOldCursorPos;    
	_TCHAR							m_pszBuffer[50];
/*						DirectX SDK Member Variables					*/

	ID3D11Device					*m_pd3dDevice;
	IDXGISwapChain					*m_pDXGISwapChain;
	ID3D11RenderTargetView			*m_pd3dRenderTargetView;
	ID3D11Texture2D					*m_pd3dDepthStencilBuffer;
	ID3D11DepthStencilView			*m_pd3dDepthStencilView;
	ID3D11DeviceContext				*m_pd3dImmediateDeviceContext;

/*						Physx SDK Member Variables					*/

	PxPhysics						*m_pPxPhysicsSDK;
	PxDefaultErrorCallback			m_PxDefaultErrorCallback;
	PxDefaultAllocator				m_PxDefaultAllocatorCallback;
	PxScene							*m_pPxScene;
	PxFoundation					*m_pPxFoundation;

/*						User Defined Member Variables					*/

	CGameTimer						m_GameTimer;
	CScene							*m_pScene;
    CPlayer							**m_ppPlayers; 

	int								m_nPlayers;
};
