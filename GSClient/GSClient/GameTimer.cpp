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
	m_TimeElapsed = std::chrono::system_clock::now() - m_CurrentTime;//����ð��� �����ð��� ���ؼ�
	m_CurrentTime = std::chrono::system_clock::now();//����ð� ����
}

void CGameTimer::UpdateCurrentTime()
{
	m_CurrentTime = std::chrono::system_clock::now();//����ð� ����
}

void CGameTimer::Init()
{
	m_CurrentTime = std::chrono::system_clock::now();//����ð� ����
	m_TimeElapsed = m_CurrentTime - m_CurrentTime;
}

double CGameTimer::GetElapsedTime(std::chrono::system_clock::time_point lastUpdateTime) const
{
	std::chrono::duration<double> elapsedTime = std::chrono::system_clock::now() - lastUpdateTime;
	return elapsedTime.count();
}
