#pragma once

#include "Core/Platform.h"
#include "Rendering/Graphics.h"

#include "Scene.h"

class Game
{
protected:
	UniquePtr<Scene> _activeScene;

public:
	Game();
	virtual ~Game() { }

	virtual bool Initialize();
	virtual void Shutdown();

	virtual void Update(float32 deltaTime);
	virtual void Render(Graphics& graphics);

	const UniquePtr<Scene>& GetActiveScene() const { return _activeScene; }
};
