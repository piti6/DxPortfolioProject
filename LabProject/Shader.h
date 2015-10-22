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
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dImmediateDeviceContext, D3DXMATRIX *pd3dxmtxWorld);
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, CTexture *pTexture);
	virtual void OnPostRender(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void Render(ID3D11DeviceContext *pd3dImmediateDeviceContext, CCamera *pCamera);
	
	virtual void AnimateObjects(float fTimeElapsed,PxScene *pPxScene);
	virtual void BuildObjects(ID3D11Device *pd3dDevice, PxPhysics *pPxPhysics, PxScene *pPxScene);
	virtual void ReleaseObjects();
	
protected:
	ID3D11VertexShader				*m_pd3dVertexShader;
	ID3D11InputLayout				*m_pd3dVertexLayout;
	
	ID3D11PixelShader				*m_pd3dPixelShader;
	CGameObject						**m_ppObjects;       
	int								m_nObjects;

	ID3D11Buffer					*m_pd3dcbMaterial;
	ID3D11Buffer					*m_pd3dcbWorldMatrix;
};

class CTexturedIlluminatedShader : public CShader
{
public:
    CTexturedIlluminatedShader();
    ~CTexturedIlluminatedShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void CreateShaderVariables(ID3D11Device *pd3dDevice);
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dImmediateDeviceContext, D3DXMATRIX *pd3dxmtxWorld);
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, MATERIAL *pMaterial=NULL);
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, CTexture *pTexture);

	virtual void BuildObjects(ID3D11Device *pd3dDevice, PxPhysics *pPxPhysics, PxScene *pPxScene);
	virtual void ReleaseObjects();
	//virtual void AnimateObjects(float fTimeElapsed,PxScene *pPxScene);
	virtual void Render(ID3D11DeviceContext *pd3dImmediateDeviceContext, CCamera *pCamera=NULL);
};

class CPlayerShader : public CShader
{
public:
    CPlayerShader();
    ~CPlayerShader();
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, CTexture *pTexture);
	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void CreateShaderVariables(ID3D11Device *pd3dDevice);
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dImmediateDeviceContext, D3DXMATRIX *pd3dxmtxWorld);
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, MATERIAL *pMaterial=NULL);
	
};

class CInstancingShader : public CTexturedIlluminatedShader
{
public:
	CInstancingShader();
	~CInstancingShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dImmediateDeviceContext, CCamera *pCamera);

	virtual void BuildObjects(ID3D11Device *pd3dDevice, PxPhysics *pPxPhysics, PxScene *pPxScene);
	virtual void ReleaseObjects();
	//virtual void AnimateObjects(float fTimeElapsed,PxScene *pPxScene);
	virtual void Render(ID3D11DeviceContext *pd3dImmediateDeviceContext, CCamera *pCamera=NULL);
	ID3D11Buffer* CreateInstanceBuffer(ID3D11Device *pd3dDevice, int nObjects, UINT nBufferStride, void *pBufferData);

private:
	int					m_nVisibleInstances;
//객체의 인스턴스 버퍼이다.
	ID3D11Buffer		*m_pd3dInstances;

	UINT				m_nMatrixBufferStride;
	UINT				m_nMatrixBufferOffset;
	
	CMesh				*m_pMesh;

};

class CTexturedShader : public CShader
{
public:
    CTexturedShader();
    virtual ~CTexturedShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void CreateShaderVariables(ID3D11Device *pd3dDevice);
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, CTexture *pTexture);
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dImmediateDeviceContext, D3DXMATRIX *pd3dxmtxWorld);
};

class CSkyBoxShader : public CTexturedShader
{
public:
    CSkyBoxShader();
    virtual ~CSkyBoxShader();

	virtual void BuildObjects(ID3D11Device *pd3dDevice, PxPhysics *pPxPhysics, PxScene *pPxScene);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
};