#pragma once
#include "Animation.h"
class CAnimationController
{
public:
	CAnimationController();
	~CAnimationController(){}

/////////////// Load Animation ///////////////

	void LoadAnimationFromFile(FbxManager *pFbxSdkManager, char * filename, string AnimationName,bool isLooping);
	void SetBoneNameIndex(FbxScene *pFbxScene,FbxNode *pNode,string AnimationName);
	void SetAnimationData(FbxScene *pFbxScene,FbxNode *pNode,string AnimationName);
	void SetBoneMatrixVectorAtTime(FbxNode * pNode, string AnimationName, FbxPose* pPose, FbxTime& pTime);
	D3DXMATRIX GetClusterMatrix(FbxAMatrix & pGlobalPosition ,FbxMesh * pMesh, FbxCluster * pCluster, FbxTime& pTime, FbxPose* pPose);

/////////////// Controller Part ///////////////

	void UpdateTime(float fElapsedTime);

	void Play(string _AnimationName);
	void Stop();
	void Pause();

	float						m_fCurrentAnimTime;
	bool						m_bIsPlaying;


	string						m_CurrentPlayingAnimationName;
	
	vector<string>				m_vBoneName;
	CAnimationList				m_vAnimationList;

	
};