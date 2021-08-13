#include "stdafx.h"
#include "PacketProcessor.h"
#include "protocol.h" 
#include "MapObjects.h"
#include "Enemy.h"
#include "Arrow.h"

extern int g_Heights[TERRAIN_HEIGHT_MAP_HEIGHT + 1][TERRAIN_HEIGHT_MAP_WIDTH + 1];

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
	else {
		return;
	}
}
 
int PacketProcessor::IsCanLogin()
{
	for (int i = 0; i <= MAX_PLAYER; ++i) {
		if (PLST_FREE == m_Clients[i].m_state) { 
			return i;
		}
	}
	return -1; 
}

int PacketProcessor::GetNewPlayerId(SOCKET socket)
{
	for (int i = 0; i <= MAX_PLAYER; ++i) {
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

void PacketProcessor::Update(float elapsedTime)
{
	for (int i = 0; i < m_Rooms.size(); ++i) {
		if (m_Rooms[i].IsActive()) {
			m_Rooms[i].Update(elapsedTime);
			m_Rooms[i].SendSyncUpdatePacket();
			m_Rooms[i].SendMonsterActPacket();
		}  
	}
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
		if (WSA_IO_PENDING != err_no) {
			error_display("Error in RecvPacket: ");
			//Disconnect(s_id);
		}
	}
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
			g_Heights[i][j] = stoi(text);
		} 
	} 

	for (int i = 0; i < 25; ++i)
	{
		for (int j = 0; j < 25; ++j)
		{
			int WidthBlock_Count = 9, DepthBlock_Count = 9;
			int WidthBlock_Index = 257, DepthBlock_Index = 257;
			int xStart = 0, zStart = 0;

			float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX;

			int* copyHeights = new int[25];

			int xIndex = 4 * j;
			int zIndex = 4 * i;
			for (int a = 0, b = 4, z = (zStart + 10 - 1); z >= zStart; z -= 2, --b)
			{
				for (int x = xStart; x < (xStart + 10 - 1); x += 2, a++)
				{
					if (a >= 25) break;

					copyHeights[a] = g_Heights[zIndex + b][xIndex + a % 5];

				}
			}

			g_GridHeights[i][j] = copyHeights;
		}
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
		g_ObjectPositions.emplace(OBJECT_ID((int)OBJECT_ID::BRIDEGE_SEC2_SEC3_1 + i),
			GetPosition(str, d));
	}

	g_ObjectPositions.emplace(OBJECT_ID::SIGN_SCROLL,
		GetPosition("SIGN_SCROLL", d));
	g_ObjectPositions.emplace(OBJECT_ID::SIGN_PUZZLE,
		GetPosition("SIGN_PUZZLE", d));
	g_ObjectPositions.emplace(OBJECT_ID::SIGN_MEDUSA,
		GetPosition("SIGN_MEDUSA", d));
	g_ObjectPositions.emplace(OBJECT_ID::SIGN_BOSS,
		GetPosition("SIGN_BOSS", d));

	for (int i = 0; i < 2; ++i) {
		string str = "PUZZLE_" + to_string(i + 1);
		g_ObjectPositions.emplace(OBJECT_ID((int)OBJECT_ID::PUZZLE_1 + i),
			GetPosition(str, d));
	}

	for (int i = 0; i < 5; ++i) {
		string str = "DOOR_WALL_SEC" + to_string(i + 1);
		g_ObjectPositions.emplace(OBJECT_ID((int)OBJECT_ID::DOOR_WALL_SEC1 + i),
			GetPosition(str, d));
	}
	g_ObjectPositions.emplace(OBJECT_ID::DRY_FOREST_ROCK_1,
		GetPosition("DRY_FOREST_ROCK_1", d));
	g_ObjectPositions.emplace(OBJECT_ID::DRY_FOREST_ROCK_2,
		GetPosition("DRY_FOREST_ROCK_2", d));

	for (int i = 0; i < 4; ++i) {
		string str = "DRY_FOREST_DRY_TREE_" + to_string(i + 1);
		g_ObjectPositions.emplace(OBJECT_ID((int)OBJECT_ID::DRY_FOREST_DRY_TREE_1 + i),
			GetPosition(str, d));
	}
	g_ObjectPositions.emplace(OBJECT_ID::DRY_FOREST_STUMP_1,
		GetPosition("DRY_FOREST_STUMP_1", d));

	for (int i = 0; i < 3; ++i) {
		string str = "DRY_FOREST_DEAD_TREE_" + to_string(i + 1);
		g_ObjectPositions.emplace(OBJECT_ID((int)OBJECT_ID::DRY_FOREST_DEAD_TREE_1 + i),
			GetPosition(str, d));
	}
	for (int i = 0; i < 10; ++i) {
		string str = "DESERT_ROCK_" + to_string(i + 1);
		g_ObjectPositions.emplace(OBJECT_ID((int)OBJECT_ID::DESERT_ROCK_1 + i),
			GetPosition(str, d));
	}

	for (int i = 0; i < 10; ++i) {
		string str = "PUZZLE_BOX_" + to_string(i + 1);
		g_ObjectPositions.emplace(OBJECT_ID((int)OBJECT_ID::PUZZLE_BOX_1 + i),
			GetPosition(str, d));
	}
}

XMFLOAT3 PacketProcessor::GetPosition(const string& name, const Document& document)
{
	XMFLOAT3 pos;
	pos.x = document[name.c_str()].GetArray()[0].GetInt() * MAP_SCALE_SIZE;
	pos.y = document[name.c_str()].GetArray()[1].GetInt();
	pos.z = document[name.c_str()].GetArray()[2].GetInt() * MAP_SCALE_SIZE;

	return pos;
}

void PacketProcessor::Disconnect(CLIENT& client)
{
	cout << "로그 아웃\n";

	m_Rooms[client.m_RoomIndex].Disconnect(client);
}

void PacketProcessor::ProcessPacket(CLIENT& client, unsigned char* p_buf)
{  
	// buffer[0]의 값은 packet protocol size
	// buffer[1]의 값은 packet protocol type
	PACKET_PROTOCOL type = (PACKET_PROTOCOL)p_buf[1];
	switch (type) {
	case PACKET_PROTOCOL::C2S_REQUEST_ROOM_INFO:
	{ 
		P_C2S_REQUEST_ROOM_INFO p_RequestRoomInfo = *reinterpret_cast<P_C2S_REQUEST_ROOM_INFO*>(p_buf);
		int roomNo = p_RequestRoomInfo.baseRoomNo;

		P_S2C_SEND_ROOM_INFO p_RoomInfo; 
		ZeroMemory(&p_RoomInfo, sizeof(P_S2C_SEND_ROOM_INFO));
		p_RoomInfo.size = sizeof(P_S2C_SEND_ROOM_INFO);
		p_RoomInfo.type = PACKET_PROTOCOL::S2C_SEND_ROOM_INFO;
		for (int i = 0; i < 4; ++i) {
			if (false == m_Rooms[roomNo + i- 1].IsActive()){
				break;
			}
			for (int j = 0; j < MAX_ROOM_PLAYER; ++i) {
				CPlayer* player = m_Rooms[roomNo + i - 1].GetPlayer(j);
				if (player->IsExist()) {
					p_RoomInfo.weapons[i * 4 + j] = (int)player->GetWeaponType();
				}
			}
		}  
		SendPacket(client.m_socket, &p_RoomInfo);
	}
		break;
	case PACKET_PROTOCOL::C2S_LOGIN:
	{ 
		P_C2S_LOGIN p_login = *reinterpret_cast<P_C2S_LOGIN*>(p_buf);
		client.m_RoomIndex = p_login.roomIndex; 
		if (client.m_RoomIndex == -1) {
			for (int i = 0; i < MAX_PLAYER; ++i) {
				if (m_Rooms[i].CanEnter()) {
					client.m_RoomIndex = i;
					break;
				}
			}
		}
		m_Rooms[client.m_RoomIndex].EnterPlayer(client);
	} 
	// break; 안해요
	default:
		m_Rooms[client.m_RoomIndex].ProcessPacket(client.id, p_buf); 
	}
}
 
void recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags)
{
	auto client = reinterpret_cast<CLIENT*>(overlapped);
	SOCKET client_s = client->m_socket;
	auto ex_over = &client->m_recv_over;

	if (dataBytes == 0) {
		PacketProcessor::GetInstance()->Disconnect(*client);
		return;
	}
	unsigned char* packet_ptr = ex_over->m_packetbuf;
	int num_data = dataBytes + client->m_prev_size;
	int packet_size = packet_ptr[0];

	while (num_data >= packet_size) {
		PacketProcessor::GetInstance()->ProcessPacket(*client, packet_ptr);
		num_data -= packet_size;
		packet_ptr += packet_size;
		if (0 >= num_data) break;
		packet_size = packet_ptr[0];
	}
	client->m_prev_size = num_data;
	if (0 != num_data) {
		memcpy(ex_over->m_packetbuf, packet_ptr, num_data);
	}

	PacketProcessor::GetInstance()->DoRecv(client->id);
}

void send_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags)
{
}

