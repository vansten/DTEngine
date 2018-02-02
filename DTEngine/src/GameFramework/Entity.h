#pragma once

#include <vector>

#include "Core/Archive.h"
#include "Core/Platform.h"
#include "Utility/Math.h"

#include "Component.h"
#include "Rendering/Graphics.h"
#include "Transform.h"

class Entity : public EnableSharedFromThis<Entity>
{
private:
	String _name;
	bool _enabled;

	DynamicArray<SharedPtr<Component>> _components;
	DynamicArray<SharedPtr<Component>> _newComponents;
	DynamicArray<SharedPtr<Component>> _componentsToRemove;

	DynamicArray<SharedPtr<Entity>> _children;
	SharedPtr<Entity> _parent;

	Transform _transform;

	bool _isInUpdate;

public:
	Entity();
	Entity(const String& name);
	Entity(const Entity& other);

	DECLARE_SHARED_FROM_THIS(Entity)

	inline void AddChild(SharedPtr<Entity> entity)
	{
		if(std::find(_children.begin(), _children.end(), entity) == _children.end())
		{
			_children.push_back(entity);
		}
	}

	inline void RemoveChild(SharedPtr<Entity> entity)
	{
		auto& childIterator = std::find(_children.begin(), _children.end(), entity);
		if(childIterator != _children.end())
		{
			_children.erase(childIterator);
		}
	}

public:
	SharedPtr<Entity> Copy() const;

	void Initialize();
	void Shutdown();
	void Load(Archive& archive);
	void Save(Archive& archive);
	void PostLoad();
	void PreSave();

	void Update(float deltaTime);

	void Render(Graphics& graphics);

	void OnTransformUpdated();
	void SetEnabled(bool enabled);

	bool IsEnabledInHierarchy() const;

	inline const String& GetName() const { return _name; }
	inline bool IsEnabled() const { return _enabled; }

	inline void SetName(const String& name) { _name = name; }

	inline Transform& GetTransform() { return _transform; }
	inline const Transform& GetTransform() const { return _transform; }
	inline void SetTransform(const Transform& transform) { _transform = transform; }

	inline const XMFLOAT3& GetPosition() const { return _transform._position; }
	inline const XMFLOAT3& GetRotation() const { return _transform._rotation; }
	inline const XMFLOAT3& GetScale() const { return _transform._scale; }

	inline void SetPosition(const XMFLOAT3& position) { _transform.SetPosition(position); }
	inline void SetRotation(const XMFLOAT3& rotation) { _transform.SetRotation(rotation); }
	inline void SetScale(const XMFLOAT3& scale) { _transform.SetScale(scale); }

	inline const DynamicArray<SharedPtr<Entity>>& GetChildren() const { return _children; }
	inline size_t GetChildrenCount() const { return (size_t)_children.size(); }
	inline SharedPtr<Entity> GetChildAt(size_t index) const
	{
		if(_children.size() == 0 || index > _children.size())
		{
			return nullptr;
		}

		return _children[index];
	}

	inline void SetParent(SharedPtr<Entity> entity)
	{
		if(_parent)
		{
			_parent->RemoveChild(SharedFromThis());
		}
		_parent = entity;
		if(_parent)
		{
			_parent->AddChild(SharedFromThis());
		}
	}

	inline void SetParent(SharedPtr<const Entity> entity)
	{
		SetParent(ConstPointerCast<Entity>(entity));
	}

	template<typename T>
	inline SharedPtr<T> AddComponent();
	template<typename T>
	inline SharedPtr<T> GetComponent();
	void RemoveComponent(SharedPtr<Component> component);
};

template<typename T>
inline SharedPtr<T> Entity::AddComponent()
{
	SharedPtr<T> newComponent = SharedPtr<T>(new T(SharedFromThis()));
	if (_isInUpdate)
	{
		_newComponents.push_back(newComponent);
	}
	else
	{
		_components.push_back(newComponent);
	}

	// Initialize new component when created (do not defer this)
	newComponent->OnInitialize();
	newComponent->PostLoad();
	return newComponent;
}

template<typename T>
inline SharedPtr<T> Entity::GetComponent()
{
	SharedPtr<T> component = nullptr;
	
	for (auto comp : _components)
	{
		component = DynamicPointerCast<T>(comp);
		if (component)
		{
			return component;
		}
	}

	return component;
}