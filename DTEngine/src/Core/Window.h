#pragma once

#include "App.h"
#include "Platform.h"

class Window
{
protected:
	uint16 _width;
	uint16  _height;
	float32 _aspectRatio;
	String _title;

public:	
	inline Window(const String& title, uint16 width, uint16 height) : _width(width), _height(height), _title(title), _aspectRatio((float32)_width / (float32)_height)
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
	static UniquePtr<Window> Create(const String& title, uint16 width, uint16 height);
};
