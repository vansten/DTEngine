#pragma once

#include "Core/Window.h"

class WindowWin32 : public Window
{
protected:
	HWND _hWnd;

public:
	inline WindowWin32(const string& title, uint16 width, uint16 height) : Window(title, width, height)
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