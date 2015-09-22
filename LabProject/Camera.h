#pragma once

#define FRAME_BUFFER_WIDTH			640
#define FRAME_BUFFER_HEIGHT			480
#define ASPECT_RATIO				(float(FRAME_BUFFER_WIDTH)/float(FRAME_BUFFER_HEIGHT))

#define FIRST_PERSON_CAMERA			0x01
#define THIRD_PERSON_CAMERA			0x03

struct VS_CB_VIEWPROJECTION_MATRIX
{
	D3DXMATRIX						m_d3dxmtxView;         	
    D3DXMATRIX						m_d3dxmtxProjection;   
};

class CPlayer;

class CCamera
{
protected:
	D3DXVECTOR3						m_d3dxvPosition;             
    D3DXVECTOR3						m_d3dxvRight;             
    D3DXVECTOR3						m_d3dxvUp;             
    D3DXVECTOR3						m_d3dxvLook;   

    float           				m_fPitch;             
    float           				m_fRoll;              
    float           				m_fYaw;               

	DWORD							m_nMode;

    D3DXVECTOR3						m_d3dxvLookAtWorld;   
	D3DXVECTOR3						m_d3dxvOffset;
    float           				m_fTimeLag;

	D3DXMATRIX						m_d3dxmtxView;         	
    D3DXMATRIX						m_d3dxmtxProjection;   

	D3D11_VIEWPORT					m_d3dViewport;

	ID3D11Buffer					*m_pd3dcbViewProjection;

	CPlayer							*m_pPlayer;

	D3DXPLANE						 m_d3dxFrustumPlanes[6]; 

public:
	CCamera(CCamera *pCamera);
	~CCamera();

	void SetPlayer(CPlayer *pPlayer) { m_pPlayer = pPlayer; }
	void SetViewport(ID3D11DeviceContext *pd3dImmediateDeviceContext, DWORD xStart, DWORD yStart, DWORD nWidth, DWORD nHeight, float fMinZ=0.0f, float fMaxZ=1.0f);

	void GenerateViewMatrix();
	void RegenerateViewMatrix();
	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle);

	void CreateShaderVariables(ID3D11Device *pd3dDevice);
	void UpdateShaderVariables(ID3D11DeviceContext *pd3dImmediateDeviceContext);

	void SetMode(DWORD nMode) { m_nMode = nMode; }
	DWORD GetMode() { return(m_nMode); }

	CPlayer *GetPlayer() { return(m_pPlayer); }
	D3DXMATRIX GetViewMatrix() { return(m_d3dxmtxView); }
	D3DXMATRIX GetProjectionMatrix() { return(m_d3dxmtxProjection); }
	D3D11_VIEWPORT GetViewport() { return(m_d3dViewport); }
	ID3D11Buffer *GetViewProjectionConstantBuffer() { return(m_pd3dcbViewProjection); }

    void SetPosition(D3DXVECTOR3 d3dxvPosition) { m_d3dxvPosition = d3dxvPosition; }
    D3DXVECTOR3& GetPosition() { return(m_d3dxvPosition); }
	void SetLookAtPosition(D3DXVECTOR3 d3dxvLookAtWorld) { m_d3dxvLookAtWorld = d3dxvLookAtWorld; }
    D3DXVECTOR3& GetLookAtPosition() { return(m_d3dxvLookAtWorld); }

    D3DXVECTOR3& GetRightVector() { return(m_d3dxvRight); }
    D3DXVECTOR3& GetUpVector() { return(m_d3dxvUp); }
    D3DXVECTOR3& GetLookVector() { return(m_d3dxvLook); }

    float& GetPitch() { return(m_fPitch); }
    float& GetRoll() { return(m_fRoll); }
    float& GetYaw() { return(m_fYaw); }

	void SetOffset(D3DXVECTOR3 d3dxvOffset) { m_d3dxvOffset = d3dxvOffset; m_d3dxvPosition += d3dxvOffset; } 
    D3DXVECTOR3& GetOffset() { return(m_d3dxvOffset); }
	void SetTimeLag(float fTimeLag) { m_fTimeLag = fTimeLag; } 
    float GetTimeLag() { return(m_fTimeLag); }

    virtual void Move(const D3DXVECTOR3& d3dxvShift) { m_d3dxvPosition += d3dxvShift; }
	virtual void Rotate(float fPitch=0.0f, float fYaw=0.0f, float fRoll=0.0f) { }
    virtual void Update(float fTimeElapsed) { }
	virtual void SetLookAt(D3DXVECTOR3& vLookAt) { }

	//절두체의 6개 평면을 계산한다.
    void CalculateFrustumPlanes();
	//바운딩 박스가 절두체에 완전히 포함되거나 일부라도 포함되는 가를 검사한다.
    bool IsInFrustum(D3DXVECTOR3& d3dxvMinimum, D3DXVECTOR3& d3dxvMaximum);

};

class CFirstPersonCamera : public CCamera
{
public:
    CFirstPersonCamera(CCamera *pCamera);

	virtual void Rotate(float fPitch=0.0f, float fYaw=0.0f, float fRoll=0.0f);
};

class CThirdPersonCamera : public CCamera
{
public:
    CThirdPersonCamera(CCamera *pCamera);

    virtual void Update(float fTimeScale);
    virtual void SetLookAt(D3DXVECTOR3& vLookAt);
};

