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
	m_CurrentTime = std::chrono::system_clock::now();//����ð� ����
	m_TimeElapsed = std::chrono::system_clock::now() - m_CurrentTime;//����ð��� �����ð��� ���ؼ�
}

void CGameTimer::Init()
{
	m_CurrentTime = std::chrono::system_clock::now();//����ð� ����
	m_TimeElapsed = m_CurrentTime - m_CurrentTime;
}
