#pragma once
#define _CRT_SECURE_NO_WARNINGS         // 최신 VC++ 컴파일 시 경고 방지
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지

#include "../../GSServer/GSServer/protocol.h"

#pragma comment(lib, "ws2_32")
#include <WinSock2.h>  

#define	WM_SOCKET				WM_USER + 1
struct EX_OVER {
	WSAOVERLAPPED   m_over;
	WSABUF          m_wsabuf;
	unsigned char   m_packetbuf[MAX_BUFFER];
	SOCKET m_csocket; // OP_ACCEPT 에서만 사용
};
 
struct SESSION {
	EX_OVER		m_recv_over;
	SOCKET		m_socket;  

	int			m_prev_size;

	char		m_name[200]; 
};

extern SESSION g_Client;

void textConvert(const char* msg, wchar_t* ret);

void error_quit(const char* msg);

void error_display(const char* msg);

int recvn(SOCKET s, char* buf, int len, int flag);

void SendPacket(void* p);
bool SendPacket(SOCKET& sock, char* packet, int packetSize, int& retVal);
bool RecvPacket(SOCKET& sock, char* buf, int& retVal);

void ProcessPacket(SOCKET& sock, char* packet, int packetSize, PACKET_PROTOCOL packetType);

int ConvertoIntFromText(const char* text, const char* token);

XMFLOAT3 GetVectorFromText(const char* text);
 