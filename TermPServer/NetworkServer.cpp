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

int CNetworkServer::WSAInit(){
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return -1;
	return 0;
}

void CNetworkServer::SocketInit(){
	TCPsock = socket(AF_INET, SOCK_STREAM, 0);
	UDPsock = socket(AF_INET, SOCK_DGRAM, 0);

	if(TCPsock == INVALID_SOCKET) err_quit("socket()");
	if(UDPsock == INVALID_SOCKET) err_quit("socket()");
}

void CNetworkServer::Bind(){
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(ServerPort);
	TCPretval = bind(TCPsock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(TCPretval == SOCKET_ERROR) err_quit("bind()");

	int ttl = 2;
	UDPretval = setsockopt(UDPsock, IPPROTO_IP, IP_MULTICAST_TTL,
		(char *)&ttl, sizeof(ttl));
	if(UDPretval == SOCKET_ERROR) err_quit("setsockopt()");
}

void CNetworkServer::Listen(){
	TCPretval = listen(TCPsock, SOMAXCONN);
	if(TCPretval == SOCKET_ERROR) err_quit("listen()");
}

bool CNetworkServer::TCPReceive(SOCKET* sock,char* Temp){
	char cProtocol[20];
	int TCPretval = recvn(*sock,(char*)&Temp[0],sizeof(char),0);
		TCPretval = recvn(*sock,(char*)&Temp[1],sizeof(char),0);
		TCPretval = recvn(*sock,(char*)&Temp[2],Temp[1],0);
	if(TCPretval == SOCKET_ERROR){
		//err_display("recv()");
		return false;
	}
	else{
		ProtocolToChar(Temp[0],cProtocol);
		printf("[%s -> TCPReceive] Protocol : %s, Datasize : %d\n",inet_ntoa(serveraddr.sin_addr),cProtocol,Temp[1]);
	}
	return true;
}

void CNetworkServer::TCPSendChar(SOCKET* sock,char _Protocol,char* _Data){
	char cProtocol[20];
	int size = strlen(_Data);
	int TCPSend = send(*sock,(char*)&_Protocol,sizeof(char),0);
	TCPSend = send(*sock,(char*)&size,sizeof(char),0);
	TCPSend = send(*sock,_Data,size,0);
	if(TCPSend == SOCKET_ERROR)
		err_display("send()");
	else{
		ProtocolToChar(_Protocol,cProtocol);
		printf("[%s -> TCPSend]Protocol : %s, DataSize : %d, Data : %s\n",inet_ntoa(clientaddr.sin_addr),cProtocol,size,_Data);
	}
}

void CNetworkServer::TCPSendInt(SOCKET* sock,char _Protocol,int _Data){
	char cProtocol[20];
	int retval;
	int size = sizeof(_Data);
	retval = send(*sock,(char*)&_Protocol,sizeof(char),0);
	retval = send(*sock,(char*)&size,sizeof(char),0);
	retval = send(*sock,(char*)&_Data,size,0);
	ProtocolToChar(_Protocol,cProtocol);
	if(retval == SOCKET_ERROR)
		err_display("send()");
	else
		printf("[%s -> TCPSend] Protocol : %s, DataSize : %d, Data:%d\n",inet_ntoa(serveraddr.sin_addr),cProtocol,size,_Data);
}

void CNetworkServer::BufferResize(){
	int optval, optlen,retval;
	optlen = sizeof(optval);
	retval = getsockopt(TCPsock, SOL_SOCKET, SO_RCVBUF,
	(char *)&optval, &optlen);
	if(retval == SOCKET_ERROR) err_quit("getsockopt()");
	optval *= 100;
	retval = setsockopt(TCPsock, SOL_SOCKET, SO_RCVBUF,
	(char *)&optval, sizeof(optval));
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

void CNetworkServer::Release(){
	closesocket(TCPsock);
	closesocket(UDPsock);

	WSACleanup();
}
bool CNetworkServer::Accept(){
		addrsize = sizeof(clientaddr);
		TCPClientsock = accept(TCPsock, (SOCKADDR *)&clientaddr, &addrsize);
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		if(TCPClientsock == INVALID_SOCKET){
			err_display("accept()");
			return false;
		}
		return true;
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