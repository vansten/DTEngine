#include "Time.h"

#if DT_WINDOWS

#include "Win32/TimeWin32.h"

#else



#endif

Time::Time() : _deltaTime(0.0f), _timeSinceStartup(0.0f), _timeScale(1.0f)
{

}

UniquePtr<Time> Time::Create()
{
#if DT_WINDOWS

	return UniquePtr<Time>(new TimeWin32());

#else

	return nullptr;
	
#endif
}
