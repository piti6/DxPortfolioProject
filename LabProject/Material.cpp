#include "stdafx.h"
#include "Material.h"

CMaterial::CMaterial(D3DXCOLOR _d3dxcAmbient, D3DXCOLOR _d3dxcDiffuse, D3DXCOLOR _d3dxcSpecular, D3DXCOLOR _d3dxcEmissive)
{
	m_Material.m_d3dxcAmbient = _d3dxcAmbient;
	m_Material.m_d3dxcDiffuse = _d3dxcDiffuse;
	m_Material.m_d3dxcEmissive = _d3dxcEmissive;
	m_Material.m_d3dxcSpecular = _d3dxcSpecular;
	m_nReferences = 0;
}
CMaterial::~CMaterial()
{
}

void CMaterial::AddRef()
{
	m_nReferences++;
}
void CMaterial::Release()
{
	if (m_nReferences > 0) m_nReferences--;
	if (m_nReferences == 0) delete this;
}
