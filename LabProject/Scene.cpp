//-----------------------------------------------------------------------------
// File: Scene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Scene.h"


CScene::CScene(PxPhysics *pPxPhysicsSDK, PxScene *pPxScene,CPlayer **ppPlayers)
{
	m_ppShaders = NULL;
	m_nShaders = 0;
	m_pLights = NULL;
	m_pd3dcbLights = NULL;

	m_pPxPhysicsSDK = pPxPhysicsSDK;
	m_pPxScene = pPxScene;
	m_ppPlayers = ppPlayers;
}

CScene::~CScene()
{
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		break;
	case WM_RBUTTONDOWN:
		break;
	case WM_MOUSEMOVE:
		break;
	case WM_LBUTTONUP:
		break;
	case WM_RBUTTONUP:
		break;
	default:
		break;
	}
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'F':
			m_pInstancingShader->AddObject(m_pPxPhysicsSDK, m_pPxScene, 1, 0, 1, m_ppPlayers[0]->GetPosition(), false, m_ppPlayers[0]->GetCamera()->GetLookVector() * 100);
			break;
		}
		
		break;
	default:
		break;
	}
	return(false);
}

void CScene::BuildObjects(ID3D11Device *pd3dDevice, PxPhysics *pPxPhysics, PxScene *pPxScene, FbxManager *pFbxSdkManager)
{
	m_nShaders = 2;
	m_ppShaders = new CShader*[m_nShaders];
	m_pInstancingShader = new CInstancingShader();
	m_ppShaders[0] = m_pInstancingShader;
	m_ppShaders[0]->CreateShader(pd3dDevice);
	m_ppShaders[0]->BuildObjects(pd3dDevice,pPxPhysics,pPxScene, pFbxSdkManager);

	m_ppShaders[1] = new CSkyBoxShader();
	m_ppShaders[1]->CreateShader(pd3dDevice);
	m_ppShaders[1]->BuildObjects(pd3dDevice,pPxPhysics,pPxScene, pFbxSdkManager);
	/*
	m_ppShaders[2] = new CTerrainShader();
	m_ppShaders[2]->CreateShader(pd3dDevice);
	m_ppShaders[2]->BuildObjects(pd3dDevice,pPxPhysics,pPxScene);
	*/
	BuildLights(pd3dDevice);
}

void CScene::ReleaseObjects()
{
	ReleaseLights();
	for (int j = 0; j < m_nShaders; j++) {
		if(m_ppShaders[j]) delete m_ppShaders[j];
	}
	if (m_ppShaders) delete [] m_ppShaders;
}

bool CScene::ProcessInput()
{
	return(false);
}

void CScene::AnimateObjects(float fTimeElapsed,PxScene *pPxScene)
{
	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->AnimateObjects(fTimeElapsed,pPxScene);
	}
}
void CScene::Render(ID3D11DeviceContext	*pd3dImmediateDeviceContext, int nThreadID, CRITICAL_SECTION *pCriticalSection, CCamera *pCamera)
{
	if (m_pLights && m_pd3dcbLights) 
	{
		UpdateLights(pd3dImmediateDeviceContext);
	}

	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->Render(pd3dImmediateDeviceContext, nThreadID, pCriticalSection, pCamera);
	}
}
void CScene::BuildLights(ID3D11Device *pd3dDevice)
{
	m_pLights = new LIGHTS;
	m_pLights->m_d3dxcGlobalAmbient = D3DXCOLOR(0.4f, 0.4f, 0.4f, 0.4f);

	//3°³ÀÇ Á¶¸í(Á¡ ±¤¿ø, ½ºÆÌ ±¤¿ø, ¹æÇâ¼º ±¤¿ø)À» ¼³Á¤ÇÑ´Ù.
	/*
	m_pLights->m_pLights[0].m_bEnable = 0.0f;
	m_pLights->m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[0].m_fRange = 100.0f;
	m_pLights->m_pLights[0].m_d3dxcAmbient = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[0].m_d3dxcDiffuse = D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[0].m_d3dxcSpecular = D3DXCOLOR(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[0].m_d3dxvPosition = D3DXVECTOR3(0.0f, 5.0f, 0.0f);
	m_pLights->m_pLights[0].m_d3dxvDirection = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[0].m_d3dxvAttenuation = D3DXVECTOR3(1.0f, 0.001f, 0.0001f);
	m_pLights->m_pLights[1].m_bEnable = 0.0f;
	m_pLights->m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[1].m_fRange = 150.0f;
	m_pLights->m_pLights[1].m_d3dxcAmbient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[1].m_d3dxcDiffuse = D3DXCOLOR(0.3f, 0.3f, 0.0f, 1.0f);
	m_pLights->m_pLights[1].m_d3dxcSpecular = D3DXCOLOR(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[1].m_d3dxvPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[1].m_d3dxvDirection = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[1].m_d3dxvAttenuation = D3DXVECTOR3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[1].m_fFalloff = 8.0f;
	m_pLights->m_pLights[1].m_fPhi = (float)cos(D3DXToRadian(40.0f));
	m_pLights->m_pLights[1].m_fTheta = (float)cos(D3DXToRadian(20.0f));*/
	m_pLights->m_pLights[0].m_bEnable = 1.0f;
	m_pLights->m_pLights[0].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[0].m_d3dxcAmbient = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[0].m_d3dxcDiffuse = D3DXCOLOR(0.7f, 0.7f, 0.7f, 0.7f);
	m_pLights->m_pLights[0].m_d3dxcSpecular = D3DXCOLOR(0.7f, 0.7f, 0.7f, 0.7f);
	m_pLights->m_pLights[0].m_d3dxvDirection = D3DXVECTOR3(0.0f, -1.0f, 0.0f);

	

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(d3dBufferDesc));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(LIGHTS);
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pLights;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dcbLights);
}
void CScene::ReleaseLights()
{
	if (m_pLights) delete m_pLights;
	if (m_pd3dcbLights) m_pd3dcbLights->Release();
}
void CScene::UpdateLights(ID3D11DeviceContext *pd3dDeviceContext)
{
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbLights, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	LIGHTS *pcbLight = (LIGHTS *)d3dMappedResource.pData;
	memcpy(pcbLight, m_pLights, sizeof(LIGHTS));
	pd3dDeviceContext->Unmap(m_pd3dcbLights, 0);
	pd3dDeviceContext->PSSetConstantBuffers(PS_SLOT_LIGHT, 1, &m_pd3dcbLights);
}
