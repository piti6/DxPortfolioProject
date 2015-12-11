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
	m_d3dxvOffset = D3DXVECTOR3(0, 0, 0);
}

CGameObject::~CGameObject()
{
	for (int i = 0; i < m_MeshesVector.size(); ++i){
		if (m_MeshesVector[i]) m_MeshesVector[i]->Release();
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
	if (0 <= _Index  && _Index < m_MeshesVector.size())
		return m_MeshesVector[_Index];
	return NULL;
}

D3DXMATRIX CGameObject::GetWorldMatrix()
{
	return m_d3dxmtxWorld;
}

void CGameObject::BuildObjects(PxPhysics *pPxPhysics, PxScene *pPxScene, PxMaterial *pPxMaterial)
{
}

void CGameObject::Animate(float fTimeElapsed, PxScene *pPxScene)
{
	//cout << fTimeElapsed << endl;
}

void CGameObject::Render(ID3D11DeviceContext *pd3dImmediateDeviceContext)
{
	for (int i = 0; i < m_MeshesVector.size(); ++i)
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

void CGameObject::Rotate(D3DXVECTOR3 rotation)
{
	D3DXMATRIX mtxRotate;
	D3DXMatrixRotationYawPitchRoll(&mtxRotate, (float)D3DXToRadian(rotation.x), (float)D3DXToRadian(rotation.y), (float)D3DXToRadian(rotation.z));
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
}

void CStaticObject::BuildObjects(PxPhysics *pPxPhysics, PxScene *pPxScene, PxMaterial *pPxMaterial, const char* name)
{
	m_pPxMaterial = pPxMaterial;
	D3DXVECTOR3 _d3dxvBoundMinimum = m_MeshesVector[0]->GetBoundingCube().GetMinimum();
	D3DXVECTOR3 _d3dxvBoundMaximum = m_MeshesVector[0]->GetBoundingCube().GetMaximum();
	D3DXVECTOR3 _d3dxvExtents =
		D3DXVECTOR3((abs(_d3dxvBoundMinimum.x) + abs(_d3dxvBoundMaximum.x)) / 2, (abs(_d3dxvBoundMinimum.y) + abs(_d3dxvBoundMaximum.y)) / 2, (abs(_d3dxvBoundMinimum.z) + abs(_d3dxvBoundMaximum.z)) / 2);
	PxTransform _PxTransform(0,0,0);
	PxBoxGeometry _PxBoxGeometry(_d3dxvExtents.x, _d3dxvExtents.y, _d3dxvExtents.z);
	m_pPxActor = PxCreateStatic(*pPxPhysics, _PxTransform, _PxBoxGeometry, *m_pPxMaterial);
	m_pPxActor->setName(name);
	pPxScene->addActor(*m_pPxActor);
}

void CStaticObject::SetPosition(D3DXVECTOR3 d3dxvPosition)
{
	CGameObject::SetPosition(d3dxvPosition);
	D3DXVECTOR3 _d3dxvBoundMinimum = m_MeshesVector[0]->GetBoundingCube().GetMinimum();
	D3DXVECTOR3 _d3dxvBoundMaximum = m_MeshesVector[0]->GetBoundingCube().GetMaximum();
	D3DXVECTOR3 _d3dxvExtents =
		D3DXVECTOR3((abs(_d3dxvBoundMinimum.x) + abs(_d3dxvBoundMaximum.x)) / 2, (abs(_d3dxvBoundMinimum.y) + abs(_d3dxvBoundMaximum.y)) / 2, (abs(_d3dxvBoundMinimum.z) + abs(_d3dxvBoundMaximum.z)) / 2);
	m_pPxActor->setGlobalPose(PxTransform(PxVec3(d3dxvPosition.x, d3dxvPosition.y + _d3dxvExtents.y, d3dxvPosition.z)));
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//

CDynamicObject::CDynamicObject(bool _HasAnimation)
{
	m_pPxActor = NULL;
	m_bHasAnimation = _HasAnimation;
}

CDynamicObject::~CDynamicObject()
{
}

void CDynamicObject::BuildObjects(PxPhysics *pPxPhysics, PxScene *pPxScene, PxMaterial *pPxMaterial)
{
	D3DXVECTOR3 _d3dxvBoundMinimum = m_MeshesVector[0]->GetBoundingCube().GetMinimum();
	D3DXVECTOR3 _d3dxvBoundMaximum = m_MeshesVector[0]->GetBoundingCube().GetMaximum();
	D3DXVECTOR3 _d3dxvExtents =
		D3DXVECTOR3((abs(_d3dxvBoundMinimum.x) + abs(_d3dxvBoundMaximum.x)) / 2, (abs(_d3dxvBoundMinimum.y) + abs(_d3dxvBoundMaximum.y)) / 2, (abs(_d3dxvBoundMinimum.z) + abs(_d3dxvBoundMaximum.z)) / 2);
	PxTransform _PxTransform(0, 0, 0);
	PxBoxGeometry _PxBoxGeometry(_d3dxvExtents.x, _d3dxvExtents.y, _d3dxvExtents.z);
	m_pPxActor = PxCreateDynamic(*pPxPhysics, _PxTransform, _PxBoxGeometry, *pPxMaterial, 2000.0f);
	pPxScene->addActor(*m_pPxActor);
}

void CDynamicObject::Animate(float fTimeElapsed, PxScene *pPxScene)
{
	if (m_bHasAnimation)
		m_AnimationController.UpdateTime(fTimeElapsed);
	PxTransform pT = m_pPxActor->getGlobalPose();
	PxMat44 m = PxMat44(pT);
	PxVec3 RotatedOffset = m.rotate(PxVec3(m_d3dxvOffset.x, m_d3dxvOffset.y, m_d3dxvOffset.z));
	m.setPosition(PxVec3(m.getPosition()+RotatedOffset));
	m_d3dxmtxWorld = D3DXMATRIX(m.front());
}

void CDynamicObject::SetPosition(D3DXVECTOR3 d3dxvPosition)
{
	D3DXVECTOR3 _d3dxvBoundMinimum = m_MeshesVector[0]->GetBoundingCube().GetMinimum();
	D3DXVECTOR3 _d3dxvBoundMaximum = m_MeshesVector[0]->GetBoundingCube().GetMaximum();
	D3DXVECTOR3 _d3dxvExtents =
		D3DXVECTOR3((abs(_d3dxvBoundMinimum.x) + abs(_d3dxvBoundMaximum.x)) / 2, (abs(_d3dxvBoundMinimum.y) + abs(_d3dxvBoundMaximum.y)) / 2, (abs(_d3dxvBoundMinimum.z) + abs(_d3dxvBoundMaximum.z)) / 2);
	//m_pPxActor->setGlobalPose(PxTransform(PxVec3(d3dxvPosition.x + _d3dxvExtents.x, d3dxvPosition.y + _d3dxvExtents.y, d3dxvPosition.z + _d3dxvExtents.z)));
	m_pPxActor->setGlobalPose(PxTransform(PxVec3(d3dxvPosition.x, d3dxvPosition.y, d3dxvPosition.z)));
}

void CDynamicObject::Rotate(float fPitch, float fYaw, float fRoll)
{

	PxTransform _PxTransform = m_pPxActor->getGlobalPose();
	
	_PxTransform.q *= PxQuat(D3DXToDegree(fPitch), PxVec3(1,0,0));
	m_pPxActor->setGlobalPose(_PxTransform);
}

void CDynamicObject::SetActive(bool isActive)
{
	m_bIsActive = isActive;
	if (m_bIsActive)
		m_pPxActor->wakeUp();
	else
		m_pPxActor->putToSleep();
}

void CDynamicObject::AddForce(float fx, float fy, float fz)
{
	m_pPxActor->addForce(PxVec3(fx, fy, fz), PxForceMode::eVELOCITY_CHANGE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//

CCharacterObject::CCharacterObject(bool _HasAnimation)
{
	m_pPxCharacterController = NULL;
	D3DXMatrixIdentity(&m_d3dxmtxRotate);
	D3DXMatrixIdentity(&m_d3dxmtxRotateOffset);
	m_bHasAnimation = _HasAnimation;
}

CCharacterObject::~CCharacterObject()
{

}

void CCharacterObject::BuildObjects(PxPhysics *pPxPhysics, PxScene *pPxScene, PxMaterial *pPxMaterial, PxControllerManager *pPxControllerManager)
{

	
	D3DXVECTOR3 _d3dxvBoundMinimum = m_MeshesVector[0]->GetBoundingCube().GetMinimum();
	D3DXVECTOR3 _d3dxvBoundMaximum = m_MeshesVector[0]->GetBoundingCube().GetMaximum();
	D3DXVECTOR3 _d3dxvExtents =
		D3DXVECTOR3((abs(_d3dxvBoundMinimum.x) + abs(_d3dxvBoundMaximum.x)) / 2, (abs(_d3dxvBoundMinimum.y) + abs(_d3dxvBoundMaximum.y)) / 2, (abs(_d3dxvBoundMinimum.z) + abs(_d3dxvBoundMaximum.z)) / 2);
	PxBoxControllerDesc PxBoxdesc;
	PxBoxdesc.position = PxExtendedVec3(0, 0, 0);
	PxBoxdesc.halfForwardExtent = _d3dxvExtents.y / 2;
	PxBoxdesc.halfSideExtent = _d3dxvExtents.z / 2;
	PxBoxdesc.halfHeight = _d3dxvExtents.x / 2;
	PxBoxdesc.slopeLimit = 10;
	PxBoxdesc.contactOffset = 0.00001;
	PxBoxdesc.upDirection = PxVec3(0, 1, 0);
	PxBoxdesc.material = pPxMaterial;
	m_pPxCharacterController = pPxControllerManager->createController(PxBoxdesc);
}

void CCharacterObject::Animate(float fTimeElapsed, PxScene *pPxScene)
{
	if (m_bHasAnimation)
		m_AnimationController.UpdateTime(fTimeElapsed);
	m_pPxCharacterController->move(PxVec3(0, - 9.8f, 0) * fTimeElapsed, 0, fTimeElapsed, PxControllerFilters());
	PxMat44 m = PxMat44(PxIdentity);
	m.setPosition(PxVec3(m_pPxCharacterController->getFootPosition().x, m_pPxCharacterController->getFootPosition().y, m_pPxCharacterController->getFootPosition().z));
	m_d3dxmtxWorld = m_d3dxmtxRotateOffset * m_d3dxmtxRotate * D3DXMATRIX(m.front());
}

void CCharacterObject::SetPosition(D3DXVECTOR3 d3dxvPosition)
{
	m_pPxCharacterController->setPosition(PxExtendedVec3(d3dxvPosition.x, d3dxvPosition.y, d3dxvPosition.z));
}

void CCharacterObject::RotateOffset(float fPitch, float fYaw, float fRoll)
{
	D3DXMATRIX d3dxmtxRotateOffset;
	D3DXMatrixRotationYawPitchRoll(&d3dxmtxRotateOffset, (float)D3DXToRadian(fYaw), (float)D3DXToRadian(fPitch), (float)D3DXToRadian(fRoll));
	m_d3dxmtxRotateOffset = d3dxmtxRotateOffset;
}

void CCharacterObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	D3DXMATRIX mtxRotate;
	D3DXMatrixRotationYawPitchRoll(&mtxRotate, (float)D3DXToRadian(fYaw), (float)D3DXToRadian(fPitch), (float)D3DXToRadian(fRoll));
	m_d3dxmtxRotate = mtxRotate * m_d3dxmtxRotate;
}

void CCharacterObject::Rotate(D3DXVECTOR3 rotation)
{
	D3DXMATRIX mtxRotate;
	D3DXMatrixRotationYawPitchRoll(&mtxRotate, (float)D3DXToRadian(rotation.x), (float)D3DXToRadian(rotation.y), (float)D3DXToRadian(rotation.z));
	m_d3dxmtxRotate = mtxRotate * m_d3dxmtxRotate;
}

void CCharacterObject::Rotate(D3DXVECTOR3 *pd3dxvAxis, float fAngle)
{
	D3DXMATRIX mtxRotate;
	D3DXMatrixRotationAxis(&mtxRotate, pd3dxvAxis, (float)D3DXToRadian(fAngle));
	m_d3dxmtxRotate = mtxRotate * m_d3dxmtxRotate;
}

void CCharacterObject::MoveStrafe(float fDistance)
{
	D3DXVECTOR3 d3dxvPosition = GetPosition();
	D3DXVECTOR3 d3dxvRight = GetRight();
	d3dxvPosition += fDistance * d3dxvRight;
	SetPosition(d3dxvPosition);
}

void CCharacterObject::MoveUp(float fDistance)
{
	D3DXVECTOR3 d3dxvPosition = GetPosition();
	D3DXVECTOR3 d3dxvUp = GetUp();
	d3dxvPosition += fDistance * d3dxvUp;
	SetPosition(d3dxvPosition);
}

void CCharacterObject::MoveForward(float fDistance)
{
	D3DXVECTOR3 d3dxvPosition = GetPosition();
	D3DXVECTOR3 d3dxvLookAt = GetLookAt();
	d3dxvPosition += fDistance * d3dxvLookAt;
	SetPosition(d3dxvPosition);
}

D3DXVECTOR3 CCharacterObject::GetLookAt()
{
	D3DXVECTOR3 d3dxvLookAt(m_d3dxmtxRotate._31, m_d3dxmtxRotate._32, m_d3dxmtxRotate._33);
	D3DXVec3Normalize(&d3dxvLookAt, &d3dxvLookAt);
	return(d3dxvLookAt);
}

D3DXVECTOR3 CCharacterObject::GetUp()
{
	D3DXVECTOR3 d3dxvUp(m_d3dxmtxRotate._21, m_d3dxmtxRotate._22, m_d3dxmtxRotate._23);
	D3DXVec3Normalize(&d3dxvUp, &d3dxvUp);
	return(d3dxvUp);
}

D3DXVECTOR3 CCharacterObject::GetRight()
{
	D3DXVECTOR3 d3dxvRight(m_d3dxmtxRotate._11, m_d3dxmtxRotate._12, m_d3dxmtxRotate._13);
	D3DXVec3Normalize(&d3dxvRight, &d3dxvRight);
	return(d3dxvRight);
}

void CCharacterObject::SetActive(bool isActive)
{
	m_bIsActive = isActive;
	/*
	if (m_bIsActive)
		m_pPxCharacterController->;
	else
		m_pPxActor->putToSleep();
		*/
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CPlayer

CPlayer::CPlayer() : CCharacterObject(true)
{
	m_pCamera = NULL;

	m_d3dxvPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_d3dxvRight = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_d3dxvUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_d3dxvLook = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	
	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;
	
}

CPlayer::~CPlayer()
{
	if (m_pCamera) delete m_pCamera;
}

void CPlayer::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
}

void CPlayer::UpdateShaderVariables(ID3D11DeviceContext *pd3dImmediateDeviceContext)
{
	if (m_pCamera) m_pCamera->UpdateShaderVariables(pd3dImmediateDeviceContext);
}

/*
void CPlayer::RegenerateWorldMatrix()
{
	m_d3dxmtxWorld._11 = m_d3dxvRight.x; m_d3dxmtxWorld._12 = m_d3dxvRight.y; m_d3dxmtxWorld._13 = m_d3dxvRight.z;
	m_d3dxmtxWorld._21 = m_d3dxvUp.x; m_d3dxmtxWorld._22 = m_d3dxvUp.y; m_d3dxmtxWorld._23 = m_d3dxvUp.z;
	m_d3dxmtxWorld._31 = m_d3dxvLook.x; m_d3dxmtxWorld._32 = m_d3dxvLook.y; m_d3dxmtxWorld._33 = m_d3dxvLook.z;
	m_d3dxmtxWorld._41 = m_d3dxvPosition.x; m_d3dxmtxWorld._42 = m_d3dxvPosition.y; m_d3dxmtxWorld._43 = m_d3dxvPosition.z;
}
*/
void CPlayer::Move(DWORD dwDirection, float fDistance, float fTimeElapsed)
{
	if (dwDirection)
	{
		D3DXVECTOR3 d3dxvShift = D3DXVECTOR3(0, 0, 0);
		if (dwDirection & DIR_FORWARD) d3dxvShift += m_d3dxvLook * fDistance;
		if (dwDirection & DIR_BACKWARD) d3dxvShift -= m_d3dxvLook * fDistance;
		if (dwDirection & DIR_RIGHT) d3dxvShift += m_d3dxvRight * fDistance;
		if (dwDirection & DIR_LEFT) d3dxvShift -= m_d3dxvRight * fDistance;
		if (dwDirection & DIR_UP) d3dxvShift += m_d3dxvUp * fDistance;
		if (dwDirection & DIR_DOWN) d3dxvShift -= m_d3dxvUp * fDistance;

		Move(d3dxvShift, fTimeElapsed);
	}
}

void CPlayer::Move(const D3DXVECTOR3& d3dxvShift, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = m_pCamera->GetMode();
	if (nCurrentCameraMode == FIRST_PERSON_CAMERA || nCurrentCameraMode == THIRD_PERSON_CAMERA){
		m_pPxCharacterController->move(PxVec3(d3dxvShift.x, d3dxvShift.y, d3dxvShift.z) * fTimeElapsed, 0, fTimeElapsed, PxControllerFilters());
		if (m_AnimationController.GetCurrentAnimationName() != "Run")
			m_AnimationController.Play("Run");
	}
	else{
		m_pCamera->Move(d3dxvShift * fTimeElapsed);
	}
}

void CPlayer::Rotate(float x, float y, float z)
{
	D3DXMATRIX mtxRotate;

		if (x != 0.0f)
		{
			m_fPitch += x;
			if (m_fPitch > +60.0f) { x -= (m_fPitch - 60.0f); m_fPitch = +60.0f; }
			if (m_fPitch < -30.0f) { x -= (m_fPitch + 30.0f); m_fPitch = -30.0f; }
		}
		if (y != 0.0f)
		{
			m_fYaw += y;
			if (m_fYaw > 360.0f) m_fYaw -= 360.0f;
			if (m_fYaw < 0.0f) m_fYaw += 360.0f;
		}
		if (z != 0.0f)
		{
			m_fRoll += z;
			if (m_fRoll > +20.0f) { z -= (m_fRoll - 20.0f); m_fRoll = +20.0f; }
			if (m_fRoll < -20.0f) { z -= (m_fRoll + 20.0f); m_fRoll = -20.0f; }
		}
		m_pCamera->Rotate(x, y, z);
		
		if (y != 0.0f)
		{
			D3DXMatrixRotationAxis(&mtxRotate, &m_d3dxvUp, (float)D3DXToRadian(y));
			D3DXVec3TransformNormal(&m_d3dxvLook, &m_d3dxvLook, &mtxRotate);
			D3DXVec3TransformNormal(&m_d3dxvRight, &m_d3dxvRight, &mtxRotate);
		}
		
		D3DXVec3Normalize(&m_d3dxvLook, &m_d3dxvLook);
		D3DXVec3Cross(&m_d3dxvRight, &m_d3dxvUp, &m_d3dxvLook);
		D3DXVec3Normalize(&m_d3dxvRight, &m_d3dxvRight);
		D3DXVec3Cross(&m_d3dxvUp, &m_d3dxvLook, &m_d3dxvRight);
		D3DXVec3Normalize(&m_d3dxvUp, &m_d3dxvUp);
		
		D3DXMatrixRotationYawPitchRoll(&mtxRotate, (float)D3DXToRadian(y), 0, 0);
		m_d3dxmtxRotate = mtxRotate * m_d3dxmtxRotate;
}

void CPlayer::Update(float fTimeElapsed)
{
	DWORD nCurrentCameraMode = m_pCamera->GetMode();
	if (nCurrentCameraMode == THIRD_PERSON_CAMERA)
	{
		m_pCamera->Update(fTimeElapsed);
	}
	m_pCamera->RegenerateViewMatrix();
}

CCamera *CPlayer::OnChangeCamera(ID3D11Device *pd3dDevice, DWORD nNewCameraMode, DWORD nCurrentCameraMode)
{
	CCamera *pNewCamera = NULL;
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		pNewCamera = new CFirstPersonCamera(m_pCamera);
		break;
	case SPECTATOR_CAMERA:
		pNewCamera = new CSpectator(m_pCamera);
		break;
	case THIRD_PERSON_CAMERA:
		pNewCamera = new CThirdPersonCamera(m_pCamera);
		break;
	}

	if (pNewCamera)
	{
		if (!m_pCamera) pNewCamera->CreateShaderVariables(pd3dDevice);
		pNewCamera->SetMode(nNewCameraMode);
		pNewCamera->SetPlayer(this);
	}

	if (m_pCamera) delete m_pCamera;

	return(pNewCamera);
}

void CPlayer::ChangeCamera(ID3D11Device *pd3dDevice, DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) return;
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		m_bIsActive = false;
		m_pCamera = OnChangeCamera(pd3dDevice, FIRST_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(D3DXVECTOR3(0.0f, -2.0f, 10.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 500.0f, ASPECT_RATIO, 60.0f);
		break;
	case SPECTATOR_CAMERA:
		m_bIsActive = false;
		m_pCamera = OnChangeCamera(pd3dDevice, SPECTATOR_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 500.0f, ASPECT_RATIO, 60.0f);
		break;
	case THIRD_PERSON_CAMERA:
		m_bIsActive = true;
		m_pCamera = OnChangeCamera(pd3dDevice, THIRD_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.25f);
		m_pCamera->SetOffset(D3DXVECTOR3(0.0f, 2.0f, -10.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 500.0f, ASPECT_RATIO, 60.0f);
		break;
	default:
		break;
	}
	Update(fTimeElapsed);
}

void CPlayer::SetPosition(D3DXVECTOR3 d3dxvPosition)
{
	CCharacterObject::SetPosition(d3dxvPosition);
	m_pCamera->SetPosition(d3dxvPosition);
}

void CPlayer::Animate(float fTimeElapsed, PxScene *pPxScene)
{
	CCharacterObject::Animate(fTimeElapsed, pPxScene);
	if (m_pCamera)
		if (m_pCamera->GetMode() == FIRST_PERSON_CAMERA){
			PxExtendedVec3 pxvPos = m_pPxCharacterController->getPosition();
			D3DXVECTOR3 d3dxvPosition = D3DXVECTOR3(pxvPos.x, pxvPos.y+2, pxvPos.z);
			m_pCamera->SetPosition(d3dxvPosition);
		}
}