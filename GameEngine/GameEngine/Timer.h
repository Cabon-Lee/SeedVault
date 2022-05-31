#pragma once

class Timer :public Singleton<Timer>
{
public:
	Timer();
	~Timer();

	float TotalTime() const;  // in seconds
	float DeltaTime() const; // in seconds

	void Reset(); // Call before message loop.
	void Start(); // Call when unpaused.
	void Stop();  // Call when paused.
	void Tick();  // Call every frame.

	void SetDeltaTime(double delta);		//임시

	bool FixFrame(float fps);
	void ResetDeltaTime();

	double m_Dtime;
private:
	double m_SecondsPerCount; // 클록당 걸리는 시간
	double m_DeltaTime;
	double m_ElapsedTime;

   __int64 m_BaseTime;
   __int64 m_PausedTime;
   __int64 m_StopTime;
   __int64 m_PrevTime;
   __int64 m_CurrTime;

	bool m_Stopped;
};

