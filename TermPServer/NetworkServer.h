#pragma once
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <vector>
using namespace std;

enum{ TCP_NULL = 0, TCP_CLIENT_MOVE, TCP_CLIENT_TRANSFORM, TCP_CLIENT_THROWOBJECT, TCP_CLIENT_LOGIN, TCP_CLIENT_DISCONNECT, UDP_SERVER_PLAYER };

int recvn(SOCKET s, char *buf, int len, int flags);
void err_quit(char *msg);
void err_display(char *msg);

class CNetworkServer
{
public:
	CNetworkServer(int _ServerPort);
	~CNetworkServer(void);

	//Init
	int		Init();
	int		WSAInit();
	void	SocketInit();

	void	Bind();
	void	Listen();
	bool	Accept();
	void	BufferResize();
	
	//Receive
	bool	TCPReceive(SOCKET* sock, char* Temp);

	//Send
	void	TCPSendInt(SOCKET* sock, char _Protocol, int _Data);
	void	TCPSendChar(SOCKET* sock, char _Protocol, char* _Data);
	template <typename T>
	void	TCPSendData(SOCKET* sock, char _Protocol, T _Data);
	template <typename T>
	void	UDPSendData(SOCKET* sock, char _Protocol, T _Data, SOCKADDR_IN* udpaddr);

	//Protocol
	void ProtocolToChar(char _Protocol, char* _cProtocol);

	//Release
	void Release();

	SOCKET GetTCPSocket(){ return m_TCPClientsock; }
	SOCKET* GetUDPSocket(){ return &m_UDPsock; }
	vector<SOCKET*>* GetSocketList(){ return &m_vSocket; }

private:
	WSADATA					wsa;	

	SOCKET					m_TCPsock;
	SOCKET					m_UDPsock;

	SOCKET					m_TCPClientsock;

	SOCKADDR_IN				serveraddr;
	SOCKADDR_IN				clientaddr;

	int						m_iRetval;
	int						ServerPort;

	vector<SOCKET*>			m_vSocket;

};

template <typename T>
void CNetworkServer::TCPSendData(SOCKET* sock,char _Protocol,T _Data){
	char cProtocol[20];
	int size = sizeof(_Data);
	m_iRetval = send(*sock, (char*)&_Protocol, sizeof(char), 0);
	m_iRetval = send(*sock, (char*)&size, sizeof(char), 0);
	m_iRetval = send(*sock, (char*)&_Data, size, 0);
	if (m_iRetval == SOCKET_ERROR){
		err_display("TCPSend()");
	}
	else{
		ProtocolToChar(_Protocol,cProtocol);
		//printf("[%s -> TCPSend]Protocol : %s, DataSize : %d\n",inet_ntoa(clientaddr.sin_addr),cProtocol,size);
	}
}

template <typename T>
void CNetworkServer::UDPSendData(SOCKET* sock,char _Protocol,T _Data,SOCKADDR_IN* udpaddr){
	char cProtocol[20];
	int size = sizeof(_Data);
	int addrsize = sizeof(*udpaddr);
	m_iRetval = sendto(*sock, (char*)&_Protocol, sizeof(char), 0, (SOCKADDR *)udpaddr, addrsize);
	m_iRetval = sendto(*sock, (char*)&size, sizeof(char), 0, (SOCKADDR *)udpaddr, addrsize);
	m_iRetval = sendto(*sock, (char*)&_Data, size, 0, (SOCKADDR *)udpaddr, addrsize);
	if (m_iRetval == SOCKET_ERROR){
		err_display("UDPsend()");
	}		
	/*
	else{
		ProtocolToChar(_Protocol,cProtocol);
		printf("[%s -> UDPSend]Protocol : %s, DataSize : %d\n",inet_ntoa(clientaddr.sin_addr),cProtocol,size);
	}
	*/
}