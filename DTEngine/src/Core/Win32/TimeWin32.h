#pragma once

#include "Core/Time.h"

class TimeWin32 : public Time
{
protected:
	LARGE_INTEGER _frequency;
	LARGE_INTEGER _previousTime;

public:
	TimeWin32();

	virtual void Initialize() override;
	virtual void Tick() override;
};