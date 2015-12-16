#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////
//ポジションだけを持っている頂点クラスです。
class CVertex
{
public:
	CVertex() { m_d3dxvPosition = D3DXVECTOR3(0, 0, 0); }
	CVertex(D3DXVECTOR3 d3dxvPosition) { m_d3dxvPosition = d3dxvPosition; }
	~CVertex() { }

	GET_SET_FUNC_IMPL(D3DXVECTOR3, Position, m_d3dxvPosition);

protected:
	D3DXVECTOR3						m_d3dxvPosition;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//ポジションとカラーを持っている頂点クラスです。
class CDiffusedVertex : public CVertex
{
public:
	CDiffusedVertex(float x, float y, float z, D3DXCOLOR d3dxcDiffuse) { m_d3dxvPosition = D3DXVECTOR3(x, y, z); m_d3dxcDiffuse = d3dxcDiffuse; }
	CDiffusedVertex(D3DXVECTOR3 d3dxvPosition, D3DXCOLOR d3dxcDiffuse) { m_d3dxvPosition = d3dxvPosition; m_d3dxcDiffuse = d3dxcDiffuse; }
	CDiffusedVertex() { m_d3dxvPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f); m_d3dxcDiffuse = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f); }
	~CDiffusedVertex() { }

	GET_SET_FUNC_IMPL(D3DXCOLOR, Color, m_d3dxcDiffuse);

private:
	D3DXCOLOR						m_d3dxcDiffuse;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//ポジションとノーマルを持っている頂点クラスです。

class CNormalVertex : public CVertex
{
public:
	CNormalVertex(float x, float y, float z, float nx, float ny, float nz) { m_d3dxvPosition = D3DXVECTOR3(x, y, z); m_d3dxvNormal = D3DXVECTOR3(nx, ny, nz); }
	CNormalVertex(D3DXVECTOR3 d3dxvPosition, D3DXVECTOR3 d3dxvNormal) { m_d3dxvPosition = d3dxvPosition; m_d3dxvNormal = d3dxvNormal; }
	CNormalVertex() { m_d3dxvPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f); m_d3dxvNormal = D3DXVECTOR3(0.0f, 0.0f, 0.0f); }
	~CNormalVertex() { }

	GET_SET_FUNC_IMPL(D3DXVECTOR3, Normal, m_d3dxvNormal);

private:
	D3DXVECTOR3						m_d3dxvNormal;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//ポジションとテクスチャＵＶを持っている頂点クラスです。

class CTexturedVertex : public CVertex
{
public:
	CTexturedVertex() { m_d3dxvPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f); m_d3dxvTexCoord = D3DXVECTOR2(0.0f, 0.0f); }
	CTexturedVertex(float x, float y, float z, float u, float v) { m_d3dxvPosition = D3DXVECTOR3(x, y, z); m_d3dxvTexCoord = D3DXVECTOR2(u, v); }
	CTexturedVertex(D3DXVECTOR3 d3dxvPosition, D3DXVECTOR2 d3dxvTexture) { m_d3dxvPosition = d3dxvPosition; m_d3dxvTexCoord = d3dxvTexture; }
	~CTexturedVertex() { }

	GET_SET_FUNC_IMPL(D3DXVECTOR2, TexCoord, m_d3dxvTexCoord);

private:
	D3DXVECTOR2						m_d3dxvTexCoord;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//ポジションとノーマル、テクスチャＵＶを持っている頂点クラスです。

class CTexturedNormalVertex : public CVertex
{
public:
	CTexturedNormalVertex(float x, float y, float z, float nx, float ny, float nz, float u, float v) { m_d3dxvPosition = D3DXVECTOR3(x, y, z); m_d3dxvNormal = D3DXVECTOR3(nx, ny, nz); m_d3dxvTexCoord = D3DXVECTOR2(u, v); }
	CTexturedNormalVertex(D3DXVECTOR3 d3dxvPosition, D3DXVECTOR3 d3dxvNormal, D3DXVECTOR2 d3dxvTexCoord) { m_d3dxvPosition = d3dxvPosition; m_d3dxvNormal = d3dxvNormal; m_d3dxvTexCoord = d3dxvTexCoord; }
	CTexturedNormalVertex() { m_d3dxvPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f); m_d3dxvNormal = D3DXVECTOR3(0.0f, 0.0f, 0.0f); m_d3dxvTexCoord = D3DXVECTOR2(0.0f, 0.0f); }
	~CTexturedNormalVertex() { }

	GET_SET_FUNC_IMPL(D3DXVECTOR3, Normal, m_d3dxvNormal);
	GET_SET_FUNC_IMPL(D3DXVECTOR2, TexCoord, m_d3dxvTexCoord);

private:
	D3DXVECTOR3						m_d3dxvNormal;
	D3DXVECTOR2						m_d3dxvTexCoord;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//ポジションとノーマル、テクスチャＵＶ、ボーンのインデックスとウェイトを持っている頂点クラスです。

class CBoneWeightVertex : public CTexturedNormalVertex
{
public:
	CBoneWeightVertex(float x, float y, float z, float nx, float ny, float nz, float u, float v) : CTexturedNormalVertex(x, y, z, nx, ny, nz, u, v) { for (int i = 0; i < MAX_WEIGHT; ++i) { m_iBoneIndex[i] = NULL_IDX; m_fBoneWeight[i] = 0; } }
	CBoneWeightVertex(D3DXVECTOR3 d3dxvPosition, D3DXVECTOR3 d3dxvNormal, D3DXVECTOR2 d3dxvTexCoord) : CTexturedNormalVertex(d3dxvPosition, d3dxvNormal, d3dxvTexCoord) { for (int i = 0; i < MAX_WEIGHT; ++i) { m_iBoneIndex[i] = NULL_IDX; m_fBoneWeight[i] = 0; } }
	CBoneWeightVertex() : CTexturedNormalVertex() { for (int i = 0; i < MAX_WEIGHT; ++i) { m_iBoneIndex[i] = NULL_IDX; m_fBoneWeight[i] = 0; } }
	~CBoneWeightVertex() { }

	UINT							m_iBoneIndex[MAX_WEIGHT];
	float							m_fBoneWeight[MAX_WEIGHT];
};