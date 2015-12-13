#pragma once

#include <D3DX10Math.h>

typedef struct Network_Packet{
	D3DXVECTOR3	m_d3dxvPosition;
	D3DXVECTOR3 m_d3dxvRotation;
	char		m_ID[10];
	int			m_isMoving;
}NETWORK_PACKET;

class CGameObject
{
public:
	Network_Packet m_Network_Packet;
	CGameObject(char* _ID, D3DXVECTOR3 _d3dxvPosition, D3DXVECTOR3 _d3dxvRotation, int isMoving);
	~CGameObject(void){}
	
};
