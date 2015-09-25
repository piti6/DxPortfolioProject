#include "stdafx.h"
#include "Material.h"

CMaterial::CMaterial()
{
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
