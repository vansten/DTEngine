#include "App.h"

#include "MessageSystem.h"
#include "Window.h"
#include "Time.h"
#include "Input.h"

#include "Debug/Debug.h"

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

	_debug = UniquePtr<Debug>(new Debug());
	if(!_debug->Initialize())
	{
		return false;
	}

	_input = UniquePtr<Input>(new Input());

	_mainWindow = Window::Create(DT_TEXT("DT Engine"), 1024, 768);

	if(!_mainWindow)
	{
		GetDebug().Print(LogVerbosity::Error, CHANNEL_ENGINE, DT_TEXT("Cannot create window"));
		return false;
	}

	if(!_mainWindow->Open())
	{
		GetDebug().Print(LogVerbosity::Error, CHANNEL_ENGINE, DT_TEXT("Cannot open window"));
		return false;
	}

	if(!_mainWindow->Show())
	{
		GetDebug().Print(LogVerbosity::Error, CHANNEL_ENGINE, DT_TEXT("Cannot show window"));
		return false;
	}

	_graphics = UniquePtr<Graphics>(new Graphics());
	if(!_graphics)
	{
		GetDebug().Print(LogVerbosity::Error, CHANNEL_ENGINE, DT_TEXT("Cannot create graphics"));
		return false;
	}

	if(!_graphics->Initialize(true))
	{
		GetDebug().Print(LogVerbosity::Error, CHANNEL_ENGINE, DT_TEXT("Cannot initialize window"));
		return false;
	}

	_resourceManager = UniquePtr<ResourceManager>(new ResourceManager());
	if (!_resourceManager)
	{
		GetDebug().Print(LogVerbosity::Error, CHANNEL_ENGINE, DT_TEXT("Cannot create resource manager"));
		return false;
	}

	if (!_resourceManager->Initialize())
	{
		GetDebug().Print(LogVerbosity::Error, CHANNEL_ENGINE, DT_TEXT("Cannot initialize resource manager"));
		return false;
	}

	_globalTime = Time::Create();
	if(!_globalTime)
	{
		GetDebug().Print(LogVerbosity::Error, CHANNEL_ENGINE, DT_TEXT("Cannot create time"));
		return false;
	}

	_globalTime->Initialize();

	_game = UniquePtr<Game>(new Game());
	if(!_game)
	{
		GetDebug().Print(LogVerbosity::Error, CHANNEL_ENGINE, DT_TEXT("Cannot create game"));
		return false;
	}

	if(!_game->Initialize())
	{
		GetDebug().Print(LogVerbosity::Error, CHANNEL_ENGINE, DT_TEXT("Cannot initialize game"));
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

	if(_graphics)
	{
		_graphics->Shutdown();
	}

	if(_resourceManager)
	{
		_resourceManager->Shutdown();
	}

	if(_mainWindow)
	{
		_mainWindow->Hide();
		_mainWindow->Close();
	}

	if(_debug)
	{
		_debug->Shutdown();
	}

	if(_input)
	{
		_input->Shutdown();
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
