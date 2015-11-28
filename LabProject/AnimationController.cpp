#include "stdafx.h"
#include "AnimationController.h"
#include "FbxToDxTranslation.h"


void PrintFbxMatrix(FbxAMatrix a){
	for(int i=0;i<4;++i)
	{
		for(int j=0;j<4;++j)
		{
			cout <<"Data [" << i << "][" << j << "] : " << a.mData[i][j] << endl;
		}
		cout << endl;
	}
}

CAnimationController::CAnimationController()
{
	m_fCurrentAnimTime = 0;
}

void CAnimationController::LoadAnimationFromFile(FbxManager *pFbxSdkManager, char* filename, string AnimationName,bool isLooping)
{
	if(m_vAnimationList.m_Animation.find(AnimationName) != m_vAnimationList.m_Animation.end()){
		cout << "Same Animation Name Already Exist!" << endl;
		return;
	}

	FbxImporter* pImporter = FbxImporter::Create(pFbxSdkManager,""); // 임포트 생성
	FbxScene* pFbxScene = FbxScene::Create(pFbxSdkManager,""); // fbx 씬 생성

	if(!pImporter->Initialize( filename , -1, pFbxSdkManager->GetIOSettings()))
	{
		cout << "Fbx SDK Initialize Failed" << endl;
		return;
	}
	if(!pImporter->Import(pFbxScene)){
		cout << "Fbx SDK Scene Import Failed" << endl;
		return;
	}
	pImporter->Destroy();

	if(pFbxScene->GetSrcObjectCount<FbxAnimStack>()<=0){
		cout << "There's No Animation At " << filename << endl;
		return;
	}
	
	// 씬의 루트 노드 얻어옴
	FbxNode* pFbxRootNode = pFbxScene->GetRootNode();
	m_vAnimationList.m_Animation[AnimationName] = CAnimation(isLooping);
	SetBoneNameIndex(pFbxScene,pFbxRootNode,AnimationName);
	SetAnimationData(pFbxScene,pFbxRootNode,AnimationName);
	pFbxScene->Destroy(true);
}

void CAnimationController::SetBoneNameIndex(FbxScene *pFbxScene,FbxNode *pNode,string AnimationName){
	if (pNode->GetNodeAttribute())
	{
		if( pNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton){
			FbxAMatrix FbxmtxTemp;
			FbxmtxTemp.SetIdentity();
			m_vBoneName.push_back(pNode->GetName());
		}
	}
	for(int iChildIndex=0; iChildIndex<pNode->GetChildCount(); ++iChildIndex)
	{
		SetBoneNameIndex(pFbxScene,pNode->GetChild(iChildIndex),AnimationName);
	}
}
void CAnimationController::SetAnimationData(FbxScene *pFbxScene,FbxNode *pNode,string AnimationName){

	FbxAnimStack *pFbxAnimStack = pFbxScene->GetSrcObject<FbxAnimStack>();
	FbxAnimLayer *pFbxAnimLayer = pFbxAnimStack->GetMember<FbxAnimLayer>();
	m_vAnimationList.m_Animation[AnimationName].m_fLength = pFbxAnimStack->GetLocalTimeSpan().GetDuration().GetMilliSeconds();
	int nFrameCount = (int) m_vAnimationList.m_Animation[AnimationName].m_fLength/10;
	FbxTime Time;

	for(int i=0; i<nFrameCount; ++i)
	{
		FbxAMatrix FbxmtxParents;
		Time.SetMilliSeconds(i*10);
		m_vAnimationList.m_Animation[AnimationName].m_vAnimation.m_vBoneContainer[Time.GetMilliSeconds()].m_vBoneList.resize(m_vBoneName.size());
		SetBoneMatrixVectorAtTime(pNode,AnimationName, Time);
	}
}
void CAnimationController::UpdateTime(float fElapsedTime)
{
	if(m_vAnimationList.m_Animation[m_CurrentPlayingAnimationName].m_bIsLooping)
	{
		if(m_vAnimationList.m_Animation[m_CurrentPlayingAnimationName].m_fLength/1000 < m_fCurrentAnimTime)
			m_fCurrentAnimTime = 0;
	}
	m_fCurrentAnimTime += fElapsedTime;

}

void CAnimationController::Play(string _AnimationName)
{
	if(m_vAnimationList.m_Animation.find(_AnimationName) == m_vAnimationList.m_Animation.end())
	{
		cout << "Animation Name "<<_AnimationName<<" Not Found!" << endl;
		return;
	}

	m_CurrentPlayingAnimationName = _AnimationName;
	m_bIsPlaying = true;
}

void CAnimationController::Stop()
{
	m_bIsPlaying = false;
	m_fCurrentAnimTime = 0;
}

void CAnimationController::Pause()
{
	m_bIsPlaying = false;
}
void CAnimationController::SetBoneMatrixVectorAtTime(FbxNode * pNode, string AnimationName,FbxTime& pTime)
{
	FbxAMatrix pParentGlobalPosition;
	FbxAMatrix lGlobalPosition = pNode->EvaluateGlobalTransform(pTime);
	FbxAMatrix lGeometryOffset = GetGeometry(pNode);
	FbxAMatrix lGlobalOffPosition = lGlobalPosition * lGeometryOffset;

	FbxNodeAttribute* Attr = pNode->GetNodeAttribute();
	if(Attr)
	{
		if (Attr->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			FbxMesh * pMesh = pNode->GetMesh();

			int nSkinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);
			if (nSkinCount>0)
			{
				for ( int iSkinIndex=0; iSkinIndex<nSkinCount; ++iSkinIndex)
				{
					FbxSkin *pSkin = (FbxSkin *)pMesh->GetDeformer(iSkinIndex, FbxDeformer::eSkin);
					FbxSkin::EType lSkinningType = pSkin->GetSkinningType();
					int nClusterCount = pSkin->GetClusterCount();
					for ( int iClusterIndex=0; iClusterIndex<nClusterCount; ++iClusterIndex)
					{
						FbxCluster* pCluster = pSkin->GetCluster(iClusterIndex);
						if (!pCluster->GetLink())
							continue;

						string BoneName = pCluster->GetLink()->GetName();
						int iBoneIndex;
						for(iBoneIndex=0;iBoneIndex<m_vBoneName.size();++iBoneIndex)
						{
							if(BoneName.compare(m_vBoneName[iBoneIndex]) == 0){
								break;
							}
						}
						
						D3DXMATRIX d3dxmtxBone = GetClusterMatrix(lGlobalOffPosition, pMesh, pCluster, pTime);
						/*
						D3DXMATRIX mtxRotate;
						D3DXMatrixRotationYawPitchRoll(&mtxRotate, (float)D3DXToRadian(0), (float)D3DXToRadian(-90), (float)D3DXToRadian(0));
						d3dxmtxBone = d3dxmtxBone * mtxRotate;*/
						m_vAnimationList.m_Animation[AnimationName].m_vAnimation.m_vBoneContainer[pTime.GetMilliSeconds()].m_vBoneList[iBoneIndex] = d3dxmtxBone;

					}
				}
			}
		}
	}
	for (int lChildIndex = 0; lChildIndex < pNode->GetChildCount(); ++lChildIndex)
	{
		SetBoneMatrixVectorAtTime(pNode->GetChild(lChildIndex), AnimationName, pTime);
	}
}



D3DXMATRIX CAnimationController::GetClusterMatrix(FbxAMatrix & pGlobalPosition ,FbxMesh * pMesh, FbxCluster * pCluster, FbxTime& pTime)
{

	FbxAMatrix lReferenceGlobalInitPosition;
	FbxAMatrix lReferenceGlobalCurrentPosition;

	FbxAMatrix lClusterGlobalInitPosition;
	FbxAMatrix lClusterGlobalCurrentPosition;

	FbxAMatrix lClusterRelativeInitPosition;
	FbxAMatrix lClusterRelativeCurrentPositionInverse;

	FbxAMatrix lReferenceGeometry;

	D3DXPLANE p(1.0f, 0.0f, 0.0f, 0.0f );

	FbxAMatrix reflection;
	D3DXMatrixReflect_Fixed(&reflection, &p);

	pCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
	
	lReferenceGlobalInitPosition = reflection * lReferenceGlobalInitPosition;

	/////////////////////////////////////////////////////////////////////////////////////////
	lReferenceGlobalCurrentPosition = pGlobalPosition;
	// Multiply lReferenceGlobalInitPosition by Geometric Transformation
	lReferenceGeometry = GetGeometry(pMesh->GetNode());
	lReferenceGeometry = reflection * lReferenceGeometry;
	/////////////////////////////////////////////////////////////////////////////////////////

	lReferenceGlobalInitPosition *= lReferenceGeometry;

	// Get the link initial global position and the link current global position.
	pCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);
	lClusterGlobalInitPosition = reflection * lClusterGlobalInitPosition;
	/////////////////////////////////////////////////////////////////////////////////////////

	lClusterGlobalCurrentPosition = pCluster->GetLink()->EvaluateGlobalTransform(pTime);
	lClusterGlobalCurrentPosition = reflection * lClusterGlobalCurrentPosition;
	/////////////////////////////////////////////////////////////////////////////////////////


	// Compute the initial position of the link relative to the reference.
	lClusterRelativeInitPosition = lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;

	// Compute the current position of the link relative to the reference.
	lClusterRelativeCurrentPositionInverse = lReferenceGlobalCurrentPosition.Inverse() * lClusterGlobalCurrentPosition;

	// Compute the shift of the link relative to the reference.

	FbxAMatrix ClusterMatrix = lClusterRelativeCurrentPositionInverse * lClusterRelativeInitPosition;
	
	return GetD3DMatrix( ClusterMatrix );
}
