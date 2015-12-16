#pragma once

//光の計算に使うマテリアル構造体です。
struct MATERIAL
{
	D3DXCOLOR m_d3dxcAmbient;
	D3DXCOLOR m_d3dxcDiffuse;
	D3DXCOLOR m_d3dxcSpecular; //(r,g,b,a=power)
	D3DXCOLOR m_d3dxcEmissive;
};

class CMaterial
{
public:
	CMaterial(D3DXCOLOR _d3dxcAmbient, D3DXCOLOR _d3dxcDiffuse, D3DXCOLOR _d3dxcSpecular, D3DXCOLOR _d3dxcEmissive);
	virtual ~CMaterial();


	void AddRef();
	void Release();

	GET_SET_FUNC_IMPL(MATERIAL, Material, m_Material);

private:
	int		 m_nReferences;
	MATERIAL m_Material;
};