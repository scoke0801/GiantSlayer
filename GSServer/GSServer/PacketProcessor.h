#pragma once

#include "GameRoom.h"

constexpr int SERVER_ID = 0;

class PacketProcessor
{
private:
	array <CGameRoom, MAX_PLAYER> m_Rooms;
	array <CLIENT, MAX_PLAYER + 1> m_Clients;

private:

	// 어떤 플레이어, 몇번째 방에 입장해있는지 정보
	unordered_map<SOCKET, int>	m_SocketRegister;
	CRITICAL_SECTION			m_cs;
	 
private:
	// for update timer
	chrono::system_clock::time_point currentTime;
	chrono::duration<double> timeElapsed;

private:
	PacketProcessor()
	{
		currentTime = std::chrono::system_clock::now();
		InitializeCriticalSection(&m_cs);  
		InitTerrainHeightMap();
		//ZeroMemory(m_Players, sizeof(m_Players)); 
	}
	~PacketProcessor() { DeleteCriticalSection(&m_cs); }

	PacketProcessor(const PacketProcessor& other) = delete;
	void operator=(const PacketProcessor& other) = delete;

public:
	static PacketProcessor* GetInstance()
	{
		static PacketProcessor self;
		return &self;
	}
	void UpdateLoop();

	void RegistSocket(SOCKET& socket, int id) { m_SocketRegister[socket] = id; }
	  
	bool ProcessLogin(SOCKET& socket);


	int GetNewPlayerId(SOCKET socket);

	void InitPrevUserData(int c_id);
	void DoRecv(int c_id);

	void Disconnect(CLIENT& client);
	void ProcessPacket(CLIENT& client, unsigned char* p_buf);

private:
	void Update(float elapsedTime);
	void InitTerrainHeightMap();  

};

void CALLBACK recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags);
void CALLBACK send_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags);
