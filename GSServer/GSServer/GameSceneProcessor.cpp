#include "stdafx.h"
#include "GameSceneProcessor.h"
#include "protocol.h"

bool GameSceneProcessor::ProcessGameScene(SOCKET& socket)
{
	//static std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();
	//static std::chrono::duration<double> timeElapsed;
	//
	//timeElapsed = std::chrono::system_clock::now() - currentTime;
	//currentTime = std::chrono::system_clock::now();
	////cout << "TimeElapsed: " << timeElapsed.count() << " ";

	char buffer[BUFSIZE + 1] = {}; 
	int count = 0;
	int retval = 0;
	RecvPacket(socket, buffer, retval); 

	// buffer[1]ÀÇ °ªÀº protocol type
	PACKET_PROTOCOL type = (PACKET_PROTOCOL)buffer[1];
	switch (type) {
	case PACKET_PROTOCOL::C2S_LOGIN:
	{
		P_C2S_LOGIN p_login = *reinterpret_cast<P_C2S_LOGIN*>(buffer);

		m_Players[m_CurrentPlayerNum]->SetId(m_CurrentPlayerNum);

		P_S2C_PROCESS_LOGIN p_processLogin;
		p_processLogin.size = sizeof(p_processLogin);
		p_processLogin.type = PACKET_PROTOCOL::S2C_LOGIN_HANDLE;
		p_processLogin.isSuccess = true;
		p_processLogin.id = m_CurrentPlayerNum;

		XMFLOAT3 pos = m_Players[m_CurrentPlayerNum]->GetPosition();
		p_processLogin.x = FloatToInt(pos.x);
		p_processLogin.y = FloatToInt(pos.y);
		p_processLogin.z = FloatToInt(pos.z);
		SendPacket(socket, reinterpret_cast<char*>(&p_processLogin), sizeof(p_processLogin), retval);
		
		m_CurrentPlayerNum++;
	}
	return true;
	case PACKET_PROTOCOL::C2S_LOGOUT:
	{

	}
	return true;
	case PACKET_PROTOCOL::C2S_INGAME_MOUSE_INPUT: 
	{

	}
	return true;
	case PACKET_PROTOCOL::C2S_INGAME_KEYBOARD_INPUT:
	{
	}
	return true;
	case PACKET_PROTOCOL::C2S_INGAME_UPDATE_SYNC:
	{
		P_S2C_UPDATE_SYNC p_syncUpdate;

		p_syncUpdate.playerNum = m_CurrentPlayerNum;

		for (int i = 0; i < m_CurrentPlayerNum; ++i) {
			p_syncUpdate.id[i] = m_Players[i]->GetId(); 
			
			XMFLOAT3 pos = m_Players[i]->GetPosition();
			p_syncUpdate.posX[i] = FloatToInt(pos.x);
			p_syncUpdate.posY[i] = FloatToInt(pos.y);
			p_syncUpdate.posZ[i] = FloatToInt(pos.z);
		}
		SendPacket(socket, reinterpret_cast<char*>(&p_syncUpdate), sizeof(p_syncUpdate), retval);
	}
	return true;
	}
	      
	return false;
}

void GameSceneProcessor::InitAll()
{
	InitPlayers(); 
	InitMonsters();
}

void GameSceneProcessor::InitPlayers()
{
	XMFLOAT3 positions[MAX_USER] = {
		{ 750.0f, 230.0f, 1850.0f },
		{ 750.0f,  230.0f, 2250.0f },
		{ 950.0f,  230.0f, 1850.0f },
		{ 750.0f,  230.0f, 2050.0f },
		{ 1150.0f,  230.0f, 2250.0f }
	};
	for (int i = 0; i < MAX_USER; ++i) {
		m_Players[i] = new CPlayer();
		m_Players[i]->SetPosition(positions[i]);
	}
}

void GameSceneProcessor::InitMonsters()
{
}
