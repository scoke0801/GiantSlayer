#include "stdafx.h"
#include "PacketProcessor.h"
#include "Communicates.h"  
#include <thread>

int main(int argc, char* argv[])
{
	PacketProcessor::GetInstance();
	wcout.imbue(std::locale("korean"));

	int retVal;
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (listenSocket == INVALID_SOCKET) { error_quit("socket()"); }

	// bind()
	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(SERVERPORT);

	retVal = bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (retVal == SOCKET_ERROR) { error_quit("bind()"); }
	 
	//int opt_val = TRUE;
	//setsockopt(listenSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&opt_val, sizeof(opt_val));
	
	// listen() 
	retVal = listen(listenSocket, SOMAXCONN);
	if (retVal == SOCKET_ERROR) { error_quit("listen()"); }

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientAddr;
	int addrLen;

	HANDLE hThread;
	int registIndex = 0;

	thread worker(UpdateWorker);

	while (1) {
		// accept()
		addrLen = sizeof(clientAddr);
		client_sock = accept(listenSocket, (struct sockaddr*)&clientAddr, &addrLen);

		if (client_sock == INVALID_SOCKET) {
			error_display("accept()"); 
			break;
		}

		int c_id = PacketProcessor::GetInstance()->GetNewPlayerId(client_sock);

		if (-1 != c_id) {
			cout << "\n[TCP 서버] 클라이언트 접속 : IP 주소 = " << inet_ntoa(clientAddr.sin_addr)
				<< ", 포트 번호 = " << ntohs(clientAddr.sin_port) << endl;
			PacketProcessor::GetInstance()->RegistSocket(client_sock, registIndex++);

			PacketProcessor::GetInstance()->InitPrevUserData(c_id); 
			PacketProcessor::GetInstance()->DoRecv(c_id);  
		}
		else {
			closesocket(client_sock);
		} 
	}

	worker.join();
	// closesocket()
	closesocket(listenSocket);

	WSACleanup();
	return 0;
}
