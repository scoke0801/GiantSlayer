#pragma once

#ifndef GAMETIMER_H
#define GAMETIMER_H

class CGameTimer
{
protected:
	// 시간 처리를 위한 변수입니다. 
	std::chrono::system_clock::time_point m_CurrentTime;
	std::chrono::duration<double> m_TimeElapsed; // 시간이 얼마나 지났나

	// 확장성, 다른 방식 시간 측정
protected:
	double mSecondsPerCount;
	double mDeltaTime;

	__int64 mBaseTime;
	__int64 mPausedTime;
	__int64 mStopTime;
	__int64 mPrevTime;
	__int64 mCurrTime;

	bool mStopped;
public:
	CGameTimer();
	~CGameTimer();

	void Init();

	void UpdateElapsedTime();
	void UpdateCurrentTime();
	void UpdateTime();

	std::chrono::system_clock::time_point CurrentTime() { return m_CurrentTime; }
	
	double GetElapsedTime() const { return m_TimeElapsed.count(); }
	double GetElapsedTime(std::chrono::system_clock::time_point lastUpdateTime) const;
	std::chrono::duration<double> GetElapsedTimeRaw() const { return m_TimeElapsed; }

	// 확장성, 다른 방식 시간 측정
public:
	// 추가
	float TotalTime()const; // in seconds
	float DeltaTime()const; // in seconds

	void Reset(); // Call before message loop.
	void Start(); // Call when unpaused.
	void Stop();  // Call when paused.
	void Tick();  // Call every frame. 
};

#endif // GAMETIMER_H