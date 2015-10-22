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
	
    D3DXMATRIX						m_d3dxmtxWorld;  
	
	CMesh							*m_pMesh;
	CMaterial						*m_pMaterial;
	CTexture						*m_pTexture;

	virtual void Animate(float fTimeElapsed,PxScene *pPxScene);
	virtual void BuildObjects(PxPhysics *pPxPhysics, PxScene *pPxScene);

	void SetMaterial(CMaterial *pMaterial);
	void SetTexture(CTexture *pTexture);
	void SetMesh(CMesh *pMesh);
	
	virtual void Render(ID3D11DeviceContext *pd3dImmediateDeviceContext, CCamera *pCamera=NULL);

	virtual void SetPosition(float x, float y, float z);
	virtual void SetPosition(D3DXVECTOR3 d3dxvPosition);

	virtual void MoveStrafe(float fDistance=1.0f);
	virtual void MoveUp(float fDistance=1.0f);
	virtual void MoveForward(float fDistance=1.0f);

	virtual void Rotate(float fPitch=10.0f, float fYaw=10.0f, float fRoll=10.0f);
	virtual void Rotate(D3DXVECTOR3 *pd3dxvAxis, float fAngle);

	virtual D3DXVECTOR3 GetPosition();
	virtual D3DXVECTOR3 GetLookAt();
	virtual D3DXVECTOR3 GetUp();
	virtual D3DXVECTOR3 GetRight();
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//

class CDynamicObject : public CGameObject
{
public:
	CDynamicObject();
    virtual ~CDynamicObject();

	virtual void BuildObjects(PxPhysics *pPxPhysics, PxScene *pPxScene);
	virtual void Animate(float fTimeElapsed,PxScene *pPxScene);

	virtual void SetPosition(float x, float y, float z);
	virtual void SetPosition(D3DXVECTOR3 d3dxvPosition);

	virtual void MoveStrafe(float fDistance=1.0f);
	virtual void MoveUp(float fDistance=1.0f);
	virtual void MoveForward(float fDistance=1.0f);

	virtual void Rotate(float fPitch=10.0f, float fYaw=10.0f, float fRoll=10.0f);

	virtual D3DXVECTOR3 GetPosition();

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
	virtual void Animate(float fTimeElapsed,PxScene *pPxScene);

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