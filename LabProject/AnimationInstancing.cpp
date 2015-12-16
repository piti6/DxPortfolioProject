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
	if (m_pd3dAnimationTexture)
		m_pd3dAnimationTexture->Release();
	if (m_pd3dAnimationTextureResourceView)
		m_pd3dAnimationTextureResourceView->Release();
}

//FBX SDKからアニメーションをロードします。
void CAnimationInstancing::LoadAnimationFromFile(FbxManager *pFbxSdkManager, char* filename, string AnimationName, bool isLooping)
{
	//同じアニメーション名前があるのかを検査します。
	if (m_vAnimationList.GetAnimationMap()->find(AnimationName) != m_vAnimationList.GetAnimationMap()->end())
	{
		cout << "Same Animation Name Already Exist!" << endl;
		return;
	}

	FbxImporter* pImporter = FbxImporter::Create(pFbxSdkManager, "");
	FbxScene* pFbxScene = FbxScene::Create(pFbxSdkManager, "");

	//初期化
	if (!pImporter->Initialize(filename, -1, pFbxSdkManager->GetIOSettings()))
	{
		cout << "Fbx SDK Initialize Failed" << endl;
		return;
	}

	if (!pImporter->Import(pFbxScene)){
		cout << "Fbx SDK Scene Import Failed" << endl;
		return;
	}

	pImporter->Destroy();

	if (pFbxScene->GetSrcObjectCount<FbxAnimStack>() <= 0)
	{
		cout << "There's No Animation At " << filename << endl;
		return;
	}

	FbxNode* pFbxRootNode = pFbxScene->GetRootNode();
	unordered_map<string, CAnimation> *pAnimationMap = m_vAnimationList.GetAnimationMap();
	(*pAnimationMap)[AnimationName] = CAnimation(isLooping);
	vector<string> vBoneName;
	SetBoneNameIndex(pFbxScene, pFbxRootNode, &vBoneName);
	SetAnimationData(pFbxScene, pFbxRootNode, AnimationName, &vBoneName);
	pFbxScene->Destroy(true);
}
void CAnimationInstancing::SetBoneNameIndex(FbxScene *pFbxScene, FbxNode *pNode, vector<string> *pBoneName)
{
	//ボーンの名前のベクトルを設定します。
	if (pNode->GetNodeAttribute())
	{
		if (pNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton){
			pBoneName->push_back(pNode->GetName());
		}
	}
	for (int iChildIndex = 0; iChildIndex < pNode->GetChildCount(); ++iChildIndex)
	{
		SetBoneNameIndex(pFbxScene, pNode->GetChild(iChildIndex), pBoneName);
	}
}
void CAnimationInstancing::SetAnimationData(FbxScene *pFbxScene, FbxNode *pNode, string AnimationName, vector<string> *pBoneName){

	FbxAnimStack *pFbxAnimStack = pFbxScene->GetSrcObject<FbxAnimStack>();
	FbxAnimLayer *pFbxAnimLayer = pFbxAnimStack->GetMember<FbxAnimLayer>();
	unordered_map<string, CAnimation> *pAnimationMap = m_vAnimationList.GetAnimationMap();

	//FBX SDKからアニメーションの長さを持って来ます。
	(*pAnimationMap)[AnimationName].SetLength(pFbxAnimStack->GetLocalTimeSpan().GetDuration().GetMilliSeconds());
	int nFrameCount = (int)(*pAnimationMap)[AnimationName].GetLength() / 30;
	//アニメーションの長さを30に分けてフレームの数を決定します。(アニメーションの早さ＝30fps)


	FbxTime Time;
	CAnimationDataContainer* pAnimationData = (*pAnimationMap)[AnimationName].GetAnimationData();
	vector<CBoneContainer>* pBoneContainerVector = pAnimationData->GetBoneContainerVector();
	pBoneContainerVector->resize(nFrameCount);

	for (int i = 0; i < nFrameCount; ++i)
	{
		Time.SetMilliSeconds(i * 30);
		(*pBoneContainerVector)[i].GetBoneList()->resize(pBoneName->size());
		SetBoneMatrixVectorAtTime(pNode, AnimationName, Time, pBoneName, i);//時間に応じてボーンの行列をロードします。
	}
	(*pAnimationMap)[AnimationName].SetLength((*pAnimationMap)[AnimationName].GetLength() / 1000);//millisecondsからsecondsで単位を更新します。
}
void CAnimationInstancing::SetBoneMatrixVectorAtTime(FbxNode *pNode, string AnimationName, FbxTime& pTime, vector<string> *pBoneName, int index)
{
	FbxAMatrix pParentGlobalPosition;
	FbxAMatrix lGlobalPosition = pNode->EvaluateGlobalTransform(pTime);
	FbxAMatrix lGeometryOffset = GetGeometry(pNode);
	FbxAMatrix lGlobalOffPosition = lGlobalPosition * lGeometryOffset;

	FbxNodeAttribute* Attr = pNode->GetNodeAttribute();
	if (Attr)
	{
		if (Attr->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			FbxMesh * pMesh = pNode->GetMesh();

			int nSkinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);
			if (nSkinCount > 0)
			{
				for (int iSkinIndex = 0; iSkinIndex < nSkinCount; ++iSkinIndex)
				{
					FbxSkin *pSkin = (FbxSkin *)pMesh->GetDeformer(iSkinIndex, FbxDeformer::eSkin);
					int nClusterCount = pSkin->GetClusterCount();
					for (int iClusterIndex = 0; iClusterIndex < nClusterCount; ++iClusterIndex)
					{
						FbxCluster* pCluster = pSkin->GetCluster(iClusterIndex);
						if (!pCluster->GetLink())
							continue;

						string BoneName = pCluster->GetLink()->GetName();
						
						//ボーンの名前と比較してインデックスを持って来ます。
						int iBoneIndex;
						for (iBoneIndex = 0; iBoneIndex < pBoneName->size(); ++iBoneIndex)
						{
							if (BoneName.compare((*pBoneName)[iBoneIndex]) == 0){
								break;
							}
						}

						//現在時間のボーンのワールド行列を持って来ます。
						D3DXMATRIX d3dxmtxBone = GetClusterMatrix(lGlobalOffPosition, pMesh, pCluster, pTime);

						D3DXMATRIX scale;
						D3DXMatrixScaling(&scale, 0.015f, 0.015f, 0.015f);
						d3dxmtxBone = d3dxmtxBone * scale;

						unordered_map<string, CAnimation> *pAnimationMap = m_vAnimationList.GetAnimationMap();
						vector<CBoneContainer>* pBoneContainerVector = (*pAnimationMap)[AnimationName].GetAnimationData()->GetBoneContainerVector();
						vector<D3DXMATRIX>* pBoneList = (*pBoneContainerVector)[index].GetBoneList();
						(*pBoneList)[iBoneIndex] = d3dxmtxBone;

					}
				}
			}
		}
	}
	for (int lChildIndex = 0; lChildIndex < pNode->GetChildCount(); ++lChildIndex)
	{
		SetBoneMatrixVectorAtTime(pNode->GetChild(lChildIndex), AnimationName, pTime, pBoneName, index);
	}
}
D3DXMATRIX CAnimationInstancing::GetClusterMatrix(FbxAMatrix & pGlobalPosition, FbxMesh * pMesh, FbxCluster * pCluster, FbxTime& pTime)
{

	FbxAMatrix lReferenceGlobalInitPosition;
	FbxAMatrix lReferenceGlobalCurrentPosition;

	FbxAMatrix lClusterGlobalInitPosition;
	FbxAMatrix lClusterGlobalCurrentPosition;

	FbxAMatrix lClusterRelativeInitPosition;
	FbxAMatrix lClusterRelativeCurrentPositionInverse;

	FbxAMatrix lReferenceGeometry;
		
	//DirectXは左手座標系です。それで反射平面を作って行列を反射します。
	FbxAMatrix reflection;
	D3DXPLANE p(1.0f, 0.0f, 0.0f, 0.0f);
	D3DXMatrixReflect_Fixed(&reflection, &p);

	//グローバルポーズ行列を持って来ます。
	pCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
	lReferenceGlobalInitPosition = reflection * lReferenceGlobalInitPosition;

	//クラスターのグローバルポジション行列を持って来ます。
	lReferenceGlobalCurrentPosition = pGlobalPosition;

	//ジオメトリポジションを持って来ます。
	lReferenceGeometry = GetGeometry(pMesh->GetNode());
	lReferenceGeometry = reflection * lReferenceGeometry;
	
	//グローバルポーズ行列でジオメトリポジション行列を掛けます。
	lReferenceGlobalInitPosition *= lReferenceGeometry;

	//ボーンのグローバル行列を持って来ます。
	pCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);
	lClusterGlobalInitPosition = reflection * lClusterGlobalInitPosition;

	//ボーンの現在時間のグローバル行列を持って来ます。
	lClusterGlobalCurrentPosition = pCluster->GetLink()->EvaluateGlobalTransform(pTime);
	lClusterGlobalCurrentPosition = reflection * lClusterGlobalCurrentPosition;
	
	//クラスターのローカルポーズはグローバルポーズの逆行列でグローバルポーズ行列を掛けます。
	lClusterRelativeInitPosition = lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;

	//現在ローカルポジションはクラスターのグローバルポジションの逆行列でボーンの現在時間のグローバル行列を掛けます。
	lClusterRelativeCurrentPositionInverse = lReferenceGlobalCurrentPosition.Inverse() * lClusterGlobalCurrentPosition;

	return GetD3DMatrix(lClusterRelativeCurrentPositionInverse * lClusterRelativeInitPosition);
}

//インスタンシングをため、アニメーションテクスチャを作ります。
void CAnimationInstancing::CreateAnimationTexture(ID3D11Device* pd3dDevice)
{
	unordered_map<string, CAnimation> *pAnimationMap = m_vAnimationList.GetAnimationMap();
	if (!pAnimationMap->size()) return;

	//アニメーション全体のボーンサイズを持って来ます。
	int nTotalBones = 0;
	for (auto currentAnimation = pAnimationMap->begin(); currentAnimation != pAnimationMap->end(); ++currentAnimation)
	{
		CAnimationDataContainer *pAnimationData = currentAnimation->second.GetAnimationData();
		vector<CBoneContainer>* pBoneContainerVector = pAnimationData->GetBoneContainerVector();
		nTotalBones += pBoneContainerVector->size() * (*pBoneContainerVector)[0].GetBoneList()->size();
	}

	//基本的な行列は4*4,つまり16個のfloatですが、テクスチャ1ピクセルにはRGBA,四つのfloatが入りますので、一つのボーンで4を掛けます。
	UINT texelsPerBone = 4;

	//全体のピクセルです。
	UINT pixelCount = nTotalBones * texelsPerBone;

	UINT texWidth = 0;
	UINT texHeight = 0;

	//テクスチャのサイズは2の累乗です最小のテクスチャサイズを計算します。
	texWidth = (int)sqrt((float)pixelCount) + 1;
	texHeight = 1;
	while (texHeight < texWidth)
		texHeight = texHeight << 1;

	texWidth = texHeight;
	m_TextureWidth = texWidth;

	//テクスチャの情報を作ります。
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
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

	UINT bufferSize = texHeight*texWidth*sizeof(D3DXVECTOR4);
	D3DXVECTOR4 *pData = new D3DXVECTOR4[desc.Width*desc.Height];
	memset((void*)pData, 0, bufferSize);

	//テクスチャの実際情報を作ります。
	int nData = 0;
	for (auto currentAnimation = pAnimationMap->begin(); currentAnimation != pAnimationMap->end(); ++currentAnimation)
	{
		CAnimationDataContainer *pAnimationData = currentAnimation->second.GetAnimationData();
		vector<CBoneContainer>* pBoneContainerVector = pAnimationData->GetBoneContainerVector();
		for (int currentFrame = 0; currentFrame < pBoneContainerVector->size(); ++currentFrame)
		{
			
			CBoneContainer *pBoneContainer = &(*pBoneContainerVector)[currentFrame];
			vector<D3DXMATRIX>* pBoneList = pBoneContainer->GetBoneList();
			for (int iBone = 0; iBone < pBoneList->size(); ++iBone)
			{
				for (int i = 0; i < 4; ++i){
					pData[nData].x = (*pBoneList)[iBone].m[i][0];
					pData[nData].y = (*pBoneList)[iBone].m[i][1];
					pData[nData].z = (*pBoneList)[iBone].m[i][2];
					pData[nData++].w = (*pBoneList)[iBone].m[i][3];
				}
			}
		}
	}

	//テクスチャの作ります。
	D3D11_SUBRESOURCE_DATA srd;
	srd.pSysMem = (void*)pData;
	srd.SysMemPitch = texWidth*(sizeof(D3DXVECTOR4));
	srd.SysMemSlicePitch = 1;
	pd3dDevice->CreateTexture2D(&desc, &srd, &m_pd3dAnimationTexture);

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format = desc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = desc.MipLevels;
	pd3dDevice->CreateShaderResourceView(m_pd3dAnimationTexture, &SRVDesc, &m_pd3dAnimationTextureResourceView);

	delete[] pData;
}

//現在アニメーションの進行度を持って来ます。
int CAnimationInstancing::GetIndexAtCurrentTime(string AnimationName, float fCurrentTime)
{
	unordered_map<string, CAnimation> *pAnimationMap = m_vAnimationList.GetAnimationMap();
	if (pAnimationMap->find(AnimationName) == pAnimationMap->end())
	{
		cout << "Animation Name " << AnimationName << " Does not exist" << endl;
		return -1;
	}
	CAnimation *pAnimation = &(*pAnimationMap)[AnimationName];
	vector<CBoneContainer>* pBoneContainerVector = pAnimation->GetAnimationData()->GetBoneContainerVector();
	if (pAnimation->GetLength() < fCurrentTime)
	{
		return pBoneContainerVector->size() - 1;
	}
	float percent = fCurrentTime / pAnimation->GetLength();
	int percentINT = (int)percent;
	return (int)((float)pBoneContainerVector->size() * percent);
}

//現在アニメーションの位置を持って来ます。
UINT CAnimationInstancing::GetAnimationOffset(string AnimationName)
{
	unordered_map<string, CAnimation> *pAnimationMap = m_vAnimationList.GetAnimationMap();
	if (pAnimationMap->find(AnimationName) == pAnimationMap->end())
	{
		cout << "Animation Name " << AnimationName << " Does not exist" << endl;
		return -1;
	}
	int bonesOffset = 0;
	for (auto currentAnimation = pAnimationMap->begin(); currentAnimation != pAnimationMap->find(AnimationName); ++currentAnimation)
	{
		CAnimationDataContainer *pAnimationData = currentAnimation->second.GetAnimationData();
		vector<CBoneContainer>* pBoneContainerVector = pAnimationData->GetBoneContainerVector();
		bonesOffset += pBoneContainerVector->size() * (*pBoneContainerVector)[0].GetBoneList()->size();
	}
	return bonesOffset * 4;
}

//現在アニメーションでフレームの位置を持って来ます。
UINT CAnimationInstancing::GetFrameOffset(string AnimationName, float fCurrentTime)
{
	unordered_map<string, CAnimation> *pAnimationMap = m_vAnimationList.GetAnimationMap();
	if (pAnimationMap->find(AnimationName) == pAnimationMap->end())
	{
		cout << "Animation Name " << AnimationName << " Does not exist" << endl;
		return -1;
	}
	vector<CBoneContainer>* pBoneContainerVector = (*pAnimationMap)[AnimationName].GetAnimationData()->GetBoneContainerVector();
	
	return 4 * (*pBoneContainerVector)[0].GetBoneList()->size() * GetIndexAtCurrentTime(AnimationName, fCurrentTime);
}

//現在時間の最終的なインデックスを持って来ます。
UINT CAnimationInstancing::GetCurrentOffset(string AnimationName, float fCurrentTime)
{
	return GetAnimationOffset(AnimationName) + GetFrameOffset(AnimationName, fCurrentTime);
}

//テクスチャバッファを更新します。
void CAnimationInstancing::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext)
{
	pd3dDeviceContext->VSSetShaderResources(VS_SLOT_ANIMATION, 1, &m_pd3dAnimationTextureResourceView);
}