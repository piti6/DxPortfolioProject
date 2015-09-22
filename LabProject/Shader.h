//-----------------------------------------------------------------------------
// File: Shader.h
//-----------------------------------------------------------------------------

#pragma once

#include "Object.h"

struct VS_CB_WORLD_MATRIX
{
	D3DXMATRIX m_d3dxmtxWorld;         
};

class CShader
{
public:
    CShader();
    ~CShader();

	void CreateVertexShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11VertexShader **ppd3dVertexShader, D3D11_INPUT_ELEMENT_DESC *pd3dInputLayout, UINT nElements, ID3D11InputLayout **ppd3dVertexLayout);
	void CreatePixelShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11PixelShader **ppd3dPixelShader);

	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void CreateShaderVariables(ID3D11Device *pd3dDevice);
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, MATERIAL *pMaterial=NULL);

	virtual void Render(ID3D11DeviceContext *pd3dImmediateDeviceContext);

public:
	ID3D11VertexShader				*m_pd3dVertexShader;
	ID3D11InputLayout				*m_pd3dVertexLayout;
	ID3D11Buffer *m_pd3dcbMaterial;
	ID3D11PixelShader				*m_pd3dPixelShader;
};

class CDiffusedShader : public CShader
{
public:
    CDiffusedShader();
    ~CDiffusedShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void CreateShaderVariables(ID3D11Device *pd3dDevice);
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dImmediateDeviceContext);

	virtual void Render(ID3D11DeviceContext *pd3dImmediateDeviceContext);
};

class CSceneShader : public CDiffusedShader
{
public:
    CSceneShader();
    ~CSceneShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void CreateShaderVariables(ID3D11Device *pd3dDevice);
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dImmediateDeviceContext, D3DXMATRIX *pd3dxmtxWorld);
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, MATERIAL *pMaterial=NULL);
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, CTexture *pTexture);

	virtual void BuildObjects(ID3D11Device *pd3dDevice);
	virtual void ReleaseObjects();
    virtual void AnimateObjects(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dImmediateDeviceContext, CCamera *pCamera=NULL);

	bool Ter[50][52];
	CGameObject						**m_ppObjects;       
	int								m_nObjects;

	ID3D11Buffer					*m_pd3dcbWorldMatrix;
	ID3D11Buffer					*m_pd3dcbMaterial;

};

class CPlayerShader : public CDiffusedShader
{
public:
    CPlayerShader();
    ~CPlayerShader();
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, CTexture *pTexture);
	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void CreateShaderVariables(ID3D11Device *pd3dDevice);
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dImmediateDeviceContext, D3DXMATRIX *pd3dxmtxWorld);
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, MATERIAL *pMaterial=NULL);
	ID3D11Buffer					*m_pd3dcbWorldMatrix;
};
