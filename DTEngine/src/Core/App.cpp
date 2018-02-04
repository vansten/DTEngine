#include "App.h"

#include "MessageSystem.h"
#include "Window.h"
#include "Time.h"
#include "Input.h"

#include "Debug/Debug.h"

#include "Rendering/Graphics.h"

#include "ResourceManagement/Resources.h"

#include "GameFramework/Game.h"

App::App()
{

}

App::~App()
{

}

bool App::Initialize(UniquePtr<Game>&& game)
{
	_isRunning = false;

	if(!gDebug.Initialize())
	{
		return false;
	}

	if(!gWindow.Open(DT_TEXT("DT Engine"), 1024, 768))
	{
		gDebug.Print(LogVerbosity::Error, CHANNEL_ENGINE, DT_TEXT("Cannot open window"));
		return false;
	}

	if(!gWindow.Show())
	{
		gDebug.Print(LogVerbosity::Error, CHANNEL_ENGINE, DT_TEXT("Cannot show window"));
		return false;
	}

	if(!gGraphics.Initialize(true))
	{
		gDebug.Print(LogVerbosity::Error, CHANNEL_ENGINE, DT_TEXT("Cannot initialize window"));
		return false;
	}

	if(!gResources.Initialize())
	{
		gDebug.Print(LogVerbosity::Error, CHANNEL_ENGINE, DT_TEXT("Cannot initialize resource manager"));
		return false;
	}

	gDebug.InitializeDraws();

	gTime.Initialize();

	_game = std::move(game);
	if(!_game)
	{
		gDebug.Print(LogVerbosity::Error, CHANNEL_ENGINE, DT_TEXT("Cannot create game"));
		return false;
	}

	if(!_game->Initialize())
	{
		gDebug.Print(LogVerbosity::Error, CHANNEL_ENGINE, DT_TEXT("Cannot initialize game"));
		return false;
	}

	return true;
}

void App::Loop()
{
	_isRunning = true;
	float deltaTime = 1.0f / 60.0f;
	float timer = 0.0f;
	unsigned int frames = 0;

	while(!MessageSystem::IsPendingQuit())
	{
		MessageSystem::GatherMessages();

		gDebug.Update(deltaTime);

		// Physics::Resolve();

		_game->Update(deltaTime);

		_game->Render(gGraphics);

		gTime.Tick();
		deltaTime = gTime.GetDeltaTime();
		timer += deltaTime;
		frames += 1;

		if(timer > 1.0f)
		{
			float fps = frames / timer;
			timer = 0.0f;
			frames = 0;

			gDebug.Printf(LogVerbosity::Log, CHANNEL_GENERAL, DT_TEXT("Current FPS: %.3f"), fps);
		}
	}

	_isRunning = false;
}

void App::Shutdown()
{
	if (_game)
	{
		_game->Shutdown();
	}

	gGraphics.Shutdown();
	gResources.Shutdown();
	
	gWindow.Hide();
	gWindow.Close();

	gDebug.Shutdown();
	gInput.Shutdown();
}

int App::Run(UniquePtr<Game>&& game)
{
	if(!Initialize(std::forward<UniquePtr<Game>>(game)))
	{
		return APP_INITIALIZATION_FAILED;
	}

	Loop();

	Shutdown();

	return APP_SUCCESSFUL;
}
