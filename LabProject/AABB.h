#pragma once

//衝突判定はPhysx SDKから担当していますので、こちらのクラスは衝突体の範囲を決定する時に利用します。
class AABB
{
private:
	D3DXVECTOR3 m_d3dxvMinimum;
	D3DXVECTOR3 m_d3dxvMaximum;
public:
	AABB() { m_d3dxvMinimum = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX); m_d3dxvMaximum = D3DXVECTOR3(+FLT_MAX, +FLT_MAX, +FLT_MAX); };
	AABB(D3DXVECTOR3 d3dxvMinimum, D3DXVECTOR3 d3dxvMaximum) { m_d3dxvMinimum = d3dxvMinimum; m_d3dxvMaximum = d3dxvMaximum; }
	
	void Transform(D3DXMATRIX *pd3dxmtxTransform);
	
	GET_SET_FUNC_IMPL(D3DXVECTOR3, Minimum, m_d3dxvMinimum);
	GET_SET_FUNC_IMPL(D3DXVECTOR3, Maximum, m_d3dxvMaximum);
};