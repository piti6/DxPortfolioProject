#pragma once

#include "Timer.h"
#include "Player.h"
#include "Scene.h"

class CGameFramework
{
public:
	bool Jump;
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

	void SetActive(bool bActive) { m_bActive = bActive; }
	
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

private:   
	HINSTANCE						m_hInstance;
	HWND							m_hWnd; 
	HMENU							m_hMenu;

	int								m_nWndClientWidth;
	int								m_nWndClientHeight;
        
	ID3D11Device					*m_pd3dDevice;
	IDXGISwapChain					*m_pDXGISwapChain;
	ID3D11RenderTargetView			*m_pd3dRenderTargetView;
	ID3D11Texture2D					*m_pd3dDepthStencilBuffer;
	ID3D11DepthStencilView			*m_pd3dDepthStencilView;
	ID3D11DeviceContext				*m_pd3dImmediateDeviceContext;

	CGameTimer						m_GameTimer;

	CScene							*m_pScene;

	int								m_nPlayers;
    CPlayer							**m_ppPlayers; 

    bool							m_bActive;  

	POINT							m_ptOldCursorPos;    
	_TCHAR							m_pszBuffer[50];
};
