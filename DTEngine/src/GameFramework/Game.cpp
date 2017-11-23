#include "Game.h"

#include "Debug/Debug.h"

Game::Game() : _activeScene(nullptr)
{

}

bool Game::Initialize()
{
	_activeScene = UniquePtr<Scene>(new Scene(DT_TEXT("Untitled")));
	if (!_activeScene)
	{
		GetDebug().Print(LogVerbosity::Error, CHANNEL_ENGINE, DT_TEXT("Failed to create default scene"));
		return false;
	}

	_activeScene->Load();

	return true;
}

void Game::Shutdown()
{
	if (_activeScene)
	{
		_activeScene->Unload();
		_activeScene = nullptr;
	}
}

void Game::Update(float32 deltaTime)
{
	_activeScene->Update(deltaTime);
}

void Game::Render(Graphics& graphics)
{
	graphics.BeginScene();

	_activeScene->Render(graphics);

	graphics.EndScene();
}