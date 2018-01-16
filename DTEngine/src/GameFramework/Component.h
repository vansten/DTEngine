#pragma once

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
	virtual void OnInitialize();
	virtual void OnShutdown();

	virtual void OnOwnerEnableChanged(bool enabled);

	virtual void Load(Archive& archive);
	virtual void Save(Archive& archive);
	virtual void PostLoad();
	virtual void PreSave();

	virtual void OnOwnerTransformUpdated(const Transform& transform);
	virtual void OnUpdate(float32 deltaTime);
	virtual void OnRender(Graphics& graphics);

	virtual void OnEnableChanged(bool enabled);

	SharedPtr<Entity> GetOwner() const;

	bool IsEnabled() const;
	void SetEnabled(bool enabled);
};