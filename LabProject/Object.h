//------------------------------------------------------- ----------------------
// File: Object.h
//-----------------------------------------------------------------------------

#pragma once

#include "Timer.h"
#include "Mesh.h"
#include "Camera.h"
#include "Texture.h"
#include "Material.h"

#define DIR_FORWARD				0x01
#define DIR_BACKWARD			0x02
#define DIR_LEFT				0x04
#define DIR_RIGHT				0x08
#define DIR_UP					0x10
#define DIR_DOWN				0x20





class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();

	bool isActive();

	virtual void SetActive(bool isActive);
	virtual void SetPosition(D3DXVECTOR3 d3dxvPosition);
	
	void SetMaterial(CMaterial *pMaterial);
	void SetTexture(CTexture *pTexture);
	void SetMesh(CMesh *pMesh);

	CMaterial*			GetMaterial();
	CTexture*			GetTexture();
	CMesh*				GetMesh();
	D3DXMATRIX			GetWorldMatrix();
	virtual D3DXVECTOR3 GetPosition();
	
	virtual void MoveStrafe(float fDistance=1.0f);
	virtual void MoveUp(float fDistance=1.0f);
	virtual void MoveForward(float fDistance=1.0f);

	virtual void Rotate(float fPitch=10.0f, float fYaw=10.0f, float fRoll=10.0f);
	virtual void Rotate(D3DXVECTOR3 *pd3dxvAxis, float fAngle);

	// Get Vector
	virtual D3DXVECTOR3 GetLookAt();
	virtual D3DXVECTOR3 GetUp();
	virtual D3DXVECTOR3 GetRight();

	virtual void Animate(float fTimeElapsed,PxScene *pPxScene);
	virtual void BuildObjects(PxPhysics *pPxPhysics, PxScene *pPxScene);

	virtual void Render(ID3D11DeviceContext *pd3dImmediateDeviceContext, CCamera *pCamera=NULL);

protected:
	bool							m_bIsActive;

	CMesh							*m_pMesh;
	CMaterial						*m_pMaterial;
	CTexture						*m_pTexture;

	D3DXMATRIX						m_d3dxmtxWorld;  
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//

class CDynamicObject : public CGameObject
{
public:
	CDynamicObject();
	virtual ~CDynamicObject();

	virtual void SetActive(bool isActive);

	virtual void BuildObjects(PxPhysics *pPxPhysics, PxScene *pPxScene);
	virtual void Animate(float fTimeElapsed,PxScene *pPxScene);

	virtual void SetPosition(D3DXVECTOR3 d3dxvPosition);

	virtual void MoveStrafe(float fDistance=1.0f);
	virtual void MoveUp(float fDistance=1.0f);
	virtual void MoveForward(float fDistance=1.0f);

	virtual void Rotate(float fPitch=10.0f, float fYaw=10.0f, float fRoll=10.0f);

	void AddForce(float fx, float fy, float fz);

private:
	PxRigidDynamic				*m_pPxActor;
	PxMaterial					*m_pPxMaterial;	
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//

class CStaticObject : public CGameObject
{
public:
	CStaticObject();
	virtual ~CStaticObject();

	virtual void BuildObjects(PxPhysics *pPxPhysics, PxScene *pPxScene);

	virtual void SetPosition(D3DXVECTOR3 d3dxvPosition);

private:
	PxRigidStatic				*m_pPxActor;
	PxMaterial					*m_pPxMaterial;	
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//

class CSkyBox : public CGameObject
{
public:
	CSkyBox();
	virtual ~CSkyBox();
};