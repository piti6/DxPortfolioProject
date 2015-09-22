#pragma once

class CTexture
{
public:
	CTexture(int nTextures);
	virtual ~CTexture();

	int m_nReferences;
	void AddRef();
	void Release();

	void SetTexture(int nIndex, ID3D11ShaderResourceView *pd3dsrvTexture, ID3D11SamplerState *pd3dSamplerState);

	ID3D11ShaderResourceView **m_ppd3dsrvTextures;    
	ID3D11SamplerState **m_ppd3dSamplerStates;
	int m_nTextures;
};