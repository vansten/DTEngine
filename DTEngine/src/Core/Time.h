#pragma once

#include "App.h"
#include "Platform.h"
#include "Utility/UniqueSingleton.h"

class Time
{
protected:
	float _deltaTime;
	float _timeScale;
	float _timeSinceStartup;

public:
	Time();

	virtual void Initialize() = 0;
	virtual void Tick() = 0;

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

public:
	static UniquePtr<Time> Create();
};