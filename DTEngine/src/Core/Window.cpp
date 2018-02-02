#include "Window.h"

#if DT_WINDOWS

#include "Win32/WindowWin32.h"

#else
#endif

void Window::SetNewSize(unsigned short width, unsigned short height)
{
	if(_width == width && _height == height)
	{
		// Prevent resizing when current size already matches requested size
		return;
	}

	_width = width;
	_height = height;
	_aspectRatio = (float)_width / (float)_height;
}

UniquePtr<Window> Window::Create(const String& title, unsigned short width, unsigned short height)
{
	Window* newWindow = nullptr;
#if DT_WINDOWS

	newWindow = new WindowWin32(title, width, height);

#else

	newWindow = nullptr;

#endif

	return UniquePtr<Window>(newWindow);
}
