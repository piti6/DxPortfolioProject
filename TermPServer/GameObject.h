#pragma once

#include <D3DX10Math.h>

struct Network_Packet{
	D3DXVECTOR3	m_d3dxvPosition;
	D3DXVECTOR3 m_d3dxvRotation;
	char		m_ID[10];
	int			m_isMoving;
};

struct Network_Throw_Packet{
	int			m_iIndex;
	char		m_ID[10];
};

class CGameObject
{
public:
	Network_Packet m_Network_Packet;
	CGameObject(char* _ID, D3DXVECTOR3 _d3dxvPosition, D3DXVECTOR3 _d3dxvRotation, int isMoving);
	~CGameObject(void){}
};
