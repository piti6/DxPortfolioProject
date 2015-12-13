#include "SceneServer.h"

CSceneServer::CSceneServer(void)
{
	InitializeCriticalSection(&cs);
	m_pNetworkServer = new CNetworkServer(9000);
	m_pNetworkServer->Init();
	m_iPlayerNum = 1;
	hThreadUDP = CreateThread(NULL, 0, ThreadProcUDP,
		(LPVOID)this, 0, NULL);
}


CSceneServer::~CSceneServer(void)
{
	if (hThreadTCP != NULL)
		CloseHandle(hThreadTCP);
	if (hThreadUDP != NULL)
		CloseHandle(hThreadUDP);
	delete m_pNetworkServer;
	DeleteCriticalSection(&cs);
}

void CSceneServer::Update(void){
	while (1){
		if (m_pNetworkServer->Accept()){
			hThreadTCP = CreateThread(NULL, 0, ThreadProcTCP,
				(LPVOID)this, 0, NULL);
		}
		if (hThreadTCP == NULL) { closesocket(m_pNetworkServer->TCPClientsock); }
		else { CloseHandle(hThreadTCP); }
	}
}

DWORD WINAPI CSceneServer::ThreadProcTCP(LPVOID arg)
{
	CSceneServer* m_pSceneServer = (CSceneServer*)arg;
	return m_pSceneServer->ThreadProcTCP(m_pSceneServer);
}

DWORD WINAPI CSceneServer::ThreadProcUDP(LPVOID arg)
{

	CSceneServer* m_pSceneServer = (CSceneServer*)arg;
	return m_pSceneServer->ThreadProcUDP(m_pSceneServer);
}

DWORD CSceneServer::ThreadProcTCP(CSceneServer* _SceneServer)
{
	CSceneServer* m_pSceneServer = _SceneServer;
	SOCKET Client_Sock = m_pSceneServer->m_pNetworkServer->TCPClientsock;
	BOOL optval = TRUE; // Nagle 알고리즘 중지
	setsockopt(Client_Sock, IPPROTO_TCP, TCP_NODELAY, (char *)&optval, sizeof(optval));
	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(Client_Sock, (SOCKADDR *)&clientaddr, &addrlen);
	bool isChecked = false;
	for (auto i = m_pNetworkServer->m_SocketList.begin(); i != m_pNetworkServer->m_SocketList.end(); ++i){
		if (Client_Sock == *(*i)){
			isChecked = true;
			break;
		}
	}
	if (!isChecked)
		m_pSceneServer->m_pNetworkServer->m_SocketList.push_back(&Client_Sock); // 클라이언트 기억
	int recv;
	char* PlayerID = new char[20];
	wsprintfA(PlayerID, "%s%d", "Player", m_pSceneServer->m_iPlayerNum++);
	CGameObject* m_pPlayer = new CGameObject(PlayerID, D3DXVECTOR3(200, 1, 200),D3DXVECTOR3(0,0,0));

	NETWORK_PACKET m_PlayerPacket;
	strcpy(m_PlayerPacket.m_ID, m_pPlayer->m_Network_Packet.m_ID);
	m_PlayerPacket.m_d3dxvPosition = m_pPlayer->m_Network_Packet.m_d3dxvPosition;
	m_PlayerPacket.m_d3dxvRotation = m_pPlayer->m_Network_Packet.m_d3dxvRotation;
	m_pSceneServer->m_pNetworkServer->TCPSendData(&Client_Sock, TCP_CLIENT_LOGIN, m_PlayerPacket);
	m_pSceneServer->m_vPlayer.push_back(*m_pPlayer);
	char Data[100];

	while (1){
		recv = m_pSceneServer->m_pNetworkServer->TCPReceive(&Client_Sock, Data);
		if (!recv){
			break;
		}

		if (Data != NULL){
			switch (Data[0]){
			case TCP_NULL:
				break;
			case TCP_CLIENT_MOVE:
				memcpy(&m_PlayerPacket, &Data[2], Data[1]);
				for (auto m_vPIt = m_pSceneServer->m_vPlayer.begin(); m_vPIt != m_pSceneServer->m_vPlayer.end(); ++m_vPIt){
					if (!strcmp(m_PlayerPacket.m_ID, m_vPIt->m_Network_Packet.m_ID)){
						EnterCriticalSection(&cs);
						m_vPIt->m_Network_Packet = m_PlayerPacket;
						LeaveCriticalSection(&cs);
						printf("플레이어:%s 좌표:(%d,%d,%d)로 이동\n", m_PlayerPacket.m_ID, m_PlayerPacket.m_d3dxvPosition.x, m_PlayerPacket.m_d3dxvPosition.y, m_PlayerPacket.m_d3dxvPosition.z);
						break;
					}
				}
				break;
			case TCP_CLIENT_DISCONNECT:
				EnterCriticalSection(&cs);
				for (auto i = m_pSceneServer->m_vPlayer.begin(); i != m_pSceneServer->m_vPlayer.end(); ++i){
					if (!strcmp(i->m_Network_Packet.m_ID, m_pPlayer->m_Network_Packet.m_ID)){
						i = m_pSceneServer->m_vPlayer.erase(i);
						break;
					}
				}
				
				for (auto i = m_pNetworkServer->m_SocketList.begin(); i != m_pNetworkServer->m_SocketList.end(); ++i){
					if (*(*i) == Client_Sock){
						i = m_pNetworkServer->m_SocketList.erase(i);
						break;
					}
				}
				LeaveCriticalSection(&cs);
				closesocket(Client_Sock);
				return 0;
			}
		}
	}
}
DWORD CSceneServer::ThreadProcUDP(CSceneServer* _SceneServer)
{
	CSceneServer* m_pSceneServer = _SceneServer;
	SOCKADDR_IN remoteaddr = m_pSceneServer->remoteaddr;
	ZeroMemory(&remoteaddr, sizeof(remoteaddr));
	remoteaddr.sin_family = AF_INET;
	remoteaddr.sin_addr.s_addr = inet_addr("235.7.7.10");
	remoteaddr.sin_port = htons(9000);
	m_pSceneServer->remoteaddr = remoteaddr;
	printf("\n[UDP 서버] 멀티캐스팅 시작: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(remoteaddr.sin_addr), ntohs(remoteaddr.sin_port));
	while (1){
		EnterCriticalSection(&cs);
		for (auto m_vPIt = m_pSceneServer->m_vPlayer.begin(); m_vPIt != m_pSceneServer->m_vPlayer.end(); ++m_vPIt){
			m_pSceneServer->m_pNetworkServer->UDPSendData(&m_pSceneServer->m_pNetworkServer->UDPsock, UDP_SERVER_PLAYER, *m_vPIt, &remoteaddr);
		}
		LeaveCriticalSection(&cs);
		Sleep(30);
	}
	closesocket(m_pSceneServer->m_pNetworkServer->UDPsock);
	return 0;
}
