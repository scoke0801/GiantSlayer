#pragma once

void charToWchar(const char* msg, wchar_t* out);

// ���� �Լ� ���� ��� �� ����
void err_quit(const char* msg);

// ���� �Լ� ���� ���
void err_display(const char* msg);

// recv�� �̿��� ����� �����Լ�
int recvn(SOCKET s, char* buf, int len, int flags);

bool RecvFrameData(SOCKET& client_sock, char* buf, int& retval);
bool SendFrameData(SOCKET& sock, string& str, int& retval);
XMFLOAT3 GetVectorFromText(const char* text);

DWORD WINAPI ClientThread(LPVOID arg);