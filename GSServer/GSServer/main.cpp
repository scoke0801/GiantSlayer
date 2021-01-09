#include "stdafx.h"

#include "Communicates.h"  
int main(int argc, char* argv[])
{
#pragma region ForDebugHide
	int retVal;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) { err_quit("socket()"); }

	// bind()
	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(SERVERPORT);
	retVal = bind(listen_sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (retVal == SOCKET_ERROR) { err_quit("bind()"); }

	//int optval = 0;
	//setsockopt(listen_sock, SOL_SOCKET, SO_SNDBUF, (char*)&optval, sizeof(optval));

	int opt_val = TRUE;
	setsockopt(listen_sock, IPPROTO_TCP, TCP_NODELAY, (char*)&opt_val, sizeof(opt_val));
	// listen() 
	retVal = listen(listen_sock, SOMAXCONN);
	if (retVal == SOCKET_ERROR) { err_quit("listen()"); }
#pragma endregion
	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientAddr;
	int addrLen;

	HANDLE hThread;
	while (1) {
		// accept()
		addrLen = sizeof(clientAddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientAddr, &addrLen);
		int opt_val = TRUE;
		//setsockopt(client_sock, IPPROTO_TCP, TCP_NODELAY, (char*)&opt_val, sizeof(opt_val));

		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		cout << "\n[TCP ����] Ŭ���̾�Ʈ ���� : IP �ּ� = " << inet_ntoa(clientAddr.sin_addr)
			<< ", ��Ʈ ��ȣ = " << ntohs(clientAddr.sin_port) << endl;

		// ������ ����
		hThread = CreateThread(NULL, 0,
			ClientThread, (LPVOID)client_sock,
			0, NULL);

		if (hThread == NULL) { closesocket(client_sock); }
		else { CloseHandle(hThread); }
	}

	// closesocket()
	closesocket(listen_sock);

	WSACleanup();
	return 0;
}
