#pragma once

#include "Core/Platform.h"

class Asset
{
protected:
	String _path;

public:
	Asset();
	virtual ~Asset();

	virtual bool Initialize(const String& path);
	virtual void Shutdown() = 0;
};