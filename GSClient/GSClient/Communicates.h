#pragma once
#define _CRT_SECURE_NO_WARNINGS         // 최신 VC++ 컴파일 시 경고 방지
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지

#include "../../GSServer/GSServer/protocol.h"

#pragma comment(lib, "ws2_32")
#include <WinSock2.h> 
 
//#define SERVERIP   "127.0.0.1"		 // 루프백
//#define SERVERIP   "192.168.35.118" + 
//#define SERVERIP   "172.30.1.39"
//#define SERVERIP "192.168.122.251"
//#define SERVERIP "10.20.11.36"
  
void textConvert(const char* msg, wchar_t* ret);

void error_quit(const char* msg);

void error_display(const char* msg);

int recvn(SOCKET s, char* buf, int len, int flag);

bool SendFrameData(SOCKET& sock, test_packet packet, int& retval);
bool SendFrameData(SOCKET& sock, string& str, int& retval);
bool RecvFrameData(SOCKET& sock, char* buf, int& retval);
 
int ConvertoIntFromText(const char* text, const char* token);

XMFLOAT3 GetVectorFromText(const char* text);
