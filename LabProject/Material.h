#pragma once

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
	CMaterial();
	virtual ~CMaterial();

	
	void AddRef();
	void Release();

	int		 m_nReferences;
	MATERIAL m_Material;
};