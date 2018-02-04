#pragma once

#include "Platform.h"

class Window final
{
private:
	unsigned short _width;
	unsigned short  _height;
	float _aspectRatio;
	String _title;

	HWND _hWnd;

public:	
	inline unsigned short GetWidth() const
	{
		return _width;
	}

	inline unsigned short GetHeight() const
	{
		return _height;
	}

	inline float GetAspectRatio() const
	{
		return _aspectRatio;
	}

	inline HWND GetHWND() const
	{
		return _hWnd;
	}

	bool Open(const String& title, unsigned short width, unsigned short height);
	bool Show();
	bool Hide();
	bool Close();

	void SetNewSize(unsigned short width, unsigned short height);
};

extern Window gWindow;