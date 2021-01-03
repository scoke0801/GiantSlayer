#include "stdafx.h"
#include "GameTimer.h"

CGameTimer::CGameTimer()
{
	Init();
}

CGameTimer::~CGameTimer()
{
}

void CGameTimer::UpdateElapsedTime()
{
	m_TimeElapsed = std::chrono::system_clock::now() - m_CurrentTime;
}

void CGameTimer::UpdateCurrentTime()
{
	m_CurrentTime = std::chrono::system_clock::now();//현재시간 갱신
}

void CGameTimer::Init()
{
	m_CurrentTime = std::chrono::system_clock::now();//현재시간 갱신
	m_TimeElapsed = m_CurrentTime - m_CurrentTime;
}

double CGameTimer::GetElapsedTime(std::chrono::system_clock::time_point lastUpdateTime) const
{
	std::chrono::duration<double> elapsedTime = std::chrono::system_clock::now() - lastUpdateTime;
	return elapsedTime.count();
}
