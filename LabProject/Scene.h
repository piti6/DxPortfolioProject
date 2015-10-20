//-----------------------------------------------------------------------------
// File: Scene.h
//-----------------------------------------------------------------------------

#pragma once

#include "Timer.h"
#include "Object.h"
#include "Camera.h"
#include "Shader.h"
#include "Light.h"

class CScene
{
private:
	CShader										**m_ppShaders;
	int											m_nShaders;
	LIGHTS										*m_pLights;
	ID3D11Buffer								*m_pd3dcbLights;
public:
    CScene();
    ~CScene();

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void CreateShaders(ID3D11Device *pd3dDevice);

	void BuildObjects(ID3D11Device *pd3dDevice);
	void ReleaseObjects();

	bool ProcessInput();
    void AnimateObjects(float fTimeElapsed);

    void Render(ID3D11DeviceContext	*pD3DImmediateDeviceContext, CCamera *pCamera);
	

	void BuildLights(ID3D11Device *pd3dDevice);
	void UpdateLights(ID3D11DeviceContext *pd3dDeviceContext);
	void ReleaseLights();

};
