#include "stdafx.h"
#include "AnimationController.h"
#include "FbxToDxTranslation.h"

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

	//m_vAnimationList.m_Animation[AnimationName] = vector<CAnimationContainer>();


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

	
	FbxAxisSystem SceneAxisSystem = pFbxScene->GetGlobalSettings().GetAxisSystem();
	
	FbxAxisSystem OurAxisSystem(FbxAxisSystem::DirectX);
	
	if( SceneAxisSystem != OurAxisSystem )
    {
        OurAxisSystem.ConvertScene(pFbxScene);
    }
	
	
	if(pFbxScene->GetSrcObjectCount<FbxAnimStack>()<=0){
		cout << "There's No Animation At " << filename << endl;
		return;
	}
	
	// Convert mesh, NURBS and patch into triangle mesh
	FbxGeometryConverter lGeomConverter(pFbxSdkManager);
	lGeomConverter.Triangulate(pFbxScene, true);
	
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
	int nFrameCount = (int) m_vAnimationList.m_Animation[AnimationName].m_fLength/30;
	FbxTime Time;

	for(int i=0; i<nFrameCount; ++i)
	{
		Time.SetMilliSeconds(i*30);
		SetBoneMatrixVectorAtTime(pNode,AnimationName, NULL, Time);
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

void CAnimationController::SetBoneMatrixVectorAtTime(FbxNode * pNode, string AnimationName, FbxPose* pPose, FbxTime& pTime)
{
	FbxAMatrix pParentGlobalPosition;
	pParentGlobalPosition.SetIdentity();
	FbxAMatrix lGlobalPosition = GetGlobalPosition(pNode, pTime, pPose, &pParentGlobalPosition);
	FbxNodeAttribute* Attr = pNode->GetNodeAttribute();
	if(Attr)
	{
		if (Attr->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			FbxAMatrix lGeometryOffset = GetGeometry(pNode);
			FbxAMatrix lGlobalOffPosition = lGlobalPosition * lGeometryOffset;
			FbxMesh * pMesh = pNode->GetMesh();

			int nSkinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);
			if (nSkinCount>0)
			{
				for ( int iSkinIndex=0; iSkinIndex<nSkinCount; ++iSkinIndex)
				{
					FbxSkin *pSkin = (FbxSkin *)pMesh->GetDeformer(iSkinIndex, FbxDeformer::eSkin);
					int nClusterCount = pSkin->GetClusterCount();
					for ( int iClusterIndex=0; iClusterIndex<nClusterCount; ++iClusterIndex)
					{
						FbxCluster* pCluster = pSkin->GetCluster(iClusterIndex);

						if (!pCluster->GetLink())
							continue;
						

						/*
						FbxAMatrix InitMat;
						pCluster->GetTransformLinkMatrix(InitMat);
						InitMat = InitMat.Inverse() * pCluster->GetLink()->GetAnimationEvaluator()->GetNodeGlobalTransform(pCluster->GetLink(),pTime);
						m_vAnimationList.m_Animation[AnimationName].m_vAnimation.m_vBoneContainer[pTime.GetMilliSeconds()].m_vBoneList.push_back(GetD3DMatrix(InitMat));
						*/
						
						D3DXMATRIX d3dxmtxBone = GetClusterMatrix(lGlobalOffPosition, pMesh, pCluster, pTime, pPose);
						//D3DXMatrixInverse(&d3dxmtxBone,NULL,&d3dxmtxBone);
						m_vAnimationList.m_Animation[AnimationName].m_vAnimation.m_vBoneContainer[pTime.GetMilliSeconds()].m_vBoneList.push_back(d3dxmtxBone);
						


						
						
					}
				}
			}
		}
	}
	for (int lChildIndex = 0; lChildIndex < pNode->GetChildCount(); ++lChildIndex)
	{
		SetBoneMatrixVectorAtTime(pNode->GetChild(lChildIndex), AnimationName, pPose, pTime);
	}


}

D3DXMATRIX CAnimationController::GetClusterMatrix(FbxAMatrix & pGlobalPosition ,FbxMesh * pMesh, FbxCluster * pCluster, FbxTime& pTime, FbxPose* pPose)
{

	FbxAMatrix lReferenceGlobalInitPosition;
	FbxAMatrix lClusterGlobalInitPosition;
	FbxAMatrix lClusterGlobalCurrentPosition;
	FbxAMatrix lReferenceGeometry;

	FbxAMatrix lReferenceGlobalCurrentPosition;

	FbxAMatrix lClusterRelativeInitPosition;
	FbxAMatrix lClusterRelativeCurrentPositionInverse;
	

	pCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
		/////////////////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////////////////////////////
		lReferenceGlobalCurrentPosition = pGlobalPosition;
		//lReferenceGlobalCurrentPosition.SetIdentity();
		// Multiply lReferenceGlobalInitPosition by Geometric Transformation
		lReferenceGeometry = GetGeometry(pMesh->GetNode());
		/////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////

		lReferenceGlobalInitPosition *= lReferenceGeometry;

		// Get the link initial global position and the link current global position.
		pCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);
		/////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////

		lClusterGlobalCurrentPosition = GetGlobalPosition(pCluster->GetLink(), pTime, pPose);
		/////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////


		// Compute the initial position of the link relative to the reference.
		lClusterRelativeInitPosition = lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;

		// Compute the current position of the link relative to the reference.
		lClusterRelativeCurrentPositionInverse = lReferenceGlobalCurrentPosition.Inverse() * lClusterGlobalCurrentPosition;

		// Compute the shift of the link relative to the reference.

		FbxAMatrix ClusterMatrix = lClusterRelativeCurrentPositionInverse * lClusterRelativeInitPosition;
		return GetD3DMatrix( ClusterMatrix );
}

/*

D3DXMATRIX CAnimationController::GetClusterMatrix(FbxAMatrix & pGlobalPosition ,FbxMesh * pMesh, FbxCluster * pCluster, FbxTime& pTime, FbxPose* pPose)
{

	FbxAMatrix lReferenceGlobalInitPosition;
	FbxAMatrix lReferenceGlobalCurrentPosition;

	FbxAMatrix lClusterGlobalInitPosition;
	FbxAMatrix lClusterGlobalCurrentPosition;

	FbxAMatrix lClusterRelativeInitPosition;
	FbxAMatrix lClusterRelativeCurrentPositionInverse;

	FbxAMatrix lReferenceGeometry;

	D3DXPLANE p(1.0f, 1.0f, 0.0f, 0.0f );
	
	FbxAMatrix relfection;
	relfection.SetIdentity();
	D3DXMatrixReflect_Fixed(&relfection, &p);

	pCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
	lReferenceGlobalInitPosition = relfection   * lReferenceGlobalInitPosition;

	/////////////////////////////////////////////////////////////////////////////////////////
	lReferenceGlobalCurrentPosition = pGlobalPosition;
	// Multiply lReferenceGlobalInitPosition by Geometric Transformation
	lReferenceGeometry = GetGeometry(pMesh->GetNode());
	/////////////////////////////////////////////////////////////////////////////////////////
	lReferenceGeometry = relfection   * lReferenceGeometry;
	/////////////////////////////////////////////////////////////////////////////////////////

	lReferenceGlobalInitPosition *= lReferenceGeometry;

	// Get the link initial global position and the link current global position.
	pCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);
	/////////////////////////////////////////////////////////////////////////////////////////
	lClusterGlobalInitPosition = relfection   * lClusterGlobalInitPosition;
	/////////////////////////////////////////////////////////////////////////////////////////

	lClusterGlobalCurrentPosition = GetGlobalPosition(pCluster->GetLink(), pTime, NULL);
	/////////////////////////////////////////////////////////////////////////////////////////
	lClusterGlobalCurrentPosition = relfection   * lClusterGlobalCurrentPosition;
	/////////////////////////////////////////////////////////////////////////////////////////


	// Compute the initial position of the link relative to the reference.
	lClusterRelativeInitPosition = lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;

	// Compute the current position of the link relative to the reference.
	lClusterRelativeCurrentPositionInverse = lReferenceGlobalCurrentPosition.Inverse() * lClusterGlobalCurrentPosition;

	// Compute the shift of the link relative to the reference.

	FbxAMatrix ClusterMatrix = lClusterRelativeCurrentPositionInverse * lClusterRelativeInitPosition;
	return GetD3DMatrix( ClusterMatrix );
}
*/