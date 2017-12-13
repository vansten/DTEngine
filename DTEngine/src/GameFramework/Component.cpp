#include "Component.h"

#include "GameObject.h"

Component::Component(SharedPtr<GameObject> owner) : EnableSharedFromThis<Component>(), _owner(owner), _enabled(true)
{

}

Component::Component(const Component& other) : EnableSharedFromThis<Component>(), _owner(other._owner), _enabled(other._enabled)
{

}

Component::~Component()
{

}

SharedPtr<Component>Component::Copy(SharedPtr<GameObject> newOwner) const
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

void Component::PostLoad()
{

}

void Component::PreSave()
{

}

void Component::OnOwnerTransformUpdated(SharedPtr<Transform> transform)
{

}

void Component::OnUpdate(float32 deltaTime)
{

}

void Component::OnRender(Graphics& graphics)
{

}

void Component::OnEnableChanged(bool enabled)
{

}

SharedPtr<GameObject> Component::GetOwner() const
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
