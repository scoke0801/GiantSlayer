#pragma once
#include "Communicates.h"

#define MAX_CLIENT_NUM 4

class CPlayer;

class GameSceneProcessor
{
private:
	CRITICAL_SECTION			m_cs;
	CPlayer*					m_Players[MAX_CLIENT_NUM];

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
		return &self;
	}

	bool ProcessGameScene(SOCKET& socket);
};

