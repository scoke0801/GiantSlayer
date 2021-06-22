#pragma once

#include "Communicates.h"
#include "Player.h"


enum class CommandType
{
	None = 0,
	Move,
	MoveAttack,
	Attack
};

// 편의를 위해 Layer에서 몇 가지 객체 유형은 제외하고 별도로 관리
enum class OBJECT_LAYER : int {
	TerrainWater,
	Puzzle,
	PuzzleBox,
	Obstacle,
	MirrorBox,
	Enemy,
	PlayerArrow,
	MonsterArrow,
	Count,
};

struct EX_OVER {
	WSAOVERLAPPED   m_over;
	WSABUF         m_wsabuf;
	unsigned char   m_packetbuf[MAX_BUFFER];
	SOCKET m_csocket; // OP_ACCEPT 에서만 사용
};

enum PL_STATE { PLST_FREE, PLST_CONNECTED, PLST_INGAME };
struct CLIENT {
	EX_OVER		m_recv_over;
	SOCKET		m_socket;
	PL_STATE	m_state;
	int			id;

	char		m_name[200];
	int			m_prev_size;
	short		m_RoomIndex;
};


class CGameRoom
{
	array <CLIENT*, MAX_ROOM_PLAYER + 1>	m_Clients;
	CPlayer*								m_Players[MAX_ROOM_PLAYER];
	CCamera*								m_Cameras[MAX_ROOM_PLAYER];
	int										m_CurrentPlayerNum = 0;

	int										m_CurrentlyDeletedPlayerId;
	 
	array<vector<CGameObject*>, (int)OBJECT_LAYER::Count> m_ObjectLayers;
	 
	// 플레이어가 새 지역으로 이동 시 이전 지역으로 이동을 막기 위한 벽을 생성
	// 씬 생성 시 저장한 후, 게임 중 상황에 따라 처리
	unordered_map<int, CGameObject*>		m_BlockingPlateToPreviousSector;

	unordered_map<OBJECT_ID, XMFLOAT3>		m_ObjectPositions;
	int										m_DoorStartIndex;
	 
public:
	void Update(float elapsedTime);

	void EnterPlayer(CLIENT* client, int id);
	void Disconnect(CLIENT& client); 
	void Disconnect(int id);

	void ProcessPacket(int p_id, unsigned char* p_buf);

	void DeleteObject(CGameObject* pObject, int layerIdx);

private:
	void InitAll();
	void InitPlayers();
	void InitCameras();
	void InitMonsters();
	void InitArrows();

	void ReadObstaclesPosition();
	XMFLOAT3 GetPosition(const string& name, const Document& document);

	void InitObstacle();
	 
	void BuildBlockingRegionOnMap();

	void EnterNewSector(int sectorNum);

private:
	void ResetPlayer(int player_id);

private: 
	int GetNewPlayerId(SOCKET socket);

	void InitPrevUserData(int c_id);
	void DoRecv(int c_id);

	void SendPacket(int p_id, void* p);
	void SendSyncUpdatePacket();
	void SendMonsterActPacket();
}; 

