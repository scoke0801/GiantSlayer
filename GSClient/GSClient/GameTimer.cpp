#include "stdafx.h"
#include "GameTimer.h"

CGameTimer::CGameTimer()
{
	Init();
}

CGameTimer::~CGameTimer()
{
}

void CGameTimer::Tick()
{
	m_CurrentTime = std::chrono::system_clock::now();//현재시간 갱신
	m_TimeElapsed = std::chrono::system_clock::now() - m_CurrentTime;//현재시간과 이전시간을 비교해서
}

void CGameTimer::Init()
{
	m_CurrentTime = std::chrono::system_clock::now();//현재시간 갱신
	m_TimeElapsed = m_CurrentTime - m_CurrentTime;
}
