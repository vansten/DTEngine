#include "TimeWin32.h"

TimeWin32::TimeWin32() : Time()
{

}

void TimeWin32::Initialize()
{
	QueryPerformanceFrequency(&_frequency);
	QueryPerformanceCounter(&_previousTime);
}

void TimeWin32::Tick()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);

	_deltaTime = ((currentTime.QuadPart - _previousTime.QuadPart) * 1000.0f) / _frequency.QuadPart;
	_deltaTime *= 0.001f;
	_timeSinceStartup += _deltaTime;

	_previousTime = currentTime;
}
