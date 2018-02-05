#include "Component.h"

#include "Entity.h"

Component::Component(SharedPtr<Entity> owner) : EnableSharedFromThis<Component>(), _owner(owner), _enabled(true)
{

}

Component::Component(const Component& other) : EnableSharedFromThis<Component>(), _owner(other._owner), _enabled(other._enabled)
{

}

Component::~Component()
{

}

SharedPtr<Component>Component::Copy(SharedPtr<Entity> newOwner) const
{
	SharedPtr<Component> copy = SharedPtr<Component>(new Component(*this));
	copy->_owner = newOwner;
	return copy;
}

void Component::OnInitialize()
{

}

void Component::OnShutdown()
{

}

void Component::OnOwnerEnableChanged(bool enabled)
{

}

void Component::Load(Archive& archive)
{

}

void Component::Save(Archive& archive)
{

}

void Component::OnOwnerTransformUpdated(const Transform& transform)
{

}

void Component::OnUpdate(float deltaTime)
{

}

void Component::OnRender(Graphics& graphics)
{

}

void Component::OnEnableChanged(bool enabled)
{

}

SharedPtr<Entity> Component::GetOwner() const
{
	return _owner;
}

bool Component::IsEnabled() const
{
	return _enabled;
}

void Component::SetEnabled(bool enabled)
{
	_enabled = enabled;
	OnEnableChanged(_enabled);
}
