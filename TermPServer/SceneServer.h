#pragma once
#include "NetworkServer.h"
#include "GameObject.h"
#include <vector>
using namespace std;

class CSceneServer
{
public:
	CSceneServer(void);
	~CSceneServer(void);

	void Update(void);

	static UINT WINAPI ThreadProcTCP(LPVOID arg);
	static UINT WINAPI ThreadProcUDP(LPVOID arg);

	UINT ThreadProcTCP(CSceneServer* _SceneServer);
	UINT ThreadProcUDP(CSceneServer* _SceneServer);

public:
	vector<HANDLE>			m_vThreadTCP;
	vector<CGameObject>		m_vPlayer;

	HANDLE					m_hThreadUDP;

	CNetworkServer*			m_pNetworkServer;
	
	int						m_iPlayerNum;

	CRITICAL_SECTION		cs;
};

