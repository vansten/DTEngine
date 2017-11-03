#include "Window.h"

#if DT_WINDOWS

#include "Win32/WindowWin32.h"

#else
#endif

Window* gWindow = nullptr;

Window* Window::Create(const string& title, uint16 width, uint16 height)
{
	Window* newWindow = nullptr;
#if DT_WINDOWS

	newWindow = new WindowWin32(title, width, height);

#else

	newWindow = nullptr;

#endif

	if (!gWindow)
	{
		// Register first window as global
		gWindow = newWindow;
	}

	return newWindow;
}

void Window::Shutdown()
{
	gWindow = nullptr;
}
