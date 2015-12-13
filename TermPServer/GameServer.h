#pragma once
#include "SceneServer.h"
class CGameServer
{
public:
	CSceneServer* m_pSceneServer;
public:
	CGameServer(void);
	~CGameServer(void);
	void Update(void);
};

