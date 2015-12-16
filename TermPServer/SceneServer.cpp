#include "SceneServer.h"
#include <process.h>

CSceneServer::CSceneServer(void)
{
	InitializeCriticalSection(&cs);
	m_pNetworkServer = new CNetworkServer(9000);
	m_pNetworkServer->Init();
	m_iPlayerNum = 1;
	m_hThreadUDP = (HANDLE) ::_beginthreadex(NULL, 0, ThreadProcUDP, this, CREATE_SUSPENDED, NULL);
	ResumeThread(m_hThreadUDP);
}
CSceneServer::~CSceneServer(void)
{
	if (m_vThreadTCP.size() > 0){
		for (int i = 0; i < m_vThreadTCP.size(); ++i)
		{
			CloseHandle(m_vThreadTCP[i]);
		}
	}

	if (m_hThreadUDP != NULL)
		CloseHandle(m_hThreadUDP);
	delete m_pNetworkServer;
	DeleteCriticalSection(&cs);
}

void CSceneServer::Update(void){
	while (1){
		if (m_pNetworkServer->Accept()){
			HANDLE hThreadTCP = (HANDLE) ::_beginthreadex(NULL, 0, ThreadProcTCP, this, CREATE_SUSPENDED, NULL);
			ResumeThread(hThreadTCP);
			m_vThreadTCP.push_back(hThreadTCP);
		}
	}
}

UINT WINAPI CSceneServer::ThreadProcTCP(LPVOID arg)
{
	CSceneServer* m_pSceneServer = (CSceneServer*)arg;
	return m_pSceneServer->ThreadProcTCP(m_pSceneServer);
}
UINT WINAPI CSceneServer::ThreadProcUDP(LPVOID arg)
{

	CSceneServer* m_pSceneServer = (CSceneServer*)arg;
	return m_pSceneServer->ThreadProcUDP(m_pSceneServer);
}

UINT CSceneServer::ThreadProcTCP(CSceneServer* _SceneServer)
{
	CSceneServer* m_pSceneServer = _SceneServer;
	SOCKET ClientSock = m_pSceneServer->m_pNetworkServer->GetTCPSocket();
	vector<SOCKET*>* pSocketList = m_pNetworkServer->GetSocketList();

	BOOL optval = TRUE; // Nagle アルゴリズム停止
	setsockopt(ClientSock, IPPROTO_TCP, TCP_NODELAY, (char *)&optval, sizeof(optval));

	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(ClientSock, (SOCKADDR *)&clientaddr, &addrlen);

	bool isChecked = false;
	for (int i = 0; i < pSocketList->size(); ++i)
	{
		if (ClientSock == *(*pSocketList)[i])
		{
			isChecked = true;
			break;
		}
	}

	if (!isChecked)
	{
		pSocketList->push_back(&ClientSock);
	}

	int recv;
	char PlayerID[10] = { 0, };
	wsprintfA(PlayerID, "%s%d", "Player", m_pSceneServer->m_iPlayerNum++);
	CGameObject Player = CGameObject(PlayerID, D3DXVECTOR3(200, 1, 200),D3DXVECTOR3(0,0,0),0);

	Network_Packet m_PlayerPacket;
	strcpy(m_PlayerPacket.m_ID, Player.m_Network_Packet.m_ID);
	m_PlayerPacket.m_d3dxvPosition = Player.m_Network_Packet.m_d3dxvPosition;
	m_PlayerPacket.m_d3dxvRotation = Player.m_Network_Packet.m_d3dxvRotation;
	m_pSceneServer->m_pNetworkServer->TCPSendData(&ClientSock, TCP_CLIENT_LOGIN, m_PlayerPacket);
	m_pSceneServer->m_vPlayer.push_back(Player);
	char Data[100];

	while (1){
		recv = m_pSceneServer->m_pNetworkServer->TCPReceive(&ClientSock, Data);
		if (!recv){
			break;
		}

		if (Data != NULL){
			switch (Data[0]){
			case TCP_NULL:
				break;
			case TCP_CLIENT_MOVE:
				for (auto m_vPIt = m_pSceneServer->m_vPlayer.begin(); m_vPIt != m_pSceneServer->m_vPlayer.end(); ++m_vPIt){
					if (!strcmp(m_PlayerPacket.m_ID, m_vPIt->m_Network_Packet.m_ID)){
						EnterCriticalSection(&cs);
						m_vPIt->m_Network_Packet.m_isMoving = Data[2];
						LeaveCriticalSection(&cs);
						break;
					}
				}
				break;
			case TCP_CLIENT_THROWOBJECT:
				Network_Throw_Packet network_throw_packet;
				memcpy(&network_throw_packet, &Data[2], Data[1]);
				for (int i = 0; i < pSocketList->size(); ++i){
					m_pSceneServer->m_pNetworkServer->TCPSendData((*pSocketList)[i], TCP_CLIENT_THROWOBJECT, network_throw_packet);
				}
				break;
			case TCP_CLIENT_TRANSFORM:
				memcpy(&m_PlayerPacket, &Data[2], Data[1]);
				for (auto m_vPIt = m_pSceneServer->m_vPlayer.begin(); m_vPIt != m_pSceneServer->m_vPlayer.end(); ++m_vPIt){
					if (!strcmp(m_PlayerPacket.m_ID, m_vPIt->m_Network_Packet.m_ID)){
						EnterCriticalSection(&cs);
						m_vPIt->m_Network_Packet.m_d3dxvPosition = m_PlayerPacket.m_d3dxvPosition;
						m_vPIt->m_Network_Packet.m_d3dxvRotation = m_PlayerPacket.m_d3dxvRotation;
						LeaveCriticalSection(&cs);
						//printf("プレイヤー:%s 座標:(%5.2f,%5.2f,%5.2f)に移動\n", m_PlayerPacket.m_ID, m_PlayerPacket.m_d3dxvPosition.x, m_PlayerPacket.m_d3dxvPosition.y, m_PlayerPacket.m_d3dxvPosition.z);
						break;
					}
				}
				break;
			case TCP_CLIENT_DISCONNECT:
				EnterCriticalSection(&cs);
				for (auto i = m_pSceneServer->m_vPlayer.begin(); i != m_pSceneServer->m_vPlayer.end(); ++i){
					if (!strcmp(i->m_Network_Packet.m_ID, Player.m_Network_Packet.m_ID)){
						i = m_pSceneServer->m_vPlayer.erase(i);
						break;
					}
				}
				for (auto i = pSocketList->begin(); i != pSocketList->end(); ++i)
				{
					if (ClientSock == *(*i))
					{
						i = pSocketList->erase(i);
						break;
					}
				}
				LeaveCriticalSection(&cs);
				closesocket(ClientSock);
				return 0;
			}
		}
	}
}
UINT CSceneServer::ThreadProcUDP(CSceneServer* _SceneServer)
{
	CSceneServer* m_pSceneServer = _SceneServer;
	SOCKADDR_IN remoteaddr;
	ZeroMemory(&remoteaddr, sizeof(remoteaddr));
	remoteaddr.sin_family = AF_INET;
	remoteaddr.sin_addr.s_addr = inet_addr("235.7.7.10");
	remoteaddr.sin_port = htons(9000);

	SOCKET *pSocket = m_pSceneServer->m_pNetworkServer->GetUDPSocket();

	printf("\n[UDP サーバー] マルチキャスティング 開始: IP アドレス=%s, ポート=%d\n",
		inet_ntoa(remoteaddr.sin_addr), ntohs(remoteaddr.sin_port));

	while (1){
		EnterCriticalSection(&cs);
		for (auto m_vPIt = m_pSceneServer->m_vPlayer.begin(); m_vPIt != m_pSceneServer->m_vPlayer.end(); ++m_vPIt){
			m_pSceneServer->m_pNetworkServer->UDPSendData(pSocket, UDP_SERVER_PLAYER, *m_vPIt, &remoteaddr);
		}
		LeaveCriticalSection(&cs);
		Sleep(30);
	}
}
