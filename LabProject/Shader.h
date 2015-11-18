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
	virtual void BuildObjects(ID3D11Device *pd3dDevice, PxPhysics *pPxPhysics, PxScene *pPxScene, FbxManager *pFbxSdkManager);

	vector<pair<int,CGameObject*>>* GetObjectsVector();

protected:
	ID3D11VertexShader				*m_pd3dVertexShader;
	ID3D11PixelShader				*m_pd3dPixelShader;
	ID3D11InputLayout				*m_pd3dVertexLayout;
	
	ID3D11Buffer					*m_pd3dcbMaterial;
	ID3D11Buffer					*m_pd3dcbWorldMatrix;
	
	vector<pair<int,CGameObject*>>	m_ObjectsVector;
	vector<CMaterial*>				m_MaterialsVector;
	vector<CTexture*>				m_TexturesVector;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//

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

	virtual void BuildObjects(ID3D11Device *pd3dDevice, PxPhysics *pPxPhysics, PxScene *pPxScene, FbxManager *pFbxSdkManager);
	virtual void Render(ID3D11DeviceContext *pd3dImmediateDeviceContext, CCamera *pCamera=NULL);
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//

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

////////////////////////////////////////////////////////////////////////////////////////////////////
//
class InstanceData{
public:
	InstanceData(CMesh* _Mesh, ID3D11Buffer* _Buffer, int _nObjects)
	{ 
		m_pMesh = _Mesh; 
		m_pd3dInstances = _Buffer; 
		m_nObjects = _nObjects;
	}
	~InstanceData(){}

	void SetMesh(CMesh* _Mesh){if(m_pMesh) m_pMesh->Release(); m_pMesh = _Mesh;}
	CMesh* GetMesh(){return m_pMesh;}

	void SetInstanceBuffer(ID3D11Buffer* _InstanceBuffer){if(m_pd3dInstances) m_pd3dInstances->Release(); m_pd3dInstances = _InstanceBuffer;}
	ID3D11Buffer* GetInstanceBuffer(){return m_pd3dInstances;}

	void Release(){if(m_pMesh) m_pMesh->Release(); if(m_pd3dInstances) m_pd3dInstances->Release();}

	int				m_nObjects;
	int				m_nVisibleObjects;

private:
	CMesh			*m_pMesh;
	ID3D11Buffer	*m_pd3dInstances;
};

class CInstancingShader : public CTexturedIlluminatedShader
{
public:
	CInstancingShader();
	~CInstancingShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
	//virtual void CreateShaderVariables(ID3D11Device *pd3dDevice);
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dImmediateDeviceContext, CCamera *pCamera);

	virtual void BuildObjects(ID3D11Device *pd3dDevice, PxPhysics *pPxPhysics, PxScene *pPxScene, FbxManager *pFbxSdkManager);
	virtual void Render(ID3D11DeviceContext *pd3dImmediateDeviceContext, CCamera *pCamera=NULL);
	
	ID3D11Buffer* CreateInstanceBuffer(ID3D11Device *pd3dDevice, int nObjects, UINT nBufferStride, void *pBufferData);

	void AddObject(PxPhysics *pPxPhysics, PxScene *pPxScene, int _IndexOfInstanceDataVector, int _IndexOfMaterial, int _IndexOfTexture, D3DXVECTOR3 _d3dxvPosition, int _isStatic, D3DXVECTOR3 _Force);

public:
	//객체의 인스턴스 버퍼이다.
	

	UINT						m_nMatrixBufferStride;
	UINT						m_nMatrixBufferOffset;
	
	vector<InstanceData>		m_InstanceDataVector;

	
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//

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

////////////////////////////////////////////////////////////////////////////////////////////////////
//

class CSkyBoxShader : public CTexturedShader
{
public:
	CSkyBoxShader();
	virtual ~CSkyBoxShader();

	virtual void BuildObjects(ID3D11Device *pd3dDevice, PxPhysics *pPxPhysics, PxScene *pPxScene, FbxManager *pFbxSdkManager);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//

class CTerrainShader : public CTexturedIlluminatedShader
{
public:
	CTerrainShader();
	virtual ~CTerrainShader();

	virtual void BuildObjects(ID3D11Device *pd3dDevice, PxPhysics *pPxPhysics, PxScene *pPxScene, FbxManager *pFbxSdkManager);

	CHeightMapTerrain *GetTerrain();
};