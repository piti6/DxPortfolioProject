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
}

CShader::~CShader()
{
	if (m_pd3dVertexShader) m_pd3dVertexShader->Release();
	if (m_pd3dPixelShader) m_pd3dPixelShader->Release();
	if (m_pd3dVertexLayout) m_pd3dVertexLayout->Release();
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

void CShader::Render(ID3D11DeviceContext *pd3dImmediateDeviceContext)
{
	pd3dImmediateDeviceContext->IASetInputLayout(m_pd3dVertexLayout);
	pd3dImmediateDeviceContext->VSSetShader(m_pd3dVertexShader, NULL, 0);
	pd3dImmediateDeviceContext->PSSetShader(m_pd3dPixelShader, NULL, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CDiffusedShader::CDiffusedShader()
{
}

CDiffusedShader::~CDiffusedShader()
{
}

void CDiffusedShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSDiffusedColor", "vs_4_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSDiffusedColor", "ps_4_0", &m_pd3dPixelShader);
}

void CDiffusedShader::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	CShader::CreateShaderVariables(pd3dDevice);
}         

void CDiffusedShader::UpdateShaderVariables(ID3D11DeviceContext *pd3dImmediateDeviceContext)
{
	CShader::UpdateShaderVariables(pd3dImmediateDeviceContext);
}

void CDiffusedShader::Render(ID3D11DeviceContext *pd3dImmediateDeviceContext)
{
	CShader::Render(pd3dImmediateDeviceContext);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CSceneShader::CSceneShader()
{
	for(int i=0;i<50;++i)
		for(int j=0;j<52;++j)
			Ter[i][j]=0;
	m_ppObjects = NULL;       
	m_nObjects = 0;
	m_pd3dcbWorldMatrix = NULL;
}

CSceneShader::~CSceneShader()
{
}

void CSceneShader::BuildObjects(ID3D11Device *pd3dDevice)
{
		CMaterial **ppMaterials = new CMaterial*[3];
	ppMaterials[0] = new CMaterial();
	ppMaterials[0]->m_Material.m_d3dxcDiffuse = D3DXCOLOR(0.5f,0.0f,0.0f,1.0f);
	ppMaterials[0]->m_Material.m_d3dxcAmbient = D3DXCOLOR(0.5f,0.0f,0.0f,1.0f);
	ppMaterials[0]->m_Material.m_d3dxcSpecular = D3DXCOLOR(0.5f,0.5f,0.5f,5.0f);
	ppMaterials[0]->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.0f,0.0f,0.0f,1.0f);
	ppMaterials[1] = new CMaterial();
	ppMaterials[1]->m_Material.m_d3dxcDiffuse = D3DXCOLOR(0.0f,0.5f,0.0f,1.0f);
	ppMaterials[1]->m_Material.m_d3dxcAmbient = D3DXCOLOR(0.0f,0.5f,0.0f,1.0f);
	ppMaterials[1]->m_Material.m_d3dxcSpecular = D3DXCOLOR(0.5f,0.5f,0.5f,10.0f);
	ppMaterials[1]->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.0f,0.0f,0.0f,1.0f);
	ppMaterials[2] = new CMaterial();
	ppMaterials[2]->m_Material.m_d3dxcDiffuse = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f);
	ppMaterials[2]->m_Material.m_d3dxcAmbient = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f);
	ppMaterials[2]->m_Material.m_d3dxcSpecular = D3DXCOLOR(1.0f,1.0f,1.0f,10.0f);
	ppMaterials[2]->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.0f,0.0f,0.0f,1.0f);

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
	CTexture **ppTextures = new CTexture*[3];
	ppTextures[0] = new CTexture(1);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("3.jpg"), NULL, NULL, &pd3dTexture, NULL);
	ppTextures[0]->SetTexture(0, pd3dTexture, pd3dSamplerState);
	ppTextures[1] = new CTexture(1);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("2.jpg"), NULL, NULL, &pd3dTexture, NULL);
	ppTextures[1]->SetTexture(0, pd3dTexture, pd3dSamplerState);
	ppTextures[2] = new CTexture(1);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("1.jpg"), NULL, NULL, &pd3dTexture, NULL);
	ppTextures[2]->SetTexture(0, pd3dTexture, pd3dSamplerState);

	CMesh *pCubeMesh = new CCubeMeshIlluminatedTextured(pd3dDevice, 50.0f, 50.0f, 50.0f);
	int num_Cube=0;
	bool Terrain[50][50] = {
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, 
		{1,1,1,1,1,1,1,1,1,1,0,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{0,0,0,0,1,1,1,1,1,1,0,1,1,0,1,0,1,0,0,0,0,0,0,0,1,1,1,1,1,1,0,1,1,0,1,0,1,0,0,0,0,0,0,0,1,1,1,1,1,1},
		{1,1,1,0,1,1,1,1,1,1,0,1,1,0,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,1,0,1,1,0,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,1},
		{1,0,1,0,1,1,1,1,1,1,0,1,1,0,1,0,1,0,1,0,0,0,1,0,1,1,1,1,1,1,0,0,0,0,1,0,1,0,1,0,0,0,1,0,1,1,1,1,1,1},
		{1,0,1,0,0,0,0,0,0,0,0,1,1,0,1,0,1,0,1,0,1,0,1,0,0,0,0,0,0,0,0,1,1,0,1,0,1,0,1,0,1,0,1,0,0,0,0,0,0,1},
		{1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,0,0,0,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,1,0,0,0,1,0,0,0,1,1,1,1,1,1,1,1},
		{1,1,1,0,0,0,0,0,0,0,0,1,1,0,1,1,1,1,1,0,1,1,1,0,0,0,0,0,0,0,0,1,1,0,1,1,1,1,1,0,1,1,1,0,0,0,0,0,0,1},
		{1,0,0,0,1,1,1,1,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,0,0,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1},
		{1,0,0,0,0,1,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1},
		{1,1,1,1,0,1,0,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1},
		{1,0,0,1,0,1,0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1},
		{1,1,0,1,0,1,0,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,0,1,0,1,0,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,0,1,0,1,0,1,1,1},
		{1,1,0,1,0,1,0,0,0,0,0,1,1,0,1,0,1,0,0,0,0,1,0,1,0,1,0,0,0,0,0,1,1,0,1,0,1,0,0,0,0,1,0,1,0,1,0,0,0,1},
		{1,1,0,1,0,1,1,1,1,1,0,1,1,0,1,0,1,0,1,1,1,1,0,1,0,1,1,1,1,1,0,1,1,0,1,0,1,0,1,1,1,1,0,1,0,1,1,1,1,1},
		{1,0,0,1,0,0,0,0,0,1,0,1,1,0,1,0,1,0,0,0,0,0,0,1,0,0,0,0,0,1,0,1,1,0,1,0,1,0,0,0,0,0,0,1,0,0,0,0,0,1},
		{1,1,1,1,0,1,0,1,0,0,0,1,1,0,1,0,1,1,1,1,1,1,1,1,0,1,0,1,0,0,0,1,1,0,1,0,1,1,1,1,1,1,1,1,0,1,0,1,0,1},
		{1,0,0,0,0,1,0,1,1,0,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,1,1,1,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,1,1,1},
		{1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1}, 
		{1,1,1,1,1,1,1,1,1,1,0,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,0,0,0,1,1,1,1,1,1,0,1,1,1,1,1,1,1},
		{1,0,0,0,1,1,1,1,1,1,0,1,1,0,1,0,1,0,0,0,0,0,0,0,1,1,1,1,1,1,0,1,1,0,1,0,1,0,0,0,0,0,0,0,1,1,1,1,1,1},
		{1,1,1,0,1,1,1,1,1,1,0,1,1,0,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,1,0,1,1,0,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,1},
		{1,0,1,0,1,1,1,1,1,1,0,1,1,0,1,0,1,0,1,0,0,0,1,0,1,1,1,1,1,1,0,1,1,0,1,0,1,0,1,0,0,0,1,0,1,1,1,1,1,1},
		{1,0,1,0,0,0,0,0,0,0,0,1,1,0,1,0,1,0,1,0,1,0,1,0,0,0,0,0,0,0,0,1,1,0,1,0,1,0,1,0,1,0,1,0,0,0,0,0,0,1},
		{1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,0,0,0,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,1,0,0,0,1,0,0,0,1,1,1,1,1,1,1,1},
		{1,1,1,0,0,0,0,0,0,0,0,1,1,0,1,1,1,1,1,0,1,1,1,0,0,0,0,0,0,0,0,1,1,0,1,1,1,1,1,0,1,1,1,0,0,0,0,0,0,1},
		{1,0,0,0,1,1,1,1,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,0,0,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1},
		{1,0,0,0,0,1,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1},
		{1,1,1,1,0,1,0,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1},
		{1,0,0,1,0,1,0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1},
		{1,1,0,1,0,1,0,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,0,1,0,1,0,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,0,1,0,1,0,1,1,1},
		{1,1,0,1,0,1,0,0,0,0,0,1,1,0,1,0,1,0,0,0,0,1,0,1,0,1,0,0,0,0,0,1,1,0,1,0,1,0,0,0,0,1,0,1,0,1,0,0,0,1},
		{1,1,0,1,0,1,1,1,1,1,0,1,1,0,1,0,1,0,1,1,1,1,0,1,0,1,1,1,1,1,0,1,1,0,1,0,1,0,1,1,1,1,0,1,0,1,1,1,1,1},
		{1,0,0,1,0,0,0,0,0,1,0,1,1,0,1,0,1,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,1},
		{1,1,1,1,0,1,0,1,0,0,0,1,1,0,1,0,1,1,1,1,1,1,1,1,0,1,0,1,0,0,0,1,1,0,1,0,1,1,1,1,1,1,1,1,0,1,0,1,0,1},
		{1,0,0,0,0,1,0,1,1,1,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,1,1,1,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1}, 
		{1,1,1,1,1,1,1,1,1,1,0,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,0,0,0,1,1,1,1,1,1,0,1,1,0,1,0,1,0,0,0,0,0,0,0,1,1,1,1,1,1,0,1,1,0,1,0,1,0,0,0,0,0,0,0,1,1,1,1,1,1},
		{1,1,1,0,1,1,1,1,1,1,0,1,1,0,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,1,0,1,1,0,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,1},
		{1,0,1,0,1,1,1,1,1,1,0,1,1,0,1,0,1,0,1,0,0,0,1,0,1,1,1,1,1,1,0,1,1,0,1,0,1,0,1,0,0,0,1,0,1,1,1,1,1,1},
		{1,0,1,0,0,0,0,0,0,0,0,1,1,0,1,0,1,0,1,0,1,0,1,0,0,0,0,0,0,0,0,1,1,0,1,0,1,0,1,0,1,0,1,0,0,0,0,0,0,1},
		{1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,0,0,0,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,1,0,0,0,1,0,0,0,1,1,1,1,1,1,1,1},
		{1,1,1,0,0,0,0,0,0,0,0,1,1,0,1,1,1,1,1,0,1,1,1,0,0,0,0,0,0,0,0,1,1,0,1,1,1,1,1,0,1,1,1,0,0,0,0,0,0,0},
		{1,0,0,0,1,1,1,1,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
	};
	for(int i=0; i<50;++i)
		for(int j=0; j<50;++j)
			if(Terrain[i][j])
				num_Cube++;
	m_nObjects =num_Cube*2 + 50*52/2;
	m_ppObjects = new CGameObject*[m_nObjects];
	CRotatingObject *pRotatingObject = NULL;

	int num_Arr=0;
	for(int k=0;k<2;++k)
		for(int i=0; i<50;++i)
			for(int j=0; j<50;++j){
				if(Terrain[i][j]){
					pRotatingObject = new CRotatingObject();
					pRotatingObject->SetMesh(pCubeMesh);
					pRotatingObject->SetMaterial(ppMaterials[2]);
				    pRotatingObject->SetTexture(ppTextures[rand()%3]);

					pRotatingObject->SetPosition(50*i, 50*k,50*j);
					m_ppObjects[num_Arr++] = pRotatingObject;
				}
			}
			for(int i=0;i<50;++i)
				for(int j=0;j<52;++j){
					pRotatingObject = new CRotatingObject();
					if(j%2==1){
						pRotatingObject->SetMesh(pCubeMesh);
						pRotatingObject->SetMaterial(ppMaterials[2]);
						pRotatingObject->SetTexture(ppTextures[rand()%3]);
						pRotatingObject->SetPosition(50*i, -50,50*(j-2));
						m_ppObjects[num_Arr++] = pRotatingObject;			
						Ter[i][j] = 1;
					}
				}
				CreateShaderVariables(pd3dDevice);
				delete [] ppTextures;
				delete [] ppMaterials;

}

void CSceneShader::ReleaseObjects()
{
	if (m_pd3dcbWorldMatrix) m_pd3dcbWorldMatrix->Release();

	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) delete m_ppObjects[j];
		delete [] m_ppObjects;
	}
}

void CSceneShader::AnimateObjects(float fTimeElapsed)
{/*
 for (int j = 0; j < m_nObjects; j++)
 {
 //m_ppObjects[j]->Animate(fTimeElapsed);
 }*/
}

void CSceneShader::CreateShader(ID3D11Device *pd3dDevice)
{
	//CDiffusedShader::CreateShader(pd3dDevice);
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

void CSceneShader::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	CDiffusedShader::CreateShaderVariables(pd3dDevice);

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
void CSceneShader::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, CTexture *pTexture)
{
	pd3dDeviceContext->PSSetShaderResources(PS_SLOT_TEXTURE, pTexture->m_nTextures, pTexture->m_ppd3dsrvTextures);
	pd3dDeviceContext->PSSetSamplers(PS_SLOT_SAMPLER_STATE, pTexture->m_nTextures, pTexture->m_ppd3dSamplerStates);
}

void CSceneShader::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, MATERIAL *pMaterial)
{
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbMaterial, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	MATERIAL *pcbMaterial = (MATERIAL *)d3dMappedResource.pData;
	memcpy(pcbMaterial, pMaterial, sizeof(MATERIAL));
	pd3dDeviceContext->Unmap(m_pd3dcbMaterial, 0);
	pd3dDeviceContext->PSSetConstantBuffers(PS_SLOT_MATERIAL, 1, &m_pd3dcbMaterial);
}


void CSceneShader::UpdateShaderVariables(ID3D11DeviceContext *pd3dImmediateDeviceContext, D3DXMATRIX *pd3dxmtxWorld)
{
	CDiffusedShader::UpdateShaderVariables(pd3dImmediateDeviceContext);

	//Update World Matrix Constant Buffer
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dImmediateDeviceContext->Map(m_pd3dcbWorldMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_CB_WORLD_MATRIX *pcbWorldMatrix = (VS_CB_WORLD_MATRIX *)d3dMappedResource.pData;
	D3DXMatrixTranspose(&pcbWorldMatrix->m_d3dxmtxWorld, pd3dxmtxWorld);
	pd3dImmediateDeviceContext->Unmap(m_pd3dcbWorldMatrix, 0);
	pd3dImmediateDeviceContext->VSSetConstantBuffers(VS_SLOT_WORLD_MATRIX, 1, &m_pd3dcbWorldMatrix);
}

void CSceneShader::Render(ID3D11DeviceContext *pd3dImmediateDeviceContext, CCamera *pCamera)
{
	//카메라의 절두체에 포함되는 객체들만을 렌더링한다. 
	CDiffusedShader::Render(pd3dImmediateDeviceContext);

	bool bIsVisible = false;
	AABB bcBoundingCube;
	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			/*객체의 메쉬의 바운딩 박스(모델 좌표계)를 객체의 월드 변환 행렬로 변환하고 새로운 바운딩 박스를 계산한다.*/
			bcBoundingCube = m_ppObjects[j]->m_pMesh->m_bcBoundingCube;
			bcBoundingCube.Transform(&m_ppObjects[j]->m_d3dxmtxWorld);
			//바운딩 박스(월드 좌표계)가 카메라의 절두체에 포함되는 가를 검사하고 포함되는 경우에 렌더링한다. 
			bIsVisible = pCamera->IsInFrustum(bcBoundingCube.m_d3dxvMinimum, bcBoundingCube.m_d3dxvMaximum);
			if (bIsVisible)
			{
				if (m_ppObjects[j]->m_pTexture) 
					UpdateShaderVariables(pd3dImmediateDeviceContext, m_ppObjects[j]->m_pTexture);
				if (m_ppObjects[j]->m_pMaterial) 
					UpdateShaderVariables(pd3dImmediateDeviceContext, &m_ppObjects[j]->m_pMaterial->m_Material);
				UpdateShaderVariables(pd3dImmediateDeviceContext, &m_ppObjects[j]->m_d3dxmtxWorld);
				m_ppObjects[j]->Render(pd3dImmediateDeviceContext, pCamera);
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
	if (m_pd3dcbWorldMatrix) m_pd3dcbWorldMatrix->Release();
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
	CDiffusedShader::CreateShaderVariables(pd3dDevice);

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
	CDiffusedShader::UpdateShaderVariables(pd3dImmediateDeviceContext);

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
/*
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
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSTexturedColor", "vs_4_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSTexturedColor", "ps_4_0", &m_pd3dPixelShader);
}
void CTexturedShader::BuildObjects(ID3D11Device *pd3dDevice)
{
//텍스쳐 맵핑에 사용할 샘플러 상태 객체를 생성한다.
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

직육면체에 맵핑할 텍스쳐 객체를 생성한다. 이미지 파일은 어떤 이미지라도 상관없으므로 적당한 파일의 이름을 사용하라.
	ID3D11ShaderResourceView *pd3dTexture = NULL;    
	CTexture **ppTextures = new CTexture*[3];
	ppTextures[0] = new CTexture(1);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Data\\Stone01.jpg"), NULL, NULL, &pd3dTexture, NULL);
	ppTextures[0]->SetTexture(0, pd3dTexture, pd3dSamplerState);
	ppTextures[1] = new CTexture(1);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Data\\Brick01.jpg"), NULL, NULL, &pd3dTexture, NULL);
	ppTextures[1]->SetTexture(0, pd3dTexture, pd3dSamplerState);
	ppTextures[2] = new CTexture(1);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Data\\Wood01.jpg"), NULL, NULL, &pd3dTexture, NULL);
	ppTextures[2]->SetTexture(0, pd3dTexture, pd3dSamplerState);

	CMesh *pMeshTextured = new CTexturedCubeMesh(pd3dDevice, 12.0f, 12.0f, 12.0f);

//텍스쳐 맵핑된 직육면체와 조명과 텍스쳐 맵핑을 사용한 직육면체를 교대로 배치할 것이다.
	int xObjects = 3, yObjects = 3, zObjects = 3, i = 0, nObjectTypes = 2;
	m_nObjects = ((xObjects*2)+1) * ((yObjects*2)+1) * ((zObjects*2)+1);
	m_ppObjects = new CGameObject*[m_nObjects]; 

	float fxPitch = 12.0f * 1.7f;
	float fyPitch = 12.0f * 1.7f;
	float fzPitch = 12.0f * 1.7f;
	CRotatingObject *pRotatingObject = NULL;
	for (int x = -xObjects; x <= xObjects; x++)
	{
		for (int y = -yObjects; y <= yObjects; y++)
		{
			for (int z = -zObjects; z <= zObjects; z++)
			{
			    pRotatingObject = new CRotatingObject();
			    pRotatingObject->SetMesh(pMeshTextured);
			    pRotatingObject->SetTexture(ppTextures[i%3]);
			    pRotatingObject->SetPosition((x*(fxPitch*nObjectTypes)+0*fxPitch), (y*(fyPitch*nObjectTypes)+0*fyPitch), (z*(fzPitch*nObjectTypes)+0*fzPitch));
			    pRotatingObject->SetRotationAxis(D3DXVECTOR3(0.0f, 1.0f, 0.0f));
			    pRotatingObject->SetRotationSpeed(10.0f*(i % 10));
			    m_ppObjects[i++] = pRotatingObject;
			}
		}
	}

	CreateShaderVariables(pd3dDevice);

	delete [] ppTextures;
}
CIlluminatedTexturedShader::CIlluminatedTexturedShader()
{
}

CIlluminatedTexturedShader::~CIlluminatedTexturedShader()
{
}*/
/*
void CIlluminatedTexturedShader::CreateShader(ID3D11Device *pd3dDevice)
{
	CShader::CreateShader(pd3dDevice);

	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSTexturedLighting", "vs_4_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSTexturedLighting", "ps_4_0", &m_pd3dPixelShader);
}*/
/*
void CIlluminatedTexturedShader::BuildObjects(ID3D11Device *pd3dDevice)
{
	CMaterial **ppMaterials = new CMaterial*[3];
	ppMaterials[0] = new CMaterial();
	ppMaterials[0]->m_Material.m_d3dxcDiffuse = D3DXCOLOR(0.5f,0.0f,0.0f,1.0f);
	ppMaterials[0]->m_Material.m_d3dxcAmbient = D3DXCOLOR(0.5f,0.0f,0.0f,1.0f);
	ppMaterials[0]->m_Material.m_d3dxcSpecular = D3DXCOLOR(0.5f,0.5f,0.5f,5.0f);
	ppMaterials[0]->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.0f,0.0f,0.0f,1.0f);
	ppMaterials[1] = new CMaterial();
	ppMaterials[1]->m_Material.m_d3dxcDiffuse = D3DXCOLOR(0.0f,0.5f,0.0f,1.0f);
	ppMaterials[1]->m_Material.m_d3dxcAmbient = D3DXCOLOR(0.0f,0.5f,0.0f,1.0f);
	ppMaterials[1]->m_Material.m_d3dxcSpecular = D3DXCOLOR(0.5f,0.5f,0.5f,10.0f);
	ppMaterials[1]->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.0f,0.0f,0.0f,1.0f);
	ppMaterials[2] = new CMaterial();
	ppMaterials[2]->m_Material.m_d3dxcDiffuse = D3DXCOLOR(0.0f,0.0f,0.5f,1.0f);
	ppMaterials[2]->m_Material.m_d3dxcAmbient = D3DXCOLOR(0.0f,0.0f,0.5f,1.0f);
	ppMaterials[2]->m_Material.m_d3dxcSpecular = D3DXCOLOR(0.5f,0.0f,0.5f,10.0f);
	ppMaterials[2]->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.0f,0.0f,0.0f,1.0f);

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
	CTexture **ppTextures = new CTexture*[3];
	ppTextures[0] = new CTexture(1);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Data\\Stone03.jpg"), NULL, NULL, &pd3dTexture, NULL);
	ppTextures[0]->SetTexture(0, pd3dTexture, pd3dSamplerState);
	ppTextures[1] = new CTexture(1);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Data\\Brick02.jpg"), NULL, NULL, &pd3dTexture, NULL);
	ppTextures[1]->SetTexture(0, pd3dTexture, pd3dSamplerState);
	ppTextures[2] = new CTexture(1);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Data\\Wood03.jpg"), NULL, NULL, &pd3dTexture, NULL);
	ppTextures[2]->SetTexture(0, pd3dTexture, pd3dSamplerState);

	CMesh *pMeshIlluminatedTextured = new CCubeMeshIlluminatedTextured(pd3dDevice, 12.0f, 12.0f, 12.0f);

	int xObjects = 3, yObjects = 3, zObjects = 3, i = 0, nObjectTypes = 2;
	m_nObjects = ((xObjects*2)+1) * ((yObjects*2)+1) * ((zObjects*2)+1);
	m_ppObjects = new CGameObject*[m_nObjects]; 

	float fxPitch = 12.0f * 1.7f;
	float fyPitch = 12.0f * 1.7f;
	float fzPitch = 12.0f * 1.7f;
	CRotatingObject *pRotatingObject = NULL;
	for (int x = -xObjects; x <= xObjects; x++)
	{
		for (int y = -yObjects; y <= yObjects; y++)
		{
			for (int z = -zObjects; z <= zObjects; z++)
			{
			    pRotatingObject = new CRotatingObject();
			    pRotatingObject->SetMesh(pMeshIlluminatedTextured);
			    pRotatingObject->SetMaterial(ppMaterials[i%3]);
			    pRotatingObject->SetTexture(ppTextures[i%3]);
			    pRotatingObject->SetPosition((x*(fxPitch*nObjectTypes)+1*fxPitch), (y*(fyPitch*nObjectTypes)+1*fyPitch), (z*(fzPitch*nObjectTypes)+1*fzPitch));
			    pRotatingObject->SetRotationAxis(D3DXVECTOR3(0.0f, 1.0f, 0.0f));
			    pRotatingObject->SetRotationSpeed(10.0f*(i % 10));
			    m_ppObjects[i++] = pRotatingObject;
			}
		}
	}

	CreateShaderVariables(pd3dDevice);

	delete [] ppTextures;
	delete [] ppMaterials;
}*/
