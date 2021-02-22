#pragma once

void charToWchar(const char* msg, wchar_t* out);

// 소켓 함수 오류 출력 후 종료
void err_quit(const char* msg);

// 소켓 함수 오류 출력
void err_display(const char* msg);

// recv를 이용한 사용자 정의함수
int recvn(SOCKET s, char* buf, int len, int flags);

bool RecvFrameData(SOCKET& client_sock, char* buf, int& retval);
bool SendFrameData(SOCKET& sock, string& str, int& retval);
XMFLOAT3 GetVectorFromText(const char* text);

DWORD WINAPI ClientThread(LPVOID arg);