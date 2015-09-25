//-----------------------------------------------------------------------------
// File: CPlayer.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Camera.h"
#include "Player.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CPlayer

CPlayer::CPlayer()
{
	m_pCamera = NULL;

    m_d3dxvPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);             
    m_d3dxvRight = D3DXVECTOR3(1.0f, 0.0f, 0.0f);             
    m_d3dxvUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);             
    m_d3dxvLook = D3DXVECTOR3(0.0f, 0.0f, 1.0f);   

	m_d3dxvVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    m_d3dxvGravity = D3DXVECTOR3(0.0f, -100, 0.0f);
    m_fMaxVelocityXZ = 0.0f;
    m_fMaxVelocityY = 0.0f;
    m_fFriction = 0.0f;

    m_fPitch = 0.0f;             
    m_fRoll = 0.0f;              
    m_fYaw = 0.0f;               

	m_pPlayerUpdatedContext = NULL;
	m_pCameraUpdatedContext = NULL;

	m_pShader = NULL;
}

CPlayer::~CPlayer()
{
    if (m_pCamera) delete m_pCamera;
	if (ppMaterials) ppMaterials->Release();
}

void CPlayer::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
}

void CPlayer::UpdateShaderVariables(ID3D11DeviceContext *pd3dImmediateDeviceContext)
{
	if (m_pCamera) m_pCamera->UpdateShaderVariables(pd3dImmediateDeviceContext);
}

void CPlayer::RegenerateWorldMatrix()
{
	m_d3dxmtxWorld._11 = m_d3dxvRight.x; m_d3dxmtxWorld._12 = m_d3dxvRight.y; m_d3dxmtxWorld._13 = m_d3dxvRight.z; 
	m_d3dxmtxWorld._21 = m_d3dxvUp.x; m_d3dxmtxWorld._22 = m_d3dxvUp.y; m_d3dxmtxWorld._23 = m_d3dxvUp.z; 
	m_d3dxmtxWorld._31 = m_d3dxvLook.x; m_d3dxmtxWorld._32 = m_d3dxvLook.y; m_d3dxmtxWorld._33 = m_d3dxvLook.z;		
	m_d3dxmtxWorld._41 = m_d3dxvPosition.x; m_d3dxmtxWorld._42 = m_d3dxvPosition.y; m_d3dxmtxWorld._43 = m_d3dxvPosition.z;
}

void CPlayer::Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity)
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

		Move(d3dxvShift, bUpdateVelocity);
	}
}

void CPlayer::Move(const D3DXVECTOR3& d3dxvShift, bool bUpdateVelocity)
{
    if (bUpdateVelocity)
    {
        m_d3dxvVelocity += d3dxvShift;    
    } 
    else
    {
		D3DXVECTOR3 d3dxvPosition = m_d3dxvPosition + d3dxvShift;
		m_d3dxvPosition = d3dxvPosition;
		RegenerateWorldMatrix();
		m_pCamera->Move(d3dxvShift);   
    } 
}

void CPlayer::Rotate(float x, float y, float z)
{
    D3DXMATRIX mtxRotate;
	DWORD nCurrentCameraMode = m_pCamera->GetMode();
    if ((nCurrentCameraMode == FIRST_PERSON_CAMERA) || (nCurrentCameraMode == THIRD_PERSON_CAMERA))
    {
        if (x != 0.0f)
        {
            m_fPitch += x;
            if (m_fPitch > +89.0f) { x -= (m_fPitch - 89.0f); m_fPitch = +89.0f; }
            if (m_fPitch < -89.0f) { x -= (m_fPitch + 89.0f); m_fPitch = -89.0f; }
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
    } 

    D3DXVec3Normalize(&m_d3dxvLook, &m_d3dxvLook);
    D3DXVec3Cross(&m_d3dxvRight, &m_d3dxvUp, &m_d3dxvLook);
    D3DXVec3Normalize(&m_d3dxvRight, &m_d3dxvRight);
    D3DXVec3Cross(&m_d3dxvUp, &m_d3dxvLook, &m_d3dxvRight);
    D3DXVec3Normalize(&m_d3dxvUp, &m_d3dxvUp);

	RegenerateWorldMatrix();
}

void CPlayer::Update(float fTimeElapsed)
{
    m_d3dxvVelocity += m_d3dxvGravity * fTimeElapsed;
    float fLength = sqrtf(m_d3dxvVelocity.x * m_d3dxvVelocity.x + m_d3dxvVelocity.z * m_d3dxvVelocity.z);
    if (fLength > m_fMaxVelocityXZ)
    {
        m_d3dxvVelocity.x *= (m_fMaxVelocityXZ / fLength);
        m_d3dxvVelocity.z *= (m_fMaxVelocityXZ / fLength);    
    } 
    fLength = sqrtf(m_d3dxvVelocity.y * m_d3dxvVelocity.y);
    if (fLength > m_fMaxVelocityY) m_d3dxvVelocity.y *= (m_fMaxVelocityY / fLength);
	Move(m_d3dxvVelocity * fTimeElapsed, false);
	if (m_pPlayerUpdatedContext) OnPlayerUpdated(fTimeElapsed);

	DWORD nCurrentCameraMode = m_pCamera->GetMode();
    if (nCurrentCameraMode == THIRD_PERSON_CAMERA) m_pCamera->Update(fTimeElapsed);
	if (m_pCameraUpdatedContext) OnCameraUpdated(fTimeElapsed);
    if (nCurrentCameraMode == THIRD_PERSON_CAMERA) m_pCamera->SetLookAt(m_d3dxvPosition);
	m_pCamera->RegenerateViewMatrix();

    D3DXVECTOR3 d3dxvDeceleration = -m_d3dxvVelocity;
    D3DXVec3Normalize(&d3dxvDeceleration, &d3dxvDeceleration);
    fLength = D3DXVec3Length(&m_d3dxvVelocity);
    float fDeceleration = (m_fFriction * fTimeElapsed);
    if (fDeceleration > fLength) fDeceleration = fLength;
    m_d3dxvVelocity += d3dxvDeceleration * fDeceleration;
}

CCamera *CPlayer::OnChangeCamera(ID3D11Device *pd3dDevice, DWORD nNewCameraMode, DWORD nCurrentCameraMode)
{
    CCamera *pNewCamera = NULL;
    switch (nNewCameraMode)
    {
        case FIRST_PERSON_CAMERA:            
            pNewCamera = new CFirstPersonCamera(m_pCamera);
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
}

void CPlayer::Render(ID3D11DeviceContext *pd3dImmediateDeviceContext)
{
	if (m_pShader)
	{
		m_pShader->UpdateShaderVariables(pd3dImmediateDeviceContext, &m_d3dxmtxWorld);
		m_pShader->Render(pd3dImmediateDeviceContext,m_pCamera);
	}
	if (m_pMesh) m_pMesh->Render(pd3dImmediateDeviceContext);
}

void CPlayer::OnPlayerUpdated(float fTimeElapsed)
{
}

void CPlayer::OnCameraUpdated(float fTimeElapsed)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CGamePlayer

CGamePlayer::CGamePlayer(ID3D11Device *pd3dDevice)
{
	ID3D11SamplerState *pd3dSamplerState = NULL;
	D3D11_SAMPLER_DESC d3dSamplerDesc;
	ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = 0;
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dSamplerState);

	ID3D11ShaderResourceView *pd3dTexture = NULL;    
	CTexture **ppTextures = new CTexture*[3];
		ppTextures[0] = new CTexture(1);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("3.jpg"), NULL, NULL, &pd3dTexture, NULL);
	ppTextures[0]->SetTexture(0, pd3dTexture, pd3dSamplerState);
	ppTextures[1] = new CTexture(1);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("2.jpg"), NULL, NULL, &pd3dTexture, NULL);
	ppTextures[1]->SetTexture(0, pd3dTexture, pd3dSamplerState);
	ppTextures[2] = new CTexture(1);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("1.jpg"), NULL, NULL, &pd3dTexture, NULL);
	ppTextures[2]->SetTexture(0, pd3dTexture, pd3dSamplerState);
	ppMaterials=new CMaterial;
	ppMaterials->m_Material.m_d3dxcDiffuse = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f);
	ppMaterials->m_Material.m_d3dxcAmbient = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f);
	ppMaterials->m_Material.m_d3dxcSpecular = D3DXCOLOR(1.0f,1.0f,1.0f,10.0f);
	ppMaterials->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.0f,0.0f,0.0f,1.0f);
	CMesh *pPlayerMesh = new CCubeMeshIlluminatedTextured(pd3dDevice,5,5,5);
	SetMesh(pPlayerMesh);
	m_pShader = new CPlayerShader();
	m_pShader->CreateShader(pd3dDevice);
	m_pShader->CreateShaderVariables(pd3dDevice);
	

	


	SetMaterial(ppMaterials);
	SetTexture(ppTextures[2]);

	CreateShaderVariables(pd3dDevice);
	delete [] ppTextures;
	delete [] ppMaterials;
}

CGamePlayer::~CGamePlayer()
{
    if (m_pShader) delete m_pShader;
}

void CGamePlayer::Render(ID3D11DeviceContext *pd3dImmediateDeviceContext)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
    if ((nCurrentCameraMode == THIRD_PERSON_CAMERA) && m_pMesh)
	{
		D3DXMATRIX mtxRotate;
		D3DXMatrixRotationYawPitchRoll(&mtxRotate, 0.0f, (float)D3DXToRadian(90.0f), 0.0f);
		m_d3dxmtxWorld = mtxRotate * m_d3dxmtxWorld;
		m_pShader->UpdateShaderVariables(pd3dImmediateDeviceContext, &m_pMaterial->m_Material);
		m_pShader->UpdateShaderVariables(pd3dImmediateDeviceContext, m_pTexture);
		CPlayer::Render(pd3dImmediateDeviceContext);
	}
}

void CGamePlayer::ChangeCamera(ID3D11Device *pd3dDevice, DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
    if (nCurrentCameraMode == nNewCameraMode) return;
    switch (nNewCameraMode)
    {
        case FIRST_PERSON_CAMERA:            
            SetFriction(100.0f); 
            SetGravity(D3DXVECTOR3(0.0f,0.0f, 0.0f));
            SetMaxVelocityXZ(100.0f);
            SetMaxVelocityY(200.0f);
            m_pCamera = OnChangeCamera(pd3dDevice, FIRST_PERSON_CAMERA, nCurrentCameraMode);
            m_pCamera->SetTimeLag(0.0f);
            m_pCamera->SetOffset(D3DXVECTOR3(0.0f, 0.0f,0.0f));
			m_pCamera->GenerateProjectionMatrix(1.01f, 2000.0f, ASPECT_RATIO, 60.0f);
            break;
        case THIRD_PERSON_CAMERA:
            SetFriction(100.0f); 
            SetGravity(D3DXVECTOR3(0.0f,0.0f, 0.0f));
            SetMaxVelocityXZ(100.0f);
            SetMaxVelocityY(200.0f);
            m_pCamera = OnChangeCamera(pd3dDevice, THIRD_PERSON_CAMERA, nCurrentCameraMode);
            m_pCamera->SetTimeLag(0.0f);
            m_pCamera->SetOffset(D3DXVECTOR3(0.0f,0.0f,-20.0f));
			m_pCamera->GenerateProjectionMatrix(1.01f, 2000.0f, ASPECT_RATIO, 60.0f);
            break;
		default:
			break;
	}
	Update(fTimeElapsed);
}

