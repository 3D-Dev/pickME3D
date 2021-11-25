

#ifndef __HTIMER__
#define __HTIMER__

#include <windows.h>

class CHiResTimer
{
public:
		CHiResTimer() {}
		~CHiResTimer() {}
		bool Init();
		float GetElapsedSeconds();
		float GetFPS(unsigned long ElapsedFrames = 1);
		float LockFPS(unsigned char TargetFPS);

private:
		LARGE_INTEGER   _StartTime;
		LARGE_INTEGER   _TicksPerSecond;
};

#endif 
