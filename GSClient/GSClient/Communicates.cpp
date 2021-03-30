#include "stdafx.h"
#include "Communicates.h"


void textConvert(const char* msg, wchar_t* ret)
{
    mbstowcs(ret, msg, strlen(msg) + 1);//Plus null
}

void error_quit(const char* msg)
{
    wchar_t wMsg[20];
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    textConvert(msg, wMsg);
    MessageBox(NULL, (LPCTSTR)lpMsgBuf, wMsg, MB_ICONERROR);
    LocalFree(lpMsgBuf);
    exit(1);
}

void error_display(const char* msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    printf("[%s] %s", msg, (char*)lpMsgBuf);
    LocalFree(lpMsgBuf);
}

int recvn(SOCKET s, char* buf, int len, int flags)
{
    int received;
    char* ptr = buf;
    int left = len;

    while (left > 0) {
        received = recv(s, ptr, left, flags);
        if (received == SOCKET_ERROR)
            return SOCKET_ERROR;
        else if (received == 0)
            break;
        left -= received;
        ptr += received;
    }

    return (len - left);
}

bool SendFrameData(SOCKET& sock, test_packet packet, int& retval)
{ 
    int data_len = sizeof(packet); 
    int packet_len = sizeof(test_packet);
    if (data_len != packet_len) {
        int stop = 3;
    }
    // 데이터 보내기(고정 길이)
    retval = send(sock, (char*)&data_len, sizeof(int), 0);
    if (retval == SOCKET_ERROR)
    {
        error_display("send()");
        return false;
    }
    // 데이터 보내기(가변 길이)
    retval = send(sock, reinterpret_cast<char*>(&packet), packet_len, 0);
    if (retval == SOCKET_ERROR)
    {
        error_display("send()");
        return false;
    }
    return true;
}

bool SendFrameData(SOCKET& sock, string& str, int& retval)
{
    int len = str.length();

    // 데이터 보내기(고정 길이)
    retval = send(sock, (char*)&len, sizeof(int), 0);
    if (retval == SOCKET_ERROR)
    {
        error_display("send()");
        return false;
    }

    // 데이터 보내기(가변 길이)
    retval = send(sock, str.c_str(), len, 0);
    if (retval == SOCKET_ERROR)
    {
        error_display("send()");
        return false;
    }
    return true;
}

bool RecvFrameData(SOCKET& sock, char* buf, int& retval)
{
    // 데이터 받기(고정 길이)
    int len;
    retval = recvn(sock, (char*)&len, sizeof(int), 0);

    if (retval == SOCKET_ERROR)
    {
        error_display("recv()");
        return false;
    }
    else if (retval == 0) return false;

    // 데이터 받기(가변 길이)
    retval = recvn(sock, buf, len, 0);

    if (retval == SOCKET_ERROR)
    {
        error_display("recv()");
        return false;
    }

    buf[retval] = '\0';
    return true;
}
 
int ConvertoIntFromText(const char* text, const char* token)
{
    char buf[256];
    ZeroMemory(buf, 256);
    int tokenLen = strlen(token);
    strncpy(buf, text + tokenLen, strlen(text) - tokenLen);
    return atoi(buf);
}

XMFLOAT3 GetVectorFromText(const char* text)
{
    return XMFLOAT3();
}
