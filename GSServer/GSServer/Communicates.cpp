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
 
bool SendPacket(SOCKET& sock, char* packet, int packetSize, int& retVal)
{
	// ������ ������(���� ����)
	retVal = send(sock, (char*)&packetSize, sizeof(int), 0);
	if (retVal == SOCKET_ERROR)
	{
		error_display("send()");
		return false;
	}
	// ������ ������(���� ����)
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
	// ������ �ޱ�(���� ����)
	int len;
	retVal = recvn(sock, (char*)&len, sizeof(int), 0);

	if (retVal == SOCKET_ERROR)
	{
		error_display("recv()");
		return false;
	}
	else if (retVal == 0) return false;

	// ������ �ޱ�(���� ����)
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

DWORD __stdcall MainServerThread(LPVOID arg)
{
	SOCKET client_sock = (SOCKET)arg;
	SOCKADDR_IN clientAddr;
	int addrLen;
	 
	// Ŭ���̾�Ʈ ���� �ޱ�
	addrLen = sizeof(clientAddr);
	getpeername(client_sock, (SOCKADDR*)&clientAddr, &addrLen);
	  
	while (1) {
		static std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();
		static std::chrono::duration<double> timeElapsed;

		timeElapsed = std::chrono::system_clock::now() - currentTime;
		currentTime = std::chrono::system_clock::now();
		//cout << "TimeElapsed: " << timeElapsed.count() << " \n";
		if (false == PacketProcessor::GetInstance()->ProcessGameScene(client_sock))
			break;

		PacketProcessor::GetInstance()->UpdateLoop();
	}

	// closesocket()
	closesocket(client_sock);

	std::cout << "[TCP ����] Ŭ���̾�Ʈ ���� : IP �ּ� = " << inet_ntoa(clientAddr.sin_addr)
		<< ", ��Ʈ ��ȣ = " << ntohs(clientAddr.sin_port) << endl;

	return 0;
}