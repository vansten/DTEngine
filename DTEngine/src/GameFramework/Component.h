#pragma once

#include "Core/Platform.h"

class Archive;
class GameObject;
struct Transform;

class Component
{
protected:
	GameObject* _owner;
	bool _enabled;

public:
	Component(GameObject* owner);
	Component(const Component& other);
	virtual ~Component();

	virtual void Initialize();
	virtual void Shutdown();
	virtual void Load(Archive* archive);
	virtual void Save(Archive* archive);
	virtual void PostLoad();
	virtual void PreSave();

	virtual void OnOwnerTransformUpdated(const Transform& transform);
	virtual void Update(float32 deltaTime);
	virtual void Render();

	GameObject* GetOwner() const;

	bool IsEnabled() const;
	void SetEnabled(bool enabled);
};