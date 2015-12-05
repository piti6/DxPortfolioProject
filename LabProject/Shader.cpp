//-----------------------------------------------------------------------------
// File: Shader.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Shader.h"

#define MAX_INSTANCE 200

CShader::CShader()
{
	m_pd3dVertexShader = NULL;
	m_pd3dPixelShader = NULL;
	m_pd3dVertexLayout = NULL;
	m_pd3dcbWorldMatrix = NULL;
	m_pd3dcbMaterial = NULL;
}

CShader::~CShader()
{
	if (m_pd3dVertexShader) m_pd3dVertexShader->Release();
	if (m_pd3dPixelShader) m_pd3dPixelShader->Release();
	if (m_pd3dVertexLayout) m_pd3dVertexLayout->Release();
	if (m_pd3dcbWorldMatrix) m_pd3dcbWorldMatrix->Release();
	if (m_pd3dcbMaterial) m_pd3dcbMaterial->Release();
	for (int i = 0; i < m_ObjectsVector.size(); ++i)
		delete m_ObjectsVector[i].second;
}

void CShader::CreateVertexShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11VertexShader **ppd3dVertexShader, D3D11_INPUT_ELEMENT_DESC *pd3dInputLayout, UINT nElements, ID3D11InputLayout **ppd3dVertexLayout)
{
	HRESULT hResult;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob *pd3dVertexShaderBlob = NULL, *pd3dErrorBlob = NULL;
	if (SUCCEEDED(hResult = D3DX11CompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderModel, dwShaderFlags, 0, NULL, &pd3dVertexShaderBlob, &pd3dErrorBlob, NULL)))
	{
		pd3dDevice->CreateVertexShader(pd3dVertexShaderBlob->GetBufferPointer(), pd3dVertexShaderBlob->GetBufferSize(), NULL, ppd3dVertexShader);
		pd3dDevice->CreateInputLayout(pd3dInputLayout, nElements, pd3dVertexShaderBlob->GetBufferPointer(), pd3dVertexShaderBlob->GetBufferSize(), ppd3dVertexLayout);
		pd3dVertexShaderBlob->Release();
	}
}
void CShader::CreatePixelShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11PixelShader **ppd3dPixelShader)
{
	HRESULT hResult;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob *pd3dPixelShaderBlob = NULL, *pd3dErrorBlob = NULL;
	if (SUCCEEDED(hResult = D3DX11CompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderModel, dwShaderFlags, 0, NULL, &pd3dPixelShaderBlob, &pd3dErrorBlob, NULL)))
	{
		pd3dDevice->CreatePixelShader(pd3dPixelShaderBlob->GetBufferPointer(), pd3dPixelShaderBlob->GetBufferSize(), NULL, ppd3dPixelShader);
		pd3dPixelShaderBlob->Release();
	}
}

void CShader::CreateShader(ID3D11Device *pd3dDevice)
{
}
void CShader::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
}

void CShader::UpdateShaderVariables(ID3D11DeviceContext *pd3dImmediateDeviceContext, MATERIAL *pMaterial)
{
}
void CShader::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, CTexture *pTexture)
{
}
void CShader::UpdateShaderVariables(ID3D11DeviceContext *pd3dImmediateDeviceContext, D3DXMATRIX *pd3dxmtxWorld)
{
}

void CShader::BuildObjects(ID3D11Device *pd3dDevice, PxPhysics *pPxPhysics, PxScene *pPxScene, FbxManager *pFbxSdkManager)
{
}
void CShader::AnimateObjects(float fTimeElapsed, PxScene *pPxScene)
{
	for (int i = 0; i < m_ObjectsVector.size(); ++i)
	{
		m_ObjectsVector[i].second->Animate(fTimeElapsed, pPxScene);
	}
}

void CShader::OnPostRender(ID3D11DeviceContext *pd3dDeviceContext)
{
	pd3dDeviceContext->IASetInputLayout(m_pd3dVertexLayout);
	pd3dDeviceContext->VSSetShader(m_pd3dVertexShader, NULL, 0);
	pd3dDeviceContext->PSSetShader(m_pd3dPixelShader, NULL, 0);
}
void CShader::Render(ID3D11DeviceContext *pd3dImmediateDeviceContext, int nThreadID, CCamera *pCamera)
{
}

vector<pair<int, CGameObject*>>* CShader::GetObjectsVector()
{
	return &m_ObjectsVector;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//

CTexturedIlluminatedShader::CTexturedIlluminatedShader()
{
}
CTexturedIlluminatedShader::~CTexturedIlluminatedShader()
{
}

void CTexturedIlluminatedShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSTexturedLighting", "vs_5_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSTexturedLighting", "ps_5_0", &m_pd3dPixelShader);

}
void CTexturedIlluminatedShader::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VS_CB_WORLD_MATRIX);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbWorldMatrix);

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	d3dBufferDesc.ByteWidth = sizeof(MATERIAL);
	pd3dDevice->CreateBuffer(&d3dBufferDesc, NULL, &m_pd3dcbMaterial);
}

void CTexturedIlluminatedShader::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, CTexture *pTexture)
{
	pd3dDeviceContext->PSSetShaderResources(PS_SLOT_TEXTURE, pTexture->m_nTextures, pTexture->m_ppd3dsrvTextures);
	pd3dDeviceContext->PSSetSamplers(PS_SLOT_SAMPLER_STATE, pTexture->m_nTextures, pTexture->m_ppd3dSamplerStates);
}
void CTexturedIlluminatedShader::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, MATERIAL *pMaterial)
{
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbMaterial, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	MATERIAL *pcbMaterial = (MATERIAL *)d3dMappedResource.pData;
	memcpy(pcbMaterial, pMaterial, sizeof(MATERIAL));
	pd3dDeviceContext->Unmap(m_pd3dcbMaterial, 0);
	pd3dDeviceContext->PSSetConstantBuffers(PS_SLOT_MATERIAL, 1, &m_pd3dcbMaterial);
}
void CTexturedIlluminatedShader::UpdateShaderVariables(ID3D11DeviceContext *pd3dImmediateDeviceContext, D3DXMATRIX *pd3dxmtxWorld)
{
	//Update World Matrix Constant Buffer
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dImmediateDeviceContext->Map(m_pd3dcbWorldMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_CB_WORLD_MATRIX *pcbWorldMatrix = (VS_CB_WORLD_MATRIX *)d3dMappedResource.pData;
	D3DXMatrixTranspose(&pcbWorldMatrix->m_d3dxmtxWorld, pd3dxmtxWorld);
	pd3dImmediateDeviceContext->Unmap(m_pd3dcbWorldMatrix, 0);
	pd3dImmediateDeviceContext->VSSetConstantBuffers(VS_SLOT_WORLD_MATRIX, 1, &m_pd3dcbWorldMatrix);
}

void CTexturedIlluminatedShader::BuildObjects(ID3D11Device *pd3dDevice, PxPhysics *pPxPhysics, PxScene *pPxScene, FbxManager *pFbxSdkManager){

}

void CTexturedIlluminatedShader::Render(ID3D11DeviceContext *pd3dImmediateDeviceContext, int nThreadID, CCamera *pCamera)
{
	OnPostRender(pd3dImmediateDeviceContext);
	//카메라의 절두체에 포함되는 객체들만을 렌더링한다. 
	bool bIsVisible = false;
	AABB bcBoundingCube;
	for (int i = 0; i < m_ObjectsVector.size(); ++i)
	{
		if (i % MAX_THREAD == nThreadID) continue;
		if (m_ObjectsVector[i].second->isActive())
		{
			//객체의 메쉬의 바운딩 박스(모델 좌표계)를 객체의 월드 변환 행렬로 변환하고 새로운 바운딩 박스를 계산한다.
			bcBoundingCube = m_ObjectsVector[i].second->GetMesh(0)->GetBoundingCube();
			bcBoundingCube.Transform(&m_ObjectsVector[i].second->GetWorldMatrix());
			//바운딩 박스(월드 좌표계)가 카메라의 절두체에 포함되는 가를 검사하고 포함되는 경우에 렌더링한다. 
			bIsVisible = pCamera->IsInFrustum(bcBoundingCube.GetMinimum(), bcBoundingCube.GetMaximum());
			//bIsVisible = true;

			if (bIsVisible)
			{

				if (m_ObjectsVector[i].second->GetTexture())
					UpdateShaderVariables(pd3dImmediateDeviceContext, m_ObjectsVector[i].second->GetTexture());
				if (m_ObjectsVector[i].second->GetMaterial())
					UpdateShaderVariables(pd3dImmediateDeviceContext, &m_ObjectsVector[i].second->GetMaterial()->GetMaterial());
				UpdateShaderVariables(pd3dImmediateDeviceContext, &m_ObjectsVector[i].second->GetWorldMatrix());
				m_ObjectsVector[i].second->Render(pd3dImmediateDeviceContext);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//

CPlayerShader::CPlayerShader()
{
}
CPlayerShader::~CPlayerShader()
{
}

void CPlayerShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSTexturedLighting", "vs_5_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSTexturedLighting", "ps_5_0", &m_pd3dPixelShader);
}
void CPlayerShader::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	//Create World Matrix Constant Buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VS_CB_WORLD_MATRIX);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbWorldMatrix);

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	d3dBufferDesc.ByteWidth = sizeof(MATERIAL);
	pd3dDevice->CreateBuffer(&d3dBufferDesc, NULL, &m_pd3dcbMaterial);
}

void CPlayerShader::UpdateShaderVariables(ID3D11DeviceContext *pd3dImmediateDeviceContext, D3DXMATRIX *pd3dxmtxWorld)
{
	//Update World Matrix Constant Buffer
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dImmediateDeviceContext->Map(m_pd3dcbWorldMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_CB_WORLD_MATRIX *pcbWorldMatrix = (VS_CB_WORLD_MATRIX *)d3dMappedResource.pData;
	D3DXMatrixTranspose(&pcbWorldMatrix->m_d3dxmtxWorld, pd3dxmtxWorld);
	pd3dImmediateDeviceContext->Unmap(m_pd3dcbWorldMatrix, 0);
	pd3dImmediateDeviceContext->VSSetConstantBuffers(VS_SLOT_WORLD_MATRIX, 1, &m_pd3dcbWorldMatrix);
}
void CPlayerShader::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, MATERIAL *pMaterial)
{
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbMaterial, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	MATERIAL *pcbMaterial = (MATERIAL *)d3dMappedResource.pData;
	memcpy(pcbMaterial, pMaterial, sizeof(MATERIAL));
	pd3dDeviceContext->Unmap(m_pd3dcbMaterial, 0);
	pd3dDeviceContext->PSSetConstantBuffers(PS_SLOT_MATERIAL, 1, &m_pd3dcbMaterial);
}
void CPlayerShader::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, CTexture *pTexture)
{
	pd3dDeviceContext->PSSetShaderResources(PS_SLOT_TEXTURE, pTexture->m_nTextures, pTexture->m_ppd3dsrvTextures);
	pd3dDeviceContext->PSSetSamplers(PS_SLOT_SAMPLER_STATE, pTexture->m_nTextures, pTexture->m_ppd3dSamplerStates);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//

CTexturedShader::CTexturedShader()
{
}
CTexturedShader::~CTexturedShader()
{
}
void CTexturedShader::CreateShader(ID3D11Device *pd3dDevice)
{
	CShader::CreateShader(pd3dDevice);

	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSTexturedColor", "vs_5_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSTexturedColor", "ps_5_0", &m_pd3dPixelShader);
}
void CTexturedShader::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VS_CB_WORLD_MATRIX);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbWorldMatrix);
}

void CTexturedShader::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, CTexture *pTexture)
{
	pd3dDeviceContext->PSSetShaderResources(PS_SLOT_TEXTURE, pTexture->m_nTextures, pTexture->m_ppd3dsrvTextures);
	pd3dDeviceContext->PSSetSamplers(PS_SLOT_SAMPLER_STATE, pTexture->m_nTextures, pTexture->m_ppd3dSamplerStates);
}
void CTexturedShader::UpdateShaderVariables(ID3D11DeviceContext *pd3dImmediateDeviceContext, D3DXMATRIX *pd3dxmtxWorld)
{
	//Update World Matrix Constant Buffer
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dImmediateDeviceContext->Map(m_pd3dcbWorldMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_CB_WORLD_MATRIX *pcbWorldMatrix = (VS_CB_WORLD_MATRIX *)d3dMappedResource.pData;
	D3DXMatrixTranspose(&pcbWorldMatrix->m_d3dxmtxWorld, pd3dxmtxWorld);
	pd3dImmediateDeviceContext->Unmap(m_pd3dcbWorldMatrix, 0);
	pd3dImmediateDeviceContext->VSSetConstantBuffers(VS_SLOT_WORLD_MATRIX, 1, &m_pd3dcbWorldMatrix);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//

CInstancingShader::CInstancingShader()
{
	m_pd3dcbAnimationTextureWidth = NULL;
}
CInstancingShader::~CInstancingShader()
{
	for (int i = 0; i < m_InstanceDataVector.size(); ++i)
	{
		m_InstanceDataVector[i]->Release();
		delete m_InstanceDataVector[i];
	}
}

void CInstancingShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "INSTANCEPOS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEPOS", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEPOS", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEPOS", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "ANIMATIONOFFSET", 0, DXGI_FORMAT_R32G32B32A32_UINT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};

	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSInstancedTexturedLightingAnimation", "vs_5_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSInstancedTexturedLightingAnimation", "ps_5_0", &m_pd3dPixelShader);
}
void CInstancingShader::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	CTexturedIlluminatedShader::CreateShaderVariables(pd3dDevice);
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(ANIM_WIDTH);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbAnimationTextureWidth);
}

void CInstancingShader::BuildObjects(ID3D11Device *pd3dDevice, PxPhysics *pPxPhysics, PxScene *pPxScene, FbxManager *pFbxSdkManager){
	CreateShaderVariables(pd3dDevice);

	CMaterial *pMaterial = new CMaterial(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	m_MaterialsVector.push_back(pMaterial);
	ID3D11SamplerState *pd3dSamplerState = NULL;
	D3D11_SAMPLER_DESC d3dSamplerDesc;
	ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = 0;
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dSamplerState);

	ID3D11ShaderResourceView *pd3dTexture = NULL;
	CTexture *pTexture = new CTexture(1);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Data/Image/Ground/Ground_02.png"), NULL, NULL, &pd3dTexture, NULL);
	pTexture->SetTexture(0, pd3dTexture, pd3dSamplerState);
	m_TexturesVector.push_back(pTexture);

	pTexture = new CTexture(1);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Data/Image/Model/Atlas1.png"), NULL, NULL, &pd3dTexture, NULL);
	pTexture->SetTexture(0, pd3dTexture, pd3dSamplerState);
	m_TexturesVector.push_back(pTexture);

	pTexture = new CTexture(1);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Data/Image/Model/Roads.png"), NULL, NULL, &pd3dTexture, NULL);
	pTexture->SetTexture(0, pd3dTexture, pd3dSamplerState);
	m_TexturesVector.push_back(pTexture);

	pTexture = new CTexture(1);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Data/Image/Model/cars.png"), NULL, NULL, &pd3dTexture, NULL);
	pTexture->SetTexture(0, pd3dTexture, pd3dSamplerState);
	m_TexturesVector.push_back(pTexture);

	pTexture = new CTexture(1);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Data/Image/Model/mon_goblinWizard_df.png"), NULL, NULL, &pd3dTexture, NULL);
	pTexture->SetTexture(0, pd3dTexture, pd3dSamplerState);
	m_TexturesVector.push_back(pTexture);



	UINT nMatrixBufferStride = sizeof(InstanceBuffer);
	UINT nMatrixBufferOffset = 0;

	//인스턴스 쉐이더에서 렌더링할 메쉬이다.
	PxMaterial *pPxM = pPxPhysics->createMaterial(0.9, 0.9, 0.001);
	
	m_InstanceDataVector.push_back(new InstanceData(new CCubeMeshIlluminatedTextured(pd3dDevice, 1.0f, 1.0f, 1.0f), false,"Cube1", CreateInstanceBuffer(pd3dDevice, MAX_INSTANCE, sizeof(InstanceBuffer), NULL)));
	CStaticObject *pCubeObject = NULL;
	pCubeObject = new CStaticObject();
	pCubeObject->SetMesh(m_InstanceDataVector[0]->GetMesh());
	pCubeObject->SetMaterial(pMaterial);
	pCubeObject->SetTexture(m_TexturesVector[0]);
	pCubeObject->BuildObjects(pPxPhysics, pPxScene, pPxM);
	pCubeObject->SetPosition(D3DXVECTOR3(0, 0, 0));
	m_ObjectsVector.push_back(make_pair(0, pCubeObject));
	
	/*
	m_InstanceDataVector.push_back(new InstanceData(new CFbxMeshIlluminatedTextured(pd3dDevice, pFbxSdkManager, "Data/Model/Map/Brickwall1B.fbx"), false, "Test", CreateInstanceBuffer(pd3dDevice, MAX_INSTANCE, sizeof(InstanceBuffer), NULL)));
	CStaticObject *pMapObject = NULL;
	pMapObject = new CStaticObject();
	pMapObject->SetMesh(m_InstanceDataVector[1]->GetMesh());
	pMapObject->SetMaterial(pMaterial);
	pMapObject->SetTexture(m_TexturesVector[0]);
	pMapObject->BuildObjects(pPxPhysics, pPxScene, pPxM);
	pMapObject->SetPosition(D3DXVECTOR3(0, 0, 0));
	m_ObjectsVector.push_back(make_pair(1, pMapObject));
	
	
	m_InstanceDataVector.push_back(new InstanceData(new CCubeMeshIlluminatedTextured(pd3dDevice, 2000.0f, 0.1f, 2000.0f, 200), false, "TerrainPlane", CreateInstanceBuffer(pd3dDevice, MAX_INSTANCE, sizeof(InstanceBuffer), NULL)));
	CStaticObject *pPlaneObject = NULL;
	pPlaneObject = new CStaticObject();
	pPlaneObject->SetMesh(m_InstanceDataVector[2]->GetMesh());
	pPlaneObject->SetMaterial(pMaterial);
	pPlaneObject->SetTexture(m_TexturesVector[1]);
	pPlaneObject->BuildObjects(pPxPhysics, pPxScene, pPxM);
	pPlaneObject->SetPosition(D3DXVECTOR3(0, 0, 0));
	m_ObjectsVector.push_back(make_pair(2, pPlaneObject));
	*/
	m_InstanceDataVector.push_back(new InstanceData(new CCubeMeshIlluminatedTextured(pd3dDevice, 2000.0f, 0.1f, 2000.0f, 200), false, "TerrainPlane", CreateInstanceBuffer(pd3dDevice, MAX_INSTANCE, sizeof(InstanceBuffer), NULL)));
	CStaticObject *pPlaneObject = NULL;
	pPlaneObject = new CStaticObject();
	pPlaneObject->SetMesh(m_InstanceDataVector[1]->GetMesh());
	pPlaneObject->SetMaterial(pMaterial);
	pPlaneObject->SetTexture(m_TexturesVector[0]);
	pPlaneObject->BuildObjects(pPxPhysics, pPxScene, pPxM);
	pPlaneObject->SetPosition(D3DXVECTOR3(0, 0, 0));
	m_ObjectsVector.push_back(make_pair(1, pPlaneObject));
	//
	/*
	m_InstanceDataVector.push_back(new InstanceData(new CFbxMeshIlluminatedTextured(pd3dDevice, pFbxSdkManager, "Data/Model/mon_goblinWizard@Attack01.fbx", 1.0f, true), true, "Monster1", CreateInstanceBuffer(pd3dDevice, MAX_INSTANCE, sizeof(InstanceBuffer), NULL)));
	ID3D11Texture2D *pd3dAnimationTexture = NULL;
	m_InstanceDataVector[2]->GetAnimationInstancing()->LoadAnimationFromFile(pFbxSdkManager, "Data/Model/mon_goblinWizard@Attack01.fbx", "Attack", true);
	m_InstanceDataVector[2]->GetAnimationInstancing()->LoadAnimationFromFile(pFbxSdkManager, "Data/Model/mon_goblinWizard@Run.fbx", "Run", true);
	m_InstanceDataVector[2]->GetAnimationInstancing()->CreateAnimationTexture(pd3dDevice);
	CDynamicObject *pCharacter = NULL;
	for (int i = 0; i < 100; ++i){
		pCharacter = new CDynamicObject(true);
		CAnimationController *pAnimationController = pCharacter->GetAnimationController();
		pAnimationController->SetAnimationData("Attack", m_InstanceDataVector[2]->GetAnimationInstancing()->m_vAnimationList.m_Animation["Attack"].m_fLength);
		pAnimationController->SetAnimationData("Run", m_InstanceDataVector[2]->GetAnimationInstancing()->m_vAnimationList.m_Animation["Run"].m_fLength);
		if (i%2)
			pAnimationController->Play("Attack");
		else
			pAnimationController->Play("Run");
		pAnimationController->SetCurrentAnimationTime((float)(rand() % 300/100));
		pCharacter->SetMesh(m_InstanceDataVector[2]->GetMesh());
		pCharacter->SetMaterial(pMaterial);
		pCharacter->SetTexture(m_TexturesVector[4]);
		pCharacter->SetOffset(D3DXVECTOR3(0, -30, 0));
		pCharacter->BuildObjects(pPxPhysics, pPxScene);
		pCharacter->SetPosition(D3DXVECTOR3(200, i*10, 200));

		m_ObjectsVector.push_back(make_pair(2, pCharacter));
	}
	*/
	
	ifstream ifsFbxList;
	ifsFbxList.open("Data/ImportData/ImportModelName.txt");

	string FbxName;
	while (!ifsFbxList.eof())
	{
		ifsFbxList >> FbxName;
		string FilePath = "Data/Model/Map/" + FbxName + ".fbx";

		m_InstanceDataVector.push_back(new InstanceData(new CFbxMeshIlluminatedTextured(pd3dDevice, pFbxSdkManager, (char*)FilePath.c_str()), false, FbxName, CreateInstanceBuffer(pd3dDevice, MAX_INSTANCE, sizeof(InstanceBuffer), NULL)));
	}
	ifsFbxList.close();

	ifsFbxList.open("Data/ImportData/ImportModelNameRoad.txt");

	while (!ifsFbxList.eof())
	{
		ifsFbxList >> FbxName;
		string FilePath = "Data/Model/Map/Roads/" + FbxName + ".fbx";

		m_InstanceDataVector.push_back(new InstanceData(new CFbxMeshIlluminatedTextured(pd3dDevice, pFbxSdkManager, (char*)FilePath.c_str()), false, FbxName, CreateInstanceBuffer(pd3dDevice, MAX_INSTANCE, sizeof(InstanceBuffer), NULL)));
	}
	ifsFbxList.close();

	ifstream ifsModelTransform;
	D3DXVECTOR3 d3dxvPosition;
	D3DXVECTOR3 d3dxvEulerRotation;
	ifsModelTransform.open("Data/ImportData/ModelInform.txt");
	while (!ifsModelTransform.eof())
	{
		ifsModelTransform >> FbxName;
		ifsModelTransform >> d3dxvPosition.x;
		ifsModelTransform >> d3dxvPosition.y;
		ifsModelTransform >> d3dxvPosition.z;
		ifsModelTransform >> d3dxvEulerRotation.x;
		ifsModelTransform >> d3dxvEulerRotation.y;
		ifsModelTransform >> d3dxvEulerRotation.z;

		for (int i = 0; i < m_InstanceDataVector.size(); ++i)
		{
			if (m_InstanceDataVector[i]->GetName() == FbxName)
			{
				CStaticObject *pMapObject = NULL;
				pMapObject = new CStaticObject();
				pMapObject->SetMesh(m_InstanceDataVector[i]->GetMesh());
				pMapObject->SetMaterial(pMaterial);
				pMapObject->SetTexture(m_TexturesVector[1]);
				pMapObject->BuildObjects(pPxPhysics, pPxScene, pPxM);
				pMapObject->Rotate(d3dxvEulerRotation.x, d3dxvEulerRotation.y, d3dxvEulerRotation.z);
				pMapObject->SetPosition(d3dxvPosition);
				m_ObjectsVector.push_back(make_pair(i, pMapObject));
				break;
			}
		}
	}
	ifsModelTransform.close();

	ifsModelTransform.open("Data/ImportData/ModelInformRoad.txt");
	while (!ifsModelTransform.eof())
	{
		ifsModelTransform >> FbxName;
		ifsModelTransform >> d3dxvPosition.x;
		ifsModelTransform >> d3dxvPosition.y;
		ifsModelTransform >> d3dxvPosition.z;
		ifsModelTransform >> d3dxvEulerRotation.x;
		ifsModelTransform >> d3dxvEulerRotation.y;
		ifsModelTransform >> d3dxvEulerRotation.z;

		for (int i = 0; i < m_InstanceDataVector.size(); ++i)
		{
			if (m_InstanceDataVector[i]->GetName() == FbxName)
			{
				CStaticObject *pMapObject = NULL;
				pMapObject = new CStaticObject();
				pMapObject->SetMesh(m_InstanceDataVector[i]->GetMesh());
				pMapObject->SetMaterial(pMaterial);
				pMapObject->SetTexture(m_TexturesVector[2]);
				pMapObject->BuildObjects(pPxPhysics, pPxScene, pPxM);

				/*
				d3dxvPosition.y = 0;
				D3DXVECTOR3 a = D3DXVECTOR3(0, 1, 0);*/
				//pMapObject->Rotate(&a, d3dxvEulerRotation.y);
				pMapObject->Rotate(d3dxvEulerRotation.x, d3dxvEulerRotation.y, d3dxvEulerRotation.z);
				//pMapObject->Rotate(&pMapObject->GetUp(), d3dxvEulerRotation.y);

				pMapObject->SetPosition(d3dxvPosition);


				m_ObjectsVector.push_back(make_pair(i, pMapObject));
				break;
			}
		}
	}
	ifsModelTransform.close();
	
	//인스턴스 데이터(렌더링할 객체들의 위치 벡터 배열)를 메쉬의 정점 버퍼에 추가한다.
	for (int i = 0; i < m_InstanceDataVector.size(); ++i){
		ID3D11Buffer *InstanceBuffer = m_InstanceDataVector[i]->GetInstanceBuffer();
		m_InstanceDataVector[i]->GetMesh()->AppendVertexBuffer(1, &InstanceBuffer, &nMatrixBufferStride, &nMatrixBufferOffset);
	}
}

void CInstancingShader::AddObject(PxPhysics *pPxPhysics, PxScene *pPxScene, int _IndexOfInstanceDataVector, int _IndexOfMaterial, int _IndexOfTexture, D3DXVECTOR3 _d3dxvPosition, int _isStatic, D3DXVECTOR3 _Force)
{
	if (_IndexOfInstanceDataVector >= m_InstanceDataVector.size())
	{
		cout << "InstanceDataVector index out of range." << endl;
		return;
	}
	if (_IndexOfMaterial >= m_MaterialsVector.size())
	{
		cout << "MaterialsVector index out of range." << endl;
		return;
	}
	if (_IndexOfTexture >= m_TexturesVector.size())
	{
		cout << "TexturesVector index out of range." << endl;
		return;
	}

	PxMaterial *pPxM = pPxPhysics->createMaterial(0.9, 0.9, 0.001);
	if (_isStatic)
	{
		CStaticObject *pObject = new CStaticObject();
		pObject->SetMesh(m_InstanceDataVector[_IndexOfInstanceDataVector]->GetMesh());
		pObject->SetMaterial(m_MaterialsVector[_IndexOfMaterial]);
		pObject->SetTexture(m_TexturesVector[_IndexOfTexture]);
		pObject->BuildObjects(pPxPhysics, pPxScene, pPxM);
		pObject->SetPosition(_d3dxvPosition);
		m_ObjectsVector.push_back(make_pair(_IndexOfInstanceDataVector, pObject));
	}
	else
	{
		CDynamicObject *pObject = new CDynamicObject();
		pObject->SetMesh(m_InstanceDataVector[_IndexOfInstanceDataVector]->GetMesh());
		pObject->SetMaterial(m_MaterialsVector[_IndexOfMaterial]);
		pObject->SetTexture(m_TexturesVector[_IndexOfTexture]);
		pObject->BuildObjects(pPxPhysics, pPxScene);
		pObject->SetPosition(_d3dxvPosition);
		pObject->AddForce(_Force.x, _Force.y, _Force.z);
		m_ObjectsVector.push_back(make_pair(_IndexOfInstanceDataVector, pObject));
	}
}

ID3D11Buffer *CInstancingShader::CreateInstanceBuffer(ID3D11Device *pd3dDevice, int nObjects, UINT nBufferStride, void *pBufferData)
{
	ID3D11Buffer *pd3dInstanceBuffer = NULL;
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = (pBufferData) ? D3D11_USAGE_DEFAULT : D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = nBufferStride * nObjects;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = (pBufferData) ? 0 : D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pBufferData;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, (pBufferData) ? &d3dBufferData : NULL, &pd3dInstanceBuffer);
	return(pd3dInstanceBuffer);
}

void CInstancingShader::Render(ID3D11DeviceContext *pd3dImmediateDeviceContext, int nThreadID, CCamera *pCamera){
	OnPostRender(pd3dImmediateDeviceContext);

	AABB bcBoundingCube;
	bool bIsVisible = false;
	for (int i = 0; i < m_InstanceDataVector.size(); ++i){
		if (i % MAX_THREAD != nThreadID) continue;
		if (m_InstanceDataVector[i]->GetHasAnimation()){
			m_InstanceDataVector[i]->GetAnimationInstancing()->UpdateShaderVariables(pd3dImmediateDeviceContext);
			D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
			pd3dImmediateDeviceContext->Map(m_pd3dcbAnimationTextureWidth, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
			ANIM_WIDTH *pd3dxmTexWidth = (ANIM_WIDTH*)d3dMappedResource.pData;
			pd3dxmTexWidth->WIDTH[0] = m_InstanceDataVector[i]->GetAnimationInstancing()->GetTextureWidth();
			pd3dImmediateDeviceContext->Unmap(m_pd3dcbAnimationTextureWidth, 0);
			pd3dImmediateDeviceContext->VSSetConstantBuffers(VS_SLOT_ANIMATION_WIDTH, 1, &m_pd3dcbAnimationTextureWidth);
		}
		int nVisibleObjects = 0;
		D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
		pd3dImmediateDeviceContext->Map(m_InstanceDataVector[i]->GetInstanceBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
		InstanceBuffer *pd3dxmInstances = (InstanceBuffer *)d3dMappedResource.pData;
		CTexture *pLastTexture = NULL;
		CMaterial *pLastMaterial = NULL;
		for (int j = 0; j < m_ObjectsVector.size(); ++j)
		{
			if (m_ObjectsVector[j].first == i){
				if (m_ObjectsVector[j].second->isActive())
				{
					//객체의 메쉬의 바운딩 박스(모델 좌표계)를 객체의 월드 변환 행렬로 변환하고 새로운 바운딩 박스를 계산한다.
					bcBoundingCube = m_ObjectsVector[j].second->GetMesh(0)->GetBoundingCube();
					bcBoundingCube.Transform(&m_ObjectsVector[j].second->GetWorldMatrix());
					//바운딩 박스(월드 좌표계)가 카메라의 절두체에 포함되는 가를 검사하고 포함되는 경우에 렌더링한다. 
					bIsVisible = pCamera->IsInFrustum(bcBoundingCube.GetMinimum(), bcBoundingCube.GetMaximum());
					if (bIsVisible)
					{
						if (m_ObjectsVector[j].second->GetTexture()) {
							if (m_ObjectsVector[j].second->GetTexture() != pLastTexture){
								CTexturedIlluminatedShader::UpdateShaderVariables(pd3dImmediateDeviceContext, m_ObjectsVector[j].second->GetTexture());
								pLastTexture = m_ObjectsVector[j].second->GetTexture();
							}
						}
						
						if (m_ObjectsVector[j].second->GetMaterial())
						{
							if (m_ObjectsVector[j].second->GetMaterial() != pLastMaterial)
							CTexturedIlluminatedShader::UpdateShaderVariables(pd3dImmediateDeviceContext, &m_ObjectsVector[j].second->GetMaterial()->GetMaterial());
							pLastMaterial = m_ObjectsVector[j].second->GetMaterial();
						}
						if (m_InstanceDataVector[i]->GetHasAnimation()){
							CDynamicObject *pObject = (CDynamicObject *)m_ObjectsVector[j].second;
							CAnimationController *pAnimationController = pObject->GetAnimationController();
							pd3dxmInstances[nVisibleObjects].AnimationPos[0] =
								m_InstanceDataVector[i]->GetAnimationInstancing()->GetCurrentOffset(pAnimationController->GetCurrentAnimationName(), pAnimationController->GetCurrentAnimationTime());

						}
						D3DXMatrixTranspose(&pd3dxmInstances[nVisibleObjects++].InstancePos, &m_ObjectsVector[j].second->GetWorldMatrix());
					}
				}
			}
		}
		pd3dImmediateDeviceContext->Unmap(m_InstanceDataVector[i]->GetInstanceBuffer(), 0);
		if (nVisibleObjects>0)
			m_InstanceDataVector[i]->GetMesh()->RenderInstanced(pd3dImmediateDeviceContext, nVisibleObjects, 0);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//

CSkyBoxShader::CSkyBoxShader()
{
}

CSkyBoxShader::~CSkyBoxShader()
{
}

void CSkyBoxShader::BuildObjects(ID3D11Device *pd3dDevice, PxPhysics *pPxPhysics, PxScene *pPxScene, FbxManager *pFbxSdkManager)
{
	CreateShaderVariables(pd3dDevice);
	ID3D11SamplerState *pd3dSamplerState = NULL;
	D3D11_SAMPLER_DESC d3dSamplerDesc;
	ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = 0;
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dSamplerState);

	ID3D11ShaderResourceView *pd3dTexture = NULL;
	CTexture **ppTextures = new CTexture*[6];
	for (int i = 0; i < 6; ++i)
		ppTextures[i] = new CTexture(1);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Data/Image/Skybox/Back.png"), NULL, NULL, &pd3dTexture, NULL);
	ppTextures[0]->SetTexture(0, pd3dTexture, pd3dSamplerState);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Data/Image/Skybox/Front.png"), NULL, NULL, &pd3dTexture, NULL);
	ppTextures[1]->SetTexture(0, pd3dTexture, pd3dSamplerState);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Data/Image/Skybox/Top.png"), NULL, NULL, &pd3dTexture, NULL);
	ppTextures[2]->SetTexture(0, pd3dTexture, pd3dSamplerState);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Data/Image/Skybox/Bottom.png"), NULL, NULL, &pd3dTexture, NULL);
	ppTextures[3]->SetTexture(0, pd3dTexture, pd3dSamplerState);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Data/Image/Skybox/Right.png"), NULL, NULL, &pd3dTexture, NULL);
	ppTextures[4]->SetTexture(0, pd3dTexture, pd3dSamplerState);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Data/Image/Skybox/Left.png"), NULL, NULL, &pd3dTexture, NULL);
	ppTextures[5]->SetTexture(0, pd3dTexture, pd3dSamplerState);




	CSkyBoxMesh *pSkyBoxMesh = new CSkyBoxMesh(pd3dDevice, 200.0f, 200.0f, 200.0f);


	CSkyBox **pSkyBox = new CSkyBox*[6];
	for (int i = 0; i < 6; ++i){
		pSkyBox[i] = new CSkyBox();
		pSkyBox[i]->SetMesh(pSkyBoxMesh);
		pSkyBox[i]->SetPosition(D3DXVECTOR3(0, 0, 0));
		pSkyBox[i]->SetTexture(ppTextures[i]);
		m_ObjectsVector.push_back(make_pair(0, pSkyBox[i]));
	}

	//back
	m_ObjectsVector[0].second->Rotate(0, 180, 0);
	//front

	//top
	m_ObjectsVector[2].second->Rotate(90, 0, 0);
	//bottom
	m_ObjectsVector[3].second->Rotate(270, 0, 0);
	//left
	m_ObjectsVector[4].second->Rotate(0, 90, 0);
	//right
	m_ObjectsVector[5].second->Rotate(0, 270, 0);


}

void CSkyBoxShader::Render(ID3D11DeviceContext *pd3dDeviceContext, int nThreadID, CCamera *pCamera)
{
	OnPostRender(pd3dDeviceContext);

	D3DXVECTOR3 d3dxvCameraPos = pCamera->GetPosition();

	m_ObjectsVector[0].second->SetPosition(D3DXVECTOR3(d3dxvCameraPos.x, d3dxvCameraPos.y, d3dxvCameraPos.z + 200));
	m_ObjectsVector[1].second->SetPosition(D3DXVECTOR3(d3dxvCameraPos.x, d3dxvCameraPos.y, d3dxvCameraPos.z - 200));
	m_ObjectsVector[2].second->SetPosition(D3DXVECTOR3(d3dxvCameraPos.x, d3dxvCameraPos.y + 200, d3dxvCameraPos.z));
	m_ObjectsVector[3].second->SetPosition(D3DXVECTOR3(d3dxvCameraPos.x, d3dxvCameraPos.y - 200, d3dxvCameraPos.z));
	m_ObjectsVector[4].second->SetPosition(D3DXVECTOR3(d3dxvCameraPos.x - 200, d3dxvCameraPos.y, d3dxvCameraPos.z));
	m_ObjectsVector[5].second->SetPosition(D3DXVECTOR3(d3dxvCameraPos.x + 200, d3dxvCameraPos.y, d3dxvCameraPos.z));

	for (int i = 0; i < m_ObjectsVector.size(); ++i){
		if (i % MAX_THREAD != nThreadID) continue;
		if (m_ObjectsVector[i].second->GetTexture())
			CTexturedShader::UpdateShaderVariables(pd3dDeviceContext, m_ObjectsVector[i].second->GetTexture());
		CTexturedShader::UpdateShaderVariables(pd3dDeviceContext, &m_ObjectsVector[i].second->GetWorldMatrix());
		m_ObjectsVector[i].second->Render(pd3dDeviceContext);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//


CTerrainShader::CTerrainShader()
{
}

CTerrainShader::~CTerrainShader()
{
}

void CTerrainShader::BuildObjects(ID3D11Device *pd3dDevice, PxPhysics *pPxPhysics, PxScene *pPxScene, FbxManager *pFbxSdkManager)
{
	CTexturedIlluminatedShader::CreateShaderVariables(pd3dDevice);

	CMaterial *pTerrainMaterial = new CMaterial(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

	ID3D11SamplerState *pd3dBaseSamplerState = NULL;
	D3D11_SAMPLER_DESC d3dSamplerDesc;
	ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = 0;
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dBaseSamplerState);

	ID3D11SamplerState *pd3dDetailSamplerState = NULL;
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dDetailSamplerState);

	CTexture *pTerrainTexture = new CTexture(1);
	ID3D11ShaderResourceView *pd3dsrvBaseTexture = NULL;
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Data/Image/Terrain/Base_Texture2.jpg"), NULL, NULL, &pd3dsrvBaseTexture, NULL);
	pTerrainTexture->SetTexture(0, pd3dsrvBaseTexture, pd3dBaseSamplerState);
	pd3dsrvBaseTexture->Release();
	pd3dBaseSamplerState->Release();
	/*
	ID3D11ShaderResourceView *pd3dsrvDetailTexture = NULL;
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/Terrain/Detail_Texture_7.jpg"), NULL, NULL, &pd3dsrvDetailTexture, NULL);
	pTerrainTexture->SetTexture(1, pd3dsrvDetailTexture,pd3dDetailSamplerState);
	pd3dsrvDetailTexture->Release();
	pd3dDetailSamplerState->Release();
	*/
	D3DXVECTOR3 d3dxvScale(8.0f, 2.0f, 8.0f);
	CHeightMapTerrain *pHeightMapTerrain = new CHeightMapTerrain(pd3dDevice, _T("Data/Terrain/HeightMap2.raw"), 257, 257, 257, 257, d3dxvScale);
	pHeightMapTerrain->SetPosition(D3DXVECTOR3(0, 0, 0));
	pHeightMapTerrain->SetMaterial(pTerrainMaterial);
	pHeightMapTerrain->SetTexture(pTerrainTexture);
	//pHeightMapTerrain->Rotate(180,0,0);
	m_ObjectsVector.push_back(make_pair(0, pHeightMapTerrain));
}

CHeightMapTerrain *CTerrainShader::GetTerrain()
{
	return((CHeightMapTerrain *)m_ObjectsVector[0].second);
}
