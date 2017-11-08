#pragma once

#include "Platform.h"

class Window;

class MessageSystem
{
protected:
	static bool _pendingQuit;

public:
	static void GatherMessages(const UniquePtr<Window>& window);

	static inline void PostQuit()
	{
		_pendingQuit = true;
	}

	static inline bool IsPendingQuit()
	{
		return _pendingQuit;
	}
};