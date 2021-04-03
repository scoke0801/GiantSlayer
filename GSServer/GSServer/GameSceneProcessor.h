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

class PacketProcessor
{
private:
	CRITICAL_SECTION			m_cs;
	CPlayer*					m_Players[MAX_PLAYER];

	int							m_CurrentPlayerNum = 0;

	int							m_CurrentlyDeletedPlayerId;

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
private:
	void InitAll();
	void InitPlayers();
	void InitMonsters();

	void Update(float elapsedTime);
};

