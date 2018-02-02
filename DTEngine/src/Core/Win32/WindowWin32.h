#pragma once

#include "Core/Window.h"

class WindowWin32 : public Window
{
protected:
	HWND _hWnd;

public:
	inline WindowWin32(const String& title, unsigned short width, unsigned short height) : Window(title, width, height)
	{

	}

	inline HWND GetHWND() const
	{
		return _hWnd;
	}

	virtual bool Open() override;
	virtual bool Show() override;
	virtual bool Hide() override;
	virtual bool Close() override;
};