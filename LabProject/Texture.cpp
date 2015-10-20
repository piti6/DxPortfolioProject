#include "stdafx.h"
#include "Texture.h"

CTexture::CTexture(int nTextures)
{
	m_nReferences = 0;
	m_nTextures = nTextures;
	m_ppd3dsrvTextures = new ID3D11ShaderResourceView*[m_nTextures];
	for (int i = 0; i < m_nTextures; i++) m_ppd3dsrvTextures[i] = NULL;
	m_ppd3dSamplerStates = new ID3D11SamplerState*[m_nTextures];
	for (int i = 0; i < m_nTextures; i++) m_ppd3dSamplerStates[i] = NULL;
}

CTexture::~CTexture()
{
	if (m_ppd3dsrvTextures) delete [] m_ppd3dsrvTextures;
	if (m_ppd3dSamplerStates) delete [] m_ppd3dSamplerStates;
}

void CTexture::AddRef() 
{ 
	m_nReferences++; 
}

void CTexture::Release() 
{ 
	if (m_nReferences > 0) m_nReferences--; 
	for (int i = 0; i < m_nTextures; i++)
	{
		if (m_ppd3dsrvTextures[i]) m_ppd3dsrvTextures[i]->Release();
		if (m_ppd3dSamplerStates[i]) m_ppd3dSamplerStates[i]->Release();
	}
	if (m_nReferences == 0) delete this;
}

void CTexture::SetTexture(int nIndex, ID3D11ShaderResourceView *pd3dsrvTexture, ID3D11SamplerState *pd3dSamplerState)
{
	if (m_ppd3dsrvTextures[nIndex]) m_ppd3dsrvTextures[nIndex]->Release();
	if (m_ppd3dSamplerStates[nIndex]) m_ppd3dSamplerStates[nIndex]->Release();
	m_ppd3dsrvTextures[nIndex] = pd3dsrvTexture;
	m_ppd3dSamplerStates[nIndex] = pd3dSamplerState;
	if (m_ppd3dsrvTextures[nIndex]) m_ppd3dsrvTextures[nIndex]->AddRef();
	if (m_ppd3dSamplerStates[nIndex]) m_ppd3dSamplerStates[nIndex]->AddRef();
}