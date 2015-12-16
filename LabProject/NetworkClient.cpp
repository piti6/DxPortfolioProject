#include "stdafx.h"
#include "NetworkClient.h"


void SetSocketOption(SOCKET s, int level, int optname, char* optval, int optlen){
	int retval = setsockopt(s, level, optname, optval, optlen);
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");
}
int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
	int left = len;

	while (left > 0){
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}

//エラー処理
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
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
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&lpMsgBuf, 0, NULL);
	MessageBoxA(NULL, (LPCSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
}

CNetworkClient::CNetworkClient(char* _ServerIP, int _ServerPort){
	strcpy(ServerIP, _ServerIP);
	ServerPort = _ServerPort;
}
CNetworkClient::~CNetworkClient(){
	Release();
}

//初期化
int CNetworkClient::Init(){
	WSAInit();
	SocketInit();
	BufferResize();
	Bind();
	Connect();
	return 0;
}
int CNetworkClient::WSAInit(){
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return -1;
	return 0;
}
void CNetworkClient::SocketInit(){
	m_TCPsock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_TCPsock == INVALID_SOCKET) err_quit("socket()");
	m_UDPsock = socket(AF_INET, SOCK_DGRAM, 0);
	if (m_UDPsock == INVALID_SOCKET) err_quit("socket()");
	bool optval = TRUE;
	m_iRetval = setsockopt(m_UDPsock, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval));
	if (m_iRetval == SOCKET_ERROR) err_quit("setsockopt()");
}

void CNetworkClient::Bind(){
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(ServerIP);
	serveraddr.sin_port = htons(ServerPort);
	ZeroMemory(&localaddr, sizeof(localaddr));
	localaddr.sin_family = AF_INET;
	localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	localaddr.sin_port = htons(ServerPort);
	m_iRetval = bind(m_UDPsock, (SOCKADDR *)&localaddr, sizeof(localaddr));
	if (m_iRetval == SOCKET_ERROR) err_quit("bind()");
}
void CNetworkClient::Connect(){
	m_iRetval = connect(m_TCPsock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	printf("\n[TCP クライアント] サーバー接続成功: IP アドレス=%s, ポート=%d\n",
		inet_ntoa(serveraddr.sin_addr), ntohs(serveraddr.sin_port));
	if (m_iRetval == SOCKET_ERROR) err_quit("connect()");
	MulticastAddMember();
}
void CNetworkClient::BufferResize(){
	int optval;
	int optlen = sizeof(int);
	m_iRetval = getsockopt(m_TCPsock, SOL_SOCKET, SO_RCVBUF,
		(char *)&optval, &optlen);
	if (m_iRetval == SOCKET_ERROR) err_quit("getsockopt()");
	optval *= 10;
	m_iRetval = setsockopt(m_TCPsock, SOL_SOCKET, SO_RCVBUF,
		(char *)&optval, sizeof(optval));
	if (m_iRetval == SOCKET_ERROR) err_quit("setsockopt()");
}


//送信
void CNetworkClient::TCPSendChar(SOCKET* sock, char _Protocol, char* _Data){
	char cProtocol[20];
	int size = strlen(_Data);
	m_iRetval = send(*sock, (char*)&_Protocol, sizeof(char), 0);
	m_iRetval = send(*sock, (char*)&size, sizeof(char), 0);
	m_iRetval = send(*sock, _Data, size, 0);
	if (m_iRetval == SOCKET_ERROR)
		err_display("send()");
	else{
		ProtocolToChar(_Protocol, cProtocol);
		//printf("[%s -> TCPSend] Protocol : %s, DataSize : %d, Data:%s\n", inet_ntoa(serveraddr.sin_addr), cProtocol, size, _Data);
	}
}
void CNetworkClient::TCPSendInt(SOCKET* sock, char _Protocol, int _Data){
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

//受信
bool CNetworkClient::UDPReceive(SOCKET* sock, char* Temp){
	char cProtocol[20];
	int addrlen = sizeof(peeraddr);
	m_iRetval = recvfrom(*sock, (char*)&Temp[0], sizeof(char), 0, (SOCKADDR *)&peeraddr, &addrlen);
	m_iRetval = recvfrom(*sock, (char*)&Temp[1], sizeof(char), 0, (SOCKADDR *)&peeraddr, &addrlen);
	m_iRetval = recvfrom(*sock, (char*)&Temp[2], Temp[1], 0, (SOCKADDR *)&peeraddr, &addrlen);
	if (m_iRetval == SOCKET_ERROR){
		err_display("recv()");
		return false;
	}
	else{
		ProtocolToChar(Temp[0], cProtocol);
		//printf("[%s -> UDPReceive] Protocol : %s, Datasize : %d\n",inet_ntoa(serveraddr.sin_addr),cProtocol,Temp[1]);
	}
	return true;
}
bool CNetworkClient::TCPReceive(SOCKET* sock, char* Temp){
	char cProtocol[20];
	m_iRetval = recv(*sock, (char*)&Temp[0], sizeof(char), 0);
	m_iRetval = recv(*sock, (char*)&Temp[1], sizeof(char), 0);
	m_iRetval = recvn(*sock, (char*)&Temp[2], Temp[1], 0);
	if (m_iRetval == SOCKET_ERROR){
		err_display("recv()");
		return false;
	}
	else{
		ProtocolToChar(Temp[0], cProtocol);
		//printf("[%s -> TCPReceive] Protocol : %s, Datasize : %d\n", inet_ntoa(serveraddr.sin_addr), cProtocol, Temp[1]);
	}
	return true;
}

//Multicast
void CNetworkClient::MulticastAddMember(){
	mreq.imr_multiaddr.s_addr = inet_addr("235.7.7.10");
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	m_iRetval = setsockopt(m_UDPsock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
		(char *)&mreq, sizeof(mreq));
	if (m_iRetval == SOCKET_ERROR) err_quit("setsockopt()");
}
void CNetworkClient::MulticastDropMember(){
	m_iRetval = setsockopt(m_UDPsock, IPPROTO_IP, IP_DROP_MEMBERSHIP,
		(char *)&mreq, sizeof(mreq));
	if (m_iRetval == SOCKET_ERROR) err_quit("setsockopt()");
}

//Debug (Print Protocol Information)
void CNetworkClient::ProtocolToChar(char _Protocol, char* _cProtocol){
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

//Release
void CNetworkClient::Release(){
	closesocket(m_TCPsock);
	closesocket(m_UDPsock);
	WSACleanup();
}