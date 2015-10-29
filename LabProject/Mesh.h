//------------------------------------------------------- ----------------------
// File: Mesh.h
//-----------------------------------------------------------------------------

#pragma once
#include "Vertex.h"
#include "Timer.h"
#include "AABB.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
//
class CMesh
{
public:
    CMesh(ID3D11Device *pd3dDevice);
    virtual ~CMesh();

	void AddRef();
	void Release();

	virtual void SetRasterizerState(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dImmediateDeviceContext);
	virtual void RenderInstanced(ID3D11DeviceContext *pd3dDeviceContext, int nInstances, int nStartInstance);
	virtual void AppendVertexBuffer(int nBuffers,ID3D11Buffer **pd3dBuffer, UINT *nStride, UINT *nOffset);

	GET_SET_FUNC_IMPL(AABB,BoundingCube,m_bcBoundingCube);

protected:

	int								m_nReferences;

	D3D11_PRIMITIVE_TOPOLOGY		m_d3dPrimitiveTopology;
	ID3D11RasterizerState			*m_pd3dRasterizerState;

	UINT							m_nVertices;
	UINT							m_nIndices;
	
	UINT							*m_nStride;
	UINT							*m_nOffset;

	ID3D11Buffer					**m_ppd3dVertexBuffers;	
	UINT							m_nVertexBuffers;

	ID3D11Buffer					*m_pd3dIndexBuffer;

	AABB							m_bcBoundingCube;

	
};

/////////////////////////////////////////////////////////////////////////////////////////////////
//
class CMeshTextured : public CMesh
{
public:
    CMeshTextured(ID3D11Device *pd3dDevice);
    virtual ~CMeshTextured();

protected:
	ID3D11Buffer					*m_pd3dTexCoordBuffer;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
//
class CMeshIlluminated : public CMesh
{
public:
    CMeshIlluminated(ID3D11Device *pd3dDevice);
    virtual ~CMeshIlluminated();
//정점이 포함된 삼각형의 법선벡터를 계산하는 함수이다.
	D3DXVECTOR3 CalculateTriAngleNormal(BYTE *pVertices, UINT nIndex0, UINT nIndex1, UINT nIndex2);
	void SetTriAngleListVertexNormal(BYTE *pVertices);
//정점의 법선벡터의 평균을 계산하는 함수이다.
	void SetAverageVertexNormal(BYTE *pVertices, UINT *pIndices, int nPrimitives, int nOffset, bool bStrip);
	void CalculateVertexNormal(BYTE *pVertices, UINT *pIndices);
};

/////////////////////////////////////////////////////////////////////////////////////////////////
//
class CCubeMeshIlluminated : public CMeshIlluminated
{
public:
	CCubeMeshIlluminated(ID3D11Device *pd3dDevice, float fWidth=2.0f, float fHeight=2.0f, float fDepth=2.0f);
	virtual ~CCubeMeshIlluminated();
};

/////////////////////////////////////////////////////////////////////////////////////////////////
//
class CCubeMeshIlluminatedTextured : public CMeshIlluminated
{
public:
    CCubeMeshIlluminatedTextured(ID3D11Device *pd3dDevice, float fWidth=2.0f, float fHeight=2.0f, float fDepth=2.0f);
    virtual ~CCubeMeshIlluminatedTextured();
};

/////////////////////////////////////////////////////////////////////////////////////////////////
//
class CSkyBoxMesh : public CMeshTextured
{
public:
	CSkyBoxMesh(ID3D11Device *pd3dDevice, float fWidth=20.0f, float fHeight=20.0f, float fDepth=20.0f);
	virtual ~CSkyBoxMesh();
};

/////////////////////////////////////////////////////////////////////////////////////////////////
//

class CHeightMap
{
public:
	CHeightMap(LPCTSTR pFileName, int nWidth, int nLength, D3DXVECTOR3 d3dxvScale);
	virtual ~CHeightMap();

	float GetHeight(float x, float z, bool bReverseQuad = false);
	D3DXVECTOR3 GetHeightMapNormal(int x, int z);
	D3DXVECTOR3 GetScale() { return(m_d3dxvScale); }

	BYTE *GetHeightMapImage() { return(m_pHeightMapImage); }
	int GetHeightMapWidth() { return(m_nWidth); }
	int GetHeightMapLength() { return(m_nLength); }

private:
	BYTE						*m_pHeightMapImage;
	int							m_nWidth;
	int							m_nLength;
	D3DXVECTOR3					m_d3dxvScale;

};


class CHeightMapGridMesh : public CMeshIlluminated
{
public:
	CHeightMapGridMesh(ID3D11Device *pd3dDevice, int xStart, int zStart, int nWidth, int nLength, D3DXVECTOR3 d3dxvScale = D3DXVECTOR3(1.0f, 1.0f, 1.0f), void *pContext = NULL);
	virtual ~CHeightMapGridMesh();

	D3DXVECTOR3 GetScale() { return(m_d3dxvScale); }
	int GetWidth() { return(m_nWidth); }
	int GetLength() { return(m_nLength); }

	virtual float OnGetHeight(int x, int z, void *pContext);

private:
	int							m_nWidth;
	int							m_nLength;
	D3DXVECTOR3					m_d3dxvScale;

};

/////////////////////////////////////////////////////////////////////////////////////////////////
//

class CFbxMeshIlluminatedTextured : public CMesh
{
public:
    CFbxMeshIlluminatedTextured(ID3D11Device *pd3dDevice, FbxManager *pFbxSdkManager, char * filename, float fScaleMultiplier=1.0f);
    virtual ~CFbxMeshIlluminatedTextured();

	//HRESULT LoadFBXFromFile(ID3D11Device *pd3dDevice, FbxManager *pFbxSdkManager, char * filename, bool isAnim);

private:

	FbxScene					*m_pFbxScene;

};