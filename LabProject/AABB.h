#pragma once

class AABB
{
private:
//바운딩 박스의 최소점과 최대점을 나타내는 벡터이다.
	D3DXVECTOR3 m_d3dxvMinimum;
	D3DXVECTOR3 m_d3dxvMaximum;
public:
	AABB() { m_d3dxvMinimum = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX); m_d3dxvMaximum = D3DXVECTOR3(+FLT_MAX, +FLT_MAX, +FLT_MAX); };
	AABB(D3DXVECTOR3 d3dxvMinimum, D3DXVECTOR3 d3dxvMaximum) { m_d3dxvMinimum = d3dxvMinimum; m_d3dxvMaximum = d3dxvMaximum; }
//두 개의 바운딩 박스를 합한다.
	void Union(D3DXVECTOR3& d3dxvMinimum, D3DXVECTOR3& d3dxvMaximum);
	void Union(AABB *pAABB);
//바운딩 박스의 8개의 꼭지점을 행렬로 변환하고 최소점과 최대점을 다시 계산한다.
	void Transform(D3DXMATRIX *pd3dxmtxTransform);

	GET_SET_FUNC_IMPL(D3DXVECTOR3,Minimum,m_d3dxvMinimum);
	GET_SET_FUNC_IMPL(D3DXVECTOR3,Maximum,m_d3dxvMaximum);
};