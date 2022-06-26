#include "pch.h"

#include "Timer.h"
#include "CATrace.h"

#include <windows.h>

Timer::Timer()
   : m_SecondsPerCount(0.0f)
   , m_ElapsedTime(0.0f)
   , m_DeltaTime(0.0f)
   , m_BaseTime(0.0f)
   , m_PausedTime(0.0f)
   , m_StopTime(0.0f)
   , m_PrevTime(0.0f)
   , m_CurrTime(0.0f)
   , m_Stopped(false)
{
   __int64 countsPerSec;
   QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
   m_SecondsPerCount = 1.0f / (double)countsPerSec;
}

Timer::~Timer()
{
}

// Returns the total time elapsed since Reset() was called, NOT counting any
// time when the clock is stopped.
float Timer::TotalTime()const
{
   // If we are stopped, do not count the time that has passed since we stopped.
   // Moreover, if we previously already had a pause, the distance 
   // mStopTime - mBaseTime includes paused time, which we do not want to count.
   // To correct this, we can subtract the paused time from mStopTime:  
   //
   //                     |<--paused time-->|
   // ----*---------------*-----------------*------------*------------*------> time
   //  mBaseTime       mStopTime        startTime     mStopTime    mCurrTime

   if (m_Stopped)
   {
      return (float)(((m_StopTime - m_PausedTime) - m_BaseTime) * m_SecondsPerCount);
   }

   // The distance mCurrTime - mBaseTime includes paused time,
   // which we do not want to count.  To correct this, we can subtract 
   // the paused time from mCurrTime:  
   //
   //  (mCurrTime - mPausedTime) - mBaseTime 
   //
   //                     |<--paused time-->|
   // ----*---------------*-----------------*------------*------> time
   //  mBaseTime       mStopTime        startTime     mCurrTime

   else
   {
      return (float)(((m_CurrTime - m_PausedTime) - m_BaseTime) * m_SecondsPerCount);
   }
}

float Timer::DeltaTime() const
{
    //CA_TRACE("%f", (float)m_Dtime);
   return (float)m_Dtime;
}

void Timer::Reset()
{
   __int64 currTime;
   QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

   m_BaseTime = currTime;
   m_PrevTime = currTime;
   m_StopTime = 0.0f;
   m_Stopped = false;
}

void Timer::Start()
{
   __int64 startTime;
   QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

   // Accumulate the time elapsed between stop and start pairs.
   //
   //                     |<-------d------->|
   // ----*---------------*-----------------*------------> time
   //  mBaseTime       mStopTime        startTime     

   if (m_Stopped)
   {
      m_PausedTime += (startTime - m_StopTime);

      m_PrevTime = startTime;
      m_StopTime = 0.0f;
      m_Stopped = false;
   }
}

void Timer::Stop()
{
   if (!m_Stopped)
   {
      __int64 currTime;
      QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

      m_StopTime = currTime;
      m_Stopped = true;
   }
}

void Timer::Tick()
{
   if (m_Stopped)
   {
      m_DeltaTime = 0.0f;
      return;
   }

   __int64 currTime = 0.0f;
   QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
   m_CurrTime = currTime;

   m_DeltaTime = (m_CurrTime - m_PrevTime) * m_SecondsPerCount;

   m_PrevTime = m_CurrTime;

   m_ElapsedTime += m_DeltaTime;

   if (m_DeltaTime < 0.0f)
   {
      m_DeltaTime = 0.0f;
   }
}

void Timer::SetDeltaTime(double delta)
{
    m_Dtime = delta;
}

bool Timer::FixFrame(float fps)
{
   if (m_ElapsedTime > 1.0f / (fps))
   {
      m_Dtime = m_ElapsedTime;
      return true;
   }
   else
   {
      return false;
   }
}

void Timer::ResetDeltaTime()
{
   m_ElapsedTime = 0.0f;
}

