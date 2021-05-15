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
	Skybox,
	TerrainWater,
	Puzzle,
	Obstacle,
	Enemy,
	Arrow,
	Billboard,
	Fog,
	Count,
};

class PacketProcessor
{
private:
	unordered_map<SOCKET, int>	m_SocketRegister;

	CRITICAL_SECTION			m_cs;
	CPlayer*					m_Players[MAX_PLAYER];
	CCamera*				    m_Cameras[MAX_PLAYER];
	int							m_CurrentPlayerNum = 0;

	int							m_CurrentlyDeletedPlayerId;

	int							m_Heights[TERRAIN_HEIGHT_MAP_HEIGHT + 1][TERRAIN_HEIGHT_MAP_WIDTH + 1];

	vector<CGameObject*>		m_Objects;

	array<vector<CGameObject*>, (int)OBJECT_LAYER::Count> m_ObjectLayers;

	// 플레이어가 새 지역으로 이동 시 이전 지역으로 이동을 막기 위한 벽을 생성
	// 씬 생성 시 저장한 후, 게임 중 상황에 따라 처리
	unordered_map<int, CGameObject*> m_BlockingPlateToPreviousSector;
private:
	// for update timer
	chrono::system_clock::time_point currentTime;
	chrono::duration<double> timeElapsed;

private:
	PacketProcessor()
	{
		currentTime = std::chrono::system_clock::now();
		InitializeCriticalSection(&m_cs); 
		InitAll();
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

	bool ProcessGameScene(SOCKET& socket);

	void UpdateLoop();

	void RegistSocket(SOCKET& socket, int id) { m_SocketRegister[socket] = id; }

private:
	void Update(float elapsedTime);

private:
	void InitAll();
	void InitPlayers();
	void InitCameras();
	void InitMonsters();
	
	void ReadObstaclesPosition();
	XMFLOAT3 GetPosition(const string& name, const Document& document);

	void InitObstacle();

	void InitTerrainHeightMap(); 

	void BuildBlockingRegionOnMap();

	void EnterNewSector(int sectorNum);

private:
	unordered_map<OBJECT_ID, XMFLOAT3> m_ObjectPositions;
};