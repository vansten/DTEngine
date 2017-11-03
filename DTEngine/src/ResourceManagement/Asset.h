#pragma once

#include "Core/Platform.h"

class Asset
{
protected:
	string _path;

public:
	Asset();
	virtual ~Asset();

	virtual bool Initialize(const string& path);
	virtual void Shutdown() = 0;
};