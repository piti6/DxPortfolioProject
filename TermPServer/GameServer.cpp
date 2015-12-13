#include "GameServer.h"


CGameServer::CGameServer(void)
{
	m_pSceneServer = new CSceneServer();
}


CGameServer::~CGameServer(void)
{
	delete m_pSceneServer;
}

void CGameServer::Update(void){
	m_pSceneServer->Update();
}