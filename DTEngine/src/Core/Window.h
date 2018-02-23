
#pragma once

#include "Platform.h"

class Window final
{
private:
	unsigned int _width;
	unsigned int  _height;
	float _aspectRatio;
	String _title;

	HWND _hWnd;

public:
	inline unsigned int GetWidth() const
	{
		return _width;
	}

	inline unsigned int GetHeight() const
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

	bool Open(const String& title, unsigned int width, unsigned int height);
	bool Show();
	bool Hide();
	bool Close();

	void SetNewSize(unsigned int width, unsigned int height);
};

extern Window gWindow;