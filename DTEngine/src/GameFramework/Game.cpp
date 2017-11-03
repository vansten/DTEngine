#include "Game.h"

Game* gGame = nullptr;

Game::Game() : _activeScene(0)
{
	gGame = this;
}

bool Game::Initialize()
{
	_activeScene = new Scene(DT_TEXT(""));
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
		delete _activeScene;
		_activeScene = 0;
	}
}

void Game::Update(float32 deltaTime)
{
	_activeScene->Update(deltaTime);
}

void Game::Render(Graphics* graphics)
{
	graphics->BeginScene();

	_activeScene->Render();

	graphics->EndScene();
}

Scene* Game::GetActiveScene() const
{
	return _activeScene;
}
