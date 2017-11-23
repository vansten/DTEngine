#pragma once

#define APP_SUCCESSFUL 0x000
#define APP_INITIALIZATION_FAILED 0x001
#define APP_MEMORY_LEAKS 0x002

#include "Core/Platform.h"
#include "Utility/UniqueSingleton.h"

class Window;
class Time;
class Game;
class Graphics;
class ResourceManager;
class Input;
class Debug;

class App : public UniqueSingleton<App>
{
	friend class UniqueSingleton<App>;

protected:
	UniquePtr<Window> _mainWindow;
	UniquePtr<Time> _globalTime;
	UniquePtr<Game> _game;
	UniquePtr<Graphics> _graphics;
	UniquePtr<ResourceManager> _resourceManager;
	UniquePtr<Input> _input;
	UniquePtr<Debug> _debug;

	bool _isRunning;

protected:
	App();
	
public:
	~App();

protected:
	bool Initialize();
	void Loop();
	void Shutdown();

public:
	int Run();

	inline bool IsRunning() const
	{
		return _isRunning;
	}

	inline Window& GetWindow()
	{
		DT_ASSERT(_mainWindow);
		return *_mainWindow;
	}

	inline Time& GetGlobalTime()
	{
		DT_ASSERT(_globalTime);
		return *_globalTime;
	}

	inline Game& GetGame()
	{
		DT_ASSERT(_game);
		return *_game;
	}

	inline Graphics& GetGraphics()
	{
		DT_ASSERT(_graphics);
		return *_graphics;
	}

	inline ResourceManager& GetResourceManager() 
	{
		DT_ASSERT(_resourceManager);
		return *_resourceManager; 
	}

	inline Input& GetInput()
	{
		DT_ASSERT(_input);
		return *_input;
	}

	inline Debug& GetDebug()
	{
		DT_ASSERT(_debug);
		return *_debug;
	}
};

inline Window& GetMainWindow()
{
	return App::GetInstance()->GetWindow();
}

inline Time& GetGlobalTime()
{
	return App::GetInstance()->GetGlobalTime();
}

inline Game& GetGame()
{
	return App::GetInstance()->GetGame();
}

inline Graphics& GetGraphics()
{
	return App::GetInstance()->GetGraphics();
}

inline ResourceManager& GetResourceManager()
{
	return App::GetInstance()->GetResourceManager();
}

inline Input& GetInput()
{
	return App::GetInstance()->GetInput();
}

inline Debug& GetDebug()
{
	return App::GetInstance()->GetDebug();
}