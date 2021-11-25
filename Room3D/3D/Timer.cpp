

#include "Timer.h"



bool CHiResTimer::Init()
{
	  if (!QueryPerformanceFrequency(&_TicksPerSecond))
	  {
			return false;
	  }
	  else
	  {
			QueryPerformanceCounter(&_StartTime);
			return true;
	  }
}

float CHiResTimer::GetElapsedSeconds()
{
	  static LARGE_INTEGER LastTime = _StartTime;
	  LARGE_INTEGER CurrentTime;

	  QueryPerformanceCounter(&CurrentTime);
	  float Seconds =  ((float)CurrentTime.QuadPart - (float)LastTime.QuadPart) / (float)_TicksPerSecond.QuadPart;
	  LastTime = CurrentTime;

	  return Seconds;
}

float CHiResTimer::GetFPS(unsigned long ElapsedFrames)
{
	  static LARGE_INTEGER LastTime = _StartTime;
	  LARGE_INTEGER CurrentTime;

	  QueryPerformanceCounter(&CurrentTime);
	  float fps = (float)ElapsedFrames * (float)_TicksPerSecond.QuadPart / ((float)CurrentTime.QuadPart - (float)LastTime.QuadPart);
	  LastTime = CurrentTime;

	  return fps;
}

float CHiResTimer::LockFPS(unsigned char TargetFPS)
{
	  if (TargetFPS == 0) TargetFPS = 1;

	  static LARGE_INTEGER LastTime = _StartTime;
	  LARGE_INTEGER CurrentTime;
	  float fps;

	  do 
	  {
			QueryPerformanceCounter(&CurrentTime);
			fps = (float)_TicksPerSecond.QuadPart/((float)(CurrentTime.QuadPart - LastTime.QuadPart));

	  } while (fps > (float)TargetFPS);

	  LastTime = _StartTime;

	  return fps;
}  