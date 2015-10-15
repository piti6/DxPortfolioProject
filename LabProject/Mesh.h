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

	int								m_nReferences;
	void AddRef();
	void Release();

	D3D11_PRIMITIVE_TOPOLOGY		m_d3dPrimitiveTopology;

	ID3D11Buffer					**m_ppd3dVertexBuffers;
	UINT							m_nVertices;
	UINT							*m_nStride;
	UINT							*m_nOffset;
	UINT							m_nVertexBuffers;
	ID3D11Buffer					*m_pd3dIndexBuffer;
	UINT							m_nIndices;
	UINT							m_nStartIndex;
	int								m_nBaseVertex;

	ID3D11RasterizerState			*m_pd3dRasterizerState;
	
	AABB m_bcBoundingCube;

	virtual void SetRasterizerState(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dImmediateDeviceContext);
	virtual void RenderInstanced(ID3D11DeviceContext *pd3dDeviceContext, int nInstances, int nStartInstance);
	virtual void AppendVertexBuffer(ID3D11Buffer *pd3dBuffer, UINT nStride, UINT nOffset);
	//virtual void AppendVertexBuffer(int nBuffers,ID3D11Buffer **pd3dBuffer, UINT *nStride, UINT *nOffset);
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

class CCubeMeshIlluminatedTextured : public CMeshIlluminated
{
public:
    CCubeMeshIlluminatedTextured(ID3D11Device *pd3dDevice, float fWidth=2.0f,       float fHeight=2.0f, float fDepth=2.0f);
    virtual ~CCubeMeshIlluminatedTextured();

    virtual void SetRasterizerState(ID3D11Device *pd3dDevice);
    virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
};
