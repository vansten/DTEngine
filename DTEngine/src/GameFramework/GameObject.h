#pragma once

#include <vector>

#include "Core/Archive.h"
#include "Core/Platform.h"
#include "Utility/Math.h"

#include "Component.h"

struct Transform
{
	friend class GameObject;

protected:
	std::vector<Transform*> _children;
	Transform* _parent;
	XMMATRIX _modelMatrix;
	GameObject* _owner;

	XMFLOAT3 _position;
	XMFLOAT3 _rotation;
	XMFLOAT3 _scale;

	bool _shouldCalculateMatrix;

public:
	inline Transform(GameObject* owner) : _owner(owner), _position(0.0f, 0.0f, 0.0f), _rotation(0.0f, 0.0f, 0.0f), _scale(1.0f, 1.0f, 1.0f), _parent(nullptr), _shouldCalculateMatrix(false)
	{

	}

	inline Transform(const Transform& other) : _owner(other._owner), _position(other._position), _rotation(other._rotation), _scale(other._scale), _parent(other._parent), _shouldCalculateMatrix(false)
	{

	}
	
protected:
	void CalculateModelMatrix();

public:
	void SetParent(Transform* newParent);

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

class GameObject
{
protected:
	string _name;
	bool _enabled;

	std::vector<Component*> _components;
	std::vector<Component*> _newComponents;
	std::vector<Component*> _componentsToRemove;

	Transform _transform;

	bool _isInUpdate;

public:
	GameObject();
	GameObject(const string& name);
	GameObject(const GameObject& other);

	void Initialize();
	void Shutdown();
	void Load(Archive* archive);
	void Save(Archive* archive);
	void PostLoad();
	void PreSave();

	void Update(float32 deltaTime);
	void Render();

	const string& GetName() const;
	void SetName(const string& name);

	bool IsEnabled() const;
	void SetEnabled(bool enabled);

	Transform& GetTransform();

	template<typename T>
	inline T* AddComponent();
	template<typename T>
	inline T* GetComponent();
	void RemoveComponent(Component* component);
};

template<typename T>
inline T* GameObject::AddComponent()
{
	T* newComponent = new T(this);
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
inline T* GameObject::GetComponent()
{
	T* component = nullptr;
	
	std::vector<Component*>::iterator& it = _components.begin();
	std::vector<Component*>::iterator& end = _components.end();
	for (it; it != end; ++it)
	{
		component = dynamic_cast<T*>(*it);
		if (component)
		{
			return component;
		}
	}

	return component;
}