#pragma once

#include "Core/Platform.h"
#include "GameObject.h"
#include "Rendering/Graphics.h"

class Camera;

class Scene
{
protected:
	String _scenePath;

	DynamicArray<SharedPtr<GameObject>> _gameObjects;
	DynamicArray<SharedPtr<GameObject>> _newGameObjects;

public:
	Scene(const String& scenePath);
	~Scene();

	void Load();
	void Save();
	void Unload();

	void Update(float32 deltaTime);
	void Render(Graphics& graphics);

	SharedPtr<GameObject> SpawnObject(const String& name);
	SharedPtr<GameObject> SpawnObject(SharedPtr<GameObject> original);
	SharedPtr<GameObject> SpawnObject(SharedPtr<GameObject> original, const String& name);
};