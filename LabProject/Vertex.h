#pragma once

class CVertex
{
protected:
    D3DXVECTOR3						m_d3dxvPosition;		
public:
	CVertex() { m_d3dxvPosition = D3DXVECTOR3(0, 0, 0); }
	CVertex(D3DXVECTOR3 d3dxvPosition) { m_d3dxvPosition = d3dxvPosition; }
	~CVertex() { }

	GET_SET_FUNC_IMPL(D3DXVECTOR3,Position,m_d3dxvPosition);
};

class CDiffusedVertex : public CVertex
{
private:
    D3DXCOLOR						m_d3dxcDiffuse;		
public:
    CDiffusedVertex(float x, float y, float z, D3DXCOLOR d3dxcDiffuse) { m_d3dxvPosition = D3DXVECTOR3(x, y, z); m_d3dxcDiffuse = d3dxcDiffuse; }
    CDiffusedVertex(D3DXVECTOR3 d3dxvPosition, D3DXCOLOR d3dxcDiffuse) { m_d3dxvPosition = d3dxvPosition; m_d3dxcDiffuse = d3dxcDiffuse; }
    CDiffusedVertex() { m_d3dxvPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f); m_d3dxcDiffuse = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f); }
	~CDiffusedVertex() { }

	GET_SET_FUNC_IMPL(D3DXCOLOR,Color,m_d3dxcDiffuse);
};

class CNormalVertex : public CVertex
{
private:
	D3DXVECTOR3						m_d3dxvNormal;
	//조명의 영향을 계산하기 위하여 법선벡터가 필요하다.
public:
	CNormalVertex(float x, float y, float z, float nx, float ny, float nz) { m_d3dxvPosition = D3DXVECTOR3(x, y, z); m_d3dxvNormal = D3DXVECTOR3(nx, ny, nz); }
	CNormalVertex(D3DXVECTOR3 d3dxvPosition, D3DXVECTOR3 d3dxvNormal) { m_d3dxvPosition = d3dxvPosition; m_d3dxvNormal = d3dxvNormal; }
	CNormalVertex() { m_d3dxvPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f); m_d3dxvNormal = D3DXVECTOR3(0.0f, 0.0f, 0.0f); }
	~CNormalVertex() { }

	GET_SET_FUNC_IMPL(D3DXVECTOR3,Normal,m_d3dxvNormal);
};
class CTexturedVertex : public CVertex
{
private:
    D3DXVECTOR2						m_d3dxvTexCoord;
public:
    CTexturedVertex() { m_d3dxvPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f); m_d3dxvTexCoord = D3DXVECTOR2(0.0f, 0.0f); }
    CTexturedVertex(float x, float y, float z, float u, float v) { m_d3dxvPosition = D3DXVECTOR3(x, y, z); m_d3dxvTexCoord = D3DXVECTOR2(u, v); }
    CTexturedVertex(D3DXVECTOR3 d3dxvPosition, D3DXVECTOR2 d3dxvTexture) { m_d3dxvPosition = d3dxvPosition; m_d3dxvTexCoord = d3dxvTexture; }
    ~CTexturedVertex() { }

	GET_SET_FUNC_IMPL(D3DXVECTOR2,TexCoord,m_d3dxvTexCoord);
};

class CTexturedNormalVertex : public CVertex
{
private:
    D3DXVECTOR3						m_d3dxvNormal;   
    D3DXVECTOR2						m_d3dxvTexCoord;
public:
    CTexturedNormalVertex(float x, float y, float z, float nx, float ny, float nz, float u, float v) { m_d3dxvPosition = D3DXVECTOR3(x, y, z); m_d3dxvNormal = D3DXVECTOR3(nx, ny, nz); m_d3dxvTexCoord = D3DXVECTOR2(u, v); }
    CTexturedNormalVertex(D3DXVECTOR3 d3dxvPosition, D3DXVECTOR3 d3dxvNormal, D3DXVECTOR2 d3dxvTexCoord) { m_d3dxvPosition = d3dxvPosition; m_d3dxvNormal = d3dxvNormal; m_d3dxvTexCoord = d3dxvTexCoord; }
    CTexturedNormalVertex() { m_d3dxvPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f); m_d3dxvNormal = D3DXVECTOR3(0.0f, 0.0f, 0.0f); m_d3dxvTexCoord = D3DXVECTOR2(0.0f, 0.0f); }
    ~CTexturedNormalVertex() { }

	GET_SET_FUNC_IMPL(D3DXVECTOR3,Normal,m_d3dxvNormal);
	GET_SET_FUNC_IMPL(D3DXVECTOR2,TexCoord,m_d3dxvTexCoord);
};

class CBoneWeightVertex : public CTexturedNormalVertex
{
	
	
public:
	UINT							m_iBoneIndex[MAX_WEIGHT];
	float							m_iBoneWeight[MAX_WEIGHT];
	

    CBoneWeightVertex(float x, float y, float z, float nx, float ny, float nz, float u, float v) : CTexturedNormalVertex(x,y,z,nx,ny,nz,u,v) {for(int i=0;i<MAX_WEIGHT;++i) { m_iBoneIndex[i] = NULL_IDX; m_iBoneWeight[i] = 0;} }
    CBoneWeightVertex(D3DXVECTOR3 d3dxvPosition, D3DXVECTOR3 d3dxvNormal, D3DXVECTOR2 d3dxvTexCoord) : CTexturedNormalVertex(d3dxvPosition,d3dxvNormal,d3dxvTexCoord) {for(int i=0;i<MAX_WEIGHT;++i) { m_iBoneIndex[i] = NULL_IDX; m_iBoneWeight[i] = 0;} }
	CBoneWeightVertex() : CTexturedNormalVertex() {	for(int i=0;i<MAX_WEIGHT;++i) { m_iBoneIndex[i] = NULL_IDX; m_iBoneWeight[i] = 0;}}
    ~CBoneWeightVertex() { }
};