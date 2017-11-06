#pragma once

#include "Core/Platform.h"
#include "GameObject.h"

class Camera;

class Scene
{
protected:
	string _scenePath;

	std::vector<GameObject*> _gameObjects;
	std::vector<GameObject*> _newGameObjects;

public:
	Scene(const string& scenePath);
	~Scene();

	void Load();
	void Save();
	void Unload();

	void Update(float32 deltaTime);
	void Render();

	GameObject* SpawnObject(const string& name);
	GameObject* SpawnObject(const GameObject& original);
	GameObject* SpawnObject(const GameObject& original, const string& name);
};