#pragma once
#include "stdafx.h"
#include "Object.h"
#include "Player.h"

long double CollisionCheck(CPlayer *Player,CGameObject *Object2){
	float Distance[2];
	Distance[0] = sqrt(pow(Player->GetPosition().x-Object2->GetPosition().x,2)+pow(Player->GetPosition().y-Object2->GetPosition().y,2)+pow(Player->GetPosition().z-Object2->GetPosition().z,2));
	Distance[1] =(10 + Object2->m_pMesh->m_bcBoundingCube.m_d3dxvMaximum.x);


	
	return (Distance[1] - Distance[0]);


}
