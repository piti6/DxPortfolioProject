//------------------------------------------------------- ----------------------
// File: Object.h
//-----------------------------------------------------------------------------

#pragma once

#include "Timer.h"
#include "Mesh.h"
#include "Camera.h"
#include "Texture.h"
#include "Material.h"
#include "AnimationController.h"

#define DIR_FORWARD				0x01
#define DIR_BACKWARD			0x02
#define DIR_LEFT				0x04
#define DIR_RIGHT				0x08
#define DIR_UP					0x10
#define DIR_DOWN				0x20

//アニメーションインスタンス化をする時に使います。アニメーションテクスチャの長さを更新します。
struct ANIM_WIDTH{
	UINT WIDTH[4];
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//基本的なオブジェクトです。テクスチャとマテリアル、メッシュを持っています。

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
	void SetOffset(D3DXVECTOR3 _Offset);

	CMaterial*			GetMaterial();
	CTexture*			GetTexture();
	CMesh*				GetMesh(int _Index);
	D3DXVECTOR3			GetOffset();

	D3DXMATRIX			GetWorldMatrix();
	virtual D3DXVECTOR3 GetPosition();

	virtual void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	virtual void Rotate(D3DXVECTOR3 rotation);
	virtual void Rotate(D3DXVECTOR3 *pd3dxvAxis, float fAngle);

	// Get Vector
	virtual D3DXVECTOR3 GetLookAt();
	virtual D3DXVECTOR3 GetUp();
	virtual D3DXVECTOR3 GetRight();

	virtual void Animate(float fTimeElapsed, PxScene *pPxScene);
	virtual void BuildObjects(PxPhysics *pPxPhysics, PxScene *pPxScene, PxMaterial *pPxMaterial);

	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);

protected:
	bool							m_bIsActive;

	vector<CMesh*>					m_MeshesVector;

	CMaterial						*m_pMaterial;
	CTexture						*m_pTexture;

	D3DXMATRIX						m_d3dxmtxWorld;

	D3DXVECTOR3						m_d3dxvOffset;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//動いているオブジェクトです。Physx関連関数とアニメーションを持っています。

class CDynamicObject : public CGameObject
{
public:
	CDynamicObject(bool _HasAnimation = false);
	virtual ~CDynamicObject();

	virtual void SetActive(bool isActive);

	virtual void BuildObjects(PxPhysics *pPxPhysics, PxScene *pPxScene, PxMaterial *pPxMaterial);
	virtual void Animate(float fTimeElapsed, PxScene *pPxScene);

	virtual void SetPosition(D3DXVECTOR3 d3dxvPosition);

	virtual void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);

	void AddForce(float fx, float fy, float fz);

	CAnimationController* GetAnimationController(){ return &m_AnimationController; }

protected:
	PxRigidDynamic				*m_pPxActor;

	/////////////// Animation Part ///////////////
	bool						m_bHasAnimation;
	CAnimationController		m_AnimationController;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//止まっているオブジェクトです。Physx関連関数を持っています。マップに使います。

class CStaticObject : public CGameObject
{
public:
	CStaticObject();
	virtual ~CStaticObject();

	virtual void BuildObjects(PxPhysics *pPxPhysics, PxScene *pPxScene, PxMaterial *pPxMaterial, const char* name);
	virtual void SetPosition(D3DXVECTOR3 d3dxvPosition);

private:
	PxRigidStatic				*m_pPxActor;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//キャラクターオブジェクトです。

class CCharacterObject : public CDynamicObject
{
public:
	CCharacterObject(bool _HasAnimation = false);
	virtual ~CCharacterObject();

	virtual void SetActive(bool isActive);

	virtual void BuildObjects(PxPhysics *pPxPhysics, PxScene *pPxScene, PxMaterial *pPxMaterial, PxControllerManager *pPxControllerManager);
	virtual void Animate(float fTimeElapsed, PxScene *pPxScene);

	virtual void SetPosition(D3DXVECTOR3 d3dxvPosition);

	virtual void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	virtual void Rotate(D3DXVECTOR3 rotation);
	virtual void Rotate(D3DXVECTOR3 *pd3dxvAxis, float fAngle);
	void SetRotation(D3DXVECTOR3 d3dxvRotation);
	void RotateOffset(float fPitch, float fYaw, float fRoll);

protected:
	D3DXMATRIX					m_d3dxmtxRotate;
	D3DXMATRIX					m_d3dxmtxRotateOffset;

	PxController				*m_pPxCharacterController;

};

////////////////////////////////////////////////////////////////////////////////////////////////////
//スカイボックスです。

class CSkyBox : public CGameObject
{
public:
	CSkyBox();
	virtual ~CSkyBox();
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//プレイヤーです。CCharacterObjectにカメラを追加しました。

class CPlayer : public CCharacterObject
{
public:
	CPlayer();
	~CPlayer();

	virtual void CreateShaderVariables(ID3D11Device *pd3dDevice);
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext);

	void Animate(float fTimeElapsed, PxScene *pPxScene);

	void Move(ULONG nDirection, float fDistance, float fTimeElapsed);
	void Move(const D3DXVECTOR3& d3dxvShift, float fTimeElapsed);
	void Move(float fxOffset = 0.0f, float fyOffset = 0.0f, float fzOffset = 0.0f);

	void Rotate(float x, float y, float z);

	D3DXVECTOR3 GetPosition() { return(D3DXVECTOR3(m_pPxCharacterController->getPosition().x, m_pPxCharacterController->getPosition().y, m_pPxCharacterController->getPosition().z)); }
	D3DXVECTOR3 GetRotation() { return D3DXVECTOR3(m_d3dxvRotation.x,0,0); }
	void SetPosition(D3DXVECTOR3 d3dxvPosition);

	D3DXVECTOR3 GetLookVector() { return(m_d3dxvLook); }
	D3DXVECTOR3 GetUpVector() { return(m_d3dxvUp); }
	D3DXVECTOR3 GetRightVector() { return(m_d3dxvRight); }

	CCamera *GetCamera() { return(m_pCamera); }
	void SetCamera(CCamera *pCamera) { m_pCamera = pCamera; }

	CCamera *OnChangeCamera(ID3D11Device *pd3dDevice, DWORD nNewCameraMode, DWORD nCurrentCameraMode);
	virtual void ChangeCamera(ID3D11Device *pd3dDevice, DWORD nNewCameraMode, float fTimeElapsed);

	GET_SET_FUNC_IMPL(string, Name, m_Name);

private:
	string						m_Name;

	D3DXVECTOR3					m_d3dxvRight;
	D3DXVECTOR3					m_d3dxvUp;
	D3DXVECTOR3					m_d3dxvLook;

	float           			m_fPitch;
	float           			m_fYaw;
	float           			m_fRoll;

	CCamera						*m_pCamera;

	D3DXVECTOR3					m_d3dxvPosition;
	D3DXVECTOR3					m_d3dxvRotation;
};