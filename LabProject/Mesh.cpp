//-----------------------------------------------------------------------------
// File: CGameObject.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Mesh.h"
#include "FbxToDxTranslation.h"
#define RANDOM_COLOR	D3DXCOLOR((rand() * 0xFFFFFF) / RAND_MAX)

CMesh::CMesh(ID3D11Device *pd3dDevice)
{
	m_nVertices = 0;
	m_nStride = NULL;
	m_nOffset = NULL;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_nVertexBuffers = 1;
	m_nIndices = 0;

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
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_NONE;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	//d3dRasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	//d3dRasterizerDesc.DepthClipEnable = true;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}

void CMesh::Render(ID3D11DeviceContext *pd3dImmediateDeviceContext)
{
	if (m_ppd3dVertexBuffers) pd3dImmediateDeviceContext->IASetVertexBuffers(0, m_nVertexBuffers, m_ppd3dVertexBuffers, m_nStride, m_nOffset);
	if (m_pd3dIndexBuffer) pd3dImmediateDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	if (m_d3dPrimitiveTopology) pd3dImmediateDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	if (m_pd3dRasterizerState) pd3dImmediateDeviceContext->RSSetState(m_pd3dRasterizerState);

	if (m_pd3dIndexBuffer) 
		pd3dImmediateDeviceContext->DrawIndexed(m_nIndices, 0, 0);
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
		pd3dDeviceContext->DrawIndexedInstanced(m_nIndices, nInstances, 0, 0, nStartInstance);
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

void CMeshIlluminated::CalculateVertexNormal(BYTE *pVertices, UINT *pIndices)
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
D3DXVECTOR3 CMeshIlluminated::CalculateTriAngleNormal(BYTE *pVertices, UINT nIndex0, UINT nIndex1, UINT nIndex2)
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
void CMeshIlluminated::SetAverageVertexNormal(BYTE *pVertices, UINT *pIndices, int nPrimitives, int nOffset, bool bStrip)
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

/////////////////////////////////////////////////////////////////////////////////////////////////
//

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

	UINT pIndices[36];
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

/////////////////////////////////////////////////////////////////////////////////////////////////
//

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

/////////////////////////////////////////////////////////////////////////////////////////////////
//

CSkyBoxMesh::CSkyBoxMesh(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth) : CMeshTextured(pd3dDevice)
{
	m_nVertices = 6;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_nStride = new UINT[1];
	m_nStride[0] = sizeof(CTexturedVertex);
	m_nOffset = new UINT[1];
	m_nOffset[0] = 0;
	m_ppd3dVertexBuffers = new ID3D11Buffer*[m_nVertexBuffers];

	CTexturedVertex *pVertices = new CTexturedVertex[m_nVertices];
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

	delete [] pVertices;
}

CSkyBoxMesh::~CSkyBoxMesh()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//

CMeshTextured::CMeshTextured(ID3D11Device *pd3dDevice) : CMesh(pd3dDevice)
{
	m_pd3dTexCoordBuffer = NULL;
}

CMeshTextured::~CMeshTextured()
{
	if (m_pd3dTexCoordBuffer) m_pd3dTexCoordBuffer->Release();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//

CHeightMap::CHeightMap(LPCTSTR pFileName, int nWidth, int nLength, D3DXVECTOR3 d3dxvScale)
{
	m_nWidth = nWidth;
	m_nLength = nLength;
	m_d3dxvScale = d3dxvScale;

	BYTE *pHeightMapImage = new BYTE[m_nWidth * m_nLength];

	HANDLE hFile = ::CreateFile(pFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);
	DWORD dwBytesRead;
	::ReadFile(hFile, pHeightMapImage, (m_nWidth * m_nLength), &dwBytesRead, NULL);
	::CloseHandle(hFile);

	m_pHeightMapImage = new BYTE[m_nWidth * m_nLength];

	for (int y = 0; y < m_nLength; y++)
	{
		for (int x = 0; x < m_nWidth; x++)
		{
			m_pHeightMapImage[x + ((m_nLength - 1 - y)*m_nWidth)] = pHeightMapImage[x + (y*m_nWidth)];
			//m_pHeightMapImage[x + y*m_nWidth] = pHeightMapImage[x + (y*m_nWidth)];
		}
	}

	delete[] pHeightMapImage;
}

CHeightMap::~CHeightMap()
{
	if (m_pHeightMapImage) delete[] m_pHeightMapImage;
}

D3DXVECTOR3 CHeightMap::GetHeightMapNormal(int x, int z)
{
	if ((x < 0.0f) || (z < 0.0f) || (x >= m_nWidth) || (z >= m_nLength)) return(D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	int nHeightMapIndex = x + (z * m_nWidth);
	int xHeightMapAdd, zHeightMapAdd;
	xHeightMapAdd = (x < (m_nWidth - 1)) ? 1 : -1;
	zHeightMapAdd = (z < (m_nLength - 1)) ? m_nWidth : -m_nWidth;
	float y1 = (float)m_pHeightMapImage[nHeightMapIndex] * m_d3dxvScale.y;
	float y2 = (float)m_pHeightMapImage[nHeightMapIndex + xHeightMapAdd] * m_d3dxvScale.y;
	float y3 = (float)m_pHeightMapImage[nHeightMapIndex + zHeightMapAdd] * m_d3dxvScale.y;
	D3DXVECTOR3 vEdge1 = D3DXVECTOR3(0.0f, y3 - y1, m_d3dxvScale.z);
	D3DXVECTOR3 vEdge2 = D3DXVECTOR3(m_d3dxvScale.x, y2 - y1, 0.0f);
	D3DXVECTOR3 vNormal;
	D3DXVec3Cross(&vNormal, &vEdge1, &vEdge2);
	D3DXVec3Normalize(&vNormal, &vNormal);
	return(vNormal);
}

#define _WITH_APPROXIMATE_OPPOSITE_CORNER

float CHeightMap::GetHeight(float fx, float fz, bool bReverseQuad)
{
	fx = fx / m_d3dxvScale.x;
	fz = fz / m_d3dxvScale.z;
	if ((fx < 0.0f) || (fz < 0.0f) || (fx >= m_nWidth) || (fz >= m_nLength)) 
		return(0.0f);

	int x = (int)fx;
	int z = (int)fz;
	float fxPercent = fx - x;
	float fzPercent = fz - z;
	/*
	float fBottomLeft = (float)m_pHeightMapImage[x + (z*m_nWidth)];
	float fBottomRight = (float)m_pHeightMapImage[(x + 1) + (z*m_nWidth)];
	float fTopLeft = (float)m_pHeightMapImage[x + ((z + 1)*m_nWidth)];
	float fTopRight = (float)m_pHeightMapImage[(x + 1) + ((z + 1)*m_nWidth)];
	#ifdef _WITH_APPROXIMATE_OPPOSITE_CORNER
	if (bReverseQuad)
	{
	if (fzPercent >= fxPercent)
	fBottomRight = fBottomLeft + (fTopRight - fTopLeft);
	else
	fTopLeft = fTopRight + (fBottomLeft - fBottomRight);
	}
	else
	{
	if (fzPercent < (1.0f - fxPercent))
	fTopRight = fTopLeft + (fBottomRight - fBottomLeft);
	else
	fBottomLeft = fTopLeft + (fBottomRight - fTopRight);
	}
	#endif
	float fTopHeight = fTopLeft * (1 - fxPercent) + fTopRight * fxPercent;
	float fBottomHeight = fBottomLeft * (1 - fxPercent) + fBottomRight * fxPercent;
	float fHeight = fBottomHeight * (1 - fzPercent) + fTopHeight * fzPercent;
	*/
	float fTopLeft = m_pHeightMapImage[x+(z*m_nWidth)];
	float fTopRight = m_pHeightMapImage[(x+1)+(z*m_nWidth)];
	float fBottomLeft = m_pHeightMapImage[x+((z+1)*m_nWidth)];
	float fBottomRight = m_pHeightMapImage[(x+1)+((z+1)*m_nWidth)];
	//return(fHeight * m_d3dxvScale.y);
	//return fHeight;
#ifdef _WITH_APPROXIMATE_OPPOSITE_CORNER
	if (bReverseQuad)
	{
		if (fxPercent <= fzPercent)
			fTopRight = fTopLeft + (fBottomRight - fBottomLeft);        
		else
			fBottomLeft = fTopLeft + (fBottomRight - fTopRight);
	} 
	else
	{
		if (fxPercent < (1.0f - fzPercent)) 
			fBottomRight = fBottomLeft + (fTopRight - fTopLeft);        
		else
			fTopLeft = fTopRight + (fBottomLeft - fBottomRight);
	} 
#endif

	float fTopHeight = fTopLeft * (1 - fxPercent) + fTopRight * fxPercent;
	float fBottomHeight = fBottomLeft * (1 - fxPercent) + fBottomRight * fxPercent;
	float fHeight = fTopHeight * (1 - fzPercent) + fBottomHeight * fzPercent;
	return(fHeight * m_d3dxvScale.y);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//

CHeightMapGridMesh::CHeightMapGridMesh(ID3D11Device *pd3dDevice, int xStart, int zStart, int nWidth, int nLength, D3DXVECTOR3 d3dxvScale, void *pContext) : CMeshIlluminated(pd3dDevice)
{
	m_nVertices = nWidth * nLength;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	m_nStride = new UINT;
	*m_nStride = sizeof(CTexturedNormalVertex);

	m_nOffset = new UINT;
	*m_nOffset = 0;

	m_ppd3dVertexBuffers = new ID3D11Buffer*[m_nVertexBuffers];
	CTexturedNormalVertex *pVertices = new CTexturedNormalVertex[m_nVertices];

	m_nWidth = nWidth;
	m_nLength = nLength;
	m_d3dxvScale = d3dxvScale;

	CHeightMap *pHeightMap = (CHeightMap *)pContext;
	int cxHeightMap = pHeightMap->GetHeightMapWidth();
	int czHeightMap = pHeightMap->GetHeightMapLength();
	float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX;
	for (int i = 0, z = zStart; z < (zStart + nLength); ++z)
	{
		for (int x = xStart; x < (xStart + nWidth); ++x, ++i)
		{
			fHeight = OnGetHeight(x, z, pContext);
			//pVertices[i] = CTexturedNormalVertex(D3DXVECTOR3((x*m_d3dxvScale.x), fHeight, (z*m_d3dxvScale.z)), D3DXVECTOR3(0,0,0), D3DXVECTOR2(float(x)/float(cxHeightMap-1), float(czHeightMap-1-z)/float(czHeightMap-1)));
			pVertices[i] = CTexturedNormalVertex(D3DXVECTOR3((x*m_d3dxvScale.x), fHeight, (z*m_d3dxvScale.z)), pHeightMap->GetHeightMapNormal(x, z), D3DXVECTOR2(float(x)/float(cxHeightMap-1), float(czHeightMap-1-z)/float(czHeightMap-1)));
			if (fHeight < fMinHeight) fMinHeight = fHeight;
			if (fHeight > fMaxHeight) fMaxHeight = fHeight;
		}
	}


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


	m_nIndices = ((nWidth * 2)*(nLength - 1)) + ((nLength - 1) - 1);
	UINT *pIndices  = new UINT[m_nIndices];
	for (int j = 0, z = 0; z < nLength - 1; ++z)
	{
		if ((z % 2) == 0)
		{
			for (int x = 0; x < nWidth; ++x)
			{
				if ((x == 0) && (z > 0)) pIndices[j++] = (UINT)(x + (z * nWidth));
				pIndices[j++] = (UINT)(x + (z * nWidth));
				pIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
		else
		{
			for (int x = nWidth - 1; x >= 0; --x)
			{
				if (x == (nWidth - 1)) pIndices[j++] = (UINT)(x + (z * nWidth));
				pIndices[j++] = (UINT)(x + (z * nWidth));
				pIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
	}

	//CalculateVertexNormal((BYTE *)pVertices, pIndices);



	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(UINT) * m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pIndices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);



	m_bcBoundingCube.SetMinimum(D3DXVECTOR3(xStart*m_d3dxvScale.x, fMinHeight, zStart*m_d3dxvScale.z));
	m_bcBoundingCube.SetMaximum(D3DXVECTOR3((xStart+nWidth)*m_d3dxvScale.x, fMaxHeight, (zStart+nLength)*m_d3dxvScale.z));

	SetRasterizerState(pd3dDevice);

	delete []pVertices;
	delete []pIndices;
}

CHeightMapGridMesh::~CHeightMapGridMesh()
{
}

float CHeightMapGridMesh::OnGetHeight(int x, int z, void *pContext)
{
	CHeightMap *pHeightMap = (CHeightMap *)pContext;
	BYTE *pHeightMapImage = pHeightMap->GetHeightMapImage();
	D3DXVECTOR3 d3dxvScale = pHeightMap->GetScale();
	int nWidth = pHeightMap->GetHeightMapWidth();
	float fHeight = pHeightMapImage[x + (z*nWidth)] * d3dxvScale.y;
	return(fHeight);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//

/*
CFbxMeshIlluminatedTextured::CFbxMeshIlluminatedTextured(ID3D11Device *pd3dDevice) : CMesh(pd3dDevice)
{

}*/
CFbxMeshIlluminatedTextured::~CFbxMeshIlluminatedTextured(){
	//m_pFbxScene->Destroy(
}

//HRESULT CFbxMeshIlluminatedTextured::LoadFBXFromFile(ID3D11Device *pd3dDevice, FbxManager *pFbxSdkManager, char * filename, bool isAnim){
CFbxMeshIlluminatedTextured::CFbxMeshIlluminatedTextured(ID3D11Device *pd3dDevice, FbxManager *pFbxSdkManager, char * filename, float fScaleMultiplier) : CMesh(pd3dDevice)
{
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_nStride = new UINT[1];
	m_nStride[0] = sizeof(CTexturedNormalVertex);
	m_nOffset = new UINT[1];
	m_nOffset[0] = 0;
	m_ppd3dVertexBuffers = new ID3D11Buffer*[m_nVertexBuffers];

	FbxImporter* pImporter = FbxImporter::Create(pFbxSdkManager,""); // 임포트 생성
	m_pFbxScene = FbxScene::Create(pFbxSdkManager,""); // fbx 씬 생성

	if(!pImporter->Initialize( filename , -1, pFbxSdkManager->GetIOSettings()))
	{
		cout << "Fbx SDK Initialize Failed" << endl;
		return;
	}
	if(!pImporter->Import(m_pFbxScene)){
		cout << "Fbx SDK Scene Import Failed" << endl;
		return;
	}
	
	
	// Convert mesh, NURBS and patch into triangle mesh
	FbxGeometryConverter lGeomConverter(pFbxSdkManager);
	lGeomConverter.Triangulate(m_pFbxScene, true);
	
	// Convert Axis System to what is used in this example, if needed
	
	FbxSystemUnit ScaleMultiplier = FbxSystemUnit(1,fScaleMultiplier);
	ScaleMultiplier.ConvertScene(m_pFbxScene);
	
	/*
	//FbxAxisSystem OurAxisSystem(FbxAxisSystem(FbxAxisSystem::EUpVector::eYAxis,FbxAxisSystem::EFrontVector::,FbxAxisSystem::ECoordSystem::eLeftHanded));
	FbxAxisSystem OurAxisSystem(FbxAxisSystem::DirectX);
	OurAxisSystem.ConvertScene(m_pFbxScene);
	*/
	pImporter->Destroy();

	// 씬의 루트 노드 얻어옴
	FbxNode* pFbxRootNode = m_pFbxScene->GetRootNode();
	
	vector<CTexturedNormalVertex> VertexVector;
	// 루트 노드가 있으면 데이터 얻어옴
	if(pFbxRootNode)
	{
		// 루트 노드의 자식을 참조하여 얻어옴
		for(int i = 0; i < pFbxRootNode->GetChildCount(); ++i)
		{
			FbxNode* pFbxChildNode = pFbxRootNode->GetChild(i);
			if(pFbxChildNode->GetNodeAttribute() == NULL)
				continue;

			FbxNodeAttribute::EType AttributeType = pFbxChildNode->GetNodeAttribute()->GetAttributeType();

			
			if(AttributeType != FbxNodeAttribute::eMesh)
				continue;
			FbxMesh * pMesh;
			pMesh = (FbxMesh*) pFbxChildNode->GetNodeAttribute();

			FbxVector4* pVertices = pMesh->GetControlPoints();
			

			// UV를 얻기위한 것들 
			FbxStringList lUVSetNameList;
			pMesh->GetUVSetNames(lUVSetNameList);

			// Get the list of all the animation stack.
			//m_pFbxScene->FillAnimStackNameArray(m_AnimStackNameArray);
			// Get the list of pose in the scene
			//FillPoseArray(m_pFbxScene, m_PoseArray);

			D3DXVECTOR3 tempMin ;
			D3DXVECTOR3	tempMax;
			tempMin = tempMax = D3DXVECTOR3(0,0,0); // 메쉬의 최대최소점 저장

			D3DXMATRIX d3dxmtxGlobal = GetD3DMatrix(pFbxChildNode->EvaluateGlobalTransform());
			/*
			cout << "11 : " << d3dxmtxGlobal._11 << " ";
			cout << "12 : " << d3dxmtxGlobal._12 << " ";
			cout << "13 : " << d3dxmtxGlobal._13 << " ";
			cout << "14 : " << d3dxmtxGlobal._14 << endl;
			cout << "21 : " << d3dxmtxGlobal._21 << " ";
			cout << "22 : " << d3dxmtxGlobal._22 << " ";
			cout << "23 : " << d3dxmtxGlobal._23 << " ";
			cout << "24 : " << d3dxmtxGlobal._24 << endl;
			cout << "31 : " << d3dxmtxGlobal._31 << " ";
			cout << "32 : " << d3dxmtxGlobal._32 << " ";
			cout << "33 : " << d3dxmtxGlobal._33 << " ";
			cout << "34 : " << d3dxmtxGlobal._34 << endl;
			cout << "41 : " << d3dxmtxGlobal._41 << " ";
			cout << "42 : " << d3dxmtxGlobal._42 << " ";
			cout << "43 : " << d3dxmtxGlobal._43 << " ";
			cout << "44 : " << d3dxmtxGlobal._44 << endl;*/
			//D3DXMatrixInverse(&d3dxmtxGlobal,NULL,&d3dxmtxGlobal);
			// 폴리곤 숫자만큼 버텍스를 읽어옴
			for (int j = 0; j < pMesh->GetPolygonCount(); j++) // j가 폴리곤 인덱스
			{
				int iNumVertices = pMesh->GetPolygonSize(j); 

				for (int k = 0; k < iNumVertices; k++) // k가 포인트 인덱스
				{
					int iControlPointIndex = pMesh->GetPolygonVertex(j, k);
					CTexturedNormalVertex Vertex;

					
					//D3DXMATRIX d3dxmtxLocal = GetD3DMatrix(pFbxChildNode->EvaluateLocalTransform());
					D3DXVECTOR3 d3dxvPos = D3DXVECTOR3((float)pVertices[iControlPointIndex].mData[0],(float)pVertices[iControlPointIndex].mData[1],(float)pVertices[iControlPointIndex].mData[2]);
					D3DXVec3TransformCoord(&d3dxvPos,&d3dxvPos,&d3dxmtxGlobal);

					// 노말벡터 설정
					FbxVector4 Normal;
					pMesh->GetPolygonVertexNormal(j, k, Normal );

					FbxVector2 tex;
					bool isMapped;
					pMesh->GetPolygonVertexUV(j, k, lUVSetNameList[0], tex, isMapped);
					
					Vertex.SetPosition(d3dxvPos);
					Vertex.SetNormal(D3DXVECTOR3((float)Normal.mData[0],(float)Normal.mData[1],(float)Normal.mData[2]));
					Vertex.SetTexCoord(D3DXVECTOR2(tex[0], -tex[1]));
					
					Vertex.SetPosition(D3DXVECTOR3(-Vertex.GetPosition().x,Vertex.GetPosition().y,Vertex.GetPosition().z));
					
					if( Vertex.GetPosition().x > tempMax.x )
						tempMax.x = Vertex.GetPosition().x;
					if( Vertex.GetPosition().y > tempMax.y )
						tempMax.y = Vertex.GetPosition().y;
					if( Vertex.GetPosition().z > tempMax.z )
						tempMax.z = Vertex.GetPosition().z;

					if( Vertex.GetPosition().x < tempMin.x )
						tempMin.x = Vertex.GetPosition().x;
					if( Vertex.GetPosition().y < tempMin.y )
						tempMin.y = Vertex.GetPosition().y;
					if( Vertex.GetPosition().z < tempMin.z )
						tempMin.z = Vertex.GetPosition().z;

					VertexVector.push_back( Vertex );
				}
			}
			m_bcBoundingCube.SetMinimum(tempMin);
			m_bcBoundingCube.SetMaximum(tempMax);

		}
		


	}
	/*
	//push_back cluster index and weight
	if(isAnim)
	{
	int BoneIndex = 0;
	SetBoneAtVertecis( pFbxRootNode, BoneIndex , VertexVector);

	}*/

	m_nVertices = VertexVector.size();
	CTexturedNormalVertex *pVertices = new CTexturedNormalVertex[m_nVertices];
	for ( int i = 0;  i < m_nVertices; ++i )
	{
		pVertices[i] = VertexVector[i];
	}

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
	
	SetRasterizerState(pd3dDevice);

	/*
	if ( isAnim )
	{
	SetCurrentAnimStack(0);
	// Initialize the frame period.
	m_pFbxScene->GetGlobalSettings().SetTimeMode(  FbxTime::eFrames100  ) ;
	m_FrameTime.SetTime(0, 0, 0, 1, 0, m_pFbxScene->GetGlobalSettings().GetTimeMode());
	}*/
	//return m_nVertices;

}
