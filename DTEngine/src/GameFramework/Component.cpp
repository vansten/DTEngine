#include "Component.h"

#include "GameObject.h"

Component::Component(GameObject* owner) : _owner(owner), _enabled(true)
{

}

Component::Component(const Component& other) : _owner(other._owner), _enabled(other._enabled)
{

}

Component::~Component()
{

}

Component* Component::Copy(GameObject* newOwner) const
{
	Component* copy = new Component(*this);
	copy->_owner = newOwner;
	return copy;
}

void Component::Initialize()
{

}

void Component::Shutdown()
{

}

void Component::Load(Archive* archive)
{

}

void Component::Save(Archive* archive)
{

}

void Component::PostLoad()
{

}

void Component::PreSave()
{

}

void Component::OnOwnerTransformUpdated(const Transform& transform)
{

}

void Component::Update(float32 deltaTime)
{

}

void Component::Render()
{

}

GameObject* Component::GetOwner() const
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
}
