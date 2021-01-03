#pragma once
class CGameTimer
{
public:
	CGameTimer();
	~CGameTimer();

	void Init();

	void UpdateElapsedTime();
	void UpdateCurrentTime();	
	std::chrono::system_clock::time_point CurrentTime() { return m_CurrentTime; }
	
	double GetElapsedTime() const { return m_TimeElapsed.count(); }
	double GetElapsedTime(std::chrono::system_clock::time_point lastUpdateTime) const;
	std::chrono::duration<double> GetElapsedTimeRaw() const { return m_TimeElapsed; }

private:
	// 시간 처리를 위한 변수입니다. 
	std::chrono::system_clock::time_point m_CurrentTime;
	std::chrono::duration<double> m_TimeElapsed; // 시간이 얼마나 지났나
};

