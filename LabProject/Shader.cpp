//-----------------------------------------------------------------------------
// File: Shader.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Shader.h"

#define MAX_INSTANCE 2000

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
	for(int i=0; i<m_ObjectsVector.size(); ++i)
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
void CShader::OnPostRender(ID3D11DeviceContext *pd3dDeviceContext)
{
	pd3dDeviceContext->IASetInputLayout(m_pd3dVertexLayout);
	pd3dDeviceContext->VSSetShader(m_pd3dVertexShader, NULL, 0);
	pd3dDeviceContext->PSSetShader(m_pd3dPixelShader, NULL, 0);
}

void CShader::Render(ID3D11DeviceContext *pd3dImmediateDeviceContext, CCamera *pCamera)
{
}



void CShader::BuildObjects(ID3D11Device *pd3dDevice, PxPhysics *pPxPhysics, PxScene *pPxScene)
{
}

void CShader::AnimateObjects(float fTimeElapsed,PxScene *pPxScene)
{
	for (int i=0; i<m_ObjectsVector.size(); ++i)
	{
		m_ObjectsVector[i].second->Animate(fTimeElapsed,pPxScene);
	}
}

vector<pair<int,CGameObject*>>* CShader::GetObjectsVector(){
	return &m_ObjectsVector;
}

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
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSTexturedLighting", "vs_4_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSTexturedLighting", "ps_4_0", &m_pd3dPixelShader);

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
void CTexturedIlluminatedShader::BuildObjects(ID3D11Device *pd3dDevice, PxPhysics *pPxPhysics, PxScene *pPxScene){

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

void CTexturedIlluminatedShader::Render(ID3D11DeviceContext *pd3dImmediateDeviceContext, CCamera *pCamera)
{
	OnPostRender(pd3dImmediateDeviceContext);
	//카메라의 절두체에 포함되는 객체들만을 렌더링한다. 
	bool bIsVisible = false;
	AABB bcBoundingCube;
	for (int i=0; i<m_ObjectsVector.size(); ++i)
	{
		if (m_ObjectsVector[i].second->isActive())
		{
			//객체의 메쉬의 바운딩 박스(모델 좌표계)를 객체의 월드 변환 행렬로 변환하고 새로운 바운딩 박스를 계산한다.
			bcBoundingCube = m_ObjectsVector[i].second->GetMesh()->GetBoundingCube();
			bcBoundingCube.Transform(&m_ObjectsVector[i].second->GetWorldMatrix());
			//바운딩 박스(월드 좌표계)가 카메라의 절두체에 포함되는 가를 검사하고 포함되는 경우에 렌더링한다. 
			bIsVisible = pCamera->IsInFrustum(bcBoundingCube.GetMinimum(), bcBoundingCube.GetMaximum());
			if (bIsVisible)
			{

				if (m_ObjectsVector[i].second->GetTexture()) 
					UpdateShaderVariables(pd3dImmediateDeviceContext, m_ObjectsVector[i].second->GetTexture());
				if (m_ObjectsVector[i].second->GetMaterial()) 
					UpdateShaderVariables(pd3dImmediateDeviceContext, &m_ObjectsVector[i].second->GetMaterial()->m_Material);
				UpdateShaderVariables(pd3dImmediateDeviceContext, &m_ObjectsVector[i].second->GetWorldMatrix());
				m_ObjectsVector[i].second->Render(pd3dImmediateDeviceContext, pCamera);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//

CPlayerShader::CPlayerShader()
{
	m_pd3dcbWorldMatrix = NULL;
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
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSTexturedLighting", "vs_4_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSTexturedLighting", "ps_4_0", &m_pd3dPixelShader);
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

////////////////////////////////////////////////////////////////////////////////////////////////////
//

CInstancingShader::CInstancingShader(){
	//m_pd3dInstances = NULL;
	m_nMatrixBufferStride=0;
	m_nMatrixBufferOffset=0;
}

CInstancingShader::~CInstancingShader()
{
	for(int i=0; i<m_InstanceDataVector.size(); ++i)
		m_InstanceDataVector[i].Release();
	//if (m_pd3dInstances) m_pd3dInstances->Release();
}

void CInstancingShader::CreateShader(ID3D11Device *pd3dDevice){
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "INSTANCEPOS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEPOS", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEPOS", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEPOS", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSInstancedTexturedLighting", "vs_5_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSInstancedTexturedLighting", "ps_5_0", &m_pd3dPixelShader);
}

void CInstancingShader::BuildObjects(ID3D11Device *pd3dDevice, PxPhysics *pPxPhysics, PxScene *pPxScene){
	CreateShaderVariables(pd3dDevice);
	CMaterial *pMaterial = new CMaterial;
	pMaterial->m_Material.m_d3dxcDiffuse = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f);
	pMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f);
	pMaterial->m_Material.m_d3dxcSpecular = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f);
	pMaterial->m_Material.m_d3dxcEmissive = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f);

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
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("3.jpg"), NULL, NULL, &pd3dTexture, NULL);
	pTexture->SetTexture(0, pd3dTexture, pd3dSamplerState);
	m_TexturesVector.push_back(pTexture);
	pTexture = new CTexture(1);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("2.jpg"), NULL, NULL, &pd3dTexture, NULL);
	pTexture->SetTexture(0, pd3dTexture, pd3dSamplerState);
	m_TexturesVector.push_back(pTexture);
	pTexture = new CTexture(1);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("1.jpg"), NULL, NULL, &pd3dTexture, NULL);
	pTexture->SetTexture(0, pd3dTexture, pd3dSamplerState);
	m_TexturesVector.push_back(pTexture);

	

	m_nMatrixBufferStride = sizeof(D3DXMATRIX);
	m_nMatrixBufferOffset = 0;
	//인스턴스 쉐이더에서 렌더링할 메쉬이다.

	m_InstanceDataVector.push_back(InstanceData(new CCubeMeshIlluminatedTextured(pd3dDevice, 25.0f, 25.0f, 25.0f),CreateInstanceBuffer(pd3dDevice,MAX_INSTANCE,sizeof(D3DXMATRIX),NULL),500));
	CDynamicObject *pCubeObject = NULL;
	for(int i=0; i<m_InstanceDataVector[0].m_nObjects; ++i){
		pCubeObject = new CDynamicObject();
		pCubeObject->SetMesh(m_InstanceDataVector[0].GetMesh());
		pCubeObject->SetMaterial(pMaterial);
		pCubeObject->SetTexture(m_TexturesVector[0]);
		pCubeObject->BuildObjects(pPxPhysics,pPxScene);
		pCubeObject->SetPosition(D3DXVECTOR3(0, i * 20, 0));
		m_ObjectsVector.push_back(make_pair(0,pCubeObject));
	}

	m_InstanceDataVector.push_back(InstanceData(new CCubeMeshIlluminatedTextured(pd3dDevice, 2000.0f, 10.0f, 2000.0f),CreateInstanceBuffer(pd3dDevice,MAX_INSTANCE,sizeof(D3DXMATRIX),NULL),1));
	CStaticObject *pPlaneObject = NULL;
	for(int i=0; i<m_InstanceDataVector[1].m_nObjects; ++i){
		pPlaneObject = new CStaticObject();
		pPlaneObject->SetMesh(m_InstanceDataVector[1].GetMesh());
		pPlaneObject->SetMaterial(pMaterial);
		pPlaneObject->SetTexture(m_TexturesVector[1]);
		pPlaneObject->BuildObjects(pPxPhysics,pPxScene);
		pPlaneObject->SetPosition(D3DXVECTOR3(0, -100, 0));
		m_ObjectsVector.push_back(make_pair(1,pPlaneObject));
	}
	//인스턴스 데이터(렌더링할 객체들의 위치 벡터 배열)를 메쉬의 정점 버퍼에 추가한다.
	for(int i=0; i<m_InstanceDataVector.size();++i){
		ID3D11Buffer *InstanceBuffer = m_InstanceDataVector[i].GetInstanceBuffer();
		m_InstanceDataVector[i].GetMesh()->AppendVertexBuffer(1,&InstanceBuffer,&m_nMatrixBufferStride,&m_nMatrixBufferOffset);
	}
}

void CInstancingShader::AddObject(PxPhysics *pPxPhysics, PxScene *pPxScene, int _IndexOfInstanceDataVector, int _IndexOfMaterial, int _IndexOfTexture, D3DXVECTOR3 _d3dxvPosition, int _isStatic, D3DXVECTOR3 _Force)
{
	if(_IndexOfInstanceDataVector>=m_InstanceDataVector.size())
	{
		cout<< "InstanceDataVector index out of range." << endl;
		return;
	}
	if(_IndexOfMaterial>=m_MaterialsVector.size())
	{
		cout<< "MaterialsVector index out of range." << endl;
		return;
	}
	if(_IndexOfTexture>=m_TexturesVector.size())
	{
		cout<< "TexturesVector index out of range." << endl;
		return;
	}


	if(_isStatic)
	{
		CStaticObject *pObject = new CStaticObject();
		pObject->SetMesh(m_InstanceDataVector[_IndexOfInstanceDataVector].GetMesh());
		pObject->SetMaterial(m_MaterialsVector[_IndexOfMaterial]);
		pObject->SetTexture(m_TexturesVector[_IndexOfTexture]);
		pObject->BuildObjects(pPxPhysics,pPxScene);
		pObject->SetPosition(_d3dxvPosition);
		m_ObjectsVector.push_back(make_pair(_IndexOfInstanceDataVector,pObject));
	}
	else
	{
		CDynamicObject *pObject = new CDynamicObject();
		pObject->SetMesh(m_InstanceDataVector[_IndexOfInstanceDataVector].GetMesh());
		pObject->SetMaterial(m_MaterialsVector[_IndexOfMaterial]);
		pObject->SetTexture(m_TexturesVector[_IndexOfTexture]);
		pObject->BuildObjects(pPxPhysics,pPxScene);
		pObject->SetPosition(_d3dxvPosition);
		pObject->AddForce(_Force.x,_Force.y,_Force.z);
		m_ObjectsVector.push_back(make_pair(_IndexOfInstanceDataVector,pObject));
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

void CInstancingShader::UpdateShaderVariables(ID3D11DeviceContext *pd3dImmediateDeviceContext, CCamera *pCamera){
	//카메라의 절두체에 포함되는 객체들만을 렌더링한다. 

}
void CInstancingShader::Render(ID3D11DeviceContext *pd3dImmediateDeviceContext, CCamera *pCamera){
	OnPostRender(pd3dImmediateDeviceContext);
	UpdateShaderVariables(pd3dImmediateDeviceContext,pCamera);

	AABB bcBoundingCube;
	bool bIsVisible = false;
	for(int i=0; i<m_InstanceDataVector.size(); ++i){
		m_InstanceDataVector[i].m_nVisibleObjects = 0;
		D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
		pd3dImmediateDeviceContext->Map(m_InstanceDataVector[i].GetInstanceBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
		D3DXMATRIX *pd3dxmInstances = (D3DXMATRIX *)d3dMappedResource.pData;

		for (int j=0; j<m_ObjectsVector.size(); ++j)
		{
			if(m_ObjectsVector[j].first == i){
				if (m_ObjectsVector[j].second->isActive())
				{
					//객체의 메쉬의 바운딩 박스(모델 좌표계)를 객체의 월드 변환 행렬로 변환하고 새로운 바운딩 박스를 계산한다.
					bcBoundingCube = m_ObjectsVector[j].second->GetMesh()->GetBoundingCube();
					bcBoundingCube.Transform(&m_ObjectsVector[j].second->GetWorldMatrix());
					//바운딩 박스(월드 좌표계)가 카메라의 절두체에 포함되는 가를 검사하고 포함되는 경우에 렌더링한다. 
					bIsVisible = pCamera->IsInFrustum(bcBoundingCube.GetMinimum(), bcBoundingCube.GetMaximum());
					if (bIsVisible)
					{
						if (m_ObjectsVector[j].second->GetTexture()) 
							CTexturedIlluminatedShader::UpdateShaderVariables(pd3dImmediateDeviceContext, m_ObjectsVector[j].second->GetTexture());
						if (m_ObjectsVector[j].second->GetMaterial()) 
							CTexturedIlluminatedShader::UpdateShaderVariables(pd3dImmediateDeviceContext, &m_ObjectsVector[j].second->GetMaterial()->m_Material);
						D3DXMatrixTranspose(&pd3dxmInstances[m_InstanceDataVector[i].m_nVisibleObjects++], &m_ObjectsVector[j].second->GetWorldMatrix());
					}
				}
			}
		}
		pd3dImmediateDeviceContext->Unmap(m_InstanceDataVector[i].GetInstanceBuffer(), 0);
		m_InstanceDataVector[i].GetMesh()->RenderInstanced(pd3dImmediateDeviceContext,m_InstanceDataVector[i].m_nVisibleObjects,0);
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

void CSkyBoxShader::BuildObjects(ID3D11Device *pd3dDevice, PxPhysics *pPxPhysics, PxScene *pPxScene)
{
	CreateShaderVariables(pd3dDevice);
	/*
	m_nObjects = 6;
	m_ppObjects = new CGameObject*[m_nObjects]; 
	*/
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
	for(int i=0;i<6;++i)
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




	CSkyBoxMesh *pSkyBoxMesh = new CSkyBoxMesh(pd3dDevice, 2000.0f, 2000.0f, 2000.0f);


	CSkyBox **pSkyBox = new CSkyBox*[6];
	for(int i=0;i<6;++i){
		pSkyBox[i] = new CSkyBox();
		pSkyBox[i]->SetMesh(pSkyBoxMesh);
		pSkyBox[i]->SetPosition(D3DXVECTOR3(0,0,0));
		pSkyBox[i]->SetTexture(ppTextures[i]);
		m_ObjectsVector.push_back(make_pair(0,pSkyBox[i]));
	}

	//back
	m_ObjectsVector[0].second->Rotate(0,180,0);
	//front

	//top
	m_ObjectsVector[2].second->Rotate(90,0,0);
	//bottom
	m_ObjectsVector[3].second->Rotate(270,0,0);
	//left
	m_ObjectsVector[4].second->Rotate(0,90,0);
	//right
	m_ObjectsVector[5].second->Rotate(0,270,0);


}

void CSkyBoxShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	OnPostRender(pd3dDeviceContext);

	D3DXVECTOR3 d3dxvCameraPos = pCamera->GetPosition();

	m_ObjectsVector[0].second->SetPosition(D3DXVECTOR3(d3dxvCameraPos.x, d3dxvCameraPos.y, d3dxvCameraPos.z+2000));
	m_ObjectsVector[1].second->SetPosition(D3DXVECTOR3(d3dxvCameraPos.x, d3dxvCameraPos.y, d3dxvCameraPos.z-2000));
	m_ObjectsVector[2].second->SetPosition(D3DXVECTOR3(d3dxvCameraPos.x, d3dxvCameraPos.y+2000, d3dxvCameraPos.z));
	m_ObjectsVector[3].second->SetPosition(D3DXVECTOR3(d3dxvCameraPos.x, d3dxvCameraPos.y-2000, d3dxvCameraPos.z));
	m_ObjectsVector[4].second->SetPosition(D3DXVECTOR3(d3dxvCameraPos.x-2000, d3dxvCameraPos.y, d3dxvCameraPos.z));
	m_ObjectsVector[5].second->SetPosition(D3DXVECTOR3(d3dxvCameraPos.x+2000, d3dxvCameraPos.y, d3dxvCameraPos.z));

	for(int i=0; i<m_ObjectsVector.size(); ++i){
		if (m_ObjectsVector[i].second->GetTexture()) 
			CTexturedShader::UpdateShaderVariables(pd3dDeviceContext, m_ObjectsVector[i].second->GetTexture());
		CTexturedShader::UpdateShaderVariables(pd3dDeviceContext, &m_ObjectsVector[i].second->GetWorldMatrix());
		m_ObjectsVector[i].second->Render(pd3dDeviceContext, pCamera);
	}
}