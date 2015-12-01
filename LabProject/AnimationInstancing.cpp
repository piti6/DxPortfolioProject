#include "stdafx.h"
#include "AnimationInstancing.h"


#include "FbxToDxTranslation.h"



CAnimationInstancing::CAnimationInstancing()
{
	m_pd3dAnimationTextureResourceView = NULL;
	m_pd3dAnimationTexture = NULL;
}

CAnimationInstancing::~CAnimationInstancing()
{
	if(m_pd3dAnimationTexture)
		m_pd3dAnimationTexture->Release();
	if(m_pd3dAnimationTextureResourceView)
		m_pd3dAnimationTextureResourceView->Release();
}

void CAnimationInstancing::LoadAnimationFromFile(FbxManager *pFbxSdkManager, char* filename, string AnimationName,bool isLooping)
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
	vector<string> vBoneName;
	SetBoneNameIndex(pFbxScene,pFbxRootNode,&vBoneName);
	SetAnimationData(pFbxScene,pFbxRootNode,AnimationName,&vBoneName);
	pFbxScene->Destroy(true);
}

void CAnimationInstancing::SetBoneNameIndex(FbxScene *pFbxScene,FbxNode *pNode,vector<string> *pBoneName){
	if (pNode->GetNodeAttribute())
	{
		if( pNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton){
			pBoneName->push_back(pNode->GetName());
		}
	}
	for(int iChildIndex=0; iChildIndex<pNode->GetChildCount(); ++iChildIndex)
	{
		SetBoneNameIndex(pFbxScene,pNode->GetChild(iChildIndex),pBoneName);
	}
}
void CAnimationInstancing::SetAnimationData(FbxScene *pFbxScene,FbxNode *pNode,string AnimationName,vector<string> *pBoneName){

	FbxAnimStack *pFbxAnimStack = pFbxScene->GetSrcObject<FbxAnimStack>();
	FbxAnimLayer *pFbxAnimLayer = pFbxAnimStack->GetMember<FbxAnimLayer>();
	m_vAnimationList.m_Animation[AnimationName].m_fLength = pFbxAnimStack->GetLocalTimeSpan().GetDuration().GetMilliSeconds();
	int nFrameCount = (int) m_vAnimationList.m_Animation[AnimationName].m_fLength/60;
	FbxTime Time;

	m_vAnimationList.m_Animation[AnimationName].m_vAnimation.m_vBoneContainer.resize(nFrameCount);
	for(int i=0; i<nFrameCount; ++i)
	{
		Time.SetMilliSeconds(i*60);
		m_vAnimationList.m_Animation[AnimationName].m_vAnimation.m_vBoneContainer[i].m_vBoneList.resize(pBoneName->size());
		/*
		D3DXMATRIX d3dxmtxIdentity;
		D3DXMatrixIdentity(&d3dxmtxIdentity);
		for(int i=0; i<m_vAnimationList.m_Animation[AnimationName].m_vAnimation.m_vBoneContainer[Time.GetMilliSeconds()].m_vBoneList.size(); ++i){
			m_vAnimationList.m_Animation[AnimationName].m_vAnimation.m_vBoneContainer[Time.GetMilliSeconds()].m_vBoneList[i] = d3dxmtxIdentity;
		}*/
		SetBoneMatrixVectorAtTime(pNode,AnimationName, Time, pBoneName,i);
	}
	m_vAnimationList.m_Animation[AnimationName].m_fLength/=1000;
}
void CAnimationInstancing::SetBoneMatrixVectorAtTime(FbxNode *pNode, string AnimationName,FbxTime& pTime,vector<string> *pBoneName, int index)
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
						for(iBoneIndex=0;iBoneIndex<pBoneName->size();++iBoneIndex)
						{
							if(BoneName.compare((*pBoneName)[iBoneIndex]) == 0){
								break;
							}
						}

						D3DXMATRIX d3dxmtxBone = GetClusterMatrix(lGlobalOffPosition, pMesh, pCluster, pTime);
						D3DXMatrixTranspose(&d3dxmtxBone,&d3dxmtxBone);
						m_vAnimationList.m_Animation[AnimationName].m_vAnimation.m_vBoneContainer[index].m_vBoneList[iBoneIndex] = d3dxmtxBone;

					}
				}
			}
		}
	}
	for (int lChildIndex = 0; lChildIndex < pNode->GetChildCount(); ++lChildIndex)
	{
		SetBoneMatrixVectorAtTime(pNode->GetChild(lChildIndex), AnimationName, pTime, pBoneName,index);
	}
}
D3DXMATRIX CAnimationInstancing::GetClusterMatrix(FbxAMatrix & pGlobalPosition ,FbxMesh * pMesh, FbxCluster * pCluster, FbxTime& pTime)
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

	lReferenceGlobalCurrentPosition = pGlobalPosition;

	lReferenceGeometry = GetGeometry(pMesh->GetNode());
	lReferenceGeometry = reflection * lReferenceGeometry;
	// Multiply lReferenceGlobalInitPosition by Geometric Transformation
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


/*
This creates and populates a texture that contains all the animation data for all frames of all m_animations.
*/
void CAnimationInstancing::CreateAnimationTexture(ID3D11Device* pd3dDevice)
{
	if(!m_vAnimationList.m_Animation.size()) return;
	
	// Loop through all m_animations and find the total size of all m_animations in bones
	int nTotalBones=0;
	for(auto currentAnimation=m_vAnimationList.m_Animation.begin() ; currentAnimation!=m_vAnimationList.m_Animation.end(); ++currentAnimation)
	{
		CAnimationDataContainer *pAnimationData = &currentAnimation->second.m_vAnimation;
		nTotalBones += pAnimationData->m_vBoneContainer.size() * pAnimationData->m_vBoneContainer[0].m_vBoneList.size();
	}

	UINT texelsPerBone = 4;

	UINT pixelCount = nTotalBones * texelsPerBone;    // rowsPerBone lines per matrix, since no projection
	UINT texWidth = 0;
	UINT texHeight = 0;

	// This basically fits the animation into a roughly square texture where the 
	//      width is a multiple of rowsPerBone(our size requirement for matrix storage)
	//      AND both dimensions are power of 2 since it seems to fail without this...
	texWidth = (int)sqrt((float)pixelCount)+1;    // gives us a starting point
	texHeight = 1;
	while(texHeight < texWidth) 
		texHeight = texHeight<<1;
	texWidth = texHeight;
	m_TextureWidth = texWidth;
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory( &desc, sizeof(D3D11_TEXTURE2D_DESC) );

	// Create our texture
	desc.MipLevels = 1;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.ArraySize = 1;
	desc.Width = texWidth;
	desc.Height = texHeight;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	// Make a offline buffer that is the full size of the texture
	UINT bufferSize = texHeight*texWidth*sizeof(D3DXVECTOR4);
	D3DXVECTOR4 *pData = new D3DXVECTOR4[desc.Width*desc.Height];
	memset((void*)pData,0,bufferSize);    // clear it

	int nData=0;
	for(auto currentAnimation=m_vAnimationList.m_Animation.begin() ; currentAnimation!=m_vAnimationList.m_Animation.end(); ++currentAnimation)
	{
		CAnimationDataContainer *pAnimationData = &currentAnimation->second.m_vAnimation;
		for(int currentFrame=0; currentFrame<pAnimationData->m_vBoneContainer.size(); ++currentFrame)
		{
			for(int iBone=0; iBone<pAnimationData->m_vBoneContainer[currentFrame].m_vBoneList.size(); ++iBone)
			{
				for(int i=0;i<4;++i){
					pData[nData].x = pAnimationData->m_vBoneContainer[currentFrame].m_vBoneList[iBone].m[i][0];
					pData[nData].y = pAnimationData->m_vBoneContainer[currentFrame].m_vBoneList[iBone].m[i][1];
					pData[nData].z = pAnimationData->m_vBoneContainer[currentFrame].m_vBoneList[iBone].m[i][2];
					pData[nData++].w = pAnimationData->m_vBoneContainer[currentFrame].m_vBoneList[iBone].m[i][3];
				}
			}
		}
	}

	D3D11_SUBRESOURCE_DATA srd;
	srd.pSysMem = (void*)pData;
	srd.SysMemPitch = texWidth*(sizeof(D3DXVECTOR4));
	srd.SysMemSlicePitch = 1;
	pd3dDevice->CreateTexture2D( &desc,&srd, &m_pd3dAnimationTexture);

	delete[] pData;

	// Make a resource view for it
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory( &SRVDesc, sizeof(SRVDesc) );
	SRVDesc.Format = desc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = desc.MipLevels;
	pd3dDevice->CreateShaderResourceView( m_pd3dAnimationTexture, &SRVDesc, &m_pd3dAnimationTextureResourceView);
}


int CAnimationInstancing::GetIndexAtCurrentTime(string AnimationName,float fCurrentTime)
{
	if(m_vAnimationList.m_Animation[AnimationName].m_fLength < fCurrentTime)
	{
		return m_vAnimationList.m_Animation[AnimationName].m_vAnimation.m_vBoneContainer.size()-1;
	}
	 // get a [0.f ... 1.f) value by allowing the percent to wrap around 1
    float percent = fCurrentTime / m_vAnimationList.m_Animation[AnimationName].m_fLength;
    int percentINT = (int)percent;
    //percent = percent - (float)percentINT;

	return (int)((float)m_vAnimationList.m_Animation[AnimationName].m_vAnimation.m_vBoneContainer.size() * percent);
}

UINT CAnimationInstancing::GetAnimationOffset(string AnimationName)
{
	if(m_vAnimationList.m_Animation.find(AnimationName)==m_vAnimationList.m_Animation.end()) return 0;

    int bonesOffset = 0;
	for(auto currentAnimation=m_vAnimationList.m_Animation.begin(); currentAnimation!=m_vAnimationList.m_Animation.find(AnimationName); ++currentAnimation)
	{
		CAnimationDataContainer *pAnimationData = &currentAnimation->second.m_vAnimation;
		bonesOffset += pAnimationData->m_vBoneContainer.size() * pAnimationData->m_vBoneContainer[0].m_vBoneList.size();
	}
    
    return bonesOffset * 4;    // 4 pixels per bone
}

UINT CAnimationInstancing::GetFrameOffset(string AnimationName,float fCurrentTime)
{
	int frameOffset;
	// this is offset in elements, so # bones * frame index
	if(m_vAnimationList.m_Animation.find(AnimationName)!=m_vAnimationList.m_Animation.end())
    {
        UINT texelsPerBone = 4;
		frameOffset = texelsPerBone*m_vAnimationList.m_Animation[AnimationName].m_vAnimation.m_vBoneContainer[0].m_vBoneList.size() * GetIndexAtCurrentTime(AnimationName,fCurrentTime);
    }
	return frameOffset;
}

void CAnimationInstancing::GetCurrentOffset(string AnimationName,float fCurrentTime)
{
	m_CurrentOffset = GetAnimationOffset(AnimationName) + GetFrameOffset(AnimationName,fCurrentTime);
	//cout<<m_CurrentOffset<<endl;
}


void CAnimationInstancing::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext)
{
	pd3dDeviceContext->VSSetShaderResources(PS_SLOT_ANIMATION, 1, &m_pd3dAnimationTextureResourceView);
}