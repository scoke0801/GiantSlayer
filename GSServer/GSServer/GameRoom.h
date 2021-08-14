#pragma once

#include "Communicates.h"
#include "Player.h"
class CEnemy;
class CMummy;
class CMummyLaser;

enum class CommandType
{
	None = 0,
	Move,
	MoveAttack,
	Attack
};

// ���Ǹ� ���� Layer���� �� ���� ��ü ������ �����ϰ� ������ ����
enum class OBJECT_LAYER : int {
	TerrainWater,
	Puzzle,
	PuzzleBox,
	Obstacle,
	ChessPuzzle,
	PlayerChessPuzzle,
	Bridge,
	TerrainBoundary,
	MirrorBox,
	Enemy, 
	Mummy,
	Npc,

	PlayerArrow,
	MonsterArrow,

	Mummylaser,
	Mummylaser2,
	Mummylaser3,

	FireBall,

	Count
};

struct EX_OVER {
	WSAOVERLAPPED   m_over;
	WSABUF         m_wsabuf;
	unsigned char   m_packetbuf[MAX_BUFFER];
	SOCKET m_csocket; // OP_ACCEPT ������ ���
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
	array <CLIENT*, MAX_ROOM_PLAYER>		m_Clients;
	CPlayer*								m_Players[MAX_ROOM_PLAYER];
	CCamera*								m_Cameras[MAX_ROOM_PLAYER];

	int										m_CurrentPlayerNum = 0;

	bool									m_PlayerExistingSector[MAX_ROOM_PLAYER];

	int										m_CurrentlyDeletedPlayerId;

	array<vector<CGameObject*>, (int)OBJECT_LAYER::Count> m_ObjectLayers;

	// �÷��̾ �� �������� �̵� �� ���� �������� �̵��� ���� ���� ���� ����
	// �� ���� �� ������ ��, ���� �� ��Ȳ�� ���� ó��
	unordered_map<int, CGameObject*>		m_BlockingPlateToPreviousSector;
	 
	unordered_map<int, int>					m_IdIndexMatcher;

	int										m_DoorStartIndex;

	bool									m_IsActive = false;
	int										m_RoomIndex = -1;

	bool									m_Mummy_Reverse_Direction = false;
	
	CGameObject*							m_Chess[4];
	bool									m_ChessPlate_Check[4] = { false };		// ü����üũ��
	XMFLOAT3								m_ChessPlate[7][7];						// ü����
	bool									m_ChessCollide_Check[4] = { false,false,false,false };

	CMummy*									m_Mummy[3];
	bool									m_MummyExist[3] = { true,true,true };

	CMummyLaser*							m_MummyLaser[3];
	CMummyLaser*							m_MummyLaser2[3];
	CMummyLaser*							m_MummyLaser3[3];

	bool									m_One_Mira_Die = false;
	bool									m_One_Mira_Die_Laser = false;
	bool									m_Two_Mira_Die = false;
	bool									m_Two_Mira_Die_Laser = false;
	
	CEnemy*									m_Npc = nullptr;
	bool									m_Npc_Event = false;
	bool									m_Interaction = false;
public:
	CGameRoom();
	~CGameRoom() {}

	void InitRoom(int roomIndex) { InitAll(); m_RoomIndex = roomIndex; }
	static void SendPacket(SOCKET& socket, void* p);
public:
	void Update(float elapsedTime);

	void SendSyncUpdatePacket();
	void SendMonsterActPacket();

	void SendMummyActPacket();
	void SendMonsterArrowActPacket();
	void SendPlayerArrowActPacket();
	void SendFireballActPacket();
	void SendLaserActPacket();
	void SendChessObjectActPacket();

	void Disconnect(CLIENT& client); 
	void Disconnect(int id);

	void ProcessPacket(int p_id, unsigned char* p_buf);

	void DeleteObject(CGameObject* pObject, int layerIdx);

	void EnterPlayer(CLIENT& client, int weapontType);
	bool CanEnter();

	bool IsActive() { return m_IsActive; }
	
	CPlayer* GetPlayer(int index) { return m_Players[index]; }

public:
	void ShotPlayerArrow(int p_id);
	void ShotMonsterArrow(CEnemy* pEmeny, const XMFLOAT3& lookVector);

	void DeleteEnemy(CEnemy* pEmeny);

	void ShotMummyLaser(CMummy* pMummy, const XMFLOAT3& lookVector);

private:
	void InitAll();
	void InitPlayers();
	void InitCameras();
	void InitMonsters();
	void InitArrows();
	  
	void InitMummyLaser();
	void InitFireBall();

	void InitObstacle();
	 
	void BuildBlockingRegionOnMap();

	void EnterNewSector(int sectorNum);

private:
	void ResetPlayer(int player_id);

private: 

	void InitPrevUserData(int c_id);
	void DoRecv(int c_id);

	void SendPacket(int p_id, void* p);
}; 

