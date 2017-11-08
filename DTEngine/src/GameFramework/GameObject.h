#pragma once

#include <vector>

#include "Core/Archive.h"
#include "Core/Platform.h"
#include "Utility/Math.h"

#include "Component.h"
#include "Rendering/Graphics.h"

struct Transform : public EnableSharedFromThis<Transform>
{
	friend class GameObject;

protected:
	DynamicArray<WeakPtr<Transform>> _children;
	WeakPtr<Transform> _parent;
	XMMATRIX _modelMatrix;
	WeakPtr<GameObject> _owner;

	XMFLOAT3 _position;
	XMFLOAT3 _rotation;
	XMFLOAT3 _scale;

	bool _shouldCalculateMatrix;

public:
	inline Transform(SharedPtr<GameObject> owner) : EnableSharedFromThis<Transform>(), _owner(owner), _position(0.0f, 0.0f, 0.0f), _rotation(0.0f, 0.0f, 0.0f), _scale(1.0f, 1.0f, 1.0f), _parent(), _shouldCalculateMatrix(false)
	{

	}

	inline Transform(const Transform& other) : EnableSharedFromThis<Transform>(), _owner(other._owner), _position(other._position), _rotation(other._rotation), _scale(other._scale), _parent(other._parent), _shouldCalculateMatrix(false)
	{

	}
	
protected:
	void CalculateModelMatrix();

public:
	void SetParent(SharedPtr<Transform> newParent);
	
	inline void SetPosition(const XMFLOAT3& newPosition)
	{
		_position = newPosition;
		_shouldCalculateMatrix = true;
	}

	inline void SetRotation(const XMFLOAT3& newRotation)
	{
		_rotation = newRotation;
		_shouldCalculateMatrix = true;
	}

	inline void SetScale(const XMFLOAT3& newScale)
	{
		_scale = newScale;
		_shouldCalculateMatrix = true;
	}

	inline SharedPtr<GameObject> GetOwner() const
	{
		return _owner.lock();
	}

	inline SharedPtr<Transform> GetParent() const
	{
		return _parent.lock();
	}

	inline const DynamicArray<WeakPtr<Transform>>& GetChildren() const
	{
		return _children;
	}

	inline const XMFLOAT3& GetPosition() const
	{
		return _position;
	}

	inline const XMFLOAT3& GetRotation() const
	{
		return _rotation;
	}

	inline const XMFLOAT3& GetScale() const
	{
		return _scale;
	}

	inline const XMMATRIX& GetModelMatrix() const
	{
		return _modelMatrix;
	}

	inline XMFLOAT3 TransformDirection(XMFLOAT3 direction) const
	{
		XMFLOAT4 direction4(direction.x, direction.y, direction.z, 0.0f);
		XMVECTOR directionVector = XMLoadFloat4(&direction4);
		XMVECTOR transformedDirectionVector = XMVector4Transform(directionVector, _modelMatrix);
		XMFLOAT3 transformedDirection;
		XMStoreFloat3(&transformedDirection, transformedDirectionVector);
		return transformedDirection;
	}

	inline XMFLOAT3 GetForward() const
	{
		return TransformDirection(VectorHelpers::Forward);
	}

	inline XMFLOAT3 GetUp() const
	{
		return TransformDirection(VectorHelpers::Up);
	}

	inline XMFLOAT3 GetRight() const
	{
		return TransformDirection(VectorHelpers::Right);
	}
};

class GameObject : public EnableSharedFromThis<GameObject>
{
protected:
	String _name;
	bool _enabled;

	DynamicArray<SharedPtr<Component>> _components;
	DynamicArray<SharedPtr<Component>> _newComponents;
	DynamicArray<SharedPtr<Component>> _componentsToRemove;

	SharedPtr<Transform> _transform;

	bool _isInUpdate;

public:
	GameObject();
	GameObject(const String& name);
	GameObject(const GameObject& other);

	std::shared_ptr<GameObject> Copy() const;

	void Initialize();
	void Shutdown();
	void Load(Archive& archive);
	void Save(Archive& archive);
	void PostLoad();
	void PreSave();

	void Update(float32 deltaTime);
	void Render(Graphics& graphics);

	void OnTransformUpdated();
	void SetEnabled(bool enabled);

	bool IsEnabledInHierarchy() const;

	inline const String& GetName() const { return _name; }
	inline bool IsEnabled() const { return _enabled; }

	inline void SetName(const String& name) { _name = name; }

	SharedPtr<Transform> GetTransform();

	template<typename T>
	inline SharedPtr<T> AddComponent();
	template<typename T>
	inline SharedPtr<T> GetComponent();
	void RemoveComponent(SharedPtr<Component> component);
};

template<typename T>
inline SharedPtr<T> GameObject::AddComponent()
{
	SharedPtr<T> newComponent = SharedPtr<T>(new T(shared_from_this()));
	if (_isInUpdate)
	{
		_newComponents.push_back(newComponent);
	}
	else
	{
		_components.push_back(newComponent);
	}

	// Initialize new component when created (do not defer this)
	newComponent->Initialize();
	newComponent->PostLoad();
	return newComponent;
}

template<typename T>
inline SharedPtr<T> GameObject::GetComponent()
{
	SharedPtr<T> component = nullptr;
	
	for (auto comp : _components)
	{
		component = std::dynamic_pointer_cast<T>(comp);
		if (component)
		{
			return component;
		}
	}

	return component;
}