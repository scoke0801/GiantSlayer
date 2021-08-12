#include "stdafx.h"
#include "Communicates.h"
#include "PacketProcessor.h"

void charToWchar(const char* msg, wchar_t* out)
{
	mbstowcs(out, msg, strlen(msg) + 1);//Plus null
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

	charToWchar(msg, wMsg);
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

		if (received == SOCKET_ERROR) { return SOCKET_ERROR; }
		else if (received == 0) { break; }

		left -= received;
		ptr += received;
	}

	return (len - left);
}
 
void SendPacket(SOCKET& sock, void* p)
{
	unsigned char p_size = reinterpret_cast<unsigned char*>(p)[0];
	unsigned char p_type = reinterpret_cast<unsigned char*>(p)[1];

	EX_OVER* s_over = new EX_OVER;
	memset(&s_over->m_over, 0, sizeof(s_over->m_over));
	memcpy(s_over->m_packetbuf, p, p_size);
	s_over->m_wsabuf.buf = reinterpret_cast<CHAR*>(s_over->m_packetbuf);
	s_over->m_wsabuf.len = p_size;
	 
	auto ret = WSASend(sock, &s_over->m_wsabuf, 1, NULL, 0, &s_over->m_over, NULL);
	if (0 != ret) {
		auto err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no) {
			error_display("WSASend : "); 
		}
	}
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

XMFLOAT3 GetVectorFromText(const char* text)
{
	return XMFLOAT3();
}

void UpdateWorker()
{
	while (1) {
		//static std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();
		//static std::chrono::duration<double> timeElapsed;

		//timeElapsed = std::chrono::system_clock::now() - currentTime;
		//currentTime = std::chrono::system_clock::now(); 

		PacketProcessor::GetInstance()->UpdateLoop();
	}
}
