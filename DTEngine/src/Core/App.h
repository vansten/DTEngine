#pragma once

#define APP_SUCCESSFUL 0x000
#define APP_INITIALIZATION_FAILED 0x001
#define APP_MEMORY_LEAKS 0x002

#include "Core/Platform.h"

class Window;
class Time;
class Game;
class Graphics;
class ResourceManager;

class App
{
protected:
	Window* _mainWindow;
	Time* _globalTime;
	Game* _game;
	Graphics* _graphics;
	ResourceManager* _resourceManager;

public:
	App();

protected:
	bool Initialize();
	void Loop();
	void Shutdown();

public:
	int Run();
};
