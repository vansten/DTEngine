#include "Entity.h"

#include "Debug/Debug.h"
#include "Rendering/Graphics.h"
#include "Game.h"

Entity::Entity() : EnableSharedFromThis<Entity>(), _name(DT_TEXT("NewObject")), _enabled(true), _layer(1)
{}

Entity::Entity(const String& name) : EnableSharedFromThis<Entity>(), _name(name), _enabled(true), _layer(1)
{}

Entity::Entity(const Entity& other) : EnableSharedFromThis<Entity>(), _transform(other._transform), _name(other._name), _enabled(other._enabled), _layer(other._layer)
{}

SharedPtr<Entity> Entity::Copy() const
{
	SharedPtr<Entity> copy = SharedPtr<Entity>(new Entity(*this));
	copy->_name += DT_TEXT(" (copy)");

	auto it = _components.begin();
	auto end = _components.end();
	for (it; it != end; ++it)
	{
		copy->_components.push_back((*it)->Copy(copy));
	}

	Game& game = GetGame();

	for (const auto& child : _children)
	{
		SharedPtr<Entity> newChildEntity = game.GetActiveScene()->SpawnEntity(child);
		newChildEntity->SetParent(copy);
	}

	return copy;
}

void Entity::Initialize()
{
	_transform.CalculateModelMatrix(_parent ? &(_parent->_transform) : nullptr);

	for (const auto& component : _components)
	{
		component->OnInitialize();
	}

	Flags.RaiseFlag(EntityFlag::INITIALIZED);
}

void Entity::Shutdown()
{
	Flags.RaiseFlag(EntityFlag::PENDING_DESTROY);

	for (const auto& component : _components)
	{
		component->OnShutdown();
	}

	_components.clear();
	_children.clear();
	_parent = nullptr;

	Flags.ClearFlag(EntityFlag::INITIALIZED);
}

void Entity::Load(Archive& archive)
{
	// for components in readByts
	//		Component* c = new ComponentType(SharedFromThis());
	//		_components.push_back(c);

	for (const auto& component : _components)
	{
		component->Load(archive);
	}
}

void Entity::Save(Archive& archive)
{
	for (const auto& component : _components)
	{
		component->Save(archive);
	}
}

void Entity::Update(float deltaTime)
{
	for (const auto& component : _componentsToRemove)
	{
		RemoveComponent(component);
	}
	_componentsToRemove.clear();

	for (const auto& component : _newComponents)
	{
		_components.push_back(component);
	}
	_newComponents.clear();

	Flags.RaiseFlag(EntityFlag::DURING_UPDATE);

	if (_transform._shouldCalculateMatrix)
	{
		_transform.CalculateModelMatrix(_parent ? &(_parent->_transform) : nullptr);
		OnTransformUpdated();
	}

	for (const auto& component : _components)
	{
		if (component->IsEnabled())
		{
			component->OnUpdate(deltaTime);
		}
	}

	Flags.ClearFlag(EntityFlag::DURING_UPDATE);
}

void Entity::Render(Graphics& graphics)
{
	graphics.SetObject(this);
	for (const auto& component : _components)
	{
		component->OnRender(graphics);
	}
}

void Entity::OnTransformUpdated()
{
	for (const auto& component : _components)
	{
		component->OnOwnerTransformUpdated(_transform);
	}

	for (const auto& entity : _children)
	{
		Transform& transform = entity->_transform;
		transform.CalculateModelMatrix(&_transform);
		entity->OnTransformUpdated();
	}
}

void Entity::SetEnabled(bool enabled)
{
	_enabled = enabled;

	// Notify all component that enabled property has changed
	for (const auto& component : _components)
	{
		component->OnOwnerEnableChanged(_enabled);
	}
}

bool Entity::IsEnabledInHierarchy() const
{
	if (!IsEnabled())
	{
		return false;
	}

	SharedPtr<Entity> parent = _parent;

	if (parent)
	{
		return parent->IsEnabledInHierarchy();
	}

	return true;
}

void Entity::RemoveComponent(SharedPtr<Component> component)
{
	if (Flags.IsFlagSet(EntityFlag::DURING_UPDATE))
	{
		_componentsToRemove.push_back(component);
	}
	else
	{
		DynamicArray<SharedPtr<Component>>::iterator& it = _components.begin();
		DynamicArray<SharedPtr<Component>>::iterator& end = _components.end();
		for (it; it != end; ++it)
		{
			if ((*it) == component)
			{
				component->OnShutdown();
				_components.erase(it);
				break;
			}
		}
	}
}