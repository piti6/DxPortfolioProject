#pragma comment(lib, "ws2_32")
#include <stdlib.h>
#include <iostream>
#include <memory>
#include "GameServer.h"

int main(void){
	shared_ptr<CGameServer> m_GameServer(new CGameServer());
	
	while(1){
		m_GameServer->Update();
	}
}
