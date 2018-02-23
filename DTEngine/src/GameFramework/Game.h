#pragma once

#include "Core/App.h"
#include "Core/Platform.h"
#include "Rendering/Graphics.h"

#include "Scene.h"

class Game
{
protected:
	UniquePtr<Scene> _activeScene;

public:
	Game();
	virtual ~Game()
	{}

	virtual bool Initialize();
	virtual void Shutdown();

	virtual void Update(float deltaTime);
	virtual void Render(Graphics& graphics);

	const UniquePtr<Scene>& GetActiveScene() const
	{
		return _activeScene;
	}
};
