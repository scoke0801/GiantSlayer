#pragma once

void charToWchar(const char* msg, wchar_t* out);

// ���� �Լ� ���� ��� �� ����
void err_quit(const char* msg);

// ���� �Լ� ���� ���
void err_display(const char* msg);

// recv�� �̿��� ����� �����Լ�
int recvn(SOCKET s, char* buf, int len, int flags);

DWORD WINAPI ClientThread(LPVOID arg);