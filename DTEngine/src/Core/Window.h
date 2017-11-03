#pragma once

#include "Platform.h"

class Window
{
protected:
	uint16 _width;
	uint16  _height;
	float32 _aspectRatio;
	string _title;

public:	
	inline Window(const string& title, uint16 width, uint16 height) : _width(width), _height(height), _title(title), _aspectRatio((float32)_width / (float32)_height)
	{

	}

	inline uint16 GetWidth() const
	{
		return _width;
	}

	inline uint16 GetHeight() const
	{
		return _height;
	}

	inline float32 GetAspectRatio() const
	{
		return _aspectRatio;
	}

	virtual bool Open() = 0;
	virtual bool Show() = 0;
	virtual bool Hide() = 0;
	virtual bool Close() = 0;

public:
	static Window* Create(const string& title, uint16 width, uint16 height);
	static void Shutdown();
};

extern Window* gWindow;