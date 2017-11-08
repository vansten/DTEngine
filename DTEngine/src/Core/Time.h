#pragma once

#include "App.h"
#include "Platform.h"
#include "Utility/UniqueSingleton.h"

class Time
{
protected:
	float32 _deltaTime;
	float32 _timeScale;
	float32 _timeSinceStartup;

public:
	Time();

	virtual void Initialize() = 0;
	virtual void Tick() = 0;

	inline float32 GetTimeSinceStartup() const
	{
		return _timeSinceStartup;
	}

	inline float32 GetUnscaledDeltaTime() const
	{
		return _deltaTime;
	}

	inline float32 GetDeltaTime() const
	{
		return _deltaTime * _timeScale;
	}

	inline float32 GetTimeScale() const
	{
		return _timeScale;
	}

	inline void SetTimeScale(float32 timeScale)
	{
		_timeScale = timeScale;
	}

public:
	static UniquePtr<Time> Create();
};