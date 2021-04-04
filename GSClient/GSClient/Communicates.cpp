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

bool SendPacket(SOCKET& sock, char* packet, int packetSize, int& retVal)
{  
    // 데이터 보내기(고정 길이)
    retVal = send(sock, (char*)&packetSize, sizeof(int), 0);
    if (retVal == SOCKET_ERROR)
    {
        error_display("send()");
        return false;
    }
    // 데이터 보내기(가변 길이)
    retVal = send(sock, packet, packetSize, 0);
    if (retVal == SOCKET_ERROR)
    {
        error_display("send()");
        return false;
    }
    return true;
}

bool RecvPacket(SOCKET& sock, char* buf, int& retVal)
{
    // 데이터 받기(고정 길이)
    int len;
    retVal = recvn(sock, (char*)&len, sizeof(int), 0);

    if (retVal == SOCKET_ERROR)
    {
        error_display("recv()");
        return false;
    }
    else if (retVal == 0) return false;

    // 데이터 받기(가변 길이)
    retVal = recvn(sock, buf, len, 0);

    if (retVal == SOCKET_ERROR)
    {
        error_display("recv()");
        return false;
    }

    buf[retVal] = '\0';
    return true;
}

void ProcessPacket(SOCKET& sock, char* packet, int packetSize, PACKET_PROTOCOL packetType)
{
    int retVal = -1;
   // 
     
    switch (packetType)
    {
    // client
    ///////////////////////////////////////////////////////////////////////////////
    case PACKET_PROTOCOL::C2S_LOGIN:
    
    break;
    case PACKET_PROTOCOL::C2S_LOGOUT:
    break;
    case PACKET_PROTOCOL::C2S_INGAME_KEYBOARD_INPUT:
        break;
    case PACKET_PROTOCOL::C2S_INGAME_MOUSE_INPUT:
        break;

   // server
   ///////////////////////////////////////////////////////////////////////////////
    case PACKET_PROTOCOL::S2C_LOGIN_HANDLE:
        break;
    default:
        break;
    }
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
