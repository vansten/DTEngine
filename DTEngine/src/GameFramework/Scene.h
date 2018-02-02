#pragma once

#include "Core/Platform.h"
#include "Entity.h"
#include "Rendering/Graphics.h"

class Camera;

class Scene
{
protected:
	String _scenePath;

	DynamicArray<SharedPtr<Entity>> _entities;
	DynamicArray<SharedPtr<Entity>> _newEntities;

public:
	Scene(const String& scenePath);
	~Scene();

	void Load();
	void Save();
	void Unload();

	void Update(float deltaTime);
	void Render(Graphics& graphics);

	SharedPtr<Entity> SpawnEntity(const String& name);
	SharedPtr<Entity> SpawnEntity(SharedPtr<Entity> original);
	SharedPtr<Entity> SpawnEntity(SharedPtr<Entity> original, const String& name);
};