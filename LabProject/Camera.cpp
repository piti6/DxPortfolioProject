#include "stdafx.h"
#include "Camera.h"
#include "Object.h"


CCamera::CCamera(CCamera *pCamera) 
{
	if (pCamera)
	{
		m_d3dxvPosition = pCamera->GetPosition();
		m_d3dxmtxView = pCamera->GetViewMatrix();
		m_d3dxmtxProjection = pCamera->GetProjectionMatrix();
		m_d3dViewport = pCamera->GetViewport();
		m_d3dxvLookAtWorld = pCamera->GetLookAtPosition();
		m_d3dxvOffset = pCamera->GetOffset();
		m_fTimeLag = pCamera->GetTimeLag();
		m_pPlayer = pCamera->GetPlayer();
		if (m_pPlayer){
			m_d3dxvRight = m_pPlayer->GetRightVector();
			m_d3dxvUp = m_pPlayer->GetUpVector();
			m_d3dxvLook = m_pPlayer->GetLookVector();
		}
		/*
		m_d3dxvRight = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
		m_d3dxvUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		m_d3dxvLook = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
		*/
		m_pd3dcbViewProjection = pCamera->GetViewProjectionConstantBuffer();
		if (m_pd3dcbViewProjection) m_pd3dcbViewProjection->AddRef();
	}
	else
	{
		m_d3dxvPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f); 
		m_d3dxvRight = D3DXVECTOR3(1.0f, 0.0f, 0.0f); 
		m_d3dxvUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f); 
		m_d3dxvLook = D3DXVECTOR3(0.0f, 0.0f, 1.0f); 

		m_fTimeLag = 0.0f;

		m_d3dxvLookAtWorld = D3DXVECTOR3(0.0f, 0.0f, 0.0f); 
		m_d3dxvOffset = D3DXVECTOR3(0.0f, 0.0f, 0.0f); 

		m_nMode = 0x00;

		m_pPlayer = NULL;

		D3DXMatrixIdentity(&m_d3dxmtxView);
		D3DXMatrixIdentity(&m_d3dxmtxProjection);

		m_pd3dcbViewProjection = NULL;
	}
}

CCamera::~CCamera() 
{ 
    if (m_pd3dcbViewProjection) m_pd3dcbViewProjection->Release();
}

void CCamera::SetViewport(ID3D11DeviceContext *pd3dImmediateDeviceContext, DWORD xTopLeft, DWORD yTopLeft, DWORD nWidth, DWORD nHeight, float fMinZ, float fMaxZ)
{
    m_d3dViewport.TopLeftX = float(xTopLeft);
    m_d3dViewport.TopLeftY = float(yTopLeft);
    m_d3dViewport.Width = float(nWidth);
    m_d3dViewport.Height = float(nHeight);
    m_d3dViewport.MinDepth = fMinZ;
    m_d3dViewport.MaxDepth = fMaxZ;
    pd3dImmediateDeviceContext->RSSetViewports(1, &m_d3dViewport);
}

void CCamera::GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle)
{
	D3DXMatrixPerspectiveFovLH(&m_d3dxmtxProjection, (float)D3DXToRadian(fFOVAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
}

void CCamera::GenerateViewMatrix()
{
	D3DXMatrixLookAtLH(&m_d3dxmtxView, &m_d3dxvPosition, &m_d3dxvLookAtWorld, &m_d3dxvUp);
}

void CCamera::RegenerateViewMatrix()
{
    D3DXVec3Normalize(&m_d3dxvLook, &m_d3dxvLook);
    D3DXVec3Cross(&m_d3dxvRight, &m_d3dxvUp, &m_d3dxvLook);
    D3DXVec3Normalize(&m_d3dxvRight, &m_d3dxvRight);
    D3DXVec3Cross(&m_d3dxvUp, &m_d3dxvLook, &m_d3dxvRight);
    D3DXVec3Normalize(&m_d3dxvUp, &m_d3dxvUp);
    m_d3dxmtxView._11 = m_d3dxvRight.x; m_d3dxmtxView._12 = m_d3dxvUp.x; m_d3dxmtxView._13 = m_d3dxvLook.x;
    m_d3dxmtxView._21 = m_d3dxvRight.y; m_d3dxmtxView._22 = m_d3dxvUp.y; m_d3dxmtxView._23 = m_d3dxvLook.y;
    m_d3dxmtxView._31 = m_d3dxvRight.z; m_d3dxmtxView._32 = m_d3dxvUp.z; m_d3dxmtxView._33 = m_d3dxvLook.z;
    m_d3dxmtxView._41 =- D3DXVec3Dot(&m_d3dxvPosition, &m_d3dxvRight);
    m_d3dxmtxView._42 =- D3DXVec3Dot(&m_d3dxvPosition, &m_d3dxvUp);
    m_d3dxmtxView._43 =- D3DXVec3Dot(&m_d3dxvPosition, &m_d3dxvLook);

//카메라의 위치와 방향이 바뀌면(카메라 변환 행렬이 바뀌면) 절두체 평면을 다시 계산한다.
	CalculateFrustumPlanes();
}

void CCamera::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VS_CB_VIEWPROJECTION_MATRIX);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbViewProjection);
}

void CCamera::UpdateShaderVariables(ID3D11DeviceContext *pd3dImmediateDeviceContext)
{
    D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
    pd3dImmediateDeviceContext->Map(m_pd3dcbViewProjection, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
    VS_CB_VIEWPROJECTION_MATRIX *pcbViewProjection = (VS_CB_VIEWPROJECTION_MATRIX *)d3dMappedResource.pData;
    D3DXMatrixTranspose(&pcbViewProjection->m_d3dxmtxView, &m_d3dxmtxView);
    D3DXMatrixTranspose(&pcbViewProjection->m_d3dxmtxProjection, &m_d3dxmtxProjection);
    pd3dImmediateDeviceContext->Unmap(m_pd3dcbViewProjection, 0);

    pd3dImmediateDeviceContext->VSSetConstantBuffers(VS_SLOT_VIEWPROJECTION_MATRIX, 1, &m_pd3dcbViewProjection);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CFirstPersonCamera

CFirstPersonCamera::CFirstPersonCamera(CCamera *pCamera) : CCamera(pCamera)
{
    m_nMode = FIRST_PERSON_CAMERA;
}

void CFirstPersonCamera::Rotate(float x, float y, float z)
{
    D3DXMATRIX mtxRotate;
    if (x != 0.0f) 
    {
        D3DXMatrixRotationAxis(&mtxRotate, &m_d3dxvRight, (float)D3DXToRadian(x));        
        D3DXVec3TransformNormal(&m_d3dxvLook, &m_d3dxvLook, &mtxRotate);
        D3DXVec3TransformNormal(&m_d3dxvUp, &m_d3dxvUp, &mtxRotate);
        D3DXVec3TransformNormal(&m_d3dxvRight, &m_d3dxvRight, &mtxRotate);
    } 
    if (m_pPlayer && (y != 0.0f))
    {
        D3DXMatrixRotationAxis(&mtxRotate, &m_pPlayer->GetUpVector(), (float)D3DXToRadian(y));
        D3DXVec3TransformNormal(&m_d3dxvLook, &m_d3dxvLook, &mtxRotate);
        D3DXVec3TransformNormal(&m_d3dxvUp, &m_d3dxvUp, &mtxRotate);
        D3DXVec3TransformNormal(&m_d3dxvRight, &m_d3dxvRight, &mtxRotate);   
    } 
    if (m_pPlayer && (z != 0.0f))
    {
        D3DXMatrixRotationAxis(&mtxRotate, &m_pPlayer->GetLookVector(), (float)D3DXToRadian(z));
        m_d3dxvPosition -= m_pPlayer->GetPosition();
        D3DXVec3TransformCoord (&m_d3dxvPosition, &m_d3dxvPosition, &mtxRotate);
        m_d3dxvPosition += m_pPlayer->GetPosition();
        D3DXVec3TransformNormal(&m_d3dxvLook, &m_d3dxvLook, &mtxRotate);
        D3DXVec3TransformNormal(&m_d3dxvUp, &m_d3dxvUp, &mtxRotate);
        D3DXVec3TransformNormal(&m_d3dxvRight, &m_d3dxvRight, &mtxRotate);    
    } 
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CThirdPersonCamera

CThirdPersonCamera::CThirdPersonCamera(CCamera *pCamera) : CCamera(pCamera)
{
    m_nMode = THIRD_PERSON_CAMERA;
}

void CThirdPersonCamera::Update(float fTimeElapsed)
{
    if (m_pPlayer) 
	{
		D3DXMATRIX mtxRotate;
		D3DXMatrixIdentity(&mtxRotate);
		D3DXVECTOR3 d3dxvRight = m_pPlayer->GetRightVector();
		D3DXVECTOR3 d3dxvUp = m_pPlayer->GetUpVector();
		D3DXVECTOR3 d3dxvLook = m_pPlayer->GetLookVector();
		mtxRotate._11 = d3dxvRight.x; mtxRotate._21 = d3dxvUp.x; mtxRotate._31 = d3dxvLook.x;
		mtxRotate._12 = d3dxvRight.y; mtxRotate._22 = d3dxvUp.y; mtxRotate._32 = d3dxvLook.y;
		mtxRotate._13 = d3dxvRight.z; mtxRotate._23 = d3dxvUp.z; mtxRotate._33 = d3dxvLook.z;

		D3DXVECTOR3 d3dxvOffset;
		D3DXVec3TransformCoord(&d3dxvOffset, &m_d3dxvOffset, &mtxRotate);
		D3DXVECTOR3 d3dxvPosition = m_pPlayer->GetPosition() + d3dxvOffset;
		D3DXVECTOR3 d3dxvDirection = d3dxvPosition - m_d3dxvPosition;
		float fLength = D3DXVec3Length(&d3dxvDirection);
		D3DXVec3Normalize(&d3dxvDirection, &d3dxvDirection);
		float fTimeLagScale = (m_fTimeLag) ? fTimeElapsed * (1.0f / m_fTimeLag) : 1.0f;
		float fDistance = fLength * fTimeLagScale;
		if (fDistance > fLength) fDistance = fLength;
		if (fLength < 0.01f) fDistance = fLength;
		if (fDistance > 0)
		{
			m_d3dxvPosition += d3dxvDirection * fDistance;
			SetLookAt(m_pPlayer->GetPosition());
		} 
	}
}

void CThirdPersonCamera::SetLookAt(D3DXVECTOR3& d3dxvLookAt)
{
    D3DXMATRIX mtxLookAt;
    D3DXMatrixLookAtLH(&mtxLookAt, &m_d3dxvPosition, &d3dxvLookAt, &m_pPlayer->GetUpVector());
    m_d3dxvRight = D3DXVECTOR3(mtxLookAt._11, mtxLookAt._21, mtxLookAt._31);
    m_d3dxvUp = D3DXVECTOR3(mtxLookAt._12, mtxLookAt._22, mtxLookAt._32);
    m_d3dxvLook = D3DXVECTOR3(mtxLookAt._13, mtxLookAt._23, mtxLookAt._33);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CSpectator

CSpectator::CSpectator(CCamera *pCamera) : CCamera(pCamera)
{
	m_nMode = SPECTATOR_CAMERA;
}

void CSpectator::Rotate(float x, float y, float z)
{
	D3DXMATRIX mtxRotate;
	if (x != 0.0f)
	{
		D3DXMatrixRotationAxis(&mtxRotate, &m_d3dxvRight, (float)D3DXToRadian(x));
		D3DXVec3TransformNormal(&m_d3dxvLook, &m_d3dxvLook, &mtxRotate);
		D3DXVec3TransformNormal(&m_d3dxvUp, &m_d3dxvUp, &mtxRotate);
		D3DXVec3TransformNormal(&m_d3dxvRight, &m_d3dxvRight, &mtxRotate);
	}
	if (m_pPlayer && (y != 0.0f))
	{
		D3DXMatrixRotationAxis(&mtxRotate, &m_pPlayer->GetUpVector(), (float)D3DXToRadian(y));
		D3DXVec3TransformNormal(&m_d3dxvLook, &m_d3dxvLook, &mtxRotate);
		D3DXVec3TransformNormal(&m_d3dxvUp, &m_d3dxvUp, &mtxRotate);
		D3DXVec3TransformNormal(&m_d3dxvRight, &m_d3dxvRight, &mtxRotate);
	}
	if (m_pPlayer && (z != 0.0f))
	{
		D3DXMatrixRotationAxis(&mtxRotate, &m_pPlayer->GetLookVector(), (float)D3DXToRadian(z));
		m_d3dxvPosition -= m_pPlayer->GetPosition();
		D3DXVec3TransformCoord(&m_d3dxvPosition, &m_d3dxvPosition, &mtxRotate);
		m_d3dxvPosition += m_pPlayer->GetPosition();
		D3DXVec3TransformNormal(&m_d3dxvLook, &m_d3dxvLook, &mtxRotate);
		D3DXVec3TransformNormal(&m_d3dxvUp, &m_d3dxvUp, &mtxRotate);
		D3DXVec3TransformNormal(&m_d3dxvRight, &m_d3dxvRight, &mtxRotate);
	}
}

void CCamera::CalculateFrustumPlanes()
{
/*카메라 변환 행렬과 원근 투영 변환 행렬을 곱한 행렬을 사용하여 절두체 평면들을 구한다. 즉 월드 좌표계에서 절두체 컬링을 한다.*/
	D3DXMATRIX mtxViewProject = m_d3dxmtxView * m_d3dxmtxProjection;

//절두체의 왼쪽 평면
	m_d3dxFrustumPlanes[0].a = -(mtxViewProject._14 + mtxViewProject._11);
	m_d3dxFrustumPlanes[0].b = -(mtxViewProject._24 + mtxViewProject._21);
	m_d3dxFrustumPlanes[0].c = -(mtxViewProject._34 + mtxViewProject._31);
	m_d3dxFrustumPlanes[0].d = -(mtxViewProject._44 + mtxViewProject._41);

//절두체의 오른쪽 평면
	m_d3dxFrustumPlanes[1].a = -(mtxViewProject._14 - mtxViewProject._11);
	m_d3dxFrustumPlanes[1].b = -(mtxViewProject._24 - mtxViewProject._21);
	m_d3dxFrustumPlanes[1].c = -(mtxViewProject._34 - mtxViewProject._31);
	m_d3dxFrustumPlanes[1].d = -(mtxViewProject._44 - mtxViewProject._41);

//절두체의 위쪽 평면
	m_d3dxFrustumPlanes[2].a = -(mtxViewProject._14 - mtxViewProject._12);
	m_d3dxFrustumPlanes[2].b = -(mtxViewProject._24 - mtxViewProject._22);
	m_d3dxFrustumPlanes[2].c = -(mtxViewProject._34 - mtxViewProject._32);
	m_d3dxFrustumPlanes[2].d = -(mtxViewProject._44 - mtxViewProject._42);

//절두체의 아래쪽 평면
	m_d3dxFrustumPlanes[3].a = -(mtxViewProject._14 + mtxViewProject._12);
	m_d3dxFrustumPlanes[3].b = -(mtxViewProject._24 + mtxViewProject._22);
	m_d3dxFrustumPlanes[3].c = -(mtxViewProject._34 + mtxViewProject._32);
	m_d3dxFrustumPlanes[3].d = -(mtxViewProject._44 + mtxViewProject._42);

//절두체의 근평면
	m_d3dxFrustumPlanes[4].a = -(mtxViewProject._13);
	m_d3dxFrustumPlanes[4].b = -(mtxViewProject._23);
	m_d3dxFrustumPlanes[4].c = -(mtxViewProject._33);
	m_d3dxFrustumPlanes[4].d = -(mtxViewProject._43);

//절두체의 원평면
	m_d3dxFrustumPlanes[5].a = -(mtxViewProject._14 - mtxViewProject._13);
	m_d3dxFrustumPlanes[5].b = -(mtxViewProject._24 - mtxViewProject._23);
	m_d3dxFrustumPlanes[5].c = -(mtxViewProject._34 - mtxViewProject._33);
	m_d3dxFrustumPlanes[5].d = -(mtxViewProject._44 - mtxViewProject._43);

/*절두체의 각 평면의 법선 벡터 (a, b. c)의 크기로 a, b, c, d를 나눈다. 즉, 법선 벡터를 정규화하고 원점에서 평면까지의 거리를 계산한다.*/
	for (int i = 0; i < 6; i++) D3DXPlaneNormalize(&m_d3dxFrustumPlanes[i], &m_d3dxFrustumPlanes[i]);
}

bool CCamera::IsInFrustum(D3DXVECTOR3& d3dxvMinimum, D3DXVECTOR3& d3dxvMaximum)
{
    D3DXVECTOR3 d3dxvNearPoint, d3dxvFarPoint, d3dxvNormal;
    for (int i = 0; i < 6; i++)
    {
/*절두체의 각 평면에 대하여 바운딩 박스의 근접점을 계산한다. 근접점의 x, y, z 좌표는 법선 벡터의 각 요소가 음수이면 바운딩 박스의 최대점의 좌표가 되고 그렇지 않으면 바운딩 박스의 최소점의 좌표가 된다.*/ 
        d3dxvNormal = D3DXVECTOR3(m_d3dxFrustumPlanes[i].a, m_d3dxFrustumPlanes[i].b, m_d3dxFrustumPlanes[i].c);
        if (d3dxvNormal.x >= 0.0f)
        {
            if (d3dxvNormal.y >= 0.0f)
            {
                if (d3dxvNormal.z >= 0.0f) 
                {
//법선 벡터의 x, y, z 좌표의 부호가 모두 양수이므로 근접점은 바운딩 박스의 최소점이다.
                    d3dxvNearPoint.x = d3dxvMinimum.x; d3dxvNearPoint.y = d3dxvMinimum.y; d3dxvNearPoint.z = d3dxvMinimum.z;                
                } 
                else 
                {
/*법선 벡터의 x, y 좌표의 부호가 모두 양수이므로 근접점의 x, y 좌표는 바운딩 박스의 최소점의 x, y 좌표이고 법선 벡터의 z 좌표가 움수이므로 근접점의 z 좌표는 바운딩 박스의 최대점의 z 좌표이다.*/
                    d3dxvNearPoint.x = d3dxvMinimum.x; d3dxvNearPoint.y = d3dxvMinimum.y; d3dxvNearPoint.z = d3dxvMaximum.z;                 
                } 
            } 
            else
            {
                if (d3dxvNormal.z >= 0.0f) 
                {
/*법선 벡터의 x, z 좌표의 부호가 모두 양수이므로 근접점의 x, z 좌표는 바운딩 박스의 최소점의 x, z 좌표이고 법선 벡터의 y 좌표가 움수이므로 근접점의 y 좌표는 바운딩 박스의 최대점의 y 좌표이다.*/
                    d3dxvNearPoint.x = d3dxvMinimum.x; d3dxvNearPoint.y = d3dxvMaximum.y; d3dxvNearPoint.z = d3dxvMinimum.z;
                } 
                else 
                {
/*법선 벡터의 y, z 좌표의 부호가 모두 음수이므로 근접점의 y, z 좌표는 바운딩 박스의 최대점의 y, z 좌표이고 법선 벡터의 x 좌표가 양수이므로 근접점의 x 좌표는 바운딩 박스의 최소점의 x 좌표이다.*/
                    d3dxvNearPoint.x = d3dxvMinimum.x; d3dxvNearPoint.y = d3dxvMaximum.y; d3dxvNearPoint.z = d3dxvMaximum.z;                 
                } 
            } 
        } 
        else
        {
            if (d3dxvNormal.y >= 0.0f)
            {
                if (d3dxvNormal.z >= 0.0f) 
                {
/*법선 벡터의 y, z 좌표의 부호가 모두 양수이므로 근접점의 y, z 좌표는 바운딩 박스의 최소점의 y, z 좌표이고 법선 벡터의 x 좌표가 음수이므로 근접점의 x 좌표는 바운딩 박스의 최대점의 x 좌표이다.*/
                    d3dxvNearPoint.x = d3dxvMaximum.x; d3dxvNearPoint.y = d3dxvMinimum.y; d3dxvNearPoint.z = d3dxvMinimum.z;
                } 
                else 
                {
/*법선 벡터의 x, z 좌표의 부호가 모두 음수이므로 근접점의 x, z 좌표는 바운딩 박스의 최대점의 x, z 좌표이고 법선 벡터의 y 좌표가 양수이므로 근접점의 y 좌표는 바운딩 박스의 최소점의 y 좌표이다.*/
                    d3dxvNearPoint.x = d3dxvMaximum.x; d3dxvNearPoint.y = d3dxvMinimum.y; d3dxvNearPoint.z = d3dxvMaximum.z;                 
                } 
            } 
            else
            {
                if (d3dxvNormal.z >= 0.0f) 
                {
/*법선 벡터의 x, y 좌표의 부호가 모두 음수이므로 근접점의 x, y 좌표는 바운딩 박스의 최대점의 x, y 좌표이고 법선 벡터의 z 좌표가 양수이므로 근접점의 z 좌표는 바운딩 박스의 최소점의 z 좌표이다.*/
                    d3dxvNearPoint.x = d3dxvMaximum.x; d3dxvNearPoint.y = d3dxvMaximum.y; d3dxvNearPoint.z = d3dxvMinimum.z;                
                } 
                else 
                {
//법선 벡터의 x, y, z 좌표의 부호가 모두 음수이므로 근접점은 바운딩 박스의 최대점이다.
                    d3dxvNearPoint.x = d3dxvMaximum.x; d3dxvNearPoint.y = d3dxvMaximum.y; d3dxvNearPoint.z = d3dxvMaximum.z;                 
                } 
            } 
        } 
		if ((D3DXVec3Dot(&d3dxvNormal, &d3dxvNearPoint) + m_d3dxFrustumPlanes[i].d) > 0.0f) return(false);
    } 
    return(true);
}