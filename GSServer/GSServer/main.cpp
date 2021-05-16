#include "stdafx.h"
#include "PacketProcessor.h"
#include "Communicates.h"  
int main(int argc, char* argv[])
{
	PacketProcessor::GetInstance();
	wcout.imbue(std::locale("korean"));
#pragma region ForDebugHide
	int retVal;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) { error_quit("socket()"); }

	// bind()
	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(SERVERPORT);

	retVal = bind(listen_sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (retVal == SOCKET_ERROR) { error_quit("bind()"); }
	 
	int opt_val = TRUE;
	setsockopt(listen_sock, IPPROTO_TCP, TCP_NODELAY, (char*)&opt_val, sizeof(opt_val));
	
	// listen() 
	retVal = listen(listen_sock, SOMAXCONN);
	if (retVal == SOCKET_ERROR) { error_quit("listen()"); }
#pragma endregion
	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientAddr;
	int addrLen;

	HANDLE hThread;
	int registIndex = 0;
	while (1) {
		// accept()
		addrLen = sizeof(clientAddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientAddr, &addrLen);
		 
		if (client_sock == INVALID_SOCKET) {
			error_display("accept()");
			break;
		}

		cout << "\n[TCP ����] Ŭ���̾�Ʈ ���� : IP �ּ� = " << inet_ntoa(clientAddr.sin_addr)
			<< ", ��Ʈ ��ȣ = " << ntohs(clientAddr.sin_port) << endl;
		PacketProcessor::GetInstance()->RegistSocket(client_sock, registIndex++);
		// ������ ����
		hThread = CreateThread(NULL, 0,
			MainServerThread, (LPVOID)client_sock,
			0, NULL);

		if (hThread == NULL) { closesocket(client_sock); }
		else { CloseHandle(hThread); }
	}

	// closesocket()
	closesocket(listen_sock);

	WSACleanup();
	return 0;
}
