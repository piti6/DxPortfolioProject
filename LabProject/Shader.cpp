//-----------------------------------------------------------------------------
// File: Shader.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Shader.h"



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

//ファイルからシェーダを作ります。
void CShader::CreateVertexShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11VertexShader **ppd3dVertexShader, D3D11_INPUT_ELEMENT_DESC *pd3dInputLayout, UINT nElements, ID3D11InputLayout **ppd3dVertexLayout)
{
	HRESULT hResult;

	DWORD dwShaderFlags = NULL;
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

	DWORD dwShaderFlags = NULL;
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

void CShader::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, MATERIAL *pMaterial)
{
}
void CShader::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, CTexture *pTexture)
{
}
void CShader::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxWorld)
{
}

void CShader::BuildObjects(ID3D11Device *pd3dDevice, PxPhysics *pPxPhysics, PxScene *pPxScene, PxControllerManager *pPxControllerManager, FbxManager *pFbxSdkManager)
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
void CShader::Render(ID3D11DeviceContext *pd3dDeviceContext, int nThreadID, CCamera *pCamera)
{
}

vector<pair<int, CGameObject*>>* CShader::GetObjectsVector()
{
	return &m_ObjectsVector;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//基本的な光、テクスチャを持っているシェーダクラスです。

CTexturedIlluminatedShader::CTexturedIlluminatedShader()
{
}
CTexturedIlluminatedShader::~CTexturedIlluminatedShader()
{
}

//ポジション、ノーマル、ＵＶを持っているシェーダを登録します。
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
//定数バッファにはマテリアル、ワールドポジションの行列を登録します。
void CTexturedIlluminatedShader::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.ByteWidth = sizeof(VS_CB_WORLD_MATRIX);
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbWorldMatrix);

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.ByteWidth = sizeof(MATERIAL);
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, NULL, &m_pd3dcbMaterial);
}
//テクスチャ更新
void CTexturedIlluminatedShader::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, CTexture *pTexture)
{
	pd3dDeviceContext->PSSetShaderResources(PS_SLOT_TEXTURE, pTexture->GetNumberOfTexture(), pTexture->GetTextures());
	pd3dDeviceContext->PSSetSamplers(PS_SLOT_SAMPLER_STATE, pTexture->GetNumberOfTexture(), pTexture->GetSamplerStates());
}
//マテリアル更新
void CTexturedIlluminatedShader::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, MATERIAL *pMaterial)
{
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbMaterial, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	MATERIAL *pcbMaterial = (MATERIAL *)d3dMappedResource.pData;
	memcpy(pcbMaterial, pMaterial, sizeof(MATERIAL));
	pd3dDeviceContext->Unmap(m_pd3dcbMaterial, 0);
	pd3dDeviceContext->PSSetConstantBuffers(PS_SLOT_MATERIAL, 1, &m_pd3dcbMaterial);
}
//ポジション更新
void CTexturedIlluminatedShader::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxWorld)
{
	//Update World Matrix Constant Buffer
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbWorldMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_CB_WORLD_MATRIX *pcbWorldMatrix = (VS_CB_WORLD_MATRIX *)d3dMappedResource.pData;
	D3DXMatrixTranspose(&pcbWorldMatrix->m_d3dxmtxWorld, pd3dxmtxWorld);
	pd3dDeviceContext->Unmap(m_pd3dcbWorldMatrix, 0);
	pd3dDeviceContext->VSSetConstantBuffers(VS_SLOT_WORLD_MATRIX, 1, &m_pd3dcbWorldMatrix);
}

void CTexturedIlluminatedShader::BuildObjects(ID3D11Device *pd3dDevice, PxPhysics *pPxPhysics, PxScene *pPxScene, PxControllerManager *pPxControllerManager, FbxManager *pFbxSdkManager){

}

void CTexturedIlluminatedShader::Render(ID3D11DeviceContext *pd3dDeviceContext, int nThreadID, CCamera *pCamera)
{
	OnPostRender(pd3dDeviceContext);
	bool bIsVisible = false;
	AABB bcBoundingCube;
	for (int i = 0; i < m_ObjectsVector.size(); ++i)
	{
		if (i % MAX_THREAD == nThreadID) continue;
		if (m_ObjectsVector[i].second->isActive())
		{
			//オブジェクトのメッシュバウンディングボックス（モデル座標系）をオブジェクトのワールド変換行列に変換して、新しいバウンディングボックスを計算します。
			bcBoundingCube = m_ObjectsVector[i].second->GetMesh(0)->GetBoundingCube();
			bcBoundingCube.Transform(&m_ObjectsVector[i].second->GetWorldMatrix());
			//バウンディングボックス（ワールド座標系）がカメラの視錐台に含まれる行を検査して含まれている場合にレンダリングします。
			bIsVisible = pCamera->IsInFrustum(bcBoundingCube.GetMinimum(), bcBoundingCube.GetMaximum());
			if (bIsVisible)
			{

				if (m_ObjectsVector[i].second->GetTexture())
					UpdateShaderVariables(pd3dDeviceContext, m_ObjectsVector[i].second->GetTexture());
				if (m_ObjectsVector[i].second->GetMaterial())
					UpdateShaderVariables(pd3dDeviceContext, &m_ObjectsVector[i].second->GetMaterial()->GetMaterial());
				UpdateShaderVariables(pd3dDeviceContext, &m_ObjectsVector[i].second->GetWorldMatrix());
				m_ObjectsVector[i].second->Render(pd3dDeviceContext);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//

CTexturedShader::CTexturedShader()
{
}
CTexturedShader::~CTexturedShader()
{
}

//ポジション、ＵＶを持っているシェーダを登録します。
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
//定数バッファにはワールドポジションの行列を登録します。
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
	pd3dDeviceContext->PSSetShaderResources(PS_SLOT_TEXTURE, pTexture->GetNumberOfTexture(), pTexture->GetTextures());
	pd3dDeviceContext->PSSetSamplers(PS_SLOT_SAMPLER_STATE, pTexture->GetNumberOfTexture(), pTexture->GetSamplerStates());
}
void CTexturedShader::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxWorld)
{
	//Update World Matrix Constant Buffer
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbWorldMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_CB_WORLD_MATRIX *pcbWorldMatrix = (VS_CB_WORLD_MATRIX *)d3dMappedResource.pData;
	D3DXMatrixTranspose(&pcbWorldMatrix->m_d3dxmtxWorld, pd3dxmtxWorld);
	pd3dDeviceContext->Unmap(m_pd3dcbWorldMatrix, 0);
	pd3dDeviceContext->VSSetConstantBuffers(VS_SLOT_WORLD_MATRIX, 1, &m_pd3dcbWorldMatrix);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//

CInstancingShader::CInstancingShader(CPlayer *pPlayer)
{
	m_pPlayer = pPlayer;
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

//ポジション、ノーマル、ＵＶさらにボーンのインデックス、ウェイト、インスタンスポジション、アニメーションインデックスを持っているシェーダを登録します。
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
//定数バッファにはマテリアル、ワールドポジションの行列、さらにアニメーションテクスチャをため、アニメーションテクスチャの長さを登録します。
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
//マップ、キャラクタなどの情報を登録し、ゲーム世界を作ります。
void CInstancingShader::BuildObjects(ID3D11Device *pd3dDevice, PxPhysics *pPxPhysics, PxScene *pPxScene, PxControllerManager *pPxControllerManager, FbxManager *pFbxSdkManager){
	CreateShaderVariables(pd3dDevice);

	//マテリアルの作成
	CMaterial *pMaterial = new CMaterial(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	m_MaterialsVector.push_back(pMaterial);

	//テクスチャサンプラーの作成
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

	//テクスチャの作成
	ID3D11ShaderResourceView *pd3dTexture = NULL;
	CTexture *pTexture = new CTexture(1);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Data/Image/Ground/Ground_02.png"), NULL, NULL, &pd3dTexture, NULL);
	pTexture->SetTexture(0, pd3dTexture, pd3dSamplerState);
	m_TexturesVector.push_back(pTexture);

	pTexture = new CTexture(1);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Data/Image/Model/Map/Atlas1.png"), NULL, NULL, &pd3dTexture, NULL);
	pTexture->SetTexture(0, pd3dTexture, pd3dSamplerState);
	m_TexturesVector.push_back(pTexture);

	pTexture = new CTexture(1);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Data/Image/Model/Map/Roads.png"), NULL, NULL, &pd3dTexture, NULL);
	pTexture->SetTexture(0, pd3dTexture, pd3dSamplerState);
	m_TexturesVector.push_back(pTexture);

	pTexture = new CTexture(1);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Data/Image/Model/Map/cars.png"), NULL, NULL, &pd3dTexture, NULL);
	pTexture->SetTexture(0, pd3dTexture, pd3dSamplerState);
	m_TexturesVector.push_back(pTexture);

	pTexture = new CTexture(1);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Data/Image/Model/Character/1/1.png"), NULL, NULL, &pd3dTexture, NULL);
	pTexture->SetTexture(0, pd3dTexture, pd3dSamplerState);
	m_TexturesVector.push_back(pTexture);

	pTexture = new CTexture(1);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Data/Image/Model/Character/2/1.png"), NULL, NULL, &pd3dTexture, NULL);
	pTexture->SetTexture(0, pd3dTexture, pd3dSamplerState);
	m_TexturesVector.push_back(pTexture);

	pTexture = new CTexture(1);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Data/Image/Model/Character/3/1.png"), NULL, NULL, &pd3dTexture, NULL);
	pTexture->SetTexture(0, pd3dTexture, pd3dSamplerState);
	m_TexturesVector.push_back(pTexture);



	//物理マテリアルの作成
	PxMaterial *pPxM = pPxPhysics->createMaterial(0.9, 0.9, 0.001);

	//インスタンスデータを作りながら、オブジェクトの配置もここで行います。
	m_InstanceDataVector.push_back(new InstanceData(new CCubeMeshIlluminatedTextured(pd3dDevice, 2000.0f, 0.1f, 2000.0f, 200), false, "TerrainPlane", CreateInstanceBuffer(pd3dDevice, 1, sizeof(InstanceBuffer))));
	CStaticObject *pPlaneObject = NULL;
	pPlaneObject = new CStaticObject();
	pPlaneObject->SetMesh(m_InstanceDataVector[0]->GetMesh());
	pPlaneObject->SetMaterial(pMaterial);
	pPlaneObject->SetTexture(m_TexturesVector[0]);
	pPlaneObject->BuildObjects(pPxPhysics, pPxScene, pPxM, "Plane");
	pPlaneObject->SetPosition(D3DXVECTOR3(0, 0, 0));
	m_ObjectsVector.push_back(make_pair(0, pPlaneObject));
	
	//アニメーションキャラクタ（プレイヤー）の作成
	m_InstanceDataVector.push_back(new InstanceData(new CFbxMeshIlluminatedTextured(pd3dDevice, pFbxSdkManager, "Data/Model/Character/GoblinWizard/mon_goblinWizard@Attack01.fbx", 1.0f, true), true, "Monster1", CreateInstanceBuffer(pd3dDevice, 2000, sizeof(InstanceBuffer))));
	m_InstanceDataVector[1]->GetAnimationInstancing()->LoadAnimationFromFile(pFbxSdkManager, "Data/Model/Character/GoblinWizard/mon_goblinWizard@Attack01.fbx", "Attack", true);
	m_InstanceDataVector[1]->GetAnimationInstancing()->LoadAnimationFromFile(pFbxSdkManager, "Data/Model/Character/GoblinWizard/mon_goblinWizard@Run.fbx", "Run", true);
	m_InstanceDataVector[1]->GetAnimationInstancing()->LoadAnimationFromFile(pFbxSdkManager, "Data/Model/Character/GoblinWizard/mon_goblinWizard@Idle.fbx", "Idle", true);
	m_InstanceDataVector[1]->GetAnimationInstancing()->CreateAnimationTexture(pd3dDevice);

	m_InstanceDataVector.push_back(new InstanceData(new CFbxMeshIlluminatedTextured(pd3dDevice, pFbxSdkManager, "Data/Model/Character/OrcWarrior/mon_orcWarrior@Attack01.fbx", 1.0f, true), true, "Monster2", CreateInstanceBuffer(pd3dDevice, 2000, sizeof(InstanceBuffer))));
	m_InstanceDataVector[2]->GetAnimationInstancing()->LoadAnimationFromFile(pFbxSdkManager, "Data/Model/Character/OrcWarrior/mon_orcWarrior@Attack01.fbx", "Attack", true);
	m_InstanceDataVector[2]->GetAnimationInstancing()->LoadAnimationFromFile(pFbxSdkManager, "Data/Model/Character/OrcWarrior/mon_orcWarrior@Run.fbx", "Run", true);
	m_InstanceDataVector[2]->GetAnimationInstancing()->LoadAnimationFromFile(pFbxSdkManager, "Data/Model/Character/OrcWarrior/mon_orcWarrior@Idle.fbx", "Idle", true);
	m_InstanceDataVector[2]->GetAnimationInstancing()->CreateAnimationTexture(pd3dDevice);

	m_InstanceDataVector.push_back(new InstanceData(new CFbxMeshIlluminatedTextured(pd3dDevice, pFbxSdkManager, "Data/Model/Character/TrollCurer/mon_trolCurer@Attack01.fbx", 1.0f, true), true, "Monster3", CreateInstanceBuffer(pd3dDevice, 2000, sizeof(InstanceBuffer))));
	m_InstanceDataVector[3]->GetAnimationInstancing()->LoadAnimationFromFile(pFbxSdkManager, "Data/Model/Character/TrollCurer/mon_trolCurer@Attack01.fbx", "Attack", true);
	m_InstanceDataVector[3]->GetAnimationInstancing()->LoadAnimationFromFile(pFbxSdkManager, "Data/Model/Character/TrollCurer/mon_trolCurer@Run.fbx", "Run", true);
	m_InstanceDataVector[3]->GetAnimationInstancing()->LoadAnimationFromFile(pFbxSdkManager, "Data/Model/Character/TrollCurer/mon_trolCurer@Idle.fbx", "Idle", true);
	m_InstanceDataVector[3]->GetAnimationInstancing()->CreateAnimationTexture(pd3dDevice);

	CAnimationController *pAnimationController = m_pPlayer->GetAnimationController();
	CAnimationInstancing *pAnimationInstancing = m_InstanceDataVector[1]->GetAnimationInstancing();
	CAnimationList *pAnimationList = pAnimationInstancing->GetAnimation();
	unordered_map<string, CAnimation> *pAnimationMap = pAnimationList->GetAnimationMap();
	pAnimationController->SetAnimationData("Attack", (*pAnimationMap)["Attack"].GetLength());
	pAnimationController->SetAnimationData("Run", (*pAnimationMap)["Run"].GetLength());
	pAnimationController->SetAnimationData("Idle", (*pAnimationMap)["Idle"].GetLength());
	pAnimationController->Play("Idle");
	m_pPlayer->SetMesh(m_InstanceDataVector[1]->GetMesh());
	m_pPlayer->SetMaterial(pMaterial);
	m_pPlayer->SetTexture(m_TexturesVector[4]);
	m_pPlayer->BuildObjects(pPxPhysics, pPxScene, pPxM, pPxControllerManager);
	m_pPlayer->SetOffset(D3DXVECTOR3(0, 0, -1));
	m_pPlayer->RotateOffset(-90, 0, 0);
	m_pPlayer->ChangeCamera(pd3dDevice, FIRST_PERSON_CAMERA, 0);
	m_pPlayer->SetPosition(D3DXVECTOR3(200.0f, 1.0f, 200.0f));
	m_ObjectsVector.push_back(make_pair(1, m_pPlayer));

	/*
	//テストオブジェクトです。
	for (int i = 0; i < 1000; ++i){
		int randAnim = rand() % 3;
		CDynamicObject *pObject = new CDynamicObject(true);
		pObject->SetMesh(m_InstanceDataVector[randAnim+1]->GetMesh());
		pObject->SetMaterial(pMaterial);
		pObject->SetTexture(m_TexturesVector[randAnim+4]);
		pObject->BuildObjects(pPxPhysics, pPxScene, pPxM);
		pObject->SetPosition(D3DXVECTOR3(200, i, 200));
		CAnimationController *pAnimationController = pObject->GetAnimationController();
		CAnimationInstancing *pAnimationInstancing = m_InstanceDataVector[randAnim + 1]->GetAnimationInstancing();
		CAnimationList *pAnimationList = pAnimationInstancing->GetAnimation();
		unordered_map<string, CAnimation> *pAnimationMap = pAnimationList->GetAnimationMap();
		pAnimationController->SetAnimationData("Attack", (*pAnimationMap)["Attack"].GetLength());
		pAnimationController->SetAnimationData("Run", (*pAnimationMap)["Run"].GetLength());
		pAnimationController->SetAnimationData("Idle", (*pAnimationMap)["Idle"].GetLength());
		switch (randAnim)
		{
		case 0:
			pAnimationController->Play("Attack");
			break;
		case 1:
			pAnimationController->Play("Run");
			break;
		case 2:
			pAnimationController->Play("Idle");
			break;
		}

		m_ObjectsVector.push_back(make_pair(randAnim+1, pObject));
	}
	*/

	//Unityで作成したマップの姿（モデルの名前、ポジション、ローテーション）を、テクストファイルで保存、それをロードします。
	ifstream ifsFbxList;
	ifsFbxList.open("Data/ImportData/ImportModelName.txt");

	string FbxName;
	while (!ifsFbxList.eof())
	{
		ifsFbxList >> FbxName;
		string FilePath = "Data/Model/Map/" + FbxName + ".fbx";

		m_InstanceDataVector.push_back(new InstanceData(new CFbxMeshIlluminatedTextured(pd3dDevice, pFbxSdkManager, (char*)FilePath.c_str()), false, FbxName, CreateInstanceBuffer(pd3dDevice, MAX_INSTANCE, sizeof(InstanceBuffer))));
	}
	ifsFbxList.close();

	ifsFbxList.open("Data/ImportData/ImportModelNameRoad.txt");

	while (!ifsFbxList.eof())
	{
		ifsFbxList >> FbxName;
		string FilePath = "Data/Model/Map/Roads/" + FbxName + ".fbx";

		m_InstanceDataVector.push_back(new InstanceData(new CFbxMeshIlluminatedTextured(pd3dDevice, pFbxSdkManager, (char*)FilePath.c_str()), false, FbxName, CreateInstanceBuffer(pd3dDevice, MAX_INSTANCE, sizeof(InstanceBuffer))));
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
				
				if (FbxName == "Powerpole_cable50M" || FbxName == "Powerpole1_cable")//モデルが大きいですので、衝突体を作成せずにスキップ
				{
					CGameObject *pMapObject = NULL;
					pMapObject = new CGameObject();
					pMapObject->SetMesh(m_InstanceDataVector[i]->GetMesh());
					pMapObject->SetMaterial(pMaterial);
					pMapObject->SetTexture(m_TexturesVector[1]);
					pMapObject->BuildObjects(pPxPhysics, pPxScene, pPxM);
					pMapObject->Rotate(d3dxvEulerRotation.x, d3dxvEulerRotation.y, d3dxvEulerRotation.z);
					pMapObject->SetPosition(d3dxvPosition);
					m_ObjectsVector.push_back(make_pair(i, pMapObject));
					break;
				}
				else
				{
					CStaticObject *pMapObject = NULL;
					pMapObject = new CStaticObject();
					pMapObject->SetMesh(m_InstanceDataVector[i]->GetMesh());
					pMapObject->SetMaterial(pMaterial);
					pMapObject->SetTexture(m_TexturesVector[1]);
					pMapObject->BuildObjects(pPxPhysics, pPxScene, pPxM, FbxName.c_str());
					pMapObject->Rotate(d3dxvEulerRotation.x, d3dxvEulerRotation.y, d3dxvEulerRotation.z);
					pMapObject->SetPosition(d3dxvPosition);
					m_ObjectsVector.push_back(make_pair(i, pMapObject));
					break;
				}
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
				if (FbxName == "Street_el1")//モデルが大きいですので、衝突体を作成せずにスキップ
				{
					CGameObject *pMapObject = NULL;
					pMapObject = new CGameObject();
					pMapObject->SetMesh(m_InstanceDataVector[i]->GetMesh());
					pMapObject->SetMaterial(pMaterial);
					pMapObject->SetTexture(m_TexturesVector[2]);
					pMapObject->BuildObjects(pPxPhysics, pPxScene, pPxM);
					pMapObject->Rotate(d3dxvEulerRotation.x, d3dxvEulerRotation.y, d3dxvEulerRotation.z);
					pMapObject->SetPosition(d3dxvPosition);
					m_ObjectsVector.push_back(make_pair(i, pMapObject));
					break;
				}
				else
				{
					CStaticObject *pMapObject = NULL;
					pMapObject = new CStaticObject();
					pMapObject->SetMesh(m_InstanceDataVector[i]->GetMesh());
					pMapObject->SetMaterial(pMaterial);
					pMapObject->SetTexture(m_TexturesVector[2]);
					pMapObject->BuildObjects(pPxPhysics, pPxScene, pPxM, FbxName.c_str());
					pMapObject->Rotate(d3dxvEulerRotation.x, d3dxvEulerRotation.y, d3dxvEulerRotation.z);
					pMapObject->SetPosition(d3dxvPosition);
					m_ObjectsVector.push_back(make_pair(i, pMapObject));
					break;
				}

			}
		}
	}
	ifsModelTransform.close();

	//インスタンスデータ（レンダリングするオブジェクトの位置ベクトルの配列、アニメーションする時の現在インデックス）をメッシュの頂点バッファに追加します。
	UINT nMatrixBufferStride = sizeof(InstanceBuffer);
	UINT nMatrixBufferOffset = 0;

	for (int i = 0; i < m_InstanceDataVector.size(); ++i){
		ID3D11Buffer *InstanceBuffer = m_InstanceDataVector[i]->GetInstanceBuffer();
		m_InstanceDataVector[i]->GetMesh()->AppendVertexBuffer(1, &InstanceBuffer, &nMatrixBufferStride, &nMatrixBufferOffset);
	}
}

//ゲームの中で、オブジェクト、キャラクタの追加
void CInstancingShader::AddObject(PxPhysics *pPxPhysics, PxScene *pPxScene, int _IndexOfInstanceDataVector, int _IndexOfMaterial, int _IndexOfTexture, D3DXVECTOR3 _d3dxvPosition, D3DXVECTOR3 _Force, bool _isStatic, bool _hasAnimation)
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
		pObject->BuildObjects(pPxPhysics, pPxScene, pPxM, "UserThrown");
		pObject->SetPosition(_d3dxvPosition);
		m_ObjectsVector.push_back(make_pair(_IndexOfInstanceDataVector, pObject));
	}
	else
	{
		CDynamicObject *pObject;
		if (_hasAnimation)
			pObject = new CDynamicObject(true);
		else
			pObject = new CDynamicObject();
		pObject->SetMesh(m_InstanceDataVector[_IndexOfInstanceDataVector]->GetMesh());
		pObject->SetMaterial(m_MaterialsVector[_IndexOfMaterial]);
		pObject->SetTexture(m_TexturesVector[_IndexOfTexture]);
		pObject->BuildObjects(pPxPhysics, pPxScene, pPxM);
		pObject->SetPosition(_d3dxvPosition);
		pObject->AddForce(_Force.x, _Force.y, _Force.z);
		if (pObject->HasAnimation())
		{
			CAnimationController *pAnimationController = m_pPlayer->GetAnimationController();
			CAnimationInstancing *pAnimationInstancing = m_InstanceDataVector[1]->GetAnimationInstancing();
			CAnimationList *pAnimationList = pAnimationInstancing->GetAnimation();
			unordered_map<string, CAnimation> *pAnimationMap = pAnimationList->GetAnimationMap();
			pAnimationController->SetAnimationData("Attack", (*pAnimationMap)["Attack"].GetLength());
			pAnimationController->SetAnimationData("Run", (*pAnimationMap)["Run"].GetLength());
			pAnimationController->SetAnimationData("Idle", (*pAnimationMap)["Idle"].GetLength());
			int randAnim = rand() % 3;
			switch (randAnim)
			{
				case 0:
					pAnimationController->Play("Attack");
					break;
				case 1:
					pAnimationController->Play("Run");
					break;
				case 2:
					pAnimationController->Play("Idle");
					break;
			}
			
		}
		m_ObjectsVector.push_back(make_pair(_IndexOfInstanceDataVector, pObject));
	}
}
CCharacterObject* CInstancingShader::AddCharacter(PxPhysics *pPxPhysics, PxScene *pPxScene, PxControllerManager *pPxControllerManager, int _IndexOfInstanceDataVector, int _IndexOfMaterial, int _IndexOfTexture, D3DXVECTOR3 _d3dxvPosition)
{
	if (_IndexOfInstanceDataVector >= m_InstanceDataVector.size())
	{
		cout << "InstanceDataVector index out of range." << endl;
		return NULL;
	}
	if (_IndexOfMaterial >= m_MaterialsVector.size())
	{
		cout << "MaterialsVector index out of range." << endl;
		return NULL;
	}
	if (_IndexOfTexture >= m_TexturesVector.size())
	{
		cout << "TexturesVector index out of range." << endl;
		return NULL;
	}

	PxMaterial *pPxM = pPxPhysics->createMaterial(0.9, 0.9, 0.001);

	CCharacterObject *pObject = new CCharacterObject(true);
	pObject->SetMesh(m_InstanceDataVector[_IndexOfInstanceDataVector]->GetMesh());
	pObject->SetMaterial(m_MaterialsVector[_IndexOfMaterial]);
	pObject->SetTexture(m_TexturesVector[_IndexOfTexture]);
	pObject->BuildObjects(pPxPhysics, pPxScene, pPxM, pPxControllerManager);
	pObject->SetPosition(_d3dxvPosition);
	pObject->RotateOffset(-90, 0, 0);

	CAnimationController *pAnimationController = pObject->GetAnimationController();
	CAnimationInstancing *pAnimationInstancing = m_InstanceDataVector[1]->GetAnimationInstancing();
	CAnimationList *pAnimationList = pAnimationInstancing->GetAnimation();
	unordered_map<string, CAnimation> *pAnimationMap = pAnimationList->GetAnimationMap();

	pAnimationController->SetAnimationData("Attack", (*pAnimationMap)["Attack"].GetLength());
	pAnimationController->SetAnimationData("Run", (*pAnimationMap)["Run"].GetLength());
	pAnimationController->SetAnimationData("Idle", (*pAnimationMap)["Idle"].GetLength());
	pAnimationController->Play("Idle");
	m_ObjectsVector.push_back(make_pair(_IndexOfInstanceDataVector, pObject));

	return pObject;
}

//定数バッファに変数を登録します。
ID3D11Buffer *CInstancingShader::CreateInstanceBuffer(ID3D11Device *pd3dDevice, int nObjects, UINT nBufferStride)
{
	ID3D11Buffer *pd3dInstanceBuffer = NULL;
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = nBufferStride * nObjects;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = NULL;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, NULL, &pd3dInstanceBuffer);
	return(pd3dInstanceBuffer);
}

//視錐台カーリング、マルチスレット、インスタンシングを使ってレンダリングします。
void CInstancingShader::Render(ID3D11DeviceContext *pd3dDeviceContext, int nThreadID, CCamera *pCamera){
	OnPostRender(pd3dDeviceContext);

	AABB bcBoundingCube;
	bool bIsVisible = false;
	for (int i = 0; i < m_InstanceDataVector.size(); ++i){
		if (i % MAX_THREAD != nThreadID) continue;//スレッドの数に分けてレンダリングします。
		if (m_InstanceDataVector[i]->GetHasAnimation() == true){//アニメーションを持っている場合にはアニメーションテクスチャの長さを更新します。
			m_InstanceDataVector[i]->GetAnimationInstancing()->UpdateShaderVariables(pd3dDeviceContext);
			D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
			pd3dDeviceContext->Map(m_pd3dcbAnimationTextureWidth, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
			ANIM_WIDTH *pd3dxmTexWidth = (ANIM_WIDTH*)d3dMappedResource.pData;
			pd3dxmTexWidth->WIDTH[0] = m_InstanceDataVector[i]->GetAnimationInstancing()->GetTextureWidth();
			pd3dDeviceContext->Unmap(m_pd3dcbAnimationTextureWidth, 0);
			pd3dDeviceContext->VSSetConstantBuffers(VS_SLOT_ANIMATION_WIDTH, 1, &m_pd3dcbAnimationTextureWidth);
		}
		int nVisibleObjects = 0;

		//インスタンスのポジション更新
		D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
		pd3dDeviceContext->Map(m_InstanceDataVector[i]->GetInstanceBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
		InstanceBuffer *pd3dxmInstances = (InstanceBuffer *)d3dMappedResource.pData;
		CTexture *pLastTexture = NULL;
		CMaterial *pLastMaterial = NULL;
		for (int j = 0; j < m_ObjectsVector.size(); ++j)
		{
			if (m_ObjectsVector[j].first == i){//オブジェクトインデックスとインスタンスデータのインデックスを比較し、更新します。
				if (m_ObjectsVector[j].second->isActive())//オブジェクトがアクティブしている場合
				{
					//オブジェクトのメッシュバウンディングボックス（モデル座標系）をオブジェクトのワールド変換行列に変換して、新しいバウンディングボックスを計算します。
					bcBoundingCube = m_ObjectsVector[j].second->GetMesh(0)->GetBoundingCube();
					bcBoundingCube.Transform(&m_ObjectsVector[j].second->GetWorldMatrix());
					//バウンディングボックス（ワールド座標系）がカメラの視錐台に含まれる行を検査して含まれている場合にレンダリングします。
					bIsVisible = pCamera->IsInFrustum(bcBoundingCube.GetMinimum(), bcBoundingCube.GetMaximum());
					if (bIsVisible)
					{
						if (m_ObjectsVector[j].second->GetTexture()) 
						{
							if (m_ObjectsVector[j].second->GetTexture() != pLastTexture)//先のテクスチャと違う場合、更新します。
							{
								CTexturedIlluminatedShader::UpdateShaderVariables(pd3dDeviceContext, m_ObjectsVector[j].second->GetTexture());
								pLastTexture = m_ObjectsVector[j].second->GetTexture();
							}
						}

						if (m_ObjectsVector[j].second->GetMaterial())
						{
							if (m_ObjectsVector[j].second->GetMaterial() != pLastMaterial)//先のマテリアルと違う場合、更新します。
							{
								CTexturedIlluminatedShader::UpdateShaderVariables(pd3dDeviceContext, &m_ObjectsVector[j].second->GetMaterial()->GetMaterial());
								pLastMaterial = m_ObjectsVector[j].second->GetMaterial();
							}
						}

						if (m_InstanceDataVector[i]->GetHasAnimation())//アニメーションを持っている場合、時間に応じて現在のアニメーションインデックスを更新します。
						{
							CDynamicObject *pObject = (CDynamicObject*)m_ObjectsVector[j].second;
							CAnimationController *pAnimationController = pObject->GetAnimationController();
							pd3dxmInstances[nVisibleObjects].AnimationPos[0] =
								m_InstanceDataVector[i]->GetAnimationInstancing()->GetCurrentOffset(pAnimationController->GetCurrentAnimationName(), pAnimationController->GetCurrentAnimationTime());

						}
						D3DXMatrixTranspose(&pd3dxmInstances[nVisibleObjects++].InstancePos, &m_ObjectsVector[j].second->GetWorldMatrix());
					}
				}
			}
		}
		pd3dDeviceContext->Unmap(m_InstanceDataVector[i]->GetInstanceBuffer(), 0);
		if (nVisibleObjects > 0)
			m_InstanceDataVector[i]->GetMesh()->RenderInstanced(pd3dDeviceContext, nVisibleObjects, 0);
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

void CSkyBoxShader::BuildObjects(ID3D11Device *pd3dDevice, PxPhysics *pPxPhysics, PxScene *pPxScene, PxControllerManager *pPxControllerManager, FbxManager *pFbxSdkManager)
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