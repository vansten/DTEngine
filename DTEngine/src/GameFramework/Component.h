#pragma once

#include "Core/Platform.h"
#include "Rendering/Graphics.h"

class Archive;
class GameObject;
struct Transform;

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
	friend class GameObject;

protected:
	SharedPtr<GameObject> _owner;
	bool _enabled;

public:
	Component(SharedPtr<GameObject> owner);
	Component(const Component& other);
	virtual ~Component();

	DECLARE_SHARED_FROM_THIS(Component)

protected:
	virtual SharedPtr<Component> Copy(SharedPtr<GameObject> newOwner) const;

public:
	virtual void Initialize();
	virtual void Shutdown();

	virtual void OnOwnerEnabled();
	virtual void OnOwnerDisabled();

	virtual void Load(Archive& archive);
	virtual void Save(Archive& archive);
	virtual void PostLoad();
	virtual void PreSave();

	virtual void OnOwnerTransformUpdated(SharedPtr<Transform> transform);
	virtual void Update(float32 deltaTime);
	virtual void Render(Graphics& graphics);

	SharedPtr<GameObject> GetOwner() const;

	bool IsEnabled() const;
	void SetEnabled(bool enabled);
};