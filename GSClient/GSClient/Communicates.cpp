#include "stdafx.h"
#include "Communicates.h"

SESSION g_Client = SESSION();

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
    WCHAR* lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    cout << msg;
    wcout << lpMsgBuf << "\n";
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

void SendPacket(void* p)
{
    unsigned char p_size = reinterpret_cast<unsigned char*>(p)[0];
    unsigned char p_type = reinterpret_cast<unsigned char*>(p)[1];

    EX_OVER* s_over = new EX_OVER;
    memset(&s_over->m_over, 0, sizeof(s_over->m_over));
    memcpy(s_over->m_packetbuf, p, p_size);
    s_over->m_wsabuf.buf = reinterpret_cast<CHAR*>(s_over->m_packetbuf);
    s_over->m_wsabuf.len = p_size;

    auto ret = WSASend(g_Client.m_socket, &s_over->m_wsabuf, 1, NULL, 0, &s_over->m_over, NULL);
    if (0 != ret) {
        auto err_no = WSAGetLastError();
        if (WSA_IO_PENDING != err_no) { 
            cout << "로그아웃\n";
            //disconnect(p_id);
        }
    }
}

bool SendPacket(SOCKET& sock, char* packet, int packetSize, int& retVal)
{  
    //// 데이터 보내기(고정 길이)
    //retVal = send(sock, (char*)&packetSize, sizeof(int), 0);
    //if (retVal == SOCKET_ERROR)
    //{
    //    error_display("send()");
    //    return false;
    //}
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
   /* retVal = recvn(sock, (char*)&len, sizeof(int), 0);

    if (retVal == SOCKET_ERROR)
    {
        error_display("recv()");
        return false;
    }
    else if (retVal == 0) return false;*/

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

void recv_callback(DWORD error, DWORD num_bytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags)
{
    auto session = reinterpret_cast<SESSION*>(overlapped);
    SOCKET client_s = session->m_socket;
    auto ex_over = &session->m_recv_over;

    unsigned char* packet_ptr = ex_over->m_packetbuf;
    int num_data = num_bytes + session->m_prev_size;
    int packet_size = packet_ptr[0];

    while (num_data >= packet_size) {
        CFramework::GetInstance().GetCurrentScene()->ProcessPacket(packet_ptr); 
        num_data -= packet_size;
        packet_ptr += packet_size;
        if (0 >= num_data) break;
        packet_size = packet_ptr[0];
    }
    session->m_prev_size = num_data;
    if (0 != num_data) {
        memcpy(ex_over->m_packetbuf, packet_ptr, num_data);
    }

    CFramework::GetInstance().GetCurrentScene()->DoRecv(); 
}
