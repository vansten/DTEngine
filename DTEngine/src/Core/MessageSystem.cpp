#include "MessageSystem.h"

#include "Window.h"

bool MessageSystem::_pendingQuit = false;

void MessageSystem::GatherMessages()
{
	MSG msg;
	while(PeekMessage(&msg, gWindow.GetHWND(), 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
