//-----------------------------------------------------------------------------
// File: Scene.h
//-----------------------------------------------------------------------------

#pragma once

#include "Timer.h"
#include "Object.h"
#include "Camera.h"
#include "Shader.h"
#include "Light.h"
#include "Player.h"

class CScene
{
private:
	CShader										**m_ppShaders;
	int											m_nShaders;

	CInstancingShader							*m_pInstancingShader;
	
	LIGHTS										*m_pLights;
	ID3D11Buffer								*m_pd3dcbLights;

	PxPhysics									*m_pPxPhysicsSDK;
	PxScene										*m_pPxScene;

	CPlayer										**m_ppPlayers;
public:
    CScene(PxPhysics *pPxPhysicsSDK, PxScene *pPxScene, CPlayer **ppPlayers);
    ~CScene();

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void CreateShaders(ID3D11Device *pd3dDevice);

	void BuildObjects(ID3D11Device *pd3dDevice, PxPhysics *pPxPhysics, PxScene *pPxScene, PxControllerManager *pPxControllerManager, FbxManager *pFbxSdkManager);
	void ReleaseObjects();

	bool ProcessInput();
    void AnimateObjects(float fTimeElapsed,PxScene *pPxScene);

	void Render(ID3D11DeviceContext	*pD3DImmediateDeviceContext, int nThreadID, CCamera *pCamera);
	

	void BuildLights(ID3D11Device *pd3dDevice);
	void UpdateLights(ID3D11DeviceContext *pd3dDeviceContext);
	void ReleaseLights();

};
