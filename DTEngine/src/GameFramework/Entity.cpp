#include "Entity.h"

#include "Debug/Debug.h"
#include "Rendering/Graphics.h"
#include "Game.h"

Entity::Entity() : EnableSharedFromThis<Entity>(), _name(DT_TEXT("NewObject")), _enabled(true), _isInUpdate(false)
{

}

Entity::Entity(const String& name) : EnableSharedFromThis<Entity>(), _name(name), _enabled(true), _isInUpdate(false)
{

}

Entity::Entity(const Entity& other) : EnableSharedFromThis<Entity>(), _transform(other._transform), _name(other._name), _enabled(other._enabled), _isInUpdate(false)
{
}

SharedPtr<Entity> Entity::Copy() const
{
	SharedPtr<Entity> copy = SharedPtr<Entity>(new Entity(GetName() + DT_TEXT(" (copy)")));
	copy->_transform = _transform;

	auto it = _components.begin();
	auto end = _components.end();
	for(it; it != end; ++it)
	{
		copy->_components.push_back((*it)->Copy(copy));
	}

	Game& game = GetGame();

	for(auto child : _children)
	{
		SharedPtr<Entity> newChildEntity = game.GetActiveScene()->SpawnEntity(child);
		newChildEntity->SetParent(SharedFromThis());
	}

	return copy;
}

void Entity::Initialize()
{
	_transform.CalculateModelMatrix(_parent ? &(_parent->_transform) : nullptr);

	for (auto component : _components)
	{
		component->OnInitialize();
	}
}

void Entity::Shutdown()
{
	for (auto component : _components)
	{
		component->OnShutdown();
	}
	_components.clear();
	_children.clear();
	_parent = nullptr;
}

void Entity::Load(Archive& archive)
{
	for (auto component : _components)
	{
		component->Load(archive);
	}
}

void Entity::Save(Archive& archive)
{
	for (auto component : _components)
	{
		component->Save(archive);
	}
}

void Entity::PostLoad()
{
	for (auto component : _components)
	{
		component->PostLoad();
	}
}

void Entity::PreSave()
{
	for (auto component : _components)
	{
		component->PreSave();
	}
}

void Entity::Update(float deltaTime)
{
	for (auto component : _componentsToRemove)
	{
		RemoveComponent(component);
	}
	_componentsToRemove.clear();

	for (auto component : _newComponents)
	{
		_components.push_back(component);
	}
	_newComponents.clear();

	_isInUpdate = true;

	if (_transform._shouldCalculateMatrix)
	{
		_transform.CalculateModelMatrix(_parent ? &(_parent->_transform) : nullptr);
		OnTransformUpdated();
	}
	
	for (auto component : _components)
	{
		if (component->IsEnabled())
		{
			component->OnUpdate(deltaTime);
		}
	}

	_isInUpdate = false;
}

void Entity::Render(Graphics& graphics)
{
	graphics.SetObject(this);
	for(auto component : _components)
	{
		component->OnRender(graphics);
	}
}

void Entity::OnTransformUpdated()
{
	for(auto component : _components)
	{
		component->OnOwnerTransformUpdated(_transform);
	}

	for(auto entity : _children)
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
	for(auto component : _components)
	{
		component->OnOwnerEnableChanged(_enabled);
	}
}

bool Entity::IsEnabledInHierarchy() const
{
	if(!IsEnabled())
	{
		return false;
	}

	SharedPtr<Entity> parent = _parent;
	
	if(parent)
	{
		return parent->IsEnabledInHierarchy();
	}

	return true;
}

void Entity::RemoveComponent(SharedPtr<Component> component)
{
	if (_isInUpdate)
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