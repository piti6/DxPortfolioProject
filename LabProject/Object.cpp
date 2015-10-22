//-----------------------------------------------------------------------------
// File: CGameObject.cpp
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "Object.h"
#include "FbxToDxTranslation.h"

CGameObject::CGameObject()
{
    D3DXMatrixIdentity(&m_d3dxmtxWorld);
	m_pMesh = NULL;
	m_pMaterial = NULL;
	m_pTexture = NULL;
	
}

CGameObject::~CGameObject()
{
	if (m_pMesh)			m_pMesh->Release();
	if (m_pMaterial)		m_pMaterial->Release();
	if (m_pTexture)			m_pTexture->Release();
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
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

void CGameObject::BuildObjects(PxPhysics *pPxPhysics, PxScene *pPxScene){

}

void CGameObject::Animate(float fTimeElapsed,PxScene *pPxScene)
{
}

void CGameObject::Render(ID3D11DeviceContext *pd3dImmediateDeviceContext, CCamera *pCamera)
{
	if (m_pMesh) m_pMesh->Render(pd3dImmediateDeviceContext);
}

void CGameObject::SetPosition(float x, float y, float z) 
{ 
	m_d3dxmtxWorld._41 = x; 
	m_d3dxmtxWorld._42 = y;
	m_d3dxmtxWorld._43 = z;
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
	CGameObject::SetPosition(d3dxvPosition);
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

////////////////////////////////////////////////////////////////////////////////////////////////////
//

CStaticObject::CStaticObject(){
	m_pPxMaterial = NULL;
	m_pPxActor = NULL;
}

CStaticObject::~CStaticObject(){
	if(m_pPxMaterial) m_pPxMaterial->release();
	if(m_pPxActor) m_pPxActor->release();
}

void CStaticObject::BuildObjects(PxPhysics *pPxPhysics, PxScene *pPxScene){

	m_pPxMaterial = pPxPhysics->createMaterial(0.1,0.1,1);
	PxTransform _PxTransform(GetPosition().x,GetPosition().y,GetPosition().z);

	D3DXVECTOR3 _d3dxvBoundMinimum = m_pMesh->m_bcBoundingCube.GetMinimum();
	D3DXVECTOR3 _d3dxvBoundMaximum = m_pMesh->m_bcBoundingCube.GetMaximum();
	D3DXVECTOR3 _d3dxvExtents = 
		D3DXVECTOR3((abs(_d3dxvBoundMinimum.x) + abs(_d3dxvBoundMaximum.x))/2,(abs(_d3dxvBoundMinimum.y) + abs(_d3dxvBoundMaximum.y))/2,(abs(_d3dxvBoundMinimum.z) + abs(_d3dxvBoundMaximum.z))/2);
	PxBoxGeometry _PxBoxGeometry(_d3dxvExtents.x,_d3dxvExtents.y,_d3dxvExtents.z);

	m_pPxActor = PxCreateStatic(*pPxPhysics,_PxTransform,_PxBoxGeometry,*m_pPxMaterial);
	pPxScene->addActor(*m_pPxActor);

	PxTransform pT = m_pPxActor->getGlobalPose();
	PxMat44 m = PxMat44(pT);
	m_d3dxmtxWorld = D3DXMATRIX(m.front());	
	
}

void CStaticObject::Animate(float fTimeElapsed,PxScene *pPxScene)
{
	PxTransform pT = m_pPxActor->getGlobalPose();
	PxMat44 m = PxMat44(pT);
	m_d3dxmtxWorld = D3DXMATRIX(m.front());	
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//

CDynamicObject::CDynamicObject(){
	m_pPxMaterial = NULL;
	m_pPxActor = NULL;
}

CDynamicObject::~CDynamicObject(){
	if(m_pPxMaterial) m_pPxMaterial->release();
	if(m_pPxActor) m_pPxActor->release();
}

void CDynamicObject::BuildObjects(PxPhysics *pPxPhysics, PxScene *pPxScene){

	m_pPxMaterial = pPxPhysics->createMaterial(0.1,0.1,1);
	PxTransform _PxTransform(GetPosition().x,GetPosition().y,GetPosition().z);

	D3DXVECTOR3 _d3dxvBoundMinimum = m_pMesh->m_bcBoundingCube.GetMinimum();
	D3DXVECTOR3 _d3dxvBoundMaximum = m_pMesh->m_bcBoundingCube.GetMaximum();
	D3DXVECTOR3 _d3dxvExtents = 
		D3DXVECTOR3((abs(_d3dxvBoundMinimum.x) + abs(_d3dxvBoundMaximum.x))/2,(abs(_d3dxvBoundMinimum.y) + abs(_d3dxvBoundMaximum.y))/2,(abs(_d3dxvBoundMinimum.z) + abs(_d3dxvBoundMaximum.z))/2);
	PxBoxGeometry _PxBoxGeometry(_d3dxvExtents.x,_d3dxvExtents.y,_d3dxvExtents.z);
	m_pPxActor = PxCreateDynamic(*pPxPhysics,_PxTransform,_PxBoxGeometry,*m_pPxMaterial,2.0f);
	pPxScene->addActor(*m_pPxActor);

	PxTransform pT = m_pPxActor->getGlobalPose();
	PxMat44 m = PxMat44(pT);
	m_d3dxmtxWorld = D3DXMATRIX(m.front());	
}

void CDynamicObject::Animate(float fTimeElapsed,PxScene *pPxScene)
{
	PxTransform pT = m_pPxActor->getGlobalPose();
	PxMat44 m = PxMat44(pT);
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

void CDynamicObject::SetPosition(float x, float y, float z) 
{ 
	if(m_pPxActor)
		m_pPxActor->setGlobalPose(PxTransform(PxVec3(x,y,z)));
	else
		CGameObject::SetPosition(x,y,z);
}

void CDynamicObject::SetPosition(D3DXVECTOR3 d3dxvPosition) 
{
	if(m_pPxActor)
		m_pPxActor->setGlobalPose(PxTransform(PxVec3(d3dxvPosition.x,d3dxvPosition.y,d3dxvPosition.z)));
	else
		CGameObject::SetPosition(d3dxvPosition);
}

D3DXVECTOR3 CDynamicObject::GetPosition() 
{ 
	D3DXVECTOR3 _d3dxvPosition;
	if(m_pPxActor)
		_d3dxvPosition = D3DXVECTOR3(m_pPxActor->getGlobalPose().p.x,m_pPxActor->getGlobalPose().p.y,m_pPxActor->getGlobalPose().p.z);
	else
		_d3dxvPosition = CGameObject::GetPosition();
	return _d3dxvPosition;
}

void CDynamicObject::Rotate(float fPitch, float fYaw, float fRoll){
	m_pPxActor->setAngularVelocity(PxVec3(fPitch,fYaw,fRoll));
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

