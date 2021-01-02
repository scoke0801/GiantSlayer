#pragma once
class CGameTimer
{
public:
	CGameTimer();
	~CGameTimer();
	
	void Tick();
	void UpdateCurrentTime();
	std::chrono::system_clock::time_point CurrentTime() { return m_CurrentTime; }
	void Init();
	double GetElapsedTime() const { return m_TimeElapsed.count(); }
	double GetElapsedTime(std::chrono::system_clock::time_point lastUpdateTime) const;
	std::chrono::duration<double> GetElapsedTimeRaw() const { return m_TimeElapsed; }
private:
	// �ð� ó���� ���� �����Դϴ�. 
	std::chrono::system_clock::time_point m_CurrentTime;
	std::chrono::duration<double> m_TimeElapsed; // �ð��� �󸶳� ������
};

