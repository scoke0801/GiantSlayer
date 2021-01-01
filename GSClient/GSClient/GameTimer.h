#pragma once
class CGameTimer
{
public:
	CGameTimer();
	~CGameTimer();
	
	void Tick();
	void Init();
	double GetElapsedTime() const { return m_TimeElapsed.count(); }

private:
	// �ð� ó���� ���� �����Դϴ�. 
	std::chrono::system_clock::time_point m_CurrentTime;
	std::chrono::duration<double> m_TimeElapsed; // �ð��� �󸶳� ������
};

