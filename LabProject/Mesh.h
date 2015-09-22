//------------------------------------------------------- ----------------------
// File: Mesh.h
//-----------------------------------------------------------------------------

#pragma once
#include "Vertex.h"
#include "Timer.h"


typedef struct tagAABB
{
//바운딩 박스의 최소점과 최대점을 나타내는 벡터이다.
	D3DXVECTOR3 m_d3dxvMinimum;
	D3DXVECTOR3 m_d3dxvMaximum;

//두 개의 바운딩 박스를 합한다.
	void Union(D3DXVECTOR3& d3dxvMinimum, D3DXVECTOR3& d3dxvMaximum);
//바운딩 박스의 8개의 꼭지점을 행렬로 변환하고 최소점과 최대점을 다시 계산한다.
	void Transform(D3DXMATRIX *pd3dxmtxTransform);
} AABB;




/////////////////////////////////////////////////////////////////////////////////////////////////
//
class CMesh
{
public:
    CMesh(ID3D11Device *pd3dDevice);
    virtual ~CMesh();

	int								m_nReferences;
	void AddRef();
	void Release();

	D3D11_PRIMITIVE_TOPOLOGY		m_d3dPrimitiveTopology;

	ID3D11Buffer					*m_pd3dVertexBuffer;
	UINT							m_nVertices;
	UINT							m_nStride;
	UINT							m_nOffset;

	ID3D11Buffer					*m_pd3dIndexBuffer;
	UINT							m_nIndices;
	UINT							m_nStartIndex;
	int								m_nBaseVertex;

	ID3D11RasterizerState			*m_pd3dRasterizerState;

	AABB m_bcBoundingCube;

	virtual void SetRasterizerState(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dImmediateDeviceContext);
};

/////////////////////////////////////////////////////////////////////////////////////////////////
//
class CCubeMesh : public CMesh
{
public:
    CCubeMesh(ID3D11Device *pd3dDevice, float fWidth=2.0f, float fHeight=2.0f, float fDepth=2.0f, D3DXCOLOR d3dxColor=D3DXCOLOR(1.0f,1.0f,0.0f,0.0f));
    virtual ~CCubeMesh();

	virtual void SetRasterizerState(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dImmediateDeviceContext);
};

/////////////////////////////////////////////////////////////////////////////////////////////////
//


class CMeshIlluminated : public CMesh
{
public:
    CMeshIlluminated(ID3D11Device *pd3dDevice);
    virtual ~CMeshIlluminated();

public:
//정점이 포함된 삼각형의 법선벡터를 계산하는 함수이다.
	D3DXVECTOR3 CalculateTriAngleNormal(BYTE *pVertices, USHORT nIndex0, USHORT nIndex1, USHORT nIndex2);
	void SetTriAngleListVertexNormal(BYTE *pVertices);
//정점의 법선벡터의 평균을 계산하는 함수이다.
	void SetAverageVertexNormal(BYTE *pVertices, WORD *pIndices, int nPrimitives, int nOffset, bool bStrip);
	void CalculateVertexNormal(BYTE *pVertices, WORD *pIndices);

	virtual void Render(ID3D11DeviceContext *pd3dImmediateDeviceContext);
};
class CCubeMeshIlluminated : public CMeshIlluminated
{
public:
	CCubeMeshIlluminated(ID3D11Device *pd3dDevice, float fWidth=2.0f, float fHeight=2.0f, float fDepth=2.0f);
	virtual ~CCubeMeshIlluminated();

	virtual void SetRasterizerState(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dImmediateDeviceContext);
};

class CTexturedCubeMesh : public CMesh
{
public:
	CTexturedCubeMesh(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth);
	virtual ~CTexturedCubeMesh();

	virtual void SetRasterizerState(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
};
class CCubeMeshIlluminatedTextured : public CMeshIlluminated
{
public:
    CCubeMeshIlluminatedTextured(ID3D11Device *pd3dDevice, float fWidth=2.0f,       float fHeight=2.0f, float fDepth=2.0f);
    virtual ~CCubeMeshIlluminatedTextured();

    virtual void SetRasterizerState(ID3D11Device *pd3dDevice);
    virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
};
