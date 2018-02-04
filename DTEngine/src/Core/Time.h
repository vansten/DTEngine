#pragma once

#include "App.h"
#include "Platform.h"
#include "Utility/UniqueSingleton.h"

class Time final
{
private:
	float _deltaTime;
	float _timeScale;
	float _timeSinceStartup;

	LARGE_INTEGER _frequency;
	LARGE_INTEGER _previousTime;

public:
	Time();

	virtual void Initialize();
	virtual void Tick();

	inline float GetTimeSinceStartup() const
	{
		return _timeSinceStartup;
	}

	inline float GetUnscaledDeltaTime() const
	{
		return _deltaTime;
	}

	inline float GetDeltaTime() const
	{
		return _deltaTime * _timeScale;
	}

	inline float GetTimeScale() const
	{
		return _timeScale;
	}

	inline void SetTimeScale(float timeScale)
	{
		_timeScale = timeScale;
	}
};

extern Time gTime;