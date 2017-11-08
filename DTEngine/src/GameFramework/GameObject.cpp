#include "GameObject.h"

#include "Rendering/Graphics.h"
#include "Game.h"

void Transform::CalculateModelMatrix()
{
	XMFLOAT3 radianRotation = XMFLOAT3(XMConvertToRadians(_rotation.x), XMConvertToRadians(_rotation.y), XMConvertToRadians(_rotation.z));
	_modelMatrix =	XMMatrixScalingFromVector(XMLoadFloat3(&_scale)) * 
					XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&radianRotation)) *
					XMMatrixTranslationFromVector(XMLoadFloat3(&_position));
	XMMATRIX parentMatrix = XMMatrixIdentity();

	SharedPtr<Transform> parentShared = _parent.lock();
	
	if (parentShared != nullptr)
	{
		parentMatrix = parentShared->GetModelMatrix();
	}
	_modelMatrix = XMMatrixMultiply(_modelMatrix, parentMatrix);

	SharedPtr<GameObject> ownerShared = _owner.lock();
	if(ownerShared)
	{
		ownerShared->OnTransformUpdated();
	}

	for (auto child : _children)
	{
		SharedPtr<Transform> childShared = child.lock();
		if(childShared)
		{
			childShared->CalculateModelMatrix();
		}
	}

	_shouldCalculateMatrix = false;
}

void Transform::SetParent(SharedPtr<Transform> newParent)
{
	SharedPtr<Transform> sharedFromThis = shared_from_this();
	SharedPtr<Transform> parentShared = _parent.lock();
	if (parentShared)
	{
		SharedPtr<Transform> p = parentShared;
		while (p)
		{
			_position.x += p->_position.x; _position.y += p->_position.y; _position.z += p->_position.z;
			_rotation.x += p->_rotation.x; _rotation.y += p->_rotation.y; _rotation.z += p->_rotation.z;
			_scale.x *= p->_scale.x; _scale.y *= p->_scale.y; _scale.z *= p->_scale.z;
			p = p->_parent.lock();
		}

		auto it = std::find(parentShared->_children.begin(), parentShared->_children.end(), sharedFromThis);
		if (it != parentShared->_children.end())
		{
			parentShared->_children.erase(it);
			parentShared = nullptr;
		}
	}
	_parent = newParent;
	parentShared = _parent.lock();
	if (parentShared)
	{
		// Add to new parent
		parentShared->_children.push_back(sharedFromThis);
		SharedPtr<Transform> p = parentShared;
		while (p)
		{
			_position.x -= p->_position.x; _position.y -= p->_position.y; _position.z -= p->_position.z;
			_rotation.x -= p->_rotation.x; _rotation.y -= p->_rotation.y; _rotation.z -= p->_rotation.z;
			_scale.x /= p->_scale.x; _scale.y /= p->_scale.y; _scale.z /= p->_scale.z;
			p = p->_parent.lock();
		}
	}

	_shouldCalculateMatrix = true;
}

GameObject::GameObject() : std::enable_shared_from_this<GameObject>(), _transform(nullptr), _name(DT_TEXT("NewObject")), _enabled(true), _isInUpdate(false)
{

}

GameObject::GameObject(const String& name) : std::enable_shared_from_this<GameObject>(), _transform(nullptr), _name(name), _enabled(true), _isInUpdate(false)
{

}

GameObject::GameObject(const GameObject& other) : std::enable_shared_from_this<GameObject>(), _transform(new Transform(*(other._transform))), _name(other._name), _enabled(other._enabled), _isInUpdate(false)
{
}

std::shared_ptr<GameObject> GameObject::Copy() const
{
	SharedPtr<GameObject> copy = SharedPtr<GameObject>(new GameObject(GetName() + DT_TEXT(" (copy)")));
	copy->_transform = SharedPtr<Transform>(new Transform(*_transform));

	copy->_transform->_owner = copy;
	auto it = _components.begin();
	auto end = _components.end();
	for(it; it != end; ++it)
	{
		copy->_components.push_back((*it)->Copy(copy));
	}

	Game& game = GetGame();
	for(auto child : _transform->_children)
	{
		SharedPtr<Transform> childShared = child.lock();
		if(childShared)
		{
			SharedPtr<GameObject> childGO = childShared->_owner.lock();
			SharedPtr<GameObject> newChildGO = game.GetActiveScene()->SpawnObject(childGO);
			newChildGO->GetTransform()->SetParent(copy->_transform);
		}
	}

	return copy;
}

void GameObject::Initialize()
{
	if(!_transform)
	{
		_transform = SharedPtr<Transform>(new Transform(shared_from_this()));
	}

	_transform->CalculateModelMatrix();

	for (auto component : _components)
	{
		component->Initialize();
	}
}

void GameObject::Shutdown()
{
	for (auto component : _components)
	{
		component->Shutdown();
	}
	_components.clear();

	_transform = nullptr;
}

void GameObject::Load(Archive& archive)
{
	for (auto component : _components)
	{
		component->Load(archive);
	}
}

void GameObject::Save(Archive& archive)
{
	for (auto component : _components)
	{
		component->Save(archive);
	}
}

void GameObject::PostLoad()
{
	for (auto component : _components)
	{
		component->PostLoad();
	}
}

void GameObject::PreSave()
{
	for (auto component : _components)
	{
		component->PreSave();
	}
}

void GameObject::Update(float32 deltaTime)
{
	if(!_transform)
	{
		return;
	}

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

	if (_transform->_shouldCalculateMatrix)
	{
		_transform->CalculateModelMatrix();
	}
	
	for (auto component : _components)
	{
		if (component->IsEnabled())
		{
			component->Update(deltaTime);
		}
	}

	_isInUpdate = false;
}

void GameObject::Render(Graphics& graphics)
{
	graphics.SetObject(shared_from_this());
	for (auto component : _components)
	{
		if (component->IsEnabled())
		{
			component->Render(graphics);
		}
	}
}

void GameObject::OnTransformUpdated()
{
	for(auto component : _components)
	{
		component->OnOwnerTransformUpdated(_transform);
	}
}

void GameObject::SetEnabled(bool enabled)
{
	_enabled = enabled;

	if(_enabled)
	{
		// Owner enabled in this call, notify components
		for(auto component : _components)
		{
			component->OnOwnerEnabled();
		}
	}
	else
	{
		// Owner disabled in this call, notify components
		for(auto component : _components)
		{
			component->OnOwnerDisabled();
		}
	}
}

bool GameObject::IsEnabledInHierarchy() const
{
	SharedPtr<Transform> parent = _transform->GetParent();
	while(parent)
	{
		if(!parent->GetOwner()->IsEnabled())
		{
			return false;
		}
		parent = parent->GetParent();
	}

	return IsEnabled();
}

SharedPtr<Transform> GameObject::GetTransform()
{
	return _transform;
}

void GameObject::RemoveComponent(SharedPtr<Component> component)
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
				component->Shutdown();
				_components.erase(it);
				break;
			}
		}
	}
}