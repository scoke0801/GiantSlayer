#include "stdafx.h"
#include "PacketProcessor.h"
#include "protocol.h" 
#include "MapObjects.h"
#include "Enemy.h"
#include "Arrow.h"
 
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
			SendSyncUpdatePacket();
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
	InitCameras();
	InitMonsters();
	InitObstacle();
	InitArrows();
	BuildBlockingRegionOnMap();

	cout << "서버 준비 완료!\n";
}

int PacketProcessor::GetNewPlayerId(SOCKET socket)
{
	for (int i = SERVER_ID + 1; i <= MAX_PLAYER; ++i) { 
		if (PLST_FREE == m_Clients[i].m_state) {
			m_Clients[i].m_state = PLST_CONNECTED;
			m_Clients[i].m_socket = socket;
			m_Clients[i].m_name[0] = 0;
			m_Clients[i].id = i;
			return i;
		}
	}
	return -1;
}

void PacketProcessor::InitPrevUserData(int c_id)
{
	m_Clients[c_id].m_prev_size = 0;
}

void PacketProcessor::DoRecv(int s_id)
{
	m_Clients[s_id].m_recv_over.m_wsabuf.buf = reinterpret_cast<char*>(m_Clients[s_id].m_recv_over.m_packetbuf) + m_Clients[s_id].m_prev_size;
	m_Clients[s_id].m_recv_over.m_wsabuf.len = MAX_BUFFER - m_Clients[s_id].m_prev_size;

	memset(&m_Clients[s_id].m_recv_over.m_over, 0, sizeof(m_Clients[s_id].m_recv_over.m_over));

	DWORD r_flag = 0;
	auto ret = WSARecv(m_Clients[s_id].m_socket, &m_Clients[s_id].m_recv_over.m_wsabuf, 1, NULL,
		&r_flag, &m_Clients[s_id].m_recv_over.m_over, recv_callback);

	if (0 != ret) {
		auto err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no) 
			error_display("Error in RecvPacket: ");
	}
}

void PacketProcessor::ProcessPacket(int p_id, unsigned char* p_buf)
{
	char buf[10000];
	int copyPos = 0;
	int retval;
	// buffer[0]의 값은 packet protocol size
	// buffer[1]의 값은 packet protocol type
	PACKET_PROTOCOL type = (PACKET_PROTOCOL)p_buf[1];
	switch (type) {
	case PACKET_PROTOCOL::C2S_LOGIN:
	{
		P_S2C_PROCESS_LOGIN p_processLogin;
		p_processLogin.size = sizeof(p_processLogin);
		p_processLogin.type = PACKET_PROTOCOL::S2C_LOGIN_HANDLE;

		if (m_CurrentPlayerNum > MAX_PLAYER) {
			p_processLogin.isSuccess = false;
		}
		else {
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
		SendPacket(p_id, &p_processLogin);

		m_CurrentPlayerNum++;

		//// 로그인 이후 데이터 싱크 맞추기 작업용 패킷 전송
		//P_S2C_UPDATE_SYNC p_syncUpdate;
		//p_syncUpdate.type = PACKET_PROTOCOL::S2C_INGAME_UPDATE_PLAYERS_STATE;
		//p_syncUpdate.size = sizeof(p_syncUpdate);

		//p_syncUpdate.playerNum = m_CurrentPlayerNum;

		//for (int i = 0; i < m_CurrentPlayerNum; ++i) {
		//	p_syncUpdate.id[i] = static_cast<char>(m_Players[i]->GetId());

		//	XMFLOAT3 pos = m_Players[i]->GetPosition();
		//	p_syncUpdate.posX[i] = FloatToInt(pos.x);
		//	p_syncUpdate.posY[i] = FloatToInt(pos.y);
		//	p_syncUpdate.posZ[i] = FloatToInt(pos.z);
		//}
		//SendPacket(p_id, &p_syncUpdate); 
	}
	break;
	case PACKET_PROTOCOL::C2S_LOGOUT:
	{
		P_C2S_LOGOUT p_logout =
			*reinterpret_cast<P_C2S_LOGOUT*>(p_buf);
		m_CurrentlyDeletedPlayerId = p_logout.id;
		m_Players[p_logout.id]->SetExistence(false);
	}
	break;
	case PACKET_PROTOCOL::C2S_INGAME_KEYBOARD_INPUT:
	{
		P_C2S_KEYBOARD_INPUT p_keyboard =
			*reinterpret_cast<P_C2S_KEYBOARD_INPUT*>(p_buf);

		XMFLOAT3 pos = m_Players[p_keyboard.id]->GetPosition();
		XMFLOAT3 shift = XMFLOAT3(0, 0, 0);
		float distance = PLAYER_RUN_SPEED;

		switch (p_keyboard.keyInput)
		{
		case VK_W:
			m_Players[p_keyboard.id]->SetVelocity(Vector3::Add(shift,
				m_Cameras[p_keyboard.id]->GetLook3f(), distance));
			break;
		case VK_S:
			m_Players[p_keyboard.id]->SetVelocity(Vector3::Add(shift,
				m_Cameras[p_keyboard.id]->GetLook3f(), -distance));
			break;
		case VK_A:
			m_Players[p_keyboard.id]->SetVelocity(Vector3::Add(shift,
				m_Cameras[p_keyboard.id]->GetRight3f(), -distance));
			break;
		case VK_D:
			m_Players[p_keyboard.id]->SetVelocity(Vector3::Add(shift,
				m_Cameras[p_keyboard.id]->GetRight3f(), distance));
			break;
		}

		P_S2C_PROCESS_KEYBOARD p_keyboardProcess;
		p_keyboardProcess.size = sizeof(p_keyboardProcess);
		p_keyboardProcess.type = PACKET_PROTOCOL::S2C_INGAME_KEYBOARD_INPUT;

		p_keyboardProcess.posX = FloatToInt(pos.x);
		p_keyboardProcess.posY = FloatToInt(pos.y);
		p_keyboardProcess.posZ = FloatToInt(pos.z);

		XMFLOAT3 look = Vector3::Normalize(m_Players[p_keyboard.id]->GetLook());
		//DisplayVector3(look);
		p_keyboardProcess.lookX = FloatToInt(look.x);
		p_keyboardProcess.lookY = FloatToInt(look.y);
		p_keyboardProcess.lookZ = FloatToInt(look.z);
		SendPacket(p_id, &p_keyboardProcess);
	}
	break;
	case PACKET_PROTOCOL::C2S_INGAME_UPDATE_SYNC:
	{
		P_C2S_UPDATE_SYNC_REQUEST p_updateSyncRequest =
			*reinterpret_cast<P_C2S_UPDATE_SYNC_REQUEST*>(p_buf);

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
			SendPacket(p_id, &p_addPlayer);
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
			XMFLOAT3 look = Vector3::Normalize(m_Players[i]->GetLook());
			p_syncUpdate.posX[i] = FloatToInt(pos.x);
			p_syncUpdate.posY[i] = FloatToInt(pos.y);
			p_syncUpdate.posZ[i] = FloatToInt(pos.z);

			p_syncUpdate.lookX[i] = FloatToInt(look.x);
			p_syncUpdate.lookY[i] = FloatToInt(look.y);
			p_syncUpdate.lookZ[i] = FloatToInt(look.z);
		}
		for (int i = 0; i < MAX_PLAYER; ++i) {
			p_syncUpdate.existance[i] = m_Players[i]->IsExist();
		}
		SendPacket(p_id, &p_syncUpdate);
	}
	break;
	case PACKET_PROTOCOL::C2S_INGAME_MOUSE_INPUT:
	{
		P_C2S_MOUSE_INPUT p_mouse = *reinterpret_cast<P_C2S_MOUSE_INPUT*>(p_buf);

		BYTE size;
		PACKET_PROTOCOL type;
		MOUSE_INPUT_TYPE InputType;
		short inputNum;
		int xInput[MAX_MOUSE_INPUT];
		int yInput[MAX_MOUSE_INPUT];

		P_S2C_PROCESS_MOUSE p_mouseProcess;
		ZeroMemory(&p_mouseProcess, sizeof(P_S2C_PROCESS_MOUSE));
		p_mouseProcess.size = sizeof(P_S2C_PROCESS_MOUSE);
		p_mouseProcess.type = PACKET_PROTOCOL::S2C_INGAME_MOUSE_INPUT;
		float playerRotateY = 0.0f;
		float cameraRotateY = 0.0f;
		float cameraOffset = 0.0f;
		if (p_mouse.InputType == MOUSE_INPUT_TYPE::M_LMOVE) {
			for (int i = 0; i < p_mouse.inputNum; ++i) {
				float dx = IntToFloat(p_mouse.xInput[i]);

				m_Cameras[p_mouse.id]->RotateAroundTarget(XMFLOAT3(0, 1, 0), dx * 75);
				playerRotateY += dx;
				if (m_Players[p_mouse.id]->IsMoving())
				{
					//p_mouseProcess.playerRotateY += dx;
					m_Players[p_mouse.id]->Rotate(XMFLOAT3(0, 1, 0), dx * 150);
				}
			}
			p_mouseProcess.playerRotateY = FloatToInt(playerRotateY);
		}
		else if (p_mouse.InputType == MOUSE_INPUT_TYPE::M_RMOVE) {
			for (int i = 0; i < p_mouse.inputNum; ++i) {
				float offset = IntToFloat(p_mouse.yInput[i]);
				cameraOffset += offset;
				m_Cameras[p_mouse.id]->MoveOffset(XMFLOAT3(0, 0, offset));
			}
			p_mouseProcess.cameraOffset = FloatToInt(cameraOffset);
		}
		SendPacket(p_id, &p_mouseProcess);
	}
		break;
	default: 
		cout << "Unknown Packet Type from Client[" << p_id << "] Packet Type [" << +p_buf[1] << "]" << endl;
		while (true) {
			// 멈춰
		}
	}
}

void PacketProcessor::Update(float elapsedTime)
{
	for (int i = 0; i < m_ObjectLayers.size(); ++i) {
		for (auto pObject : m_ObjectLayers[i]) {
			pObject->Update(elapsedTime);
			pObject->UpdateColliders();
		}
	} 
	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (m_Players[i]->IsExist()) {
			m_Players[i]->Update(elapsedTime);
			m_Players[i]->UpdateColliders();
			m_Players[i]->FixPositionByTerrain(m_Heights);
			m_Players[i]->FixCameraByTerrain(m_Heights);
		}
	} 
	for (auto pObstacle : m_ObjectLayers[(int)OBJECT_LAYER::Obstacle]) { 
		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (m_Players[i]->IsExist() == false) continue;

			if (pObstacle->CollisionCheck(m_Players[i])) {
				m_Players[i]->FixCollision(pObstacle);
				//cout << "충돌발생 - [오브젝트, 플레이어 " << i << "]\n";
			}
		}
	}

	for (auto pEnemy : m_ObjectLayers[(int)OBJECT_LAYER::Enemy]) { 
		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (m_Players[i]->IsExist() == false) continue;

			if (pEnemy->CollisionCheck(m_Players[i])) { 
				if (m_Players[i]->Attacked(pEnemy)) {
					m_Players[i]->FixCollision();
					cout << "충돌 : 플레이어 - 적\n";
				}
			}
		}
	}
	for (auto pArrow : m_ObjectLayers[(int)OBJECT_LAYER::MonsterArrow]) {
		// 변수명 변경으로 인한 true/false 반전..
		if (true == pArrow->IsUsable()) {
			continue;
		}
		
		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (m_Players[i]->IsExist() == false) continue;
			if (pArrow->CollisionCheck(m_Players[i])) {
				if (m_Players[i]->Attacked(pArrow)) {
					pArrow->SetIsUsable(true);
				}
			}
		}
	}
	for (auto pArrow : m_ObjectLayers[(int)OBJECT_LAYER::PlayerArrow]) {
		// 변수명 변경으로 인한 true/false 반전..
		if (true == pArrow->IsUsable()) {
			continue;
		}
		for (auto pEnemy : m_ObjectLayers[(int)OBJECT_LAYER::Enemy])
		{
			if (pArrow->CollisionCheck(pEnemy)) {
				pEnemy->ChangeState(ObjectState::Attacked, pArrow);
				pArrow->SetIsUsable(true);

				cout << "충돌 : 플레이어 화살 - 적\n";
				break;
			}
		}
	}
	for (auto pPuzzle : m_ObjectLayers[(int)OBJECT_LAYER::Puzzle]) {
		
		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (m_Players[i]->IsExist() == false) continue;
			if (pPuzzle->CollisionCheck(m_Players[i])) {
				m_Players[i]->FixCollision(pPuzzle);
				//m_isPlayerBoxCollide = true; 
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
		m_Players[i]->Scale(7, 7, 7);
		m_Players[i]->SetPosition(positions[i]);
		m_Players[i]->SetExistence(false); 
		m_Players[i]->AddBoundingBox(BoundingBox(XMFLOAT3(0, 0, 0), XMFLOAT3(10, 36, 10)));
	}
}

void PacketProcessor::InitCameras()
{
	int nCameras = MAX_PLAYER; 
	const float PI = 3.141592; 
	FRAME_BUFFER_WIDTH;
	for (int i = 0; i < nCameras; ++i)
	{
		CCamera* pCamera = new CCamera;
		pCamera->SetLens(0.25f * PI, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 1.0f, 60000.0f);
		m_Cameras[i] = pCamera;

		m_Cameras[i]->SetPosition(m_Players[i]->GetPosition());
		m_Cameras[i]->Pitch(XMConvertToRadians(15));
		m_Cameras[i]->SetOffset(XMFLOAT3(0.0f, 450.0f, -1320.0f));	
		m_Cameras[i]->SetTarget(m_Players[i]);
		m_Players[i]->SetCamera(m_Cameras[i]);
	}
}

void PacketProcessor::InitMonsters()
{
	CEnemy* pEnemy;
	XMFLOAT3 scale = { 125.0f,125.0f,125.0f };
	{	// Monster Area1
		pEnemy = new CRangedEnemy();
		pEnemy->Scale(scale.x, scale.y, scale.z);
		pEnemy->SetPosition({ 2005.0f, GetDetailHeight(m_Heights,2005.0f, 11650.0f), 11650.0f });
		pEnemy->SetActivityScope({ 1825, 0, 3050 }, { 2005.0f, GetDetailHeight(m_Heights, 2005.0f, 11650.0f), 11650.0f });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum); 
		pEnemy->AddBoundingBox(BoundingBox(XMFLOAT3{ 0,-1.5f,0 }, XMFLOAT3(2.25f, 1.5f, 3.25f)));
		pEnemy->SetSightBoundingBox({ 1825 * 0.75f / scale.x, 10, 3050 * 0.75f / scale.z }); 
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));

		pEnemy = new CMeleeEnemy();
		pEnemy->Scale(scale.x, scale.y, scale.z);
		pEnemy->SetPosition({ 2005.0f,  GetDetailHeight(m_Heights,2005.0f, 11650.0f), 11650.0f });
		pEnemy->SetActivityScope({ 1825, 0, 3050 }, { 2005.0f,  GetDetailHeight(m_Heights,2005.0f, 11650.0f), 11650.0f });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
		pEnemy->AddBoundingBox(BoundingBox(XMFLOAT3{ 0,-1.5f,0 }, XMFLOAT3(2.25f, 1.5f, 3.25f)));
		pEnemy->SetSightBoundingBox({ 1825 * 0.75f / scale.x, 10, 3050 * 0.75f / scale.z });
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));

		pEnemy = new CMeleeEnemy(); 
		pEnemy->Scale(scale.x, scale.y, scale.z); 
		pEnemy->SetPosition({ 2005.0f,  GetDetailHeight(m_Heights,2005.0f, 11650.0f), 11650.0f });
		pEnemy->SetActivityScope({ 1825, 0, 3050 }, { 2005.0f,  GetDetailHeight(m_Heights,2005.0f, 11650.0f), 11650.0f });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
		pEnemy->AddBoundingBox(BoundingBox(XMFLOAT3{ 0,-1.5f,0 }, XMFLOAT3(2.25f, 1.5f, 3.25f)));
		pEnemy->SetSightBoundingBox({ 1825 * 0.75f / scale.x, 10, 3050 * 0.75f / scale.z }); 
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));
		 
		pEnemy = new CMeleeEnemy();
		pEnemy->Scale(scale.x, scale.y, scale.z);
		pEnemy->SetPosition({ 2005.0f,  GetDetailHeight(m_Heights,2005.0f, 11650.0f), 11650.0f });
		pEnemy->SetActivityScope({ 1825, 0, 3050 }, { 2005.0f,  GetDetailHeight(m_Heights,2005.0f, 11650.0f), 11650.0f });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
		pEnemy->AddBoundingBox(BoundingBox(XMFLOAT3{ 0,-1.5f,0 }, XMFLOAT3(2.25f, 1.5f, 3.25f)));
		pEnemy->SetSightBoundingBox({ 1825 * 0.75f / scale.x, 10, 3050 * 0.75f / scale.z });
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));
	}

	{	// Monster Area1-2
		pEnemy = new CRangedEnemy(); 
		pEnemy->Scale(scale.x, scale.y, scale.z); 
		pEnemy->SetPosition({ 7800.0f, GetDetailHeight(m_Heights,7800.0f,  11450.0f),  11450.0f });
		pEnemy->SetActivityScope({ 1600, 0, 2950 }, { 7800.0f, GetDetailHeight(m_Heights, 800.0f,  11450.0f),  11450.0f });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
		pEnemy->AddBoundingBox(BoundingBox(XMFLOAT3{ 0,-1.5f,0 }, XMFLOAT3(2.25f, 1.5f, 3.25f)));
		pEnemy->SetSightBoundingBox({ 1600 * 0.75f / scale.x , 10, 2950 * 0.75f / scale.z });
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));

		pEnemy = new CRangedEnemy();
		pEnemy->Scale(scale.x, scale.y, scale.z);
		pEnemy->SetPosition({ 7800.0f, GetDetailHeight(m_Heights,7800.0f,  11450.0f),  11450.0f });
		pEnemy->SetActivityScope({ 1600, 0, 2950 }, { 7800.0f, GetDetailHeight(m_Heights, 800.0f,  11450.0f),  11450.0f });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
		pEnemy->AddBoundingBox(BoundingBox(XMFLOAT3{ 0,-1.5f,0 }, XMFLOAT3(2.25f, 1.5f, 3.25f)));
		pEnemy->SetSightBoundingBox({ 1600 * 0.75f / scale.x , 10, 2950 * 0.75f / scale.z });
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));

		pEnemy = new CMeleeEnemy(); 
		pEnemy->Scale(scale.x, scale.y, scale.z); 
		pEnemy->SetPosition({ 2005.0f, GetDetailHeight(m_Heights, 2005.0f, 11650.0f), 11650.0f });
		pEnemy->SetActivityScope({ 1825, 0, 3050 }, { 2005.0f, GetDetailHeight(m_Heights,2005.0f, 11650.0f), 11650.0f });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum); 
		pEnemy->AddBoundingBox(BoundingBox(XMFLOAT3{ 0,-1.5f,0 }, XMFLOAT3(2.25f, 1.5f, 3.25f)));
		pEnemy->SetSightBoundingBox({ 1825 * 0.75f / scale.x, 10, 3050 * 0.75f / scale.z }); 
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));

		pEnemy = new CMeleeEnemy();
		pEnemy->Scale(scale.x, scale.y, scale.z);
		pEnemy->SetPosition({ 2005.0f, GetDetailHeight(m_Heights, 2005.0f, 11650.0f), 11650.0f });
		pEnemy->SetActivityScope({ 1825, 0, 3050 }, { 2005.0f, GetDetailHeight(m_Heights,2005.0f, 11650.0f), 11650.0f });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
		pEnemy->AddBoundingBox(BoundingBox(XMFLOAT3{ 0,-1.5f,0 }, XMFLOAT3(2.25f, 1.5f, 3.25f)));
		pEnemy->SetSightBoundingBox({ 1825 * 0.75f / scale.x, 10, 3050 * 0.75f / scale.z });
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));
	} 
	{// Monster Area2-1

		pEnemy = new CRangedEnemy(); 
		//pEnemy->Rotate(XMFLOAT3(1, 0, 0), -90.0f);
		pEnemy->Scale(scale.x, scale.y, scale.z); 
		pEnemy->SetPosition({ 12100.0f,GetDetailHeight(m_Heights,12100.0f, 17950.0f), 17950.0f });
		pEnemy->SetActivityScope({ 1300, 0, 1450 }, { 12100.0f,GetDetailHeight(m_Heights,12100.0f, 17950.0f), 17950.0f });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum); 
		pEnemy->AddBoundingBox(BoundingBox(XMFLOAT3{ 0,-1.5f,0 }, XMFLOAT3(2.25f, 1.5f, 3.25f)));
		pEnemy->SetSightBoundingBox({ 1300 * 0.75f / scale.x, 10, 1450 * 0.75f / scale.z }); 
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));

		pEnemy = new CRangedEnemy(); 
		//pEnemy->Rotate(XMFLOAT3(1, 0, 0), -90.0f);
		pEnemy->Scale(scale.x, scale.y, scale.z); 
		pEnemy->SetPosition({ 12100.0f,GetDetailHeight(m_Heights,12100.0f, 17950.0f), 17950.0f });
		pEnemy->SetActivityScope({ 1300, 0, 1450 }, { 12100.0f,GetDetailHeight(m_Heights,12100.0f, 17950.0f), 17950.0f });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum); 
		pEnemy->AddBoundingBox(BoundingBox(XMFLOAT3{ 0,-1.5f,0 }, XMFLOAT3(2.25f, 1.5f, 3.25f)));
		pEnemy->SetSightBoundingBox({ 1300 * 0.75f / scale.x, 10, 1450 * 0.75f / scale.z }); 
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));

		pEnemy = new CMeleeEnemy(); 
		//pEnemy->Rotate(XMFLOAT3(1, 0, 0), -90.0f);
		pEnemy->Scale(scale.x, scale.y, scale.z); 
		pEnemy->SetPosition({ 12100.0f,GetDetailHeight(m_Heights,12100.0f, 17950.0f), 17950.0f });
		pEnemy->SetActivityScope({ 1300, 0, 1450 }, { 12100.0f,GetDetailHeight(m_Heights,12100.0f, 17950.0f), 17950.0f });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum); 
		pEnemy->AddBoundingBox(BoundingBox(XMFLOAT3{ 0,-1.5f,0 }, XMFLOAT3(2.25f, 1.5f, 3.25f)));
		pEnemy->SetSightBoundingBox({ 1300 * 0.75f / scale.x, 10, 1450 * 0.75f / scale.z }); 
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));
	}

	{// Monster Area2-2
		pEnemy = new CRangedEnemy();
		pEnemy->Scale(scale.x, scale.y, scale.z);
		pEnemy->SetPosition({ 11900.0f,GetDetailHeight(m_Heights,11900.0f, 13300.0f), 13300.0f });
		pEnemy->SetActivityScope({ 1400, 0, 1200 }, { 11900.0f,GetDetailHeight(m_Heights,11900.0f, 13300.0f), 13300.0f });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
		pEnemy->AddBoundingBox(BoundingBox(XMFLOAT3{ 0,-1.5f,0 }, XMFLOAT3(2.25f, 1.5f, 3.25f)));
		pEnemy->SetSightBoundingBox({ 1400 * 0.75f / scale.x , 10, 1200 * 0.75f / scale.z });
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));

		pEnemy = new CMeleeEnemy();
		pEnemy->Scale(scale.x, scale.y, scale.z);
		pEnemy->SetPosition({ 11900.0f,GetDetailHeight(m_Heights,11900.0f, 13300.0f), 13300.0f });
		pEnemy->SetActivityScope({ 1400, 0, 1200 }, { 11900.0f,GetDetailHeight(m_Heights,11900.0f, 13300.0f), 13300.0f });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
		pEnemy->AddBoundingBox(BoundingBox(XMFLOAT3{ 0,-1.5f,0 }, XMFLOAT3(2.25f, 1.5f, 3.25f)));
		pEnemy->SetSightBoundingBox({ 1400 * 0.75f / scale.x , 10, 1200 * 0.75f / scale.z });
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));

		pEnemy = new CMeleeEnemy();
		pEnemy->Scale(scale.x, scale.y, scale.z);
		pEnemy->SetPosition({ 11900.0f,GetDetailHeight(m_Heights,11900.0f, 13300.0f), 13300.0f });
		pEnemy->SetActivityScope({ 1400, 0, 1200 }, { 11900.0f,GetDetailHeight(m_Heights,11900.0f, 13300.0f), 13300.0f });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
		pEnemy->AddBoundingBox(BoundingBox(XMFLOAT3{ 0,-1.5f,0 }, XMFLOAT3(2.25f, 1.5f, 3.25f)));
		pEnemy->SetSightBoundingBox({ 1400 * 0.75f / scale.x , 10, 1200 * 0.75f / scale.z });
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));
	}

	{// Monster Area3
		pEnemy = new CRangedEnemy();
		//pEnemy->Rotate(XMFLOAT3(1, 0, 0), -90.0f);
		pEnemy->Scale(scale.x, scale.y, scale.z);
		pEnemy->SetPosition({ 11900.0f,GetDetailHeight(m_Heights,11900.0f, 3250.0f), 3250.0f });
		pEnemy->SetActivityScope({ 1200, 0, 2750 }, { 11900.0f,GetDetailHeight(m_Heights,11900.0f, 3250.0f), 3250.0f });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
		pEnemy->AddBoundingBox(BoundingBox(XMFLOAT3{ 0,-1.5f,0 }, XMFLOAT3(2.25f, 1.5f, 3.25f)));
		pEnemy->SetSightBoundingBox({ 1200 * 0.75f / scale.x , 10, 2750 * 0.75f / scale.z });
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));

		pEnemy = new CRangedEnemy();
		pEnemy->Scale(scale.x, scale.y, scale.z);
		pEnemy->SetPosition({ 11900.0f,GetDetailHeight(m_Heights,11900.0f, 3250.0f), 3250.0f });
		pEnemy->SetActivityScope({ 1200, 0, 2750 }, { 11900.0f, GetDetailHeight(m_Heights,11900.0f, 3250.0f), 3250.0f });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
		pEnemy->AddBoundingBox(BoundingBox(XMFLOAT3{ 0,-1.5f,0 }, XMFLOAT3(2.25f, 1.5f, 3.25f)));
		pEnemy->SetSightBoundingBox({ 1200 * 0.75f / scale.x , 10, 2750 * 0.75f / scale.z });
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));

		pEnemy = new CRangedEnemy();
		pEnemy->Scale(scale.x, scale.y, scale.z);
		pEnemy->SetPosition({ 11900.0f, GetDetailHeight(m_Heights,11900.0f, 3250.0f), 3250.0f });
		pEnemy->SetActivityScope({ 1200, 0, 2750 }, { 11900.0f, GetDetailHeight(m_Heights,11900.0f, 3250.0f), 3250.0f });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
		pEnemy->AddBoundingBox(BoundingBox(XMFLOAT3{ 0,-1.5f,0 }, XMFLOAT3(2.25f, 1.5f, 3.25f)));
		pEnemy->SetSightBoundingBox({ 1200 * 0.75f / scale.x , 10, 2750 * 0.75f / scale.z });

		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));

		pEnemy = new CMeleeEnemy();
		pEnemy->Scale(scale.x, scale.y, scale.z);
		pEnemy->SetPosition({ 11900.0f, GetDetailHeight(m_Heights,11900.0f, 3250.0f), 3250.0f });
		pEnemy->SetActivityScope({ 1200, 0, 2750 }, { 11900.0f, GetDetailHeight(m_Heights,11900.0f, 3250.0f), 3250.0f });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
		pEnemy->AddBoundingBox(BoundingBox(XMFLOAT3{ 0,-1.5f,0 }, XMFLOAT3(2.25f, 1.5f, 3.25f)));
		pEnemy->SetSightBoundingBox({ 1200 * 0.75f / scale.x , 10, 2750 * 0.75f / scale.z });
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));
	}
}

void PacketProcessor::InitArrows()
{
	for (int i = 0; i < 10; ++i) {
		CArrow* pArrow = new CArrow();
		pArrow->SetPosition({ 500.0f,  100.0f, 1500.0f });
		pArrow->SetTargetPosition({ 500.0f, 100.0f, 5000.0f });
		pArrow->Scale(25.0f, 25.0f, 25.0f);
		pArrow->AddBoundingBox(BoundingBox(XMFLOAT3(0, 0, 5), XMFLOAT3(0.25f, 0.25f, 7.5f)));
		m_ObjectLayers[(int)OBJECT_LAYER::PlayerArrow].push_back(pArrow);
	}

	for (int i = 0; i < 10; ++i) {
		CArrow* pArrow = new CArrow();
		pArrow->SetPosition({ 500.0f,  100.0f, 1500.0f });
		pArrow->SetTargetPosition({ 500.0f, 100.0f, 5000.0f });
		pArrow->Scale(100.0f, 100.0f, 50.0f);
		pArrow->AddBoundingBox(BoundingBox(XMFLOAT3(0, 0, 5), XMFLOAT3(0.25f, 0.25f, 7.5f)));
		m_ObjectLayers[(int)OBJECT_LAYER::MonsterArrow].push_back(pArrow);
	}
}

void PacketProcessor::ReadObstaclesPosition()
{
	FILE* fp = fopen("resources/ObjectPositionData.json", "rb"); // non-Windows use "r"
	char readBuffer[4096];
	FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	Document d;
	d.ParseStream(is);	//==d.Parse(is);
	fclose(fp);

	for (int i = 0; i < 3; ++i) {
		string str = "BRIDEGE_SEC2_SEC3_" + to_string(i + 1);
		m_ObjectPositions.emplace(OBJECT_ID((int)OBJECT_ID::BRIDEGE_SEC2_SEC3_1 + i),
			GetPosition(str, d));
	} 

	m_ObjectPositions.emplace(OBJECT_ID::SIGN_SCROLL,
		GetPosition("SIGN_SCROLL", d));
	m_ObjectPositions.emplace(OBJECT_ID::SIGN_PUZZLE,
		GetPosition("SIGN_PUZZLE", d));
	m_ObjectPositions.emplace(OBJECT_ID::SIGN_MEDUSA,
		GetPosition("SIGN_MEDUSA", d)); 
	m_ObjectPositions.emplace(OBJECT_ID::SIGN_BOSS,
		GetPosition("SIGN_BOSS", d)); 

	for (int i = 0; i < 2; ++i) {
		string str = "PUZZLE_" + to_string(i + 1);
		m_ObjectPositions.emplace(OBJECT_ID((int)OBJECT_ID::PUZZLE_1 + i),
			GetPosition(str, d));
	}

	for (int i = 0; i < 5; ++i) {
		string str = "DOOR_WALL_SEC" + to_string(i + 1);
		m_ObjectPositions.emplace(OBJECT_ID((int)OBJECT_ID::DOOR_WALL_SEC1 + i),
			GetPosition(str, d));
	}  
	m_ObjectPositions.emplace(OBJECT_ID::DRY_FOREST_ROCK_1,
		GetPosition("DRY_FOREST_ROCK_1", d));
	m_ObjectPositions.emplace(OBJECT_ID::DRY_FOREST_ROCK_2,
		GetPosition("DRY_FOREST_ROCK_2", d));

	for (int i = 0; i < 4; ++i) {
		string str = "DRY_FOREST_DRY_TREE_" + to_string(i + 1);
		m_ObjectPositions.emplace(OBJECT_ID((int)OBJECT_ID::DRY_FOREST_DRY_TREE_1 + i),
			GetPosition(str, d));
	}
	m_ObjectPositions.emplace(OBJECT_ID::DRY_FOREST_STUMP_1,
		GetPosition("DRY_FOREST_STUMP_1", d));

	for (int i = 0; i < 3; ++i) {
		string str = "DRY_FOREST_DEAD_TREE_" + to_string(i + 1);
		m_ObjectPositions.emplace(OBJECT_ID((int)OBJECT_ID::DRY_FOREST_DEAD_TREE_1 + i),
			GetPosition(str, d));
	}
	for (int i = 0; i < 15; ++i) {
		string str = "DESERT_ROCK_" + to_string(i + 1);
		m_ObjectPositions.emplace(OBJECT_ID((int)OBJECT_ID::DESERT_ROCK_1 + i),
			GetPosition(str, d));
	}

	for (int i = 0; i < 10; ++i) {
		string str = "PUZZLE_BOX_" + to_string(i + 1);
		m_ObjectPositions.emplace(OBJECT_ID((int)OBJECT_ID::PUZZLE_BOX_1 + i),
			GetPosition(str, d));
	}
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
	CGameObject* pObject = new CBridge(OBJECT_ID::BRIDEGE_SEC2_SEC3_1);
	pObject->SetPosition(m_ObjectPositions[OBJECT_ID::BRIDEGE_SEC2_SEC3_1]);
	pObject->Rotate({ 0, 1, 0 }, 90);
	pObject->UpdateColliders();	
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CBridge(OBJECT_ID::BRIDEGE_SEC2_SEC3_2);
	pObject->SetPosition(m_ObjectPositions[OBJECT_ID::BRIDEGE_SEC2_SEC3_2]);
	pObject->Rotate({ 0, 1, 0 }, 90);
	pObject->UpdateColliders();
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CBridge(OBJECT_ID::BRIDEGE_SEC2_SEC3_3);
	pObject->SetPosition(m_ObjectPositions[OBJECT_ID::BRIDEGE_SEC2_SEC3_3]);
	pObject->Rotate({ 0, 1, 0 }, 90);
	pObject->UpdateColliders();
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
///////////////////////////////////////////////////////////////////////////////// 

// PUZZLE----------------------------------------------------------------------
	pObject = new CPuzzle(OBJECT_ID::PUZZLE_1);
	pObject->SetPosition(m_ObjectPositions[OBJECT_ID::PUZZLE_1]);
	m_ObjectLayers[(int)OBJECT_LAYER::Puzzle].push_back(pObject);

	XMFLOAT3 tempPos = m_ObjectPositions[OBJECT_ID::PUZZLE_1];
	tempPos.x += 100;
	pObject = new CPlate(OBJECT_ID::PUZZLE_1_PLATE);
	pObject->SetPosition(tempPos);
	m_ObjectLayers[(int)OBJECT_LAYER::Puzzle].push_back(pObject);

	for (int i = 0; i < 10; ++i) {
		pObject = new CPuzzleBox((OBJECT_ID)((int)OBJECT_ID::PUZZLE_BOX_1 + i));
		pObject->SetPosition(m_ObjectPositions[(OBJECT_ID)((int)OBJECT_ID::PUZZLE_BOX_1 + i)]);
		m_ObjectLayers[(int)OBJECT_LAYER::PuzzleBox].push_back(pObject);
	}
	//pObject = new CPuzzle(OBJECT_ID::PUZZLE_2);
	//pObject->SetPosition(m_ObjectPositions[OBJECT_ID::PUZZLE_2]);
	//m_Objects.push_back(std::move(pObject));
/////////////////////////////////////////////////////////////////////////////////

// DoorWall----------------------------------------------------------------------
	pObject = new CDoorWall(OBJECT_ID::DOOR_WALL_SEC1);
	pObject->SetPosition(m_ObjectPositions[OBJECT_ID::DOOR_WALL_SEC1]);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CDoorWall(OBJECT_ID::DOOR_WALL_SEC2);
	pObject->SetPosition(m_ObjectPositions[OBJECT_ID::DOOR_WALL_SEC2]);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CDoorWall(OBJECT_ID::DOOR_WALL_SEC3);
	pObject->SetPosition(m_ObjectPositions[OBJECT_ID::DOOR_WALL_SEC3]);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CDoorWall(OBJECT_ID::DOOR_WALL_SEC4);
	pObject->SetPosition(m_ObjectPositions[OBJECT_ID::DOOR_WALL_SEC4]);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CDoorWall(OBJECT_ID::DOOR_WALL_SEC5);
	pObject->SetPosition(m_ObjectPositions[OBJECT_ID::DOOR_WALL_SEC5]);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CGameObject(); 
	pObject->Rotate({ 0,1,0 }, 90);
	pObject->SetPosition(m_ObjectPositions[OBJECT_ID::WALL_1]);
	pObject->AddBoundingBox(BoundingBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1500 * 0.5f, 2500 * 0.5f, 500 * 0.5f)));
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CGameObject();
	pObject->Rotate({ 0,1,0 }, 90);
	pObject->SetPosition(m_ObjectPositions[OBJECT_ID::WALL_2]);
	pObject->AddBoundingBox(BoundingBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1500 * 0.5f, 2500 * 0.5f, 500 * 0.5f)));
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
////////////////////////////////////////////////////////////////////////////////

// Sign-------------------------------------------------------------------------
	pObject = new CSign(OBJECT_ID::SIGN_SCROLL);
	pObject->SetPosition(m_ObjectPositions[OBJECT_ID::SIGN_SCROLL]);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CSign(OBJECT_ID::SIGN_PUZZLE);
	pObject->SetPosition(m_ObjectPositions[OBJECT_ID::SIGN_PUZZLE]);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CSign(OBJECT_ID::SIGN_MEDUSA);
	pObject->SetPosition(m_ObjectPositions[OBJECT_ID::SIGN_MEDUSA]);
	pObject->Rotate({ 0,1,0 }, 90.0f);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	//pObject = new CSign(OBJECT_ID::BOSS);
	//pObject->SetPosition(m_ObjectPositions[OBJECT_ID::BOSS]);
	//m_Objects.push_back(std::move(pObject));
////////////////////////////////////////////////////////////////////////////////
// FBX Models
//
	for (int i = 0; i < 2; ++i) {
		pObject = new CGameObject();
		pObject->SetPosition(m_ObjectPositions[(OBJECT_ID)((int)OBJECT_ID::DRY_FOREST_ROCK_1 + i)]);
		pObject->Scale(50, 50, 50);
		pObject->AddBoundingBox(BoundingBox(XMFLOAT3(0, 0, 0), XMFLOAT3(5 * 0.5f, 7 * 0.5f, 3 * 0.5f)));
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	} 

	for (int i = 0; i < 2; ++i) {
		pObject = new CGameObject(); 
		pObject->Scale(0.5f + 0.5 * i, 0.5f, 0.5f + 0.5 * i);
		pObject->Rotate({ 0,1,0 }, 60 + 30 * i);
		pObject->SetPosition(m_ObjectPositions[(OBJECT_ID)((int)OBJECT_ID::DRY_FOREST_DRY_TREE_1 + i)]);
		pObject->AddBoundingBox(BoundingBox(XMFLOAT3(0, 0, 100), XMFLOAT3(200 * 0.5f, 1500 * 0.5f, 150 * 0.5f)));
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}
	for (int i = 0; i < 2; ++i) {
		pObject = new CGameObject();
		pObject->Scale(0.5f + 0.5 * i, 0.5f, 0.5f + 0.5 * i);
		pObject->Rotate({ 0,1,0 }, 0 + 15 * i);
		pObject->SetPosition(m_ObjectPositions[(OBJECT_ID)((int)OBJECT_ID::DRY_FOREST_DRY_TREE_3 + i)]);
		pObject->AddBoundingBox(BoundingBox(XMFLOAT3(0, 0, 100), XMFLOAT3(200 * 0.5f, 1500 * 0.5f, 150 * 0.5f)));
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}

	pObject = new CGameObject();
	pObject->Scale(20.0f, 20.0f, 20.0f);
	pObject->SetPosition(m_ObjectPositions[OBJECT_ID::DRY_FOREST_STUMP_1]);
	pObject->AddBoundingBox(BoundingBox(XMFLOAT3(0, 0, 0), XMFLOAT3(15 *0.5f, 10 * 0.5f, 15 * 0.5f)));
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CGameObject();	
	pObject->Scale(150.0f, 150.0f, 150.0f);
	pObject->SetPosition(m_ObjectPositions[(OBJECT_ID)((int)OBJECT_ID::DRY_FOREST_DEAD_TREE_1)]);
	pObject->AddBoundingBox(BoundingBox(XMFLOAT3(1, -5, -2.5), XMFLOAT3(1 * 0.5f, 5 * 0.5f, 1 * 0.5f)));
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	for (int i = 0; i < 2; ++i) {
		pObject = new CGameObject(); 
		pObject->Scale(150.0f + 50 * i, 150.0f + 50 * i, 150.0f + 50 * i);
		pObject->SetPosition(m_ObjectPositions[(OBJECT_ID)((int)OBJECT_ID::DRY_FOREST_DEAD_TREE_2 + i)]);
		pObject->AddBoundingBox(BoundingBox(XMFLOAT3(1, -5, -2.5), XMFLOAT3(1 * 0.5f, 5 * 0.5f, 1 * 0.5f)));
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}

	for (int i = 0; i < 5; ++i) {
		if (i == 0){
			pObject->Scale(4.0f, 4.0f, 4.0f);
		}
		else if (i == 1){
			pObject->Rotate({ 0,1,0 }, 90);
			pObject->Scale(2.0f, 2.0f, 2.0f);
		}
		else if (i == 4){
			pObject->Scale(1.0f, 1.0f, 1.0f);
		}
		pObject->Scale(0.5f, 0.5f, 0.5f);

		pObject = new CGameObject();
		pObject->SetPosition(m_ObjectPositions[(OBJECT_ID)((int)OBJECT_ID::DESERT_ROCK_1 + i)]);
		pObject->AddBoundingBox(BoundingBox(XMFLOAT3(0, 220, 0), XMFLOAT3(600*0.5f, 250 * 0.5f, 600 * 0.5f)));
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}

	for (int i = 0; i < 6; ++i) {
		if (i == 0) {
			pObject->Scale(3.0f, 3.0f, 3.0f);
		}
		else if (i == 3) { 
			pObject->Rotate({ 0,1,0 }, 270);
			pObject->Scale(3.0f, 3.0f, 3.0f); 
		}
		else if (i == 1) { 
			pObject->Rotate({ 0,1,0 }, 90);
			pObject->Scale(1.5f, 1.5f, 1.5f);
		}
		else if (i == 5) { 
			pObject->Rotate({ 0,1,0 }, 135);
			pObject->Scale(1.5f, 1.5f, 1.5f);
		} 
		pObject->Scale(0.5f, 0.5f, 0.5f);

		pObject = new CGameObject();
		pObject->SetPosition(m_ObjectPositions[(OBJECT_ID)((int)OBJECT_ID::DESERT_ROCK_6 + i)]);
		pObject->AddBoundingBox(BoundingBox(XMFLOAT3(0, 220, 0), XMFLOAT3(600 * 0.5f, 250 * 0.5f, 600 * 0.5f)));
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}

	for (int i = 0; i < 4; ++i) {
		pObject->Scale(0.5f, 0.5f, 0.5f);

		pObject = new CGameObject();
		pObject->SetPosition(m_ObjectPositions[(OBJECT_ID)((int)OBJECT_ID::DESERT_ROCK_12 + i)]);
		pObject->AddBoundingBox(BoundingBox(XMFLOAT3(0, 220, 0), XMFLOAT3(600 * 0.5f, 250 * 0.5f, 600 * 0.5f)));
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}
	int stop = 3;
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

void PacketProcessor::BuildBlockingRegionOnMap()
{
	CGameObject* pObject = new CGameObject();
	pObject->SetPosition({ 0,-2000,10000 });
	pObject->AddBoundingBox(BoundingBox(XMFLOAT3(0, 0, 0),
		XMFLOAT3(100 * 0.5f, 10000 * 0.5f, 20000 * 0.5f)));
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CGameObject();
	pObject->AddBoundingBox(BoundingBox(XMFLOAT3(0, 0, 0), 
		XMFLOAT3(100 * 0.5f, 10000 * 0.5f, 20000 * 0.5f)));
	pObject->SetPosition({ 19950,-2000,10000 });
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CGameObject();
	pObject->AddBoundingBox(BoundingBox(XMFLOAT3(0, 0, 0), 
		XMFLOAT3(20000 * 0.5f, 10000 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 10000,-2000,00 });
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CGameObject();
	pObject->AddBoundingBox(BoundingBox(XMFLOAT3(0, 0, 0), 
		XMFLOAT3(20000 * 0.5f, 10000 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 10000,-2000,19950 });
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	// Forest to DryDesrt 아래 방향 벽  
	pObject = new CGameObject();
	pObject->AddBoundingBox( BoundingBox(XMFLOAT3(0, 0, 0), 
		XMFLOAT3(9600 * 0.5f, 800 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 4800,-1000, 15900 });
	m_BlockingPlateToPreviousSector[0] = (std::move(pObject));

	// Forest to Desert 왼쪽 벽
	pObject = new CGameObject();
	pObject->AddBoundingBox( BoundingBox(XMFLOAT3(0, 0, 0), 
		XMFLOAT3(800 * 0.5f, 10000 * 0.5f, 15200 * 0.5f)));
	pObject->SetPosition({ 10000,-2000, 7600 });
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	// Forest 지역 내 못가는 지형 
	pObject = new CGameObject();
	pObject->AddBoundingBox( BoundingBox(XMFLOAT3(0, 0, 0), 
		XMFLOAT3(2000 * 0.5f, 10000 * 0.5f, 7000 * 0.5f)));
	pObject->SetPosition({ 4000 + 1000, -2000, 11100 });
	pObject->UpdateColliders();
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	// Desrt to DryDesrt and Rock 왼쪽 벽
	pObject = new CGameObject();
	pObject->AddBoundingBox( BoundingBox(XMFLOAT3(0, 0, 0), 
		XMFLOAT3(400 * 0.5f, 10000 * 0.5f, 12800 * 0.5f)));
	pObject->SetPosition({ 13800, -2000, 7200 + 6400 });
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	// boss 지역 중간 벽
	pObject = new CGameObject();
	pObject->AddBoundingBox( BoundingBox(XMFLOAT3(0, 0, 0), 
		XMFLOAT3(800 * 0.5f, 10000 * 0.5f, 5600 * 0.5f)));
	pObject->SetPosition({ 15200 + 400,-2000, 2800 + 8000 });
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CGameObject();
	pObject->AddBoundingBox( BoundingBox(XMFLOAT3(0, 0, 0), 
		XMFLOAT3(800 * 0.5f, 10000 * 0.5f, 5600 * 0.5f)));
	pObject->SetPosition({ 17600 + 400,-2000, 2800 + 8000 });
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	// 사막 지역 가로 벽
	pObject = new CGameObject();
	pObject->AddBoundingBox( BoundingBox(XMFLOAT3(0, 0, 0),
		XMFLOAT3(4000 * 0.5f, 1000 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 2000 + 9600,-2000, 15600 });
	m_BlockingPlateToPreviousSector[1] = (std::move(pObject));

	pObject = new CGameObject();
	pObject->AddBoundingBox( BoundingBox(XMFLOAT3(0, 0, 0), 
		XMFLOAT3(4000 * 0.5f, 1000 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 2000 + 9600,-3000, 3600 });
	m_BlockingPlateToPreviousSector[2] = (std::move(pObject));

	// 보스 지역 입구 가로 벽
	pObject = new CGameObject();
	pObject->AddBoundingBox( BoundingBox(XMFLOAT3(0, 0, 0), 
		XMFLOAT3(2400 * 0.5f, 10000 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 1200 + 13600,-2000, 8000 });
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CGameObject();
	pObject->AddBoundingBox( BoundingBox(XMFLOAT3(0, 0, 0), 
		XMFLOAT3(2400 * 0.5f, 10000 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 1200 + 13600 + 1600 + 2400,-2000, 8000 });
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
}

void PacketProcessor::EnterNewSector(int sectorNum)
{
	if (m_BlockingPlateToPreviousSector.size() <= 0) return;

	CGameObject* obj = m_BlockingPlateToPreviousSector[sectorNum - 1];
	m_BlockingPlateToPreviousSector.erase(sectorNum - 1);

	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(obj);
	EnterNewSector(sectorNum - 1);
}

void PacketProcessor::SendPacket(int p_id, void* p)
{
	unsigned char p_size = reinterpret_cast<unsigned char*>(p)[0];
	unsigned char p_type = reinterpret_cast<unsigned char*>(p)[1];

	EX_OVER* s_over = new EX_OVER;
	memset(&s_over->m_over, 0, sizeof(s_over->m_over));
	memcpy(s_over->m_packetbuf, p, p_size);
	s_over->m_wsabuf.buf = reinterpret_cast<CHAR*>(s_over->m_packetbuf);
	s_over->m_wsabuf.len = p_size;
	
	auto ret = WSASend(m_Clients[p_id].m_socket, &s_over->m_wsabuf, 1, NULL, 0, &s_over->m_over, NULL);
	if (0 != ret) {
		auto err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no) {
			error_display("WSASend : ");
			m_Players[p_id]->SetExistence(false);
			cout << "로그아웃\n";
			//disconnect(p_id);
		}
	}
}

void PacketProcessor::SendSyncUpdatePacket()
{
	P_S2C_UPDATE_SYNC p_syncUpdate;
	p_syncUpdate.type = PACKET_PROTOCOL::S2C_INGAME_UPDATE_PLAYERS_STATE;
	p_syncUpdate.size = sizeof(p_syncUpdate);

	p_syncUpdate.playerNum = m_CurrentPlayerNum;

	for (int i = 0; i < m_CurrentPlayerNum; ++i) {
		p_syncUpdate.id[i] = static_cast<char>(m_Players[i]->GetId());

		XMFLOAT3 pos = m_Players[i]->GetPosition();
		XMFLOAT3 look = Vector3::Normalize(m_Players[i]->GetLook());
		p_syncUpdate.posX[i] = FloatToInt(pos.x);
		p_syncUpdate.posY[i] = FloatToInt(pos.y);
		p_syncUpdate.posZ[i] = FloatToInt(pos.z);

		p_syncUpdate.lookX[i] = FloatToInt(look.x);
		p_syncUpdate.lookY[i] = FloatToInt(look.y);
		p_syncUpdate.lookZ[i] = FloatToInt(look.z);
	}
	for (int i = 0; i < MAX_PLAYER; ++i) {
		p_syncUpdate.existance[i] = m_Players[i]->IsExist();
	}
	for (int i = SERVER_ID + 1; i <= MAX_PLAYER; ++i) {
		if (m_Clients[i].m_state == PL_STATE::PLST_CONNECTED) { 
			SendPacket(i, &p_syncUpdate);
		}
	}
}

void recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags)
{
	if (dataBytes == 0) { 
	}
	auto session = reinterpret_cast<CLIENT*>(overlapped);
	SOCKET client_s = session->m_socket;
	auto ex_over = &session->m_recv_over;

	unsigned char* packet_ptr = ex_over->m_packetbuf;
	int num_data = dataBytes + session->m_prev_size;
	int packet_size = packet_ptr[0];
	 
	while (num_data >= packet_size) { 
		PacketProcessor::GetInstance()->ProcessPacket(session->id, packet_ptr);
		num_data -= packet_size;
		packet_ptr += packet_size;
		if (0 >= num_data) break;
		packet_size = packet_ptr[0];
	}
	session->m_prev_size = num_data;
	if (0 != num_data) {
		memcpy(ex_over->m_packetbuf, packet_ptr, num_data);
	}

	PacketProcessor::GetInstance()->DoRecv(session->id);
}

void send_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags)
{
}
