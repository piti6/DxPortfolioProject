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


struct ANIM_WIDTH{
	UINT WIDTH[4];
};

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

	virtual void MoveStrafe(float fDistance = 1.0f);
	virtual void MoveUp(float fDistance = 1.0f);
	virtual void MoveForward(float fDistance = 1.0f);

	virtual void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	virtual void Rotate(D3DXVECTOR3 rotation);
	virtual void Rotate(D3DXVECTOR3 *pd3dxvAxis, float fAngle);

	// Get Vector
	virtual D3DXVECTOR3 GetLookAt();
	virtual D3DXVECTOR3 GetUp();
	virtual D3DXVECTOR3 GetRight();

	virtual void Animate(float fTimeElapsed, PxScene *pPxScene);
	virtual void BuildObjects(PxPhysics *pPxPhysics, PxScene *pPxScene, PxMaterial *pPxMaterial);

	virtual void UpdateAnimation(ID3D11DeviceContext *pd3dImmediateDeviceContext);
	virtual void Render(ID3D11DeviceContext *pd3dImmediateDeviceContext);

protected:
	bool							m_bIsActive;

	vector<CMesh*>					m_MeshesVector;

	CMaterial						*m_pMaterial;
	CTexture						*m_pTexture;

	D3DXMATRIX						m_d3dxmtxWorld;

	D3DXVECTOR3						m_d3dxvOffset;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//

class CDynamicObject : public CGameObject
{
public:
	CDynamicObject(bool _HasAnimation = false);
	virtual ~CDynamicObject();

	virtual void SetActive(bool isActive);

	virtual void BuildObjects(PxPhysics *pPxPhysics, PxScene *pPxScene, PxMaterial *pPxMaterial);
	virtual void Animate(float fTimeElapsed, PxScene *pPxScene);

	virtual void SetPosition(D3DXVECTOR3 d3dxvPosition);

	virtual void MoveStrafe(float fDistance = 1.0f);
	virtual void MoveUp(float fDistance = 1.0f);
	virtual void MoveForward(float fDistance = 1.0f);

	virtual void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);

	void AddForce(float fx, float fy, float fz);

	CAnimationController* GetAnimationController(){ return &m_AnimationController; }


private:
	CAnimationController		m_AnimationController;

	PxRigidDynamic				*m_pPxActor;
	PxMaterial					*m_pPxMaterial;

	/////////////// Animation Part ///////////////

	bool						m_bHasAnimation;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//

class CStaticObject : public CGameObject
{
public:
	CStaticObject();
	virtual ~CStaticObject();

	virtual void BuildObjects(PxPhysics *pPxPhysics, PxScene *pPxScene, PxMaterial *pPxMaterial);
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

////////////////////////////////////////////////////////////////////////////////////////////////////
//

class CHeightMapTerrain : public CGameObject
{
public:
	CHeightMapTerrain(ID3D11Device *pd3dDevice, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, D3DXVECTOR3 d3dxvScale);
	virtual ~CHeightMapTerrain();

private:
	CHeightMap					*m_pHeightMap;

	int							m_nWidth;
	int							m_nLength;

	D3DXVECTOR3					m_d3dxvScale;

public:
	float GetHeight(float x, float z, bool bReverseQuad = false) { return(m_pHeightMap->GetHeight(x, z, bReverseQuad) * m_d3dxvScale.y); } //World
	D3DXVECTOR3 GetNormal(float x, float z) { return(m_pHeightMap->GetHeightMapNormal(int(x / m_d3dxvScale.x), int(z / m_d3dxvScale.z))); }

	int GetHeightMapWidth() { return(m_pHeightMap->GetHeightMapWidth()); }
	int GetHeightMapLength() { return(m_pHeightMap->GetHeightMapLength()); }

	D3DXVECTOR3 GetScale() { return(m_d3dxvScale); }
	float GetWidth() { return(m_nWidth * m_d3dxvScale.x); }
	float GetLength() { return(m_nLength * m_d3dxvScale.z); }

	//float GetPeakHeight() { return(m_bcMeshBoundingCube.m_d3dxvMaximum.y); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//