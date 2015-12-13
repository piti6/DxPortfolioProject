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
	this->Release();
}

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
	TCPsock = socket(AF_INET, SOCK_STREAM, 0);
	if (TCPsock == INVALID_SOCKET) err_quit("socket()");
	UDPsock = socket(AF_INET, SOCK_DGRAM, 0);
	if (UDPsock == INVALID_SOCKET) err_quit("socket()");
	optval = TRUE;
	retval = setsockopt(UDPsock, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");
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
	retval = bind(UDPsock, (SOCKADDR *)&localaddr, sizeof(localaddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");
}

void CNetworkClient::BufferResize(){
	int optval;
	int optlen = sizeof(int);
	int retval = getsockopt(TCPsock, SOL_SOCKET, SO_RCVBUF,
		(char *)&optval, &optlen);
	if (retval == SOCKET_ERROR) err_quit("getsockopt()");
	optval *= 10;
	retval = setsockopt(TCPsock, SOL_SOCKET, SO_RCVBUF,
		(char *)&optval, sizeof(optval));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");
}
void CNetworkClient::Connect(){
	m_iTCPReceive = connect(TCPsock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	printf("\n[TCP 클라] 서버 접속 성공: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(serveraddr.sin_addr), ntohs(serveraddr.sin_port));
	if (m_iTCPReceive == SOCKET_ERROR) err_quit("connect()");
	MulticastAddMember();
}
//Send
void CNetworkClient::TCPSendChar(SOCKET* sock, char _Protocol, char* _Data){
	char cProtocol[20];
	int size = strlen(_Data);
	m_iTCPSend = send(*sock, (char*)&_Protocol, sizeof(char), 0);
	m_iTCPSend = send(*sock, (char*)&size, sizeof(char), 0);
	m_iTCPSend = send(*sock, _Data, size, 0);
	ProtocolToChar(_Protocol, cProtocol);
	if (m_iTCPSend == SOCKET_ERROR)
		err_display("send()");
	else
		printf("[%s -> TCPSend] Protocol : %s, DataSize : %d, Data:%s\n", inet_ntoa(serveraddr.sin_addr), cProtocol, size, _Data);
}
void CNetworkClient::TCPSendInt(SOCKET* sock, char _Protocol, int _Data){
	char cProtocol[20];
	int size = sizeof(_Data);
	m_iTCPSend = send(*sock, (char*)&_Protocol, sizeof(char), 0);
	m_iTCPSend = send(*sock, (char*)&size, sizeof(char), 0);
	m_iTCPSend = send(*sock, (char*)&_Data, size, 0);
	ProtocolToChar(_Protocol, cProtocol);
	if (m_iTCPSend == SOCKET_ERROR)
		err_display("send()");
	else
		printf("[%s -> TCPSend] Protocol : %s, DataSize : %d, Data:%d\n", inet_ntoa(serveraddr.sin_addr), cProtocol, size, _Data);
}


//Receive
bool CNetworkClient::UDPReceive(SOCKET* sock, char* Temp){
	char cProtocol[20];
	int addrlen = sizeof(peeraddr);
	int UDPretval = recvfrom(*sock, (char*)&Temp[0], sizeof(char), 0, (SOCKADDR *)&peeraddr, &addrlen);
	UDPretval = recvfrom(*sock, (char*)&Temp[1], sizeof(char), 0, (SOCKADDR *)&peeraddr, &addrlen);
	UDPretval = recvfrom(*sock, (char*)&Temp[2], Temp[1], 0, (SOCKADDR *)&peeraddr, &addrlen);
	if (UDPretval == SOCKET_ERROR){
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
	int TCPretval = recv(*sock, (char*)&Temp[0], sizeof(char), 0);
	TCPretval = recv(*sock, (char*)&Temp[1], sizeof(char), 0);
	TCPretval = recvn(*sock, (char*)&Temp[2], Temp[1], 0);
	if (TCPretval == SOCKET_ERROR){
		err_display("recv()");
		return false;
	}
	else{
		ProtocolToChar(Temp[0], cProtocol);
		printf("[%s -> TCPReceive] Protocol : %s, Datasize : %d\n", inet_ntoa(serveraddr.sin_addr), cProtocol, Temp[1]);
	}
	return true;
}

//Multicast
void CNetworkClient::MulticastAddMember(){
	mreq.imr_multiaddr.s_addr = inet_addr("235.7.7.10");
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	retval = setsockopt(UDPsock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
		(char *)&mreq, sizeof(mreq));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");
}
void CNetworkClient::MulticastDropMember(){
	retval = setsockopt(UDPsock, IPPROTO_IP, IP_DROP_MEMBERSHIP,
		(char *)&mreq, sizeof(mreq));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");
}

//프로토콜 출력시 사용
void CNetworkClient::ProtocolToChar(char _Protocol, char* _cProtocol){
	switch (_Protocol){
	case 0:
		strcpy(_cProtocol, "TCP_NULL");
		break;
	case 1:
		strcpy(_cProtocol, "TCP_MOVE");
		break;
	case 2:
		strcpy(_cProtocol, "TCP_LOGIN");
		break;
	case 3:
		strcpy(_cProtocol, "TCP_DISCONNECT");
		break;
	case 4:
		strcpy(_cProtocol, "UDP_PLAYER");
		break;
	}
}

//Release
void CNetworkClient::Release(){
	closesocket(TCPsock);
	closesocket(UDPsock);
	WSACleanup();
}