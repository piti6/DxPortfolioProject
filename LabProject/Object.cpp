//-----------------------------------------------------------------------------
// File: CGameObject.cpp
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "Object.h"
#include "FbxToDxTranslation.h"


CGameObject::CGameObject()
{
	D3DXMatrixIdentity(&m_d3dxmtxWorld);
	m_pMaterial = NULL;
	m_pTexture = NULL;
	m_bIsActive = true;
	m_d3dxvOffset = D3DXVECTOR3(0,0,0);
}

CGameObject::~CGameObject()
{
	for(int i=0; i<m_MeshesVector.size(); ++i){
		if(m_MeshesVector[i]) m_MeshesVector[i]->Release();
	}
	if (m_pMaterial)		m_pMaterial->Release();
	if (m_pTexture)			m_pTexture->Release();
}

void CGameObject::SetActive(bool isActive){
	m_bIsActive = isActive;
}

bool CGameObject::isActive(){
	return m_bIsActive;
}

void CGameObject::SetTexture(CTexture *pTexture)
{
	if (m_pTexture) m_pTexture->Release();
	m_pTexture = pTexture;
	if (m_pTexture) m_pTexture->AddRef();
}

void CGameObject::SetMaterial(CMaterial *pMaterial)
{
	if (m_pMaterial) m_pMaterial->Release();
	m_pMaterial = pMaterial;
	if (m_pMaterial) m_pMaterial->AddRef();
}

void CGameObject::SetMesh(CMesh *pMesh)
{
	m_MeshesVector.push_back(pMesh);
}

CMaterial* CGameObject::GetMaterial()
{
	return m_pMaterial;
}

CTexture* CGameObject::GetTexture()
{
	return m_pTexture;
}

CMesh* CGameObject::GetMesh(int _Index)
{
	if(0 <= _Index  && _Index < m_MeshesVector.size())
		return m_MeshesVector[_Index];
	return NULL;
}

D3DXMATRIX CGameObject::GetWorldMatrix()
{
	return m_d3dxmtxWorld;
}

void CGameObject::BuildObjects(PxPhysics *pPxPhysics, PxScene *pPxScene)
{
}

void CGameObject::Animate(float fTimeElapsed,PxScene *pPxScene)
{
	//cout << fTimeElapsed << endl;
}

void CGameObject::Render(ID3D11DeviceContext *pd3dImmediateDeviceContext)
{
	for(int i=0; i<m_MeshesVector.size(); ++i)
		if (m_MeshesVector[i]) m_MeshesVector[i]->Render(pd3dImmediateDeviceContext);
}

void CGameObject::UpdateAnimation(ID3D11DeviceContext *pd3dImmediateDeviceContext){

}

void CGameObject::SetPosition(D3DXVECTOR3 d3dxvPosition) 
{ 
	m_d3dxmtxWorld._41 = d3dxvPosition.x; 
	m_d3dxmtxWorld._42 = d3dxvPosition.y;
	m_d3dxmtxWorld._43 = d3dxvPosition.z;
}

D3DXVECTOR3 CGameObject::GetPosition() 
{ 
	return(D3DXVECTOR3(m_d3dxmtxWorld._41, m_d3dxmtxWorld._42, m_d3dxmtxWorld._43)); 
}

D3DXVECTOR3 CGameObject::GetLookAt() 
{ 	
	D3DXVECTOR3 d3dxvLookAt(m_d3dxmtxWorld._31, m_d3dxmtxWorld._32, m_d3dxmtxWorld._33);
	D3DXVec3Normalize(&d3dxvLookAt, &d3dxvLookAt);
	return(d3dxvLookAt);
}

D3DXVECTOR3 CGameObject::GetUp() 
{ 	
	D3DXVECTOR3 d3dxvUp(m_d3dxmtxWorld._21, m_d3dxmtxWorld._22, m_d3dxmtxWorld._23);
	D3DXVec3Normalize(&d3dxvUp, &d3dxvUp);
	return(d3dxvUp);
}

D3DXVECTOR3 CGameObject::GetRight()
{ 	
	D3DXVECTOR3 d3dxvRight(m_d3dxmtxWorld._11, m_d3dxmtxWorld._12, m_d3dxmtxWorld._13);
	D3DXVec3Normalize(&d3dxvRight, &d3dxvRight);
	return(d3dxvRight);
}

void CGameObject::MoveStrafe(float fDistance)
{
	D3DXVECTOR3 d3dxvPosition = GetPosition();
	D3DXVECTOR3 d3dxvRight = GetRight();
	d3dxvPosition += fDistance * d3dxvRight;
	SetPosition(d3dxvPosition);
}

void CGameObject::MoveUp(float fDistance)
{
	D3DXVECTOR3 d3dxvPosition = GetPosition();
	D3DXVECTOR3 d3dxvUp = GetUp();
	d3dxvPosition += fDistance * d3dxvUp;
	SetPosition(d3dxvPosition);
}

void CGameObject::MoveForward(float fDistance)
{
	D3DXVECTOR3 d3dxvPosition = GetPosition();
	D3DXVECTOR3 d3dxvLookAt = GetLookAt();
	d3dxvPosition += fDistance * d3dxvLookAt;
	SetPosition(d3dxvPosition);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	D3DXMATRIX mtxRotate;
	D3DXMatrixRotationYawPitchRoll(&mtxRotate, (float)D3DXToRadian(fYaw), (float)D3DXToRadian(fPitch), (float)D3DXToRadian(fRoll));
	m_d3dxmtxWorld = mtxRotate * m_d3dxmtxWorld;
}

void CGameObject::Rotate(D3DXVECTOR3 *pd3dxvAxis, float fAngle)
{
	D3DXMATRIX mtxRotate;
	D3DXMatrixRotationAxis(&mtxRotate, pd3dxvAxis, (float)D3DXToRadian(fAngle));
	m_d3dxmtxWorld = mtxRotate * m_d3dxmtxWorld;
}

void CGameObject::SetOffset(D3DXVECTOR3 _Offset)
{
	m_d3dxvOffset = _Offset;
}

D3DXVECTOR3 CGameObject::GetOffset()
{
	return m_d3dxvOffset;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//

CStaticObject::CStaticObject()
{
	m_pPxMaterial = NULL;
	m_pPxActor = NULL;
}

CStaticObject::~CStaticObject()
{
	if(m_pPxMaterial) m_pPxMaterial->release();
	if(m_pPxActor) m_pPxActor->release();
}

void CStaticObject::BuildObjects(PxPhysics *pPxPhysics, PxScene *pPxScene)
{
	m_pPxMaterial = pPxPhysics->createMaterial(0.9,0.9,0.001);
	PxTransform _PxTransform(GetPosition().x,GetPosition().y,GetPosition().z);
	D3DXVECTOR3 _d3dxvBoundMinimum = m_MeshesVector[0]->GetBoundingCube().GetMinimum();
	D3DXVECTOR3 _d3dxvBoundMaximum = m_MeshesVector[0]->GetBoundingCube().GetMaximum();
	D3DXVECTOR3 _d3dxvExtents = 
		D3DXVECTOR3((abs(_d3dxvBoundMinimum.x) + abs(_d3dxvBoundMaximum.x))/2,(abs(_d3dxvBoundMinimum.y) + abs(_d3dxvBoundMaximum.y))/2,(abs(_d3dxvBoundMinimum.z) + abs(_d3dxvBoundMaximum.z))/2);
	PxBoxGeometry _PxBoxGeometry(_d3dxvExtents.x,_d3dxvExtents.y,_d3dxvExtents.z);
	m_pPxActor = PxCreateStatic(*pPxPhysics,_PxTransform,_PxBoxGeometry,*m_pPxMaterial);
	pPxScene->addActor(*m_pPxActor);

	PxTransform pT = m_pPxActor->getGlobalPose();
	PxMat44 m = PxMat44(pT);
	m_d3dxmtxWorld = D3DXMATRIX(m.front());	
}

void CStaticObject::SetPosition(D3DXVECTOR3 d3dxvPosition) 
{
	CGameObject::SetPosition(d3dxvPosition);
	m_pPxActor->setGlobalPose(PxTransform(PxVec3(d3dxvPosition.x+m_d3dxvOffset.x,d3dxvPosition.y+m_d3dxvOffset.y,d3dxvPosition.z+m_d3dxvOffset.z)));
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//

CDynamicObject::CDynamicObject(bool _HasAnimation)
{
	m_pPxMaterial = NULL;
	m_pPxActor = NULL;
	m_bHasAnimation = _HasAnimation;

}

CDynamicObject::~CDynamicObject()
{
	if(m_pPxMaterial) m_pPxMaterial->release();
	if(m_pPxActor) m_pPxActor->release();
}

void CDynamicObject::BuildObjects(PxPhysics *pPxPhysics, PxScene *pPxScene)
{

	m_pPxMaterial = pPxPhysics->createMaterial(0.9,0.9,0.0001f);
	PxTransform _PxTransform(GetPosition().x + m_d3dxvOffset.x,GetPosition().y + m_d3dxvOffset.y,GetPosition().z + m_d3dxvOffset.z);

	D3DXVECTOR3 _d3dxvBoundMinimum = m_MeshesVector[0]->GetBoundingCube().GetMinimum();
	D3DXVECTOR3 _d3dxvBoundMaximum = m_MeshesVector[0]->GetBoundingCube().GetMaximum();
	D3DXVECTOR3 _d3dxvExtents = 
		D3DXVECTOR3((abs(_d3dxvBoundMinimum.x) + abs(_d3dxvBoundMaximum.x))/2,(abs(_d3dxvBoundMinimum.y) + abs(_d3dxvBoundMaximum.y))/2,(abs(_d3dxvBoundMinimum.z) + abs(_d3dxvBoundMaximum.z))/2);
	PxBoxGeometry _PxBoxGeometry(_d3dxvExtents.x,_d3dxvExtents.y,_d3dxvExtents.z);
	m_pPxActor = PxCreateDynamic(*pPxPhysics,_PxTransform,_PxBoxGeometry,*m_pPxMaterial,2000.0f);
	pPxScene->addActor(*m_pPxActor);
	/*
	PxTransform pT = m_pPxActor->getGlobalPose();
	PxMat44 m = PxMat44(pT);
	m.setPosition(PxVec3(m.column3.x,m.column3.y,m.column3.z));
	m_d3dxmtxWorld = D3DXMATRIX(m.front());	*/
}

void CDynamicObject::Animate(float fTimeElapsed,PxScene *pPxScene)
{
	m_AnimationController.UpdateTime(fTimeElapsed);
	PxTransform pT = m_pPxActor->getGlobalPose();
	PxMat44 m = PxMat44(pT);
	m = m * PxMat44(PxTransform(m_d3dxvOffset.x,m_d3dxvOffset.y,m_d3dxvOffset.z));
	//m.setPosition(PxVec3(m.column3.x + m_d3dxvOffset.x,m.column3.y + m_d3dxvOffset.y,m.column3.z + m_d3dxvOffset.z));

	m_d3dxmtxWorld = D3DXMATRIX(m.front());	
}

void CDynamicObject::MoveStrafe(float fDistance)
{
	D3DXVECTOR3 d3dxvPosition = GetPosition();
	D3DXVECTOR3 d3dxvRight = GetRight();
	d3dxvPosition += fDistance * d3dxvRight;
	SetPosition(d3dxvPosition);
}

void CDynamicObject::MoveUp(float fDistance)
{
	D3DXVECTOR3 d3dxvPosition = GetPosition();
	D3DXVECTOR3 d3dxvUp = GetUp();
	d3dxvPosition += fDistance * d3dxvUp;
	SetPosition(d3dxvPosition);
}

void CDynamicObject::MoveForward(float fDistance)
{
	D3DXVECTOR3 d3dxvPosition = GetPosition();
	D3DXVECTOR3 d3dxvLookAt = GetLookAt();
	d3dxvPosition += fDistance * d3dxvLookAt;
	SetPosition(d3dxvPosition);
}

void CDynamicObject::SetPosition(D3DXVECTOR3 d3dxvPosition) 
{
	m_pPxActor->setGlobalPose(PxTransform(PxVec3(d3dxvPosition.x,d3dxvPosition.y,d3dxvPosition.z)));
}

void CDynamicObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	m_pPxActor->setAngularVelocity(PxVec3(fPitch,fYaw,fRoll));
}

void CDynamicObject::SetActive(bool isActive)
{
	m_bIsActive = isActive;
	if(m_bIsActive)
		m_pPxActor->wakeUp();
	else
		m_pPxActor->putToSleep();
}

void CDynamicObject::AddForce(float fx, float fy, float fz)
{
	m_pPxActor->addForce(PxVec3(fx,fy,fz),PxForceMode::eVELOCITY_CHANGE);
}

void CDynamicObject::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	D3D11_BUFFER_DESC d3dBufferDesc;	// 본에 대한 행렬을 넣어줄 상수 버퍼 생성
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	d3dBufferDesc.ByteWidth = sizeof(D3DXMATRIX) * MAX_BONE;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, NULL, &m_pd3dBoneMatrix);
}

void CDynamicObject::UpdateAnimation(ID3D11DeviceContext *pd3dImmediateDeviceContext)
{
	if(m_bHasAnimation)
	{
		if(m_AnimationController.m_bIsPlaying)
		{
			float fCurrentTimeClosest = 0;
			for(auto i = m_AnimationController.m_vAnimationList.m_Animation[m_AnimationController.m_CurrentPlayingAnimationName].m_vAnimation.m_vBoneContainer.begin();
				i!=m_AnimationController.m_vAnimationList.m_Animation[m_AnimationController.m_CurrentPlayingAnimationName].m_vAnimation.m_vBoneContainer.end();++i)
			{
				fCurrentTimeClosest = i->first;
				if(m_AnimationController.m_fCurrentAnimTime < fCurrentTimeClosest/1000)
				{
					break;
				}
			}
			D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
			pd3dImmediateDeviceContext->Map(m_pd3dBoneMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
			BONE_MATRIX *pd3dxmBone = (BONE_MATRIX*)d3dMappedResource.pData;
			
			for(int i=0;i<m_AnimationController.m_vAnimationList.m_Animation[m_AnimationController.m_CurrentPlayingAnimationName].m_vAnimation.m_vBoneContainer[fCurrentTimeClosest].m_vBoneList.size(); ++i)
			{
				D3DXMatrixTranspose(&pd3dxmBone->BONE[i],&m_AnimationController.m_vAnimationList.m_Animation[m_AnimationController.m_CurrentPlayingAnimationName].m_vAnimation.m_vBoneContainer[fCurrentTimeClosest].m_vBoneList[i]);
			}
			pd3dImmediateDeviceContext->Unmap(m_pd3dBoneMatrix, 0);
			pd3dImmediateDeviceContext->VSSetConstantBuffers(VS_SLOT_BONE_MATRIX, 1, &m_pd3dBoneMatrix);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//

CSkyBox::CSkyBox()
{
}


CSkyBox::~CSkyBox()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//

CHeightMapTerrain::CHeightMapTerrain(ID3D11Device *pd3dDevice, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, D3DXVECTOR3 d3dxvScale)
{
	m_nWidth = nWidth;
	m_nLength = nLength;

	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;

	m_d3dxvScale = d3dxvScale;

	m_pHeightMap = new CHeightMap(pFileName, nWidth, nLength, d3dxvScale);

	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;

	CHeightMapGridMesh *pHeightMapGridMesh = NULL;
	for (int z = 0, zStart = 0; z < czBlocks; ++z)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; ++x)
		{
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);
			pHeightMapGridMesh = new CHeightMapGridMesh(pd3dDevice, xStart, zStart, nBlockWidth, nBlockLength, d3dxvScale, m_pHeightMap);
			SetMesh(pHeightMapGridMesh);
		}
	}
	//D3DXMatrixIdentity(&m_d3dxmtxWorld);
}

CHeightMapTerrain::~CHeightMapTerrain(void)
{
	if (m_pHeightMap) delete m_pHeightMap;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//

