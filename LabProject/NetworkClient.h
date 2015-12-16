#pragma once

//ネットワークで送信、受信する時に使うパケットです。
struct Network_Packet{
	D3DXVECTOR3	m_d3dxvPosition;
	D3DXVECTOR3	m_d3dxvRotation;
	char		m_ID[10];
	int			m_isMoving;
};

struct Network_Throw_Packet{
	int			m_iIndex;
	char		m_ID[10];
};

//プロトコルです。
enum{ TCP_NULL = 0, TCP_CLIENT_MOVE, TCP_CLIENT_TRANSFORM, TCP_CLIENT_THROWOBJECT, TCP_CLIENT_LOGIN, TCP_CLIENT_DISCONNECT, UDP_SERVER_PLAYER };

void SetSocketOption(SOCKET s, int level, int optname, char* optval, int optlen);
int recvn(SOCKET s, char *buf, int len, int flags);
void err_quit(char *msg);
void err_display(char *msg);

//ネットワーク接続、初期化などのネットワーク関数を管理するクラスです。
class CNetworkClient
{
public:
	CNetworkClient(char* _ServerIP, int _ServerPort);
	~CNetworkClient(void);

	//Init
	int		Init();
	int		WSAInit();
	void	SocketInit();
	void	Bind();
	void	BufferResize();
	void	Connect();

	//Receive
	bool TCPReceive(SOCKET* sock, char* Temp);
	bool UDPReceive(SOCKET* sock, char* Temp);

	//Send
	void TCPSendChar(SOCKET* sock, char _Protocol, char* _Data);
	void TCPSendInt(SOCKET* sock, char _Protocol, int _Data);
	template <typename T>
	void TCPSendData(SOCKET* sock, char _Protocol, T _Data);

	//Multicast
	void MulticastAddMember();
	void MulticastDropMember();

	//Debug (Print Protocol Information)
	void ProtocolToChar(char _Protocol, char* _cProtocol);

	//Release
	void Release();

	SOCKET* GetTCPSocket(){ return &m_TCPsock; }
	SOCKET* GetUDPSocket(){ return &m_UDPsock; }

private:
	WSADATA			wsa;

	SOCKET			m_TCPsock;
	SOCKET			m_UDPsock;

	SOCKADDR_IN		serveraddr;
	SOCKADDR_IN		localaddr;
	SOCKADDR_IN		peeraddr;

	int				m_iRetval;

	char			ServerIP[20];
	int				ServerPort;

	struct			ip_mreq mreq;
};

template <typename T>
void CNetworkClient::TCPSendData(SOCKET* sock, char _Protocol, T _Data){
	char cProtocol[20];
	int size = sizeof(_Data);
	m_iRetval = send(*sock, (char*)&_Protocol, sizeof(char), 0);
	m_iRetval = send(*sock, (char*)&size, sizeof(char), 0);
	m_iRetval = send(*sock, (char*)&_Data, size, 0);
	if (m_iRetval == SOCKET_ERROR){
		err_display("TCPsend()");
	}
	else{
		ProtocolToChar(_Protocol, cProtocol);
		//printf("[%s -> TCPSend] Protocol : %s, DataSize : %d\n", inet_ntoa(serveraddr.sin_addr), cProtocol, size, _Data);
	}
}