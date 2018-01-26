#pragma once

#include "Core/Platform.h"

class Asset
{
protected:
	String _path;

public:
	Asset();
	virtual ~Asset();

	virtual bool Load(const String& path);
	virtual bool Save(const String& path);

	virtual bool Initialize();
	virtual void Shutdown();

	inline const String& GetPath() const { return _path; }
};