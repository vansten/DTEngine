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

App::~App()
{

}

bool App::Initialize()
{
	_isRunning = false;

	_input = UniquePtr<Input>(new Input());

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

	_graphics = UniquePtr<Graphics>(new Graphics());
	if(!_graphics)
	{
		return false;
	}

	if(!_graphics->Initialize(true))
	{
		return false;
	}

	_resourceManager = UniquePtr<ResourceManager>(new ResourceManager());
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

	_game = UniquePtr<Game>(new Game());
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
	_isRunning = true;
	float32 deltaTime = 1.0f / 60.0f;

	while(!MessageSystem::IsPendingQuit())
	{
		MessageSystem::GatherMessages(_mainWindow);

		// Physics::Resolve();

		_game->Update(deltaTime);

		_game->Render(*_graphics);

		_globalTime->Tick();
		deltaTime = _globalTime->GetDeltaTime();
	}

	_isRunning = false;
}

void App::Shutdown()
{
	if (_game)
	{
		_game->Shutdown();
	}

	if(_input)
	{
		_input->Shutdown();
	}

	if (_resourceManager)
	{
		_resourceManager->Shutdown();
	}

	if(_graphics)
	{
		_graphics->Shutdown();
	}

	if(_mainWindow)
	{
		_mainWindow->Hide();
		_mainWindow->Close();
	}
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
