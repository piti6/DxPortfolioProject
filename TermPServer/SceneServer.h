#pragma once
#include "NetworkServer.h"
#include "GameObject.h"
#include <list>

using namespace std;

class CSceneServer
{
public:
	HANDLE			hThreadTCP;
	HANDLE			hThreadUDP;

	CNetworkServer* m_pNetworkServer;
	list <CGameObject>  m_vPlayer;

	CRITICAL_SECTION cs;

	int				m_iPlayerNum;
	SOCKADDR_IN		remoteaddr;
public:
	CSceneServer(void);
	~CSceneServer(void);
	void Update(void);
	static DWORD WINAPI ThreadProcTCP(LPVOID arg);
	static DWORD WINAPI ThreadProcUDP(LPVOID arg);

	DWORD ThreadProcTCP (CSceneServer* _SceneServer);
	DWORD ThreadProcUDP (CSceneServer* _SceneServer);
};

