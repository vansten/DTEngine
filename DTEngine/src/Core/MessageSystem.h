#pragma once

class Window;

class MessageSystem
{
protected:
	static bool _pendingQuit;

public:
	static void GatherMessages(Window* window);

	static inline void PostQuit()
	{
		_pendingQuit = true;
	}

	static inline bool IsPendingQuit()
	{
		return _pendingQuit;
	}
};