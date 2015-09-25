//-----------------------------------------------------------------------------
// File: Scene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Scene.h"

CScene::CScene()
{
	m_pShaders = NULL;
	m_nShaders = 0;
	m_pLights = NULL;
	m_pd3dcbLights = NULL;

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
			break;
		default:
			break;
	}
	return(false);
}

void CScene::BuildObjects(ID3D11Device *pd3dDevice)
{
	m_nShaders = 1;
	m_pShaders = new CTexturedIlluminatedShader[m_nShaders];
	m_pShaders[0].CreateShader(pd3dDevice);
	m_pShaders[0].BuildObjects(pd3dDevice);
	BuildLights(pd3dDevice);
}

void CScene::ReleaseObjects()
{
	for (int j = 0; j < m_nShaders; j++) m_pShaders[j].ReleaseObjects();
	if (m_pShaders) delete [] m_pShaders;
}

bool CScene::ProcessInput()
{
	return(false);
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	for (int i = 0; i < m_nShaders; i++)
	{
		m_pShaders[i].AnimateObjects(fTimeElapsed);
	}
}

void CScene::Render(ID3D11DeviceContext	*pd3dImmediateDeviceContext, CCamera *pCamera)
{
	if (m_pLights && m_pd3dcbLights) 
	{
/*두 번째 조명은 플레이어가 가지고 있는 손전등이다. 그러므로 카메라의 위치가 바뀌면 조명의 위치와 방향을 카메라의 위치와 방향으로 변경한다.*/ 
		D3DXVECTOR3 d3dxvCameraPosition = pCamera->GetPosition();
		m_pLights->m_d3dxvCameraPosition = D3DXVECTOR4(d3dxvCameraPosition, 1.0f);

		m_pLights->m_pLights[1].m_d3dxvPosition = d3dxvCameraPosition;
		m_pLights->m_pLights[1].m_d3dxvDirection = pCamera->GetLookVector();
		

		UpdateLights(pd3dImmediateDeviceContext);
	}

	for (int i = 0; i < m_nShaders; i++)
	{
		m_pShaders[i].Render(pd3dImmediateDeviceContext, pCamera);
	}
}
void CScene::BuildLights(ID3D11Device *pd3dDevice)
{
	m_pLights = new LIGHTS;
	//::ZeroMemory(m_pLights, sizeof(LIGHTS));
//게임 월드 전체를 비추는 주변조명을 설정한다.
	m_pLights->m_d3dxcGlobalAmbient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);

//3개의 조명(점 광원, 스팟 광원, 방향성 광원)을 설정한다.
	m_pLights->m_pLights[0].m_bEnable = 1.0f;
	m_pLights->m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[0].m_fRange = 100.0f;
	m_pLights->m_pLights[0].m_d3dxcAmbient = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[0].m_d3dxcDiffuse = D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[0].m_d3dxcSpecular = D3DXCOLOR(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[0].m_d3dxvPosition = D3DXVECTOR3(0.0f, 5.0f, 0.0f);
	m_pLights->m_pLights[0].m_d3dxvDirection = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[0].m_d3dxvAttenuation = D3DXVECTOR3(1.0f, 0.001f, 0.0001f);
	m_pLights->m_pLights[1].m_bEnable = 1.0f;
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
	m_pLights->m_pLights[1].m_fTheta = (float)cos(D3DXToRadian(20.0f));
	m_pLights->m_pLights[2].m_bEnable = 1.0f;
	m_pLights->m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[2].m_d3dxcAmbient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[2].m_d3dxcDiffuse = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[2].m_d3dxcSpecular = D3DXCOLOR(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[2].m_d3dxvDirection = D3DXVECTOR3(0.0f, -1.0f, 0.0f);

	for(int i=3;i<10; ++i)
	{

	m_pLights->m_pLights[i].m_bEnable = 1.0f;
	m_pLights->m_pLights[i].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[i].m_fRange = 100.0f;
	m_pLights->m_pLights[i].m_d3dxcAmbient = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[i].m_d3dxcDiffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_pLights->m_pLights[i].m_d3dxcSpecular = D3DXCOLOR(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[i].m_d3dxvDirection = D3DXVECTOR3(0, 0, 0);
	m_pLights->m_pLights[i].m_d3dxvAttenuation = D3DXVECTOR3(1.0f, 0.001f, 0.0001f);
	}
	m_pLights->m_pLights[3].m_d3dxvPosition = D3DXVECTOR3(-50, 0, 0);
	m_pLights->m_pLights[4].m_d3dxvPosition = D3DXVECTOR3(100, 0, 100);
	m_pLights->m_pLights[5].m_d3dxvPosition = D3DXVECTOR3(150, 0, 100);
	m_pLights->m_pLights[6].m_d3dxvPosition = D3DXVECTOR3(250, 0, 300);
	
	
	
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
