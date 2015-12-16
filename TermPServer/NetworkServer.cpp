#include "NetworkServer.h"
#include <stdlib.h>
#include <stdio.h>

int recvn(SOCKET s, char *buf, int len, int flags)
{
   int received;
   char *ptr = buf;
   int left = len;
 
   while(left > 0){
      received = recv(s, ptr, left, flags);
      if(received == SOCKET_ERROR)
         return SOCKET_ERROR;
      else if(received <= 0)
         break;
      left -= received;
      ptr += received;
   }
 
   return (len - left);
}

void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&lpMsgBuf, 0, NULL);
	MessageBoxA(NULL, (LPCSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&lpMsgBuf, 0, NULL);
	MessageBoxA(NULL, (LPCSTR)lpMsgBuf,msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
}

CNetworkServer::CNetworkServer(int _ServerPort){
	ServerPort = _ServerPort;
}
CNetworkServer::~CNetworkServer(){
	Release();
}

void CNetworkServer::Release(){
	closesocket(m_TCPsock);
	closesocket(m_UDPsock);

	WSACleanup();
}

int CNetworkServer::Init(){
	int err_code;
	err_code = WSAInit();
	SocketInit();
	Bind();
	BufferResize();
	Listen();
	return err_code;
}
int CNetworkServer::WSAInit(){
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return -1;
	return 0;
}
void CNetworkServer::SocketInit(){
	m_TCPsock = socket(AF_INET, SOCK_STREAM, 0);
	m_UDPsock = socket(AF_INET, SOCK_DGRAM, 0);

	if (m_TCPsock == INVALID_SOCKET) err_quit("socket()");
	if (m_UDPsock == INVALID_SOCKET) err_quit("socket()");
}
void CNetworkServer::Bind(){
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(ServerPort);
	m_iRetval = bind(m_TCPsock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (m_iRetval == SOCKET_ERROR) err_quit("bind()");

	int ttl = 2;
	m_iRetval = setsockopt(m_UDPsock, IPPROTO_IP, IP_MULTICAST_TTL,
		(char *)&ttl, sizeof(ttl));
	if (m_iRetval == SOCKET_ERROR) err_quit("setsockopt()");
}
void CNetworkServer::Listen(){
	m_iRetval = listen(m_TCPsock, SOMAXCONN);
	if (m_iRetval == SOCKET_ERROR) err_quit("listen()");
}
bool CNetworkServer::Accept(){
	int addrsize = sizeof(clientaddr);
	m_TCPClientsock = accept(m_TCPsock, (SOCKADDR *)&clientaddr, &addrsize);
	printf("\n[TCP サーバー] クライアント接続: IP アドレス=%s, ポート=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	if (m_TCPClientsock == INVALID_SOCKET){
		err_display("accept()");
		return false;
	}
	return true;
}
void CNetworkServer::BufferResize(){
	int optval, optlen;
	optlen = sizeof(optval);
	m_iRetval = getsockopt(m_TCPsock, SOL_SOCKET, SO_RCVBUF,
		(char *)&optval, &optlen);
	if (m_iRetval == SOCKET_ERROR) err_quit("getsockopt()");
	optval *= 100;
	m_iRetval = setsockopt(m_TCPsock, SOL_SOCKET, SO_RCVBUF,
		(char *)&optval, sizeof(optval));
	if (m_iRetval == SOCKET_ERROR) err_quit("setsockopt()");
}

bool CNetworkServer::TCPReceive(SOCKET* sock,char* Temp){
	char cProtocol[20];
	m_iRetval = recvn(*sock, (char*)&Temp[0], sizeof(char), 0);
	m_iRetval = recvn(*sock, (char*)&Temp[1], sizeof(char), 0);
	m_iRetval = recvn(*sock, (char*)&Temp[2], Temp[1], 0);
	if (m_iRetval == SOCKET_ERROR){
		err_display("recv()");
		return false;
	}
	else{
		ProtocolToChar(Temp[0],cProtocol);
		//printf("[%s -> TCPReceive] Protocol : %s, Datasize : %d\n",inet_ntoa(serveraddr.sin_addr),cProtocol,Temp[1]);
	}
	return true;
}
void CNetworkServer::TCPSendChar(SOCKET* sock,char _Protocol,char* _Data){
	char cProtocol[20];
	int size = strlen(_Data);
	m_iRetval = send(*sock, (char*)&_Protocol, sizeof(char), 0);
	m_iRetval = send(*sock, (char*)&size, sizeof(char), 0);
	m_iRetval = send(*sock, _Data, size, 0);
	if (m_iRetval == SOCKET_ERROR)
		err_display("send()");
	else{
		ProtocolToChar(_Protocol,cProtocol);
		//printf("[%s -> TCPSend]Protocol : %s, DataSize : %d, Data : %s\n",inet_ntoa(clientaddr.sin_addr),cProtocol,size,_Data);
	}
}
void CNetworkServer::TCPSendInt(SOCKET* sock,char _Protocol,int _Data){
	char cProtocol[20];
	int size = sizeof(_Data);
	m_iRetval = send(*sock, (char*)&_Protocol, sizeof(char), 0);
	m_iRetval = send(*sock, (char*)&size, sizeof(char), 0);
	m_iRetval = send(*sock, (char*)&_Data, size, 0);
	if (m_iRetval == SOCKET_ERROR)
		err_display("send()");
	else{
		ProtocolToChar(_Protocol, cProtocol);
		//printf("[%s -> TCPSend] Protocol : %s, DataSize : %d, Data:%d\n", inet_ntoa(serveraddr.sin_addr), cProtocol, size, _Data);
	}
}

void CNetworkServer::ProtocolToChar(char _Protocol, char* _cProtocol){
	switch (_Protocol){
	case 0:
		strcpy(_cProtocol, "TCP_NULL");
		break;
	case 1:
		strcpy(_cProtocol, "TCP_MOVE");
		break;
	case 2:
		strcpy(_cProtocol, "TCP_TRANSFORM");
		break;
	case 3:
		strcpy(_cProtocol, "TCP_THROWOBJECT");
		break;
	case 4:
		strcpy(_cProtocol, "TCP_LOGIN");
		break;
	case 5:
		strcpy(_cProtocol, "TCP_DISCONNECT");
		break;
	case 6:
		strcpy(_cProtocol, "UDP_PLAYER");
		break;
	}
}