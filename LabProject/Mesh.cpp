//-----------------------------------------------------------------------------
// File: CGameObject.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Mesh.h"
#include "FbxToDxTranslation.h"

CMesh::CMesh(ID3D11Device *pd3dDevice)
{
	m_nVertices = 0;
	m_nIndices = 0;
	m_nReferences = 0;
	m_nStride = NULL;
	m_nOffset = NULL;
	m_nVertexBuffers = 1;

	m_ppd3dVertexBuffers = NULL;
	m_pd3dIndexBuffer = NULL;
	m_pd3dRasterizerState = NULL;

	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_bcBoundingCube.SetMinimum(D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX));
	m_bcBoundingCube.SetMaximum(D3DXVECTOR3(+FLT_MAX, +FLT_MAX, +FLT_MAX));
}
CMesh::~CMesh()
{
	if (m_pd3dRasterizerState) m_pd3dRasterizerState->Release();
	if (m_ppd3dVertexBuffers)
	{
		for (UINT i = 0; i < m_nVertexBuffers; i++) if (m_ppd3dVertexBuffers[i]) m_ppd3dVertexBuffers[i]->Release();
		delete[] m_ppd3dVertexBuffers;
	}
	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();
	if (m_nStride) delete[] m_nStride;
	if (m_nOffset) delete[] m_nOffset;
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

//ラスタライザステートを設定します。基本的にはD3D11_CULL_BACKを使ってメッシュの後面は表現しません。
void CMesh::SetRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_BACK;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	d3dRasterizerDesc.DepthClipEnable = true;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}

//シェーダの状態を設定、Draw関数を実行します。
void CMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	if (m_ppd3dVertexBuffers) pd3dDeviceContext->IASetVertexBuffers(0, m_nVertexBuffers, m_ppd3dVertexBuffers, m_nStride, m_nOffset);
	if (m_pd3dIndexBuffer) pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	if (m_d3dPrimitiveTopology) pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	if (m_pd3dRasterizerState) pd3dDeviceContext->RSSetState(m_pd3dRasterizerState);

	if (m_pd3dIndexBuffer)
		pd3dDeviceContext->DrawIndexed(m_nIndices, 0, 0);
	else
		pd3dDeviceContext->Draw(m_nVertices, 0);
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

//頂点バッファに新たな情報を入ります。
void CMesh::AppendVertexBuffer(int nBuffers, ID3D11Buffer **pd3dBuffer, UINT *nStride, UINT *nOffset)
{
	ID3D11Buffer **ppd3dVertexBuffers = new ID3D11Buffer*[m_nVertexBuffers + nBuffers];
	//기존의 배열들 보다 하나 큰 배열을 생성하고 기존의 배열을 복사한 후 새로운 원소를 추가한다.
	UINT *pnVertexStrides = new UINT[m_nVertexBuffers + nBuffers];
	UINT *pnVertexOffsets = new UINT[m_nVertexBuffers + nBuffers];

	if (m_nVertexBuffers > 0)
	{
		for (UINT i = 0; i < m_nVertexBuffers; i++)
		{
			ppd3dVertexBuffers[i] = m_ppd3dVertexBuffers[i];
			pnVertexStrides[i] = m_nStride[i];
			pnVertexOffsets[i] = m_nOffset[i];
		}
		if (m_ppd3dVertexBuffers) delete[] m_ppd3dVertexBuffers;
		if (m_nStride) delete[] m_nStride;
		if (m_nOffset) delete[] m_nOffset;

		for (int i = 0; i < nBuffers; ++i)
		{
			ppd3dVertexBuffers[m_nVertexBuffers + i] = pd3dBuffer[i];
			pnVertexStrides[m_nVertexBuffers + i] = nStride[i];
			pnVertexOffsets[m_nVertexBuffers + i] = nOffset[i];
		}

		//기존의 정점 배열들 보다 하나 큰 배열을 생성하고 기존의 배열을 복사한 후 새로운 원소를 추가한다.

		m_nVertexBuffers += nBuffers;
		m_ppd3dVertexBuffers = ppd3dVertexBuffers;
		m_nStride = pnVertexStrides;
		m_nOffset = pnVertexOffsets;
		//if (pd3dBuffer) pd3dBuffer->AddRef();
	}
}
//頂点バッファを作って登録します。
ID3D11Buffer* CMesh::CreateVertexBuffer(ID3D11Device *pd3dDevice, int nObjects, UINT nBufferStride, void *pBufferData)
{
	ID3D11Buffer *pd3dInstanceBuffer = NULL;
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = nBufferStride * nObjects;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pBufferData;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, (pBufferData) ? &d3dBufferData : NULL, &pd3dInstanceBuffer);
	return(pd3dInstanceBuffer);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//


CMeshIlluminated::CMeshIlluminated(ID3D11Device *pd3dDevice) : CMesh(pd3dDevice)
{
}
CMeshIlluminated::~CMeshIlluminated()
{
}

//頂点のノーマルを計算します。
void CMeshIlluminated::CalculateVertexNormal(BYTE *pVertices, UINT *pIndices)
{
	switch (m_d3dPrimitiveTopology)
	{
		/*プリミティブが三角形リストである時、インデックスバッファがある場合とない場合を区別して頂点の法線ベクトルを計算します。インデックスバッファを使用していない場合は、各頂点の法線ベクトルは、その頂点が含まれている三角形の法線ベクトルで計算します。インデックスバッファを使用する場合は、各頂点の法線ベクトルは、その頂点が含まれている三角形の法線ベクトルの平均で計算します。*/
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		if (!pIndices)
			SetTriAngleListVertexNormal(pVertices);
		else
			SetAverageVertexNormal(pVertices, pIndices, (m_nIndices / 3), 3, false);
		break;
		/*プリミティブが三角形ストリップである時、各頂点の法線ベクトルは、その頂点が含まれている三角形の法線ベクトルの平均で計算します。*/
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		SetAverageVertexNormal(pVertices, pIndices, (pIndices) ? (m_nIndices - 2) : (m_nVertices - 2), 1, true);
		break;
	default:
		break;
	}
}
void CMeshIlluminated::SetTriAngleListVertexNormal(BYTE *pVertices)
{
	D3DXVECTOR3 d3dxvNormal;
	CNormalVertex *pVertex = NULL;
	/*三角形（プリミティブ）の数を求め、各三角形の法線ベクトルを計算し、三角形を構成する各頂点の法線ベクトルで指定します。*/
	int nPrimitives = m_nVertices / 3;
	for (int i = 0; i < nPrimitives; i++)
	{
		d3dxvNormal = CalculateTriAngleNormal(pVertices, (i * 3 + 0), (i * 3 + 1), (i * 3 + 2));
		pVertex = (CNormalVertex *)(pVertices + ((i * 3 + 0) * m_nStride[0]));
		pVertex->SetNormal(d3dxvNormal);
		pVertex = (CNormalVertex *)(pVertices + ((i * 3 + 1) * m_nStride[0]));
		pVertex->SetNormal(d3dxvNormal);
		pVertex = (CNormalVertex *)(pVertices + ((i * 3 + 2) * m_nStride[0]));
		pVertex->SetNormal(d3dxvNormal);
	}
}

//三角形の三頂点を使用して三角形の法線ベクトルを計算します。
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

/*プリミティブがインデックスバッファを使用する三角形のリストまたは三角形ストリップである場合、頂点の法線ベクトルは、その頂点を含む三角形の法線ベクトルの平均として計算します。*/
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
			nIndex0 = (bStrip) ? (((i % 2) == 0) ? (i*nOffset + 0) : (i*nOffset + 1)) : (i*nOffset + 0);
			if (pIndices) nIndex0 = pIndices[nIndex0];
			nIndex1 = (bStrip) ? (((i % 2) == 0) ? (i*nOffset + 1) : (i*nOffset + 0)) : (i*nOffset + 1);
			if (pIndices) nIndex1 = pIndices[nIndex1];
			nIndex2 = (pIndices) ? pIndices[i*nOffset + 2] : (i*nOffset + 2);
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
	m_nVertices = 8;
	m_nStride = new UINT[1];
	m_nStride[0] = sizeof(CNormalVertex);
	m_nOffset = new UINT[1];
	m_nOffset[0] = 0;

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

	m_ppd3dVertexBuffers[0] = CreateVertexBuffer(pd3dDevice, m_nVertices, m_nStride[0], pVertices);

	D3D11_BUFFER_DESC d3dBufferDesc;
	D3D11_SUBRESOURCE_DATA d3dBufferData;

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

CCubeMeshIlluminatedTextured::CCubeMeshIlluminatedTextured(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth, float fRepeatUV) : CMeshIlluminated(pd3dDevice)
{
	m_nVertices = 36;
	m_nStride = new UINT;
	*m_nStride = sizeof(CTexturedNormalVertex);
	m_nOffset = new UINT;
	*m_nOffset = 0;
	m_ppd3dVertexBuffers = new ID3D11Buffer*[m_nVertexBuffers];
	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	CTexturedNormalVertex pVertices[36];
	int i = 0;
	//직육면체의 한 면에 텍스쳐 전체가 맵핑되도록 텍스쳐 좌표를 설정한다.
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f * fRepeatUV, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f * fRepeatUV, 1.0f * fRepeatUV));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f * fRepeatUV, 1.0f * fRepeatUV));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 1.0f * fRepeatUV));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f * fRepeatUV, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f * fRepeatUV, 1.0f * fRepeatUV));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f * fRepeatUV, 1.0f * fRepeatUV));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 1.0f * fRepeatUV));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f * fRepeatUV, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f * fRepeatUV, 1.0f * fRepeatUV));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f * fRepeatUV, 1.0f * fRepeatUV));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 1.0f * fRepeatUV));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f * fRepeatUV, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f * fRepeatUV, 1.0f * fRepeatUV));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f * fRepeatUV, 1.0f * fRepeatUV));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 1.0f * fRepeatUV));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f * fRepeatUV, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f * fRepeatUV, 1.0f * fRepeatUV));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f * fRepeatUV, 1.0f * fRepeatUV));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 1.0f * fRepeatUV));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f * fRepeatUV, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f * fRepeatUV, 1.0f * fRepeatUV));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f * fRepeatUV, 1.0f * fRepeatUV));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, -fz), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 1.0f * fRepeatUV));

	m_bcBoundingCube.SetMinimum(D3DXVECTOR3(-fx, -fy, -fz));
	m_bcBoundingCube.SetMaximum(D3DXVECTOR3(+fx, +fy, +fz));
	//각 정점의 법선벡터를 계산한다.
	CalculateVertexNormal((BYTE *)pVertices, NULL);

	m_ppd3dVertexBuffers[0] = CreateVertexBuffer(pd3dDevice, m_nVertices, m_nStride[0], pVertices);

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
	m_nStride = new UINT[1];
	m_nStride[0] = sizeof(CTexturedVertex);
	m_nOffset = new UINT[1];
	m_nOffset[0] = 0;
	m_ppd3dVertexBuffers = new ID3D11Buffer*[m_nVertexBuffers];

	CTexturedVertex *pVertices = new CTexturedVertex[m_nVertices];
	int i = 0;
	float fx = fWidth, fy = fHeight, fz = fDepth;

	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(+fx, +fy, 0), D3DXVECTOR2(1.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(-fx, +fy, 0), D3DXVECTOR2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(-fx, -fy, 0), D3DXVECTOR2(0.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(-fx, -fy, 0), D3DXVECTOR2(0.0f, 1.0f));
	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(+fx, -fy, 0), D3DXVECTOR2(1.0f, 1.0f));
	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(+fx, +fy, 0), D3DXVECTOR2(1.0f, 0.0f));

	m_ppd3dVertexBuffers[0] = CreateVertexBuffer(pd3dDevice, m_nVertices, m_nStride[0], pVertices);

	m_bcBoundingCube.SetMinimum(D3DXVECTOR3(-fx, -fy, -fz));
	m_bcBoundingCube.SetMaximum(D3DXVECTOR3(+fx, +fy, +fz));

	SetRasterizerState(pd3dDevice);

	delete[] pVertices;
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
//FBX SDKを使ってモデルメッシュをロードします。

CFbxMeshIlluminatedTextured::CFbxMeshIlluminatedTextured(ID3D11Device *pd3dDevice, FbxManager *pFbxSdkManager, string filename, float fScaleMultiplier, bool _bHasAnimation) : CMesh(pd3dDevice)
{
	m_nStride = new UINT[1];
	m_nStride[0] = sizeof(CBoneWeightVertex);
	m_nOffset = new UINT[1];
	m_nOffset[0] = 0;
	m_ppd3dVertexBuffers = new ID3D11Buffer*[m_nVertexBuffers];

	FbxImporter* pImporter = FbxImporter::Create(pFbxSdkManager, "");
	FbxScene *pFbxScene = FbxScene::Create(pFbxSdkManager, "");

	if (!pImporter->Initialize(filename.c_str(), -1, pFbxSdkManager->GetIOSettings()))
	{
		cout << "Fbx SDK Initialize Failed" << endl;
		return;
	}
	if (!pImporter->Import(pFbxScene)){
		cout << "Fbx SDK Scene Import Failed" << endl;
		return;
	}
	pImporter->Destroy();
	m_bHasAnimation = _bHasAnimation;

	//メッシュが三角形のメッシュじゃない場合は、三角形のメッシュに変換します。
	FbxGeometryConverter lGeomConverter(pFbxSdkManager);
	lGeomConverter.Triangulate(pFbxScene, true);

	FbxNode* pFbxRootNode = pFbxScene->GetRootNode();

	vector<CBoneWeightVertex> VertexVector;
	unordered_map<int, vector<pair<UINT, float>>> ClusterIndexVector;

	//メッシュがアニメーションを持っている場合、ボーンデータを設定します。
	if (m_bHasAnimation)
	{
		vector<string> vBoneName;
		SetBoneNameIndex(pFbxRootNode, &vBoneName);
		SetBoneAtVertices(pFbxRootNode, &ClusterIndexVector, &vBoneName);
	}
	else{
		// Convert Axis System to what is used in this example, if needed
		FbxAxisSystem SceneAxisSystem = pFbxScene->GetGlobalSettings().GetAxisSystem();
		FbxAxisSystem OurAxisSystem(FbxAxisSystem::eMayaYUp);

		if (SceneAxisSystem != OurAxisSystem)
		{
			OurAxisSystem.ConvertScene(pFbxScene);
		}
	}

	//頂点情報を設定します。
	SetVertices(pFbxRootNode, &VertexVector, &ClusterIndexVector);

	m_nVertices = VertexVector.size();
	CBoneWeightVertex *pVertices = new CBoneWeightVertex[m_nVertices];

	for (int i = 0; i < m_nVertices; ++i)
	{
		pVertices[i] = VertexVector[i];
	}

	m_ppd3dVertexBuffers[0] = CreateVertexBuffer(pd3dDevice, m_nVertices, m_nStride[0], pVertices);

	SetRasterizerState(pd3dDevice);

	delete[]pVertices;
	pFbxScene->Destroy(true);
}
CFbxMeshIlluminatedTextured::~CFbxMeshIlluminatedTextured()
{
}

void CFbxMeshIlluminatedTextured::SetRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_FRONT;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	//d3dRasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	d3dRasterizerDesc.DepthClipEnable = true;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}

//ボーンのインデックスを設定する時、名前で比較するための関数です。
void CFbxMeshIlluminatedTextured::SetBoneNameIndex(FbxNode *pNode, vector<string> *pBoneName)
{
	if (pNode->GetNodeAttribute())
	{
		if (pNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton){
			pBoneName->push_back(pNode->GetName());
		}
	}
	for (int iChildIndex = 0; iChildIndex < pNode->GetChildCount(); ++iChildIndex)
	{
		SetBoneNameIndex(pNode->GetChild(iChildIndex), pBoneName);
	}
}

void CFbxMeshIlluminatedTextured::SetVertices(FbxNode *pNode, vector<CBoneWeightVertex> *pVertexVector, unordered_map<int, vector<pair<UINT, float>>> *pClusterIndexVector)
{
	if (pNode->GetNodeAttribute())
	{
		if (pNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			FbxMesh * pMesh = pNode->GetMesh();
			FbxVector4* pVertices = pMesh->GetControlPoints();

			D3DXVECTOR3 tempMin;
			D3DXVECTOR3	tempMax;
			tempMin = tempMax = D3DXVECTOR3(0, 0, 0);

			//ポリゴン数だけ頂点を読む
			for (int j = 0; j < pMesh->GetPolygonCount(); j++)
			{
				int iNumVertices = pMesh->GetPolygonSize(j);

				for (int k = 0; k < iNumVertices; k++)
				{
					int iControlPointIndex = pMesh->GetPolygonVertex(j, k);
					CBoneWeightVertex Vertex = CBoneWeightVertex();
					D3DXVECTOR3 d3dxvPos = D3DXVECTOR3((float)-pVertices[iControlPointIndex].mData[0], (float)pVertices[iControlPointIndex].mData[1], (float)pVertices[iControlPointIndex].mData[2]);
					if (!m_bHasAnimation)
					{
						D3DXMATRIX d3dxmtxPivot = GetD3DMatrix(pNode->EvaluateGlobalTransform());
						D3DXVec3TransformCoord(&d3dxvPos, &d3dxvPos, &d3dxmtxPivot);
					}

					Vertex.SetPosition(d3dxvPos);

					//ノーマル設定
					FbxVector4 Normal;
					pMesh->GetPolygonVertexNormal(j, k, Normal);
					D3DXVECTOR3 d3dxvNormal = D3DXVECTOR3((float)Normal.mData[0], (float)Normal.mData[1], (float)Normal.mData[2]);
					if (!m_bHasAnimation){
						D3DXMATRIX d3dxmtxPivot = GetD3DMatrix(pNode->EvaluateGlobalTransform());
						D3DXVec3TransformCoord(&d3dxvNormal, &d3dxvNormal, &d3dxmtxPivot);
					}
					Vertex.SetNormal(d3dxvNormal);

					//UV設定
					FbxStringList lUVSetNameList;
					pMesh->GetUVSetNames(lUVSetNameList);
					FbxVector2 textureUV;
					bool isMapped;
					pMesh->GetPolygonVertexUV(j, k, lUVSetNameList[0], textureUV, isMapped);
					Vertex.SetTexCoord(D3DXVECTOR2(textureUV[0], -textureUV[1]));

					if (m_bHasAnimation)
					{
						if (pClusterIndexVector->find(iControlPointIndex) != pClusterIndexVector->end())
						{
							for (int i = 0; i<(*pClusterIndexVector)[iControlPointIndex].size(); ++i){
								Vertex.m_iBoneIndex[i] = (*pClusterIndexVector)[iControlPointIndex][i].first;
								Vertex.m_fBoneWeight[i] = (*pClusterIndexVector)[iControlPointIndex][i].second;
							}
						}

					}

					//メッシュの最大最小点を保存
					if (Vertex.GetPosition().x > tempMax.x)
						tempMax.x = Vertex.GetPosition().x;
					if (Vertex.GetPosition().y > tempMax.y)
						tempMax.y = Vertex.GetPosition().y;
					if (Vertex.GetPosition().z > tempMax.z)
						tempMax.z = Vertex.GetPosition().z;
					if (Vertex.GetPosition().x < tempMin.x)
						tempMin.x = Vertex.GetPosition().x;
					if (Vertex.GetPosition().y < tempMin.y)
						tempMin.y = Vertex.GetPosition().y;
					if (Vertex.GetPosition().z < tempMin.z)
						tempMin.z = Vertex.GetPosition().z;
					pVertexVector->push_back(Vertex);
				}
			}

			if (m_bHasAnimation)
			{
				tempMin *= 0.015f;
				tempMax *= 0.015f;
			}

			m_bcBoundingCube.SetMinimum(tempMin);
			m_bcBoundingCube.SetMaximum(tempMax);
		}
	}
	for (int iChildIndex = 0; iChildIndex < pNode->GetChildCount(); ++iChildIndex)
	{
		SetVertices(pNode->GetChild(iChildIndex), pVertexVector, pClusterIndexVector);
	}
}
void CFbxMeshIlluminatedTextured::SetBoneAtVertices(FbxNode *pNode, unordered_map<int, vector<pair<UINT, float>>> *pClusterIndexVector, vector<string> *pBoneName)
{
	if (pNode->GetNodeAttribute())
	{
		if (pNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			FbxMesh * pMesh = pNode->GetMesh();
			int nSkinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);
			if (nSkinCount > 0)
			{
				for (int iSkinIndex = 0; iSkinIndex < nSkinCount; ++iSkinIndex)
				{
					FbxSkin *pSkin = (FbxSkin *)pMesh->GetDeformer(iSkinIndex, FbxDeformer::eSkin);
					int nClusterCount = pSkin->GetClusterCount();
					for (int iClusterIndex = 0; iClusterIndex < nClusterCount; ++iClusterIndex)
					{
						FbxCluster *pCluster = pSkin->GetCluster(iClusterIndex);

						if (!pCluster->GetLink())
							continue;

						string BoneName = pCluster->GetLink()->GetName();
						int iBoneIndex;
						for (iBoneIndex = 0; iBoneIndex < pBoneName->size(); ++iBoneIndex)
						{
							if (BoneName.compare((*pBoneName)[iBoneIndex]) == 0){
								break;
							}
						}
						int nVertexIndexCount = pCluster->GetControlPointIndicesCount();
						for (int i = 0; i < nVertexIndexCount; ++i)
						{
							//メッシュのインデックスとウェイトの保存
							int iVertexIndex = pCluster->GetControlPointIndices()[i];
							float iWeight = pCluster->GetControlPointWeights()[i];
							(*pClusterIndexVector)[iVertexIndex].push_back(make_pair(iBoneIndex, iWeight));
						}
					}
				}
			}

		}
	}

	for (int lChildIndex = 0; lChildIndex < pNode->GetChildCount(); ++lChildIndex)
	{
		SetBoneAtVertices(pNode->GetChild(lChildIndex), pClusterIndexVector, pBoneName);
	}
}


