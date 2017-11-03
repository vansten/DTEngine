#include "Time.h"

#if DT_WINDOWS

#include "Win32/TimeWin32.h"

#else



#endif

Time* gTime = nullptr;

Time::Time() : _deltaTime(0), _timeSinceStartup(0), _timeScale(1)
{

}

Time* Time::Create()
{
#if DT_WINDOWS

	if (!gTime)
	{
		gTime = new TimeWin32();
		return gTime;
	}

	return new TimeWin32();

#else

	return nullptr;
	
#endif
}
