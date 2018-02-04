#pragma once

#include "Platform.h"

class Window;

class MessageSystem final
{
protected:
	static bool _pendingQuit;

public:
	static void GatherMessages();

	static inline void PostQuit()
	{
		_pendingQuit = true;
	}

	static inline bool IsPendingQuit()
	{
		return _pendingQuit;
	}
};