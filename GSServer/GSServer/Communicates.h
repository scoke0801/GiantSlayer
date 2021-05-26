#pragma once
#include "protocol.h"
void charToWchar(const char* msg, wchar_t* out);

// 소켓 함수 오류 출력 후 종료
void error_quit(const char* msg);

// 소켓 함수 오류 출력
void error_display(const char* msg);

// recv를 이용한 사용자 정의함수
int recvn(SOCKET s, char* buf, int len, int flags); 

bool SendPacket(SOCKET& sock, char* packet, int packetSize, int& retVal);
bool RecvPacket(SOCKET& sock, char* buf, int& retVal);

XMFLOAT3 GetVectorFromText(const char* text);

DWORD WINAPI MainServerThread(LPVOID arg);
void UpdateWorker();