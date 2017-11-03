#include "App.h"

#include "MessageSystem.h"
#include "Window.h"
#include "Time.h"
#include "Input.h"

#include "Rendering/Graphics.h"

#include "ResourceManagement/ResourceManager.h"

#include "GameFramework/Game.h"

App::App() : _mainWindow(nullptr), _resourceManager(nullptr), _graphics(nullptr), _globalTime(nullptr), _game(nullptr)
{

}

bool App::Initialize()
{
	_mainWindow = Window::Create(DT_TEXT("DT Engine"), 1024, 768);

	if(!_mainWindow)
	{
		return false;
	}

	if(!_mainWindow->Open())
	{
		return false;
	}

	if(!_mainWindow->Show())
	{
		return false;
	}

	_graphics = new Graphics();
	if(!_graphics)
	{
		return false;
	}

	if(!_graphics->Initialize(_mainWindow, true))
	{
		return false;
	}

	_resourceManager = new ResourceManager();
	if (!_resourceManager)
	{
		return false;
	}

	if (!_resourceManager->Initialize())
	{
		return false;
	}

	_globalTime = Time::Create();
	if(!_globalTime)
	{
		return false;
	}

	_globalTime->Initialize();

	Input::Initialize();

	_game = new Game();
	if(!_game)
	{
		return false;
	}

	if(!_game->Initialize())
	{
		return false;
	}
	
	return true;
}

void App::Loop()
{
	float32 deltaTime = 1.0f / 60.0f;

	while(!MessageSystem::IsPendingQuit())
	{
		MessageSystem::GatherMessages(_mainWindow);

		// Physics::Resolve();

		_game->Update(deltaTime);

		_game->Render(_graphics);

		_globalTime->Tick();
		deltaTime = _globalTime->GetDeltaTime();
	}
}

void App::Shutdown()
{
	if (_game)
	{
		_game->Shutdown();
		delete _game;
		_game = nullptr;
	}

	Input::Shutdown();

	if (_globalTime)
	{
		delete _globalTime;
		_globalTime = nullptr;
	}

	if (_resourceManager)
	{
		_resourceManager->Shutdown();
		delete _resourceManager;
		_resourceManager = nullptr;
	}

	if(_graphics)
	{
		_graphics->Shutdown();
		delete _graphics;
		_graphics = nullptr;
	}

	if(_mainWindow)
	{
		_mainWindow->Hide();
		_mainWindow->Close();
		delete _mainWindow;
		_mainWindow = nullptr;
	}

	Window::Shutdown();
}

int App::Run()
{
	if(!Initialize())
	{
		return APP_INITIALIZATION_FAILED;
	}

	Loop();

	Shutdown();

	return APP_SUCCESSFUL;
}
