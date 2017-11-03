#include "MessageSystem.h"

#include "Platform.h"

#if DT_WINDOWS

#include "Win32/WindowWin32.h"

#else

#endif

bool MessageSystem::_pendingQuit = false;

void MessageSystem::GatherMessages(Window* window)
{
#if DT_WINDOWS
	
	WindowWin32* windowWin32 = (WindowWin32*)window;
	if(windowWin32)
	{
		MSG msg;
		if(PeekMessage(&msg, windowWin32->GetHWND(), 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	
#else

	// Do nothing now, undefined platform

#endif
}
