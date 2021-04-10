#include "stdafx.h"
#include "PacketProcessor.h"
#include "protocol.h" 
#include "MapObjects.h"

bool PacketProcessor::ProcessGameScene(SOCKET& socket)
{ 
	char buffer[BUFSIZE + 1] = {}; 
	int count = 0;
	int retval = 0;
	RecvPacket(socket, buffer, retval); 

	// buffer[0]의 값은 packet protocol size
	// buffer[1]의 값은 packet protocol type
	PACKET_PROTOCOL type = (PACKET_PROTOCOL)buffer[1];
	switch (type) {
	case PACKET_PROTOCOL::C2S_LOGIN:
	{
		//P_C2S_LOGIN p_login = *reinterpret_cast<P_C2S_LOGIN*>(buffer);
		 
		P_S2C_PROCESS_LOGIN p_processLogin;
		p_processLogin.size = sizeof(p_processLogin);
		p_processLogin.type = PACKET_PROTOCOL::S2C_LOGIN_HANDLE;
		
		if (m_CurrentPlayerNum > MAX_PLAYER) {
			p_processLogin.isSuccess = false;
		}
		else{
			p_processLogin.isSuccess = true; 

			m_Players[m_CurrentPlayerNum]->SetExistence(true);
			m_Players[m_CurrentPlayerNum]->SetId(m_CurrentPlayerNum);
			XMFLOAT3 pos = m_Players[m_CurrentPlayerNum]->GetPosition();

			p_processLogin.x = FloatToInt(pos.x);
			p_processLogin.y = FloatToInt(pos.y);
			p_processLogin.z = FloatToInt(pos.z);
		} 
		p_processLogin.id = m_CurrentPlayerNum;
		for (int i = 0; i < MAX_PLAYER; ++i) {
			p_processLogin.existPlayer[i] = m_Players[i]->IsExist();
		}
		SendPacket(socket, reinterpret_cast<char*>(&p_processLogin), sizeof(p_processLogin), retval);
		
		m_CurrentPlayerNum++;

		// 로그인 이후 데이터 싱크 맞추기 작업용 패킷 전송
		P_S2C_UPDATE_SYNC p_syncUpdate;
		p_syncUpdate.type = PACKET_PROTOCOL::S2C_INGAME_UPDATE_PLAYERS_STATE;
		p_syncUpdate.size = sizeof(p_syncUpdate);

		p_syncUpdate.playerNum = m_CurrentPlayerNum;

		for (int i = 0; i < m_CurrentPlayerNum; ++i) {
			p_syncUpdate.id[i] = static_cast<char>(m_Players[i]->GetId());

			XMFLOAT3 pos = m_Players[i]->GetPosition();
			p_syncUpdate.posX[i] = FloatToInt(pos.x);
			p_syncUpdate.posY[i] = FloatToInt(pos.y);
			p_syncUpdate.posZ[i] = FloatToInt(pos.z);
		}
		SendPacket(socket, reinterpret_cast<char*>(&p_syncUpdate), sizeof(p_syncUpdate), retval);
	}
	return true;
	case PACKET_PROTOCOL::C2S_LOGOUT:
	{
		P_C2S_LOGOUT p_logout = 
			*reinterpret_cast<P_C2S_LOGOUT*>(buffer);
		m_CurrentlyDeletedPlayerId = p_logout.id;
		m_Players[p_logout.id]->SetExistence(false);
	}
	return true;
	case PACKET_PROTOCOL::C2S_INGAME_MOUSE_INPUT: 
	{

	}
	return true;
	case PACKET_PROTOCOL::C2S_INGAME_KEYBOARD_INPUT:
	{
		P_C2S_KEYBOARD_INPUT p_keyboard =
			*reinterpret_cast<P_C2S_KEYBOARD_INPUT*>(buffer);
		
		XMFLOAT3 pos = m_Players[p_keyboard.id]->GetPosition();

		//m_Players[p_keyboard.id]->Update();

		// update player using key...
		switch (p_keyboard.keyInput)
		{
		case VK_W:
			m_Players[p_keyboard.id]->SetVelocity(OBJ_DIRECTION::Front);
			break;
		case VK_S:
			m_Players[p_keyboard.id]->SetVelocity(OBJ_DIRECTION::Back);
			break; 
		case VK_A:
			m_Players[p_keyboard.id]->SetVelocity(OBJ_DIRECTION::Left);
			break;
		case VK_D:
			m_Players[p_keyboard.id]->SetVelocity(OBJ_DIRECTION::Right);
			break;
		}

		P_S2C_PROCESS_KEYBOARD p_keyboardProcess;
		p_keyboardProcess.size = sizeof(p_keyboardProcess);
		p_keyboardProcess.type = PACKET_PROTOCOL::C2S_INGAME_KEYBOARD_INPUT;

		p_keyboardProcess.posX = FloatToInt(pos.x);
		p_keyboardProcess.posY = FloatToInt(pos.y);
		p_keyboardProcess.posZ = FloatToInt(pos.z);

		SendPacket(socket, reinterpret_cast<char*>(&p_keyboardProcess), p_keyboardProcess.size, retval);
	}
	return true;
	case PACKET_PROTOCOL::C2S_INGAME_UPDATE_SYNC:
	{
		P_C2S_UPDATE_SYNC_REQUEST p_updateSyncRequest =
			*reinterpret_cast<P_C2S_UPDATE_SYNC_REQUEST*>(buffer);

		// 클라이언트의 유저 수와 다르게
		// 새롭게 추가된 유저가 있는 경우 
		// 추가된 유저의 정보를 클라이언트에 전송합니다.
		int clientPlayerNum = p_updateSyncRequest.playerNum;
		if (clientPlayerNum < m_CurrentPlayerNum) {
			P_S2C_ADD_PLAYER p_addPlayer;
			p_addPlayer.size = sizeof(p_addPlayer);
			p_addPlayer.type = PACKET_PROTOCOL::S2C_NEW_PLAYER;
			p_addPlayer.id = m_CurrentPlayerNum - 1;	// 가장 마지막에 들어온 유저
			
			XMFLOAT3 pos = m_Players[p_addPlayer.id]->GetPosition();
			p_addPlayer.x = FloatToInt(pos.x);
			p_addPlayer.y = FloatToInt(pos.y);
			p_addPlayer.z = FloatToInt(pos.z);
			SendPacket(socket, reinterpret_cast<char*>(&p_addPlayer), sizeof(p_addPlayer), retval);
		}
		else if (clientPlayerNum > m_CurrentPlayerNum) {
			P_S2C_DELETE_PLAYER p_deletePlayer;
			p_deletePlayer.size = sizeof(p_deletePlayer);
			p_deletePlayer.type = PACKET_PROTOCOL::S2C_DELETE_PLAYER;
			p_deletePlayer.id = m_CurrentlyDeletedPlayerId;
			SendPacket(socket, reinterpret_cast<char*>(&p_deletePlayer), p_deletePlayer.size, retval);
		} 

		// 도어 변경 조건에 따라서
		// 게임 엔딩 조건에 따라서 
		// 추가적인 내용을 더 보내도록 코드 수정 필요 

		P_S2C_UPDATE_SYNC p_syncUpdate;
		p_syncUpdate.type = PACKET_PROTOCOL::S2C_INGAME_UPDATE_PLAYERS_STATE;
		p_syncUpdate.size = sizeof(p_syncUpdate);

		p_syncUpdate.playerNum = m_CurrentPlayerNum;

		for (int i = 0; i < m_CurrentPlayerNum; ++i) {
			p_syncUpdate.id[i] = static_cast<char>(m_Players[i]->GetId()); 
			
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

void PacketProcessor::UpdateLoop()
{	 
	timeElapsed = std::chrono::system_clock::now() - currentTime;
	float dLag = 0.0f;

	if (timeElapsed.count() > FPS)
	{
		currentTime = std::chrono::system_clock::now();

		//게임 시간이 늦어진 경우 이를 따라잡을 때 까지 업데이트 시킵니다.
		dLag += timeElapsed.count();
		for (int i = 0; dLag > FPS && i < MAX_LOOP_TIME; ++i)
		{
			Update(FPS);
			dLag -= FPS;
		}
	}
	else
		return; 
}

void PacketProcessor::InitAll()
{
	InitTerrainHeightMap();
	ReadObstaclesPosition();

	InitPlayers(); 
	InitMonsters();
	InitObstacle();
}

void PacketProcessor::Update(float elapsedTime)
{
	for (auto pObject : m_Objects) {
		pObject->Update(elapsedTime);
		pObject->UpdateColliders();
	}
	  
	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (m_Players[i]->IsExist()) {
			m_Players[i]->Update(elapsedTime);
			m_Players[i]->FixPositionByTerrain(m_Heights);
			m_Players[i]->UpdateColliders();
		}
	}

	// 오브젝트 - 플레이어 간 충돌처리
	for (auto pObject : m_Objects) {
		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (m_Players[i]->IsExist() == false) continue;

			if (pObject->CollisionCheck(m_Players[i])) {
				m_Players[i]->FixCollision(); 
				cout << "충돌발생 - [오브젝트, 플레이어 " << i << "]\n";
			}
		}
	}
}

void PacketProcessor::InitPlayers()
{
	// 플레이어 시작 위치..
	XMFLOAT3 positions[MAX_PLAYER] = {
		{ 550.0f,   230.0f,  1850.0f },
		{ 850.0f,   230.0f,  1850.0f },
		{ 1250.0f,  230.0f,  1850.0f },
		{ 850.0f,   230.0f,  2200.0f },
		{ 850.0f,   230.0f,  1500.0f }
	};
	for (int i = 0; i < MAX_PLAYER; ++i) {
		m_Players[i] = new CPlayer(); 
		m_Players[i]->Scale(50, 50, 50);
		m_Players[i]->SetPosition(positions[i]);
		m_Players[i]->SetExistence(false); 
		m_Players[i]->AddBoundingBox(BoundingBox(XMFLOAT3(0, 0, 0), XMFLOAT3(5, 5, 5)));
	}
}

void PacketProcessor::InitMonsters()
{
	
}

void PacketProcessor::ReadObstaclesPosition()
{
	FILE* fp = fopen("resources/ObjectPositionData.json", "rb"); // non-Windows use "r"
	char readBuffer[4096];
	FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	Document d;
	d.ParseStream(is);	//==d.Parse(is);
	fclose(fp);

	m_ObjectPositions.emplace(OBJECT_ID::BRIDEGE_SEC2_SEC3_1,
		GetPosition("BRIDEGE_SEC2_SEC3_1", d));
	m_ObjectPositions.emplace(OBJECT_ID::BRIDEGE_SEC2_SEC3_2,
		GetPosition("BRIDEGE_SEC2_SEC3_2", d));
	m_ObjectPositions.emplace(OBJECT_ID::BRIDEGE_SEC2_SEC3_3,
		GetPosition("BRIDEGE_SEC2_SEC3_3", d));

	m_ObjectPositions.emplace(OBJECT_ID::SIGN_SCROLL,
		GetPosition("SIGN_SCROLL", d));
	m_ObjectPositions.emplace(OBJECT_ID::SIGN_PUZZLE,
		GetPosition("SIGN_PUZZLE", d));
	m_ObjectPositions.emplace(OBJECT_ID::SIGN_MEDUSA,
		GetPosition("SIGN_MEDUSA", d)); 
	m_ObjectPositions.emplace(OBJECT_ID::SIGN_BOSS,
		GetPosition("SIGN_BOSS", d));

	m_ObjectPositions.emplace(OBJECT_ID::DOOR_WALL_SEC1,
		GetPosition("DOOR_WALL_SEC1", d));
	m_ObjectPositions.emplace(OBJECT_ID::DOOR_WALL_SEC2,
		GetPosition("DOOR_WALL_SEC2", d));
	m_ObjectPositions.emplace(OBJECT_ID::DOOR_WALL_SEC3,
		GetPosition("DOOR_WALL_SEC3", d));
	m_ObjectPositions.emplace(OBJECT_ID::DOOR_WALL_SEC4,
		GetPosition("DOOR_WALL_SEC4", d));
	m_ObjectPositions.emplace(OBJECT_ID::DOOR_WALL_SEC5,
		GetPosition("DOOR_WALL_SEC5", d));
}

XMFLOAT3 PacketProcessor::GetPosition(const string& name, const Document& document)
{
	XMFLOAT3 pos; 
	pos.x = document[name.c_str()].GetArray()[0].GetInt();
	pos.y = document[name.c_str()].GetArray()[1].GetInt();
	pos.z = document[name.c_str()].GetArray()[2].GetInt(); 

	return pos;
}

void PacketProcessor::InitObstacle()
{
// Bridge --------------------------------------------------------------------
	CGameObject* pObject; /*= new CObjCollector(OBJECT_ID::BRIDEGE_SEC2_SEC3_1);
	pObject->SetPosition(m_ObjectPositions[OBJECT_ID::BRIDEGE_SEC2_SEC3_1]);
	pObject->Rotate({ 0, 1, 0 }, 90);
	m_Objects.push_back(std::move(pObject));

	pObject = new CObjCollector(OBJECT_ID::BRIDEGE_SEC2_SEC3_2);
	pObject->SetPosition(m_ObjectPositions[OBJECT_ID::BRIDEGE_SEC2_SEC3_2]);
	pObject->Rotate({ 0, 1, 0 }, 90);
	m_Objects.push_back(std::move(pObject));

	pObject = new CObjCollector(OBJECT_ID::BRIDEGE_SEC2_SEC3_3);
	pObject->SetPosition(m_ObjectPositions[OBJECT_ID::BRIDEGE_SEC2_SEC3_3]);
	pObject->Rotate({ 0, 1, 0 }, 90); 
	m_Objects.push_back(std::move(pObject));*/
/////////////////////////////////////////////////////////////////////////////////

// DoorWall----------------------------------------------------------------------
	pObject = new CDoorWall(OBJECT_ID::DOOR_WALL_SEC1);
	pObject->SetPosition(m_ObjectPositions[OBJECT_ID::DOOR_WALL_SEC1]);
	m_Objects.push_back(std::move(pObject));

	pObject = new CDoorWall(OBJECT_ID::DOOR_WALL_SEC2);
	pObject->SetPosition(m_ObjectPositions[OBJECT_ID::DOOR_WALL_SEC2]);
	m_Objects.push_back(std::move(pObject));

	pObject = new CDoorWall(OBJECT_ID::DOOR_WALL_SEC3);
	pObject->SetPosition(m_ObjectPositions[OBJECT_ID::DOOR_WALL_SEC3]);
	m_Objects.push_back(std::move(pObject));

	pObject = new CDoorWall(OBJECT_ID::DOOR_WALL_SEC4);
	pObject->SetPosition(m_ObjectPositions[OBJECT_ID::DOOR_WALL_SEC4]);
	m_Objects.push_back(std::move(pObject));

	pObject = new CDoorWall(OBJECT_ID::DOOR_WALL_SEC5);
	pObject->SetPosition(m_ObjectPositions[OBJECT_ID::DOOR_WALL_SEC5]);
	m_Objects.push_back(std::move(pObject));
////////////////////////////////////////////////////////////////////////////////

// Sign-------------------------------------------------------------------------
	pObject = new CSign(OBJECT_ID::SIGN_SCROLL);
	pObject->SetPosition(m_ObjectPositions[OBJECT_ID::SIGN_SCROLL]);
	m_Objects.push_back(std::move(pObject));

	pObject = new CSign(OBJECT_ID::SIGN_PUZZLE);
	pObject->SetPosition(m_ObjectPositions[OBJECT_ID::SIGN_PUZZLE]);
	m_Objects.push_back(std::move(pObject));

	pObject = new CSign(OBJECT_ID::SIGN_MEDUSA);
	pObject->SetPosition(m_ObjectPositions[OBJECT_ID::SIGN_MEDUSA]);
	pObject->Rotate({ 0,1,0 }, 90.0f);
	m_Objects.push_back(std::move(pObject));

	//pObject = new CSign(OBJECT_ID::BOSS);
	//pObject->SetPosition(m_ObjectPositions[OBJECT_ID::BOSS]);
	//m_Objects.push_back(std::move(pObject));
////////////////////////////////////////////////////////////////////////////////
}

void PacketProcessor::InitTerrainHeightMap()
{
	ifstream fileIn("resources/Heights.txt");
	for (int i = 0; i <= TERRAIN_HEIGHT_MAP_HEIGHT; ++i)
	{
		for (int j = 0; j <= TERRAIN_HEIGHT_MAP_WIDTH; ++j)
		{
			string text;
			fileIn >> text;
			if (text.compare("//") == 0)
			{
				j--;
				continue;
			} 
			m_Heights[i][j] = stoi(text);
		} 
	} 
}
