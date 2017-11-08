#include "Game.h"

Game::Game() : _activeScene(nullptr)
{

}

bool Game::Initialize()
{
	_activeScene = UniquePtr<Scene>(new Scene(DT_TEXT("Untitled")));
	if (!_activeScene)
	{
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