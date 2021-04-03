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

class GameSceneProcessor
{
private:
	CRITICAL_SECTION			m_cs;
	CPlayer*					m_Players[MAX_PLAYER];

	bool						m_isInitialized = false;

	int							m_CurrentPlayerNum = 0;

	int							m_CurrentlyDeletedPlayerId;
private:
	GameSceneProcessor()
	{
		InitializeCriticalSection(&m_cs);
		//ZeroMemory(m_Players, sizeof(m_Players)); 
	}
	~GameSceneProcessor() { DeleteCriticalSection(&m_cs); }

	GameSceneProcessor(const GameSceneProcessor& other) = delete;
	void operator=(const GameSceneProcessor& other) = delete;

public:
	static GameSceneProcessor* GetInstance()
	{
		static GameSceneProcessor self;
		if(self.m_isInitialized == false)
		{
			self.InitAll();
			 self.m_isInitialized = true;
		}
		return &self;
	}

	bool ProcessGameScene(SOCKET& socket);

private:
	void InitAll();
	void InitPlayers();
	void InitMonsters();
};

