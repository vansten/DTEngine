#pragma once

#include "Core/Platform.h"
#include "Rendering/Graphics.h"

#include "Scene.h"

class Game
{
protected:
	Scene* _activeScene;

public:
	Game();
	virtual ~Game() { }

	virtual bool Initialize();
	virtual void Shutdown();

	virtual void Update(float32 deltaTime);
	virtual void Render(Graphics* graphics);

	Scene* GetActiveScene() const;
};

extern Game* gGame;