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
	CTexture *m_pTexture;
	void SetTexture(CTexture *pTexture);


    D3DXMATRIX						m_d3dxmtxWorld;         
	CMesh							*m_pMesh;
	CMaterial						*m_pMaterial;
	void SetMaterial(CMaterial *pMaterial);


	virtual void SetMesh(CMesh *pMesh);
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dImmediateDeviceContext, CCamera *pCamera=NULL);

	virtual void SetPosition(float x, float y, float z);
	virtual void SetPosition(D3DXVECTOR3 d3dxvPosition);

	virtual void MoveStrafe(float fDistance=1.0f);
	virtual void MoveUp(float fDistance=1.0f);
	virtual void MoveForward(float fDistance=1.0f);

	virtual void Rotate(float fPitch=10.0f, float fYaw=10.0f, float fRoll=10.0f);
	virtual void Rotate(D3DXVECTOR3 *pd3dxvAxis, float fAngle);

	D3DXVECTOR3 GetPosition();
	D3DXVECTOR3 GetLookAt();
	D3DXVECTOR3 GetUp();
	D3DXVECTOR3 GetRight();
};

class CRotatingObject : public CGameObject
{
public:
	CRotatingObject();
    virtual ~CRotatingObject();

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dImmediateDeviceContext, CCamera *pCamera=NULL);

	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }	
	void SetRotationAxis(D3DXVECTOR3 d3dxvRotationAxis) { m_d3dxvRotationAxis = d3dxvRotationAxis; }	

private:
	D3DXVECTOR3					m_d3dxvRotationAxis;
	float						m_fRotationSpeed;
};

class CRevolvingObject : public CGameObject
{
public:
	CRevolvingObject();
	virtual ~CRevolvingObject();

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera=NULL);

// 공전 속력을 설정한다.
	void SetRevolutionSpeed(float fRevolutionSpeed) { m_fRevolutionSpeed = fRevolutionSpeed; }
// 공전을 위한 회전축을 설정한다.
	void SetRevolutionAxis(D3DXVECTOR3 d3dxvRevolutionAxis) { m_d3dxvRevolutionAxis = d3dxvRevolutionAxis; }

private:
// 공전 회전축과 회전 속력을 나타낸다.
	D3DXVECTOR3 m_d3dxvRevolutionAxis;
	float m_fRevolutionSpeed;
};
