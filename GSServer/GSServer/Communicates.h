#pragma once
#include "protocol.h"
void charToWchar(const char* msg, wchar_t* out);

// ���� �Լ� ���� ��� �� ����
void error_quit(const char* msg);

// ���� �Լ� ���� ���
void error_display(const char* msg);

// recv�� �̿��� ����� �����Լ�
int recvn(SOCKET s, char* buf, int len, int flags); 

bool SendPacket(SOCKET& sock, char* packet, int packetSize, int& retVal);
bool RecvPacket(SOCKET& sock, char* buf, int& retVal);

XMFLOAT3 GetVectorFromText(const char* text);

DWORD WINAPI MainServerThread(LPVOID arg);
void UpdateWorker();