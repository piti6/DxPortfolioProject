#pragma comment(lib, "ws2_32")
#include <stdlib.h>
#include "GameServer.h"

int main(void){
	CGameServer* m_GameServer;
	m_GameServer = new CGameServer();

	
	while(1){
		m_GameServer->Update();
	}

	delete m_GameServer;
}