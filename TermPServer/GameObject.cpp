#include "GameObject.h"
CGameObject::CGameObject(char* _ID, D3DXVECTOR3 _d3dxvPosition, D3DXVECTOR3 _d3dxvRotation){
	strcpy(m_Network_Packet.m_ID,_ID);
	m_Network_Packet.m_d3dxvPosition = _d3dxvPosition;
	m_Network_Packet.m_d3dxvRotation = _d3dxvRotation;
}
