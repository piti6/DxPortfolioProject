#pragma once

#include "Timer.h"
#include "Camera.h"
#include "Object.h"
#include "Shader.h"

class CPlayer : public CGameObject
{
protected:

	D3DXVECTOR3					m_d3dxvRight;             
	D3DXVECTOR3					m_d3dxvUp;             
	D3DXVECTOR3					m_d3dxvLook;   

	float           			m_fPitch;             
	float           			m_fYaw;               
	float           			m_fRoll;              


	D3DXVECTOR3     			m_d3dxvGravity;           
	float           			m_fMaxVelocityXZ;     
	float           			m_fMaxVelocityY;      
	float           			m_fFriction;    

	LPVOID						m_pPlayerUpdatedContext;
	LPVOID						m_pCameraUpdatedContext;

	CCamera						*m_pCamera;

	CTexturedIlluminatedShader	*m_pShader;

public:
	D3DXVECTOR3					m_d3dxvPosition;             
	D3DXVECTOR3					m_d3dxvVelocity;   
	CPlayer();	
	~CPlayer();
	D3DXVECTOR3 GetPosition() { return(m_d3dxvPosition); }
	D3DXVECTOR3 GetLookVector() { return(m_d3dxvLook); }
	D3DXVECTOR3 GetUpVector() { return(m_d3dxvUp); }
	D3DXVECTOR3 GetRightVector() { return(m_d3dxvRight); }

	void SetFriction(float fFriction) { m_fFriction = fFriction; }
	void SetGravity(const D3DXVECTOR3& d3dxvGravity) { m_d3dxvGravity = d3dxvGravity; }
	void SetMaxVelocityXZ(float fMaxVelocity) { m_fMaxVelocityXZ = fMaxVelocity; }
	void SetMaxVelocityY(float fMaxVelocity) { m_fMaxVelocityY = fMaxVelocity; }
	void SetVelocity(const D3DXVECTOR3& d3dxvVelocity) { m_d3dxvVelocity = d3dxvVelocity; }
	void SetPosition(const D3DXVECTOR3& d3dxvPosition) { Move((d3dxvPosition-m_d3dxvPosition), false); }

	const D3DXVECTOR3& GetVelocity() const { return(m_d3dxvVelocity); }
	float GetYaw() const { return(m_fYaw); }
	float GetPitch() const { return(m_fPitch); }
	float GetRoll() const { return(m_fRoll); }

	CCamera *GetCamera() { return(m_pCamera); }
	void SetCamera(CCamera *pCamera) { m_pCamera = pCamera; }

	void Move(ULONG nDirection, float fDistance, bool bVelocity = false);
	void Move(const D3DXVECTOR3& d3dxvShift, bool bVelocity = false);
	void Move(float fxOffset=0.0f, float fyOffset=0.0f, float fzOffset=0.0f);
	void Rotate(float x, float y, float z);

	void Update(float fTimeElapsed);

	void RegenerateWorldMatrix();

	void SetPlayerUpdatedContext(LPVOID pContext) { m_pPlayerUpdatedContext = pContext; }
	void SetCameraUpdatedContext(LPVOID pContext) { m_pCameraUpdatedContext = pContext; }

	virtual void CreateShaderVariables(ID3D11Device *pd3dDevice);
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dImmediateDeviceContext);

	CCamera *OnChangeCamera(ID3D11Device *pd3dDevice, DWORD nNewCameraMode, DWORD nCurrentCameraMode);

	virtual void ChangeCamera(ID3D11Device *pd3dDevice, DWORD nNewCameraMode, float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dImmediateDeviceContext, int nThreadID, CRITICAL_SECTION *pCriticalSection);

	virtual void OnPlayerUpdated(float fTimeElapsed);
	virtual void OnCameraUpdated(float fTimeElapsed);
};

class CGamePlayer : public CPlayer
{
public:
	CGamePlayer(ID3D11Device *pd3dDevice);
	~CGamePlayer();

	virtual void ChangeCamera(ID3D11Device *pd3dDevice, DWORD nNewCameraMode, float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dImmediateDeviceContext, int nThreadID, CRITICAL_SECTION *pCriticalSection);
};

