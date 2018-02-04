#include "Time.h"

Time gTime;

Time::Time() : _deltaTime(0.0f), _timeSinceStartup(0.0f), _timeScale(1.0f)
{

}

void Time::Initialize()
{
	QueryPerformanceFrequency(&_frequency);
	QueryPerformanceCounter(&_previousTime);
}

void Time::Tick()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);

	_deltaTime = ((currentTime.QuadPart - _previousTime.QuadPart) * 1000.0f) / _frequency.QuadPart;
	_deltaTime *= 0.001f;
	_timeSinceStartup += _deltaTime;

	_previousTime = currentTime;
}

