#pragma once
#include "Animation.h"
class CAnimationInstancing
{
public:
	CAnimationInstancing();
	~CAnimationInstancing();

	/////////////// Load Animation ///////////////

	void LoadAnimationFromFile(FbxManager *pFbxSdkManager, char * filename, string AnimationName, bool isLooping);
	void SetBoneNameIndex(FbxScene *pFbxScene, FbxNode *pNode, vector<string> *pBoneName);
	void SetAnimationData(FbxScene *pFbxScene, FbxNode *pNode, string AnimationName, vector<string> *pBoneName);
	void SetBoneMatrixVectorAtTime(FbxNode *pNode, string AnimationName, FbxTime& pTime, vector<string> *pBoneName, int index);
	D3DXMATRIX GetClusterMatrix(FbxAMatrix & pGlobalPosition, FbxMesh * pMesh, FbxCluster * pCluster, FbxTime& pTime);

	/////////////// Animation To Texture ///////////////

	void CreateAnimationTexture(ID3D11Device* pd3dDevice);

	/////////////// UpdateAnimation ///////////////

	void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext);

	UINT GetCurrentOffset(string AnimationName, float fCurrentTime);
	UINT GetAnimationOffset(string AnimationName);
	UINT GetFrameOffset(string AnimationName, float fCurrentTime);
	int GetIndexAtCurrentTime(string AnimationName, float fCurrentTime);
	UINT GetTextureWidth(){ return m_TextureWidth; }

	CAnimationList				m_vAnimationList;
public:
	ID3D11ShaderResourceView	*m_pd3dAnimationTextureResourceView;
	ID3D11Texture2D				*m_pd3dAnimationTexture;


	UINT						m_TextureWidth;

};