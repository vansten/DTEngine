#include "Window.h"

#if DT_WINDOWS

#include "Win32/WindowWin32.h"

#else
#endif

UniquePtr<Window> Window::Create(const String& title, uint16 width, uint16 height)
{
	Window* newWindow = nullptr;
#if DT_WINDOWS

	newWindow = new WindowWin32(title, width, height);

#else

	newWindow = nullptr;

#endif

	return UniquePtr<Window>(newWindow);
}
