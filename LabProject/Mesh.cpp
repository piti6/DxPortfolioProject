//-----------------------------------------------------------------------------
// File: CGameObject.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Mesh.h"

#define RANDOM_COLOR	D3DXCOLOR((rand() * 0xFFFFFF) / RAND_MAX)

CMesh::CMesh(ID3D11Device *pd3dDevice)
{
	m_nVertices = 0;
    m_nStride = NULL;
    m_nOffset = NULL;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_nVertexBuffers = 1;
	m_nIndices = 0;
	m_nStartIndex = 0;
	m_nBaseVertex = 0;

	m_pd3dIndexBuffer = NULL;
	m_ppd3dVertexBuffers = NULL;

	m_pd3dRasterizerState = NULL;

	m_nReferences = 0;

	m_bcBoundingCube.SetMinimum(D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX));

	m_bcBoundingCube.SetMaximum(D3DXVECTOR3(+FLT_MAX, +FLT_MAX, +FLT_MAX));
}

CMesh::~CMesh()
{
    if (m_pd3dRasterizerState) m_pd3dRasterizerState->Release();
    if (m_ppd3dVertexBuffers)
	{
		for (UINT i = 0; i < m_nVertexBuffers; i++) if (m_ppd3dVertexBuffers[i]) m_ppd3dVertexBuffers[i]->Release();
		delete [] m_ppd3dVertexBuffers;
	}
    if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();
	if (m_nStride) delete [] m_nStride;
	if (m_nOffset) delete [] m_nOffset;
}

void CMesh::AddRef() 
{ 
	m_nReferences++; 
}

void CMesh::Release() 
{ 
	m_nReferences--; 
	if (m_nReferences == 0) delete this;
}

void CMesh::SetRasterizerState(ID3D11Device *pd3dDevice)
{
}

void CMesh::Render(ID3D11DeviceContext *pd3dImmediateDeviceContext)
{
    if (m_ppd3dVertexBuffers) pd3dImmediateDeviceContext->IASetVertexBuffers(0, m_nVertexBuffers, m_ppd3dVertexBuffers, m_nStride, m_nOffset);
    if (m_pd3dIndexBuffer) pd3dImmediateDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    if (m_d3dPrimitiveTopology) pd3dImmediateDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	if (m_pd3dRasterizerState) pd3dImmediateDeviceContext->RSSetState(m_pd3dRasterizerState);

	if (m_pd3dIndexBuffer) 
		pd3dImmediateDeviceContext->DrawIndexed(m_nIndices, m_nStartIndex, m_nBaseVertex);
	else
		pd3dImmediateDeviceContext->Draw(m_nVertices, 0);
}
void CMesh::RenderInstanced(ID3D11DeviceContext *pd3dDeviceContext, int nInstances, int nStartInstance)
{
	if (m_ppd3dVertexBuffers) pd3dDeviceContext->IASetVertexBuffers(0, m_nVertexBuffers, m_ppd3dVertexBuffers, m_nStride, m_nOffset);
	if (m_pd3dIndexBuffer) pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	if (m_d3dPrimitiveTopology) pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	if (m_pd3dRasterizerState) pd3dDeviceContext->RSSetState(m_pd3dRasterizerState);

	if (m_pd3dIndexBuffer)
		pd3dDeviceContext->DrawIndexedInstanced(m_nIndices, nInstances, m_nStartIndex, m_nBaseVertex, nStartInstance);
	else
		pd3dDeviceContext->DrawInstanced(m_nVertices, nInstances, 0, nStartInstance);
}

void CMesh::AppendVertexBuffer(int nBuffers,ID3D11Buffer **pd3dBuffer, UINT *nStride, UINT *nOffset)
{
	ID3D11Buffer **ppd3dVertexBuffers = new ID3D11Buffer*[m_nVertexBuffers+nBuffers];
	//기존의 배열들 보다 하나 큰 배열을 생성하고 기존의 배열을 복사한 후 새로운 원소를 추가한다.
	UINT *pnVertexStrides = new UINT[m_nVertexBuffers+nBuffers];
	UINT *pnVertexOffsets = new UINT[m_nVertexBuffers+nBuffers];

	if(m_nVertexBuffers >0)
	{
		for (UINT i = 0; i < m_nVertexBuffers; i++) 
		{
			ppd3dVertexBuffers[i] = m_ppd3dVertexBuffers[i];
			pnVertexStrides[i] = m_nStride[i];
			pnVertexOffsets[i] = m_nOffset[i];
		}
		if(m_ppd3dVertexBuffers) delete [] m_ppd3dVertexBuffers;
		if(m_nStride) delete [] m_nStride;
		if(m_nOffset) delete [] m_nOffset;

		for(int i=0; i<nBuffers; ++i)
		{
			ppd3dVertexBuffers[m_nVertexBuffers+i] = pd3dBuffer[i];
			pnVertexStrides[m_nVertexBuffers+i] = nStride[i];
			pnVertexOffsets[m_nVertexBuffers+i] = nOffset[i];
		}

		//기존의 정점 배열들 보다 하나 큰 배열을 생성하고 기존의 배열을 복사한 후 새로운 원소를 추가한다.

		m_nVertexBuffers += nBuffers;
		m_ppd3dVertexBuffers = ppd3dVertexBuffers;
		m_nStride = pnVertexStrides;
		m_nOffset = pnVertexOffsets;


		//if (pd3dBuffer) pd3dBuffer->AddRef();

	}
	
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//


CMeshIlluminated::CMeshIlluminated(ID3D11Device *pd3dDevice) : CMesh(pd3dDevice)
{
}

CMeshIlluminated::~CMeshIlluminated()
{
}
void CMeshIlluminated::CalculateVertexNormal(BYTE *pVertices, WORD *pIndices)
{
	switch (m_d3dPrimitiveTopology)
	{
/*프리미티브가 삼각형 리스트일 때 인덱스 버퍼가 있는 경우와 없는 경우를 구분하여 정점의 법선 벡터를 계산한다. 인덱스 버퍼를 사용하지 않는 경우 각 정점의 법선 벡터는 그 정점이 포함된 삼각형의 법선 벡터로 계산한다. 인덱스 버퍼를 사용하는 경우 각 정점의 법선 벡터는 그 정점이 포함된 삼각형들의 법선 벡터의 평균으로(더하여) 계산한다.*/
		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
			if (!pIndices)
				SetTriAngleListVertexNormal(pVertices);
			else
				SetAverageVertexNormal(pVertices, pIndices, (m_nIndices / 3), 3, false);
			break;
/*프리미티브가 삼각형 스트립일 때 각 정점의 법선 벡터는 그 정점이 포함된 삼각형들의 법선 벡터의 평균으로(더하여) 계산한다.*/
		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
			SetAverageVertexNormal(pVertices, pIndices, (pIndices) ? (m_nIndices - 2) : (m_nVertices - 2), 1, true);
			break;
		default:
			break;
	}
}
//인덱스 버퍼를 사용하지 않는 삼각형 리스트에 대하여 정점의 법선 벡터를 계산한다.
void CMeshIlluminated::SetTriAngleListVertexNormal(BYTE *pVertices)
{
	D3DXVECTOR3 d3dxvNormal;
	CNormalVertex *pVertex = NULL;
/*삼각형(프리미티브)의 개수를 구하고 각 삼각형의 법선 벡터를 계산하고 삼각형을 구성하는 각 정점의 법선 벡터로 지정한다.*/
	int nPrimitives = m_nVertices / 3;
	for (int i = 0; i < nPrimitives; i++) 
	{
		d3dxvNormal = CalculateTriAngleNormal(pVertices, (i*3+0), (i*3+1), (i*3+2));
		pVertex = (CNormalVertex *)(pVertices + ((i*3+0) * m_nStride[0]));
		pVertex->SetNormal(d3dxvNormal);
		pVertex = (CNormalVertex *)(pVertices + ((i*3+1) * m_nStride[0]));
		pVertex->SetNormal(d3dxvNormal);
		pVertex = (CNormalVertex *)(pVertices + ((i*3+2) * m_nStride[0]));
		pVertex->SetNormal(d3dxvNormal);
	}
}
//삼각형의 세 정점을 사용하여 삼각형의 법선 벡터를 계산한다.
D3DXVECTOR3 CMeshIlluminated::CalculateTriAngleNormal(BYTE *pVertices, USHORT nIndex0, USHORT nIndex1, USHORT nIndex2)
{
	D3DXVECTOR3 d3dxvNormal(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 d3dxvP0 = *((D3DXVECTOR3 *)(pVertices + (m_nStride[0] * nIndex0)));
	D3DXVECTOR3 d3dxvP1 = *((D3DXVECTOR3 *)(pVertices + (m_nStride[0] * nIndex1)));
	D3DXVECTOR3 d3dxvP2 = *((D3DXVECTOR3 *)(pVertices + (m_nStride[0] * nIndex2)));
	D3DXVECTOR3 d3dxvEdge1 = d3dxvP1 - d3dxvP0;
	D3DXVECTOR3 d3dxvEdge2 = d3dxvP2 - d3dxvP0;
	D3DXVec3Cross(&d3dxvNormal, &d3dxvEdge1, &d3dxvEdge2);
	D3DXVec3Normalize(&d3dxvNormal, &d3dxvNormal);
	return(d3dxvNormal);
}
/*프리미티브가 인덱스 버퍼를 사용하는 삼각형 리스트 또는 삼각형 스트립인 경우 정점의 법선 벡터는 그 정점을 포함하는 삼각형의 법선 벡터들의 평균으로 계산한다.*/
void CMeshIlluminated::SetAverageVertexNormal(BYTE *pVertices, WORD *pIndices, int nPrimitives, int nOffset, bool bStrip)
{
	D3DXVECTOR3 d3dxvSumOfNormal(0.0f, 0.0f, 0.0f);
	CNormalVertex *pVertex = NULL;
	USHORT nIndex0, nIndex1, nIndex2;

	for (UINT j = 0; j < m_nVertices; j++)
	{
		d3dxvSumOfNormal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		for (int i = 0; i < nPrimitives; i++) 
		{
			nIndex0 = (bStrip) ? (((i%2)==0) ? (i*nOffset+0) : (i*nOffset+1)) : (i*nOffset+0);
			if (pIndices) nIndex0 = pIndices[nIndex0];
			nIndex1 = (bStrip) ? (((i%2)==0) ? (i*nOffset+1) : (i*nOffset+0)) : (i*nOffset+1);
			if (pIndices) nIndex1 = pIndices[nIndex1];
			nIndex2 = (pIndices) ? pIndices[i*nOffset+2] : (i*nOffset+2);
			if ((nIndex0 == j) || (nIndex1 == j) || (nIndex2 == j)) d3dxvSumOfNormal += CalculateTriAngleNormal(pVertices, nIndex0, nIndex1, nIndex2);
		}
		D3DXVec3Normalize(&d3dxvSumOfNormal, &d3dxvSumOfNormal);
		pVertex = (CNormalVertex *)(pVertices + (j * m_nStride[0]));
		pVertex->SetNormal(d3dxvSumOfNormal);
	}
}

CCubeMeshIlluminated::CCubeMeshIlluminated(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth) : CMeshIlluminated(pd3dDevice)
{
	int i = 0;
	m_nVertices = 8;
	m_nStride = new UINT[1];
	m_nStride[0] = sizeof(CNormalVertex);
	m_nOffset = new UINT[1];
	m_nOffset[0] = 0;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	CNormalVertex pVertices[8];
	pVertices[0] = CNormalVertex(D3DXVECTOR3(-fx, +fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	pVertices[1] = CNormalVertex(D3DXVECTOR3(+fx, +fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	pVertices[2] = CNormalVertex(D3DXVECTOR3(+fx, +fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	pVertices[3] = CNormalVertex(D3DXVECTOR3(-fx, +fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	pVertices[4] = CNormalVertex(D3DXVECTOR3(-fx, -fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	pVertices[5] = CNormalVertex(D3DXVECTOR3(+fx, -fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	pVertices[6] = CNormalVertex(D3DXVECTOR3(+fx, -fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	pVertices[7] = CNormalVertex(D3DXVECTOR3(-fx, -fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	m_bcBoundingCube.SetMinimum(D3DXVECTOR3(-fx, -fy, -fz));
	m_bcBoundingCube.SetMaximum(D3DXVECTOR3(+fx, +fy, +fz));
	m_nIndices = 36;

	WORD pIndices[36];
	pIndices[0] = 3; pIndices[1] = 1; pIndices[2] = 0;
	pIndices[3] = 2; pIndices[4] = 1; pIndices[5] = 3;
	pIndices[6] = 0; pIndices[7] = 5; pIndices[8] = 4;
	pIndices[9] = 1; pIndices[10] = 5; pIndices[11] = 0;
	pIndices[12] = 3; pIndices[13] = 4; pIndices[14] = 7;
	pIndices[15] = 0; pIndices[16] = 4; pIndices[17] = 3;
	pIndices[18] = 1; pIndices[19] = 6; pIndices[20] = 5;
	pIndices[21] = 2; pIndices[22] = 6; pIndices[23] = 1;
	pIndices[24] = 2; pIndices[25] = 7; pIndices[26] = 6;
	pIndices[27] = 3; pIndices[28] = 7; pIndices[29] = 2;
	pIndices[30] = 6; pIndices[31] = 4; pIndices[32] = 5;
	pIndices[33] = 7; pIndices[34] = 4; pIndices[35] = 6;

//법선 벡터를 계산한다.
	CalculateVertexNormal((BYTE *)pVertices, pIndices);

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(CNormalVertex) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_ppd3dVertexBuffers[0]);

	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(WORD) * m_nIndices;        
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pIndices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);

	SetRasterizerState(pd3dDevice);
}

CCubeMeshIlluminated::~CCubeMeshIlluminated()
{
}
void CCubeMeshIlluminated::SetRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_BACK;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}

void CCubeMeshIlluminated::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CMeshIlluminated::Render(pd3dDeviceContext);
}
void CMeshIlluminated::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CMesh::Render(pd3dDeviceContext);
}

CCubeMeshIlluminatedTextured::CCubeMeshIlluminatedTextured(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth) : CMeshIlluminated(pd3dDevice)
{
	m_nVertices = 36;
	m_nStride = new UINT;
	*m_nStride = sizeof(CTexturedNormalVertex);
	m_nOffset = new UINT;
	*m_nOffset = 0;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_ppd3dVertexBuffers = new ID3D11Buffer*[m_nVertexBuffers];
	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	CTexturedNormalVertex pVertices[36];
	int i = 0;
//직육면체의 한 면에 텍스쳐 전체가 맵핑되도록 텍스쳐 좌표를 설정한다.
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 1.0f));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 1.0f));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 1.0f));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 1.0f));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 1.0f));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 1.0f));
	m_bcBoundingCube.SetMinimum(D3DXVECTOR3(-fx, -fy, -fz));
	m_bcBoundingCube.SetMaximum(D3DXVECTOR3(+fx, +fy, +fz));
//각 정점의 법선벡터를 계산한다.
	CalculateVertexNormal((BYTE *)pVertices, NULL);

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = m_nStride[0] * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_ppd3dVertexBuffers[0]);

	SetRasterizerState(pd3dDevice);
}

CCubeMeshIlluminatedTextured::~CCubeMeshIlluminatedTextured()
{
}

void CCubeMeshIlluminatedTextured::SetRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_BACK;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}

void CCubeMeshIlluminatedTextured::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CMeshIlluminated::Render(pd3dDeviceContext);
}

CSkyBoxMesh::CSkyBoxMesh(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth) : CMeshTextured(pd3dDevice)
{
	m_nVertices = 6;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_nStride = new UINT[1];
	m_nStride[0] = sizeof(CTexturedVertex);
	m_nOffset = new UINT[1];
	m_nOffset[0] = 0;
	m_ppd3dVertexBuffers = new ID3D11Buffer*[m_nVertexBuffers];
	CTexturedVertex pVertices[36];
	int i = 0;
	float fx = fWidth, fy = fHeight , fz = fDepth;
	
	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(+fx, +fy, 0), D3DXVECTOR2(1.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(-fx, +fy, 0), D3DXVECTOR2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(-fx, -fy, 0), D3DXVECTOR2(0.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(-fx, -fy, 0), D3DXVECTOR2(0.0f, 1.0f));
	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(+fx, -fy, 0), D3DXVECTOR2(1.0f, 1.0f));
	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(+fx, +fy, 0), D3DXVECTOR2(1.0f, 0.0f));

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = m_nStride[0] * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_ppd3dVertexBuffers[0]);

	m_bcBoundingCube.SetMinimum(D3DXVECTOR3(-fx, -fy, -fz));
	m_bcBoundingCube.SetMaximum(D3DXVECTOR3(+fx, +fy, +fz));
	
	SetRasterizerState(pd3dDevice);
}

CSkyBoxMesh::~CSkyBoxMesh()
{
}
void CSkyBoxMesh::SetRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_NONE;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}
void CSkyBoxMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	if (m_ppd3dVertexBuffers) pd3dDeviceContext->IASetVertexBuffers(0, m_nVertexBuffers, m_ppd3dVertexBuffers, m_nStride, m_nOffset);
    if (m_pd3dIndexBuffer) pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    if (m_d3dPrimitiveTopology) pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	if (m_pd3dRasterizerState) pd3dDeviceContext->RSSetState(m_pd3dRasterizerState);
	/*
    for (int i = 0; i < 6; i++)
    {
		//m_pSkyboxTexture->UpdateTextureShaderVariable(pd3dDeviceContext, i, 0);
		pd3dDeviceContext->DrawIndexed(4, 0, i * 4);
		
    } */
	pd3dDeviceContext->Draw(m_nVertices,0);
}

CMeshTextured::CMeshTextured(ID3D11Device *pd3dDevice) : CMesh(pd3dDevice)
{
	m_pd3dTexCoordBuffer = NULL;
}

CMeshTextured::~CMeshTextured()
{
	if (m_pd3dTexCoordBuffer) m_pd3dTexCoordBuffer->Release();
}
