#pragma once

#include "App.h"
#include "Platform.h"

class Window
{
protected:
	unsigned short _width;
	unsigned short  _height;
	float _aspectRatio;
	String _title;

public:	
	inline Window(const String& title, unsigned short width, unsigned short height) : _width(width), _height(height), _title(title), _aspectRatio((float)_width / (float)_height)
	{

	}

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

	virtual bool Open() = 0;
	virtual bool Show() = 0;
	virtual bool Hide() = 0;
	virtual bool Close() = 0;

	void SetNewSize(unsigned short width, unsigned short height);

public:
	static UniquePtr<Window> Create(const String& title, unsigned short width, unsigned short height);
};
