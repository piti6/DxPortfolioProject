//-----------------------------------------------------------------------------
// File: Scene.h
//-----------------------------------------------------------------------------

#pragma once

#include "Timer.h"
#include "Object.h"
#include "Camera.h"
#include "Shader.h"
#include "Light.h"
/*
//#pragma once
#define MAX_LIGHTS			10 //Multiple of 4
#define POINT_LIGHT			1.0f
#define SPOT_LIGHT			2.0f
#define DIRECTIONAL_LIGHT	3.0f

struct LIGHT
{
	D3DXCOLOR				m_d3dxcAmbient;
	D3DXCOLOR				m_d3dxcDiffuse;
	D3DXCOLOR				m_d3dxcSpecular;
	D3DXVECTOR3				m_d3dxvPosition;
	float					m_fRange;
	D3DXVECTOR3				m_d3dxvDirection;
	float					m_nType;
	D3DXVECTOR3				m_d3dxvAttenuation; 
	float 					m_fFalloff;
	float 					m_fTheta; //cos(m_fTheta)
	float					m_fPhi; //cos(m_fPhi)
	float					m_bEnable; 
	float					padding;
};

struct LIGHTS
{
	LIGHT					m_pLights[MAX_LIGHTS];
	D3DXCOLOR				m_d3dxcGlobalAmbient;
	D3DXVECTOR4				m_d3dxvCameraPosition;
};

*/
class CScene
{
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
	CSceneShader					*m_pShaders;
	int								m_nShaders;
	LIGHTS *m_pLights;
	ID3D11Buffer *m_pd3dcbLights;

	void BuildLights(ID3D11Device *pd3dDevice);
	void UpdateLights(ID3D11DeviceContext *pd3dDeviceContext);
	void ReleaseLights();

};
