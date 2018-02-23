#pragma once

#include "Core/LayerManager.h"
#include "Core/Platform.h"
#include "Rendering/Graphics.h"
#include "Transform.h"

class Archive;
class Entity;

#define DECLARE_SHARED_FROM_THIS(Type) \
private: \
SharedPtr<Type> SharedFromThis() \
{ \
	return StaticPointerCast<Type>(shared_from_this()); \
} \
SharedPtr<const Type> SharedFromThis() const \
{ \
	return StaticPointerCast<const Type>(shared_from_this()); \
} \
private:

class Component : public EnableSharedFromThis<Component>
{
	friend class Entity;

protected:
	SharedPtr<Entity> _owner;
	bool _enabled;

public:
	Component(SharedPtr<Entity> owner);
	Component(const Component& other);
	virtual ~Component();

	DECLARE_SHARED_FROM_THIS(Component)

protected:
	virtual SharedPtr<Component> Copy(SharedPtr<Entity> newOwner) const;

public:
	// Called on:
	// 1) Entity::Initialize, after all components are constructed 
	//		(Entity::Initialize is called after loading all entities and its components if called from Scene::Load)
	// 2) Entity::AddComponent, after component is constructed
	// References should be used carefully (referenced entities/components may not be properly initialized on this step)
	virtual void OnInitialize();
	// Called on Entity::Shutdown
	virtual void OnShutdown();

	virtual void OnOwnerEnableChanged(bool enabled);

	virtual void Load(Archive& archive);
	virtual void Save(Archive& archive);

	virtual void OnOwnerTransformUpdated(const Transform& transform);
	virtual void OnUpdate(float deltaTime);
	virtual void OnRender(Graphics& graphics);

	virtual void OnEnableChanged(bool enabled);

	SharedPtr<Entity> GetOwner() const;

	bool IsEnabled() const;
	void SetEnabled(bool enabled);
};