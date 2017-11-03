#include "GameObject.h"

#include "Rendering/Graphics.h"

void Transform::CalculateModelMatrix()
{
	XMFLOAT3 radianRotation = XMFLOAT3(XMConvertToRadians(_rotation.x), XMConvertToRadians(_rotation.y), XMConvertToRadians(_rotation.z));
	_modelMatrix =	XMMatrixScalingFromVector(XMLoadFloat3(&_scale)) * 
					XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&radianRotation)) *
					XMMatrixTranslationFromVector(XMLoadFloat3(&_position));
	XMMATRIX parentMatrix = XMMatrixIdentity();
	if (_parent != nullptr)
	{
		parentMatrix = _parent->GetModelMatrix();
	}
	_modelMatrix = XMMatrixMultiply(_modelMatrix, parentMatrix);
	std::vector<Transform*>::iterator it = _children.begin();
	std::vector<Transform*>::iterator end = _children.end();
	for (it; it != end; ++it)
	{
		(*it)->CalculateModelMatrix();
	}

	_shouldCalculateMatrix = false;
}

void Transform::SetParent(Transform* newParent)
{
	if (_parent)
	{
		Transform* p = _parent;
		while (p)
		{
			_position.x += p->_position.x; _position.y += p->_position.y; _position.z += p->_position.z;
			_rotation.x += p->_rotation.x; _rotation.y += p->_rotation.y; _rotation.z += p->_rotation.z;
			_scale.x *= p->_scale.x; _scale.y *= p->_scale.y; _scale.z *= p->_scale.z;
			p = p->_parent;
		}
		std::vector<Transform*>::iterator it = std::find(_parent->_children.begin(), _parent->_children.end(), this);
		if (it != _parent->_children.end())
		{
			_parent->_children.erase(it);
			_parent = nullptr;
		}
	}
	_parent = newParent;
	if (_parent)
	{
		_parent->_children.push_back(this);
		Transform* p = _parent;
		while (p)
		{
			_position.x -= p->_position.x; _position.y -= p->_position.y; _position.z -= p->_position.z;
			_rotation.x -= p->_rotation.x; _rotation.y -= p->_rotation.y; _rotation.z -= p->_rotation.z;
			_scale.x /= p->_scale.x; _scale.y /= p->_scale.y; _scale.z /= p->_scale.z;
			p = p->_parent;
		}
	}
	CalculateModelMatrix();
}

GameObject::GameObject() : _name(DT_TEXT("NewObject")), _enabled(true), _isInUpdate(false)
{

}

GameObject::GameObject(const string& name) : _name(name), _enabled(true), _isInUpdate(false)
{

}

GameObject::GameObject(const GameObject& other) : _name(other._name), _enabled(other._enabled), _isInUpdate(false)
{
	_name += DT_TEXT(" (copy)");
	// TODO: Smart copy components array!
	// TODO: Smart copy transform
}

void GameObject::Initialize()
{
	_transform.CalculateModelMatrix();

	std::vector<Component*>::iterator& it = _components.begin();
	const std::vector<Component*>::iterator& end = _components.end();
	for (it; it != end; ++it)
	{
		(*it)->Initialize();
	}
}

void GameObject::Shutdown()
{
	std::vector<Component*>::iterator& it = _components.begin();
	const std::vector<Component*>::iterator& end = _components.end();
	for (it; it != end; ++it)
	{
		(*it)->Shutdown();
		delete (*it);
		(*it) = nullptr;
	}
	_components.clear();
}

void GameObject::Load(Archive* archive)
{
	std::vector<Component*>::iterator& it = _components.begin();
	const std::vector<Component*>::iterator& end = _components.end();
	for (it; it != end; ++it)
	{
		(*it)->Load(archive);
	}
}

void GameObject::Save(Archive* archive)
{
	std::vector<Component*>::iterator& it = _components.begin();
	const std::vector<Component*>::iterator& end = _components.end();
	for (it; it != end; ++it)
	{
		(*it)->Save(archive);
	}
}

void GameObject::PostLoad()
{
	std::vector<Component*>::iterator& it = _components.begin();
	const std::vector<Component*>::iterator& end = _components.end();
	for (it; it != end; ++it)
	{
		(*it)->PostLoad();
	}
}

void GameObject::PreSave()
{
	std::vector<Component*>::iterator& it = _components.begin();
	const std::vector<Component*>::iterator& end = _components.end();
	for (it; it != end; ++it)
	{
		(*it)->PreSave();
	}
}

void GameObject::Update(float32 deltaTime)
{
	std::vector<Component*>::iterator& it = _componentsToRemove.begin();
	std::vector<Component*>::iterator& end = _componentsToRemove.end();
	for (it; it != end; ++it)
	{
		RemoveComponent((*it));
	}
	_componentsToRemove.clear();

	_isInUpdate = true;

	it = _newComponents.begin();
	end = _newComponents.end();
	
	for (it; it != end; ++it)
	{
		_components.push_back((*it));
	}
	_newComponents.clear();

	if (_transform._shouldCalculateMatrix)
	{
		_transform.CalculateModelMatrix();
		it = _components.begin();
		end = _components.end();
		for (it; it != end; ++it)
		{
			(*it)->OnOwnerTransformUpdated(_transform);
		}
	}
	
	it = _components.begin();
	end = _components.end();
	for (it; it != end; ++it)
	{
		if ((*it)->IsEnabled())
		{
			(*it)->Update(deltaTime);
		}
	}

	_isInUpdate = false;
}

void GameObject::Render()
{
	assert(gGraphics);
	gGraphics->SetObject(this);
	std::vector<Component*>::iterator& it = _components.begin();
	const std::vector<Component*>::iterator& end = _components.end();
	for (it; it != end; ++it)
	{
		if ((*it)->IsEnabled())
		{
			(*it)->Render();
		}
	}
}

const string& GameObject::GetName() const
{
	return _name;
}

void GameObject::SetName(const string& name)
{
	_name = name;
}

bool GameObject::IsEnabled() const
{
	return _enabled;
}

void GameObject::SetEnabled(bool enabled)
{
	_enabled = enabled;
}

Transform& GameObject::GetTransform()
{
	return _transform;
}

void GameObject::RemoveComponent(Component* component)
{
	if (_isInUpdate)
	{
		_componentsToRemove.push_back(component);
	}
	else
	{
		std::vector<Component*>::iterator& it = _components.begin();
		std::vector<Component*>::iterator& end = _components.end();
		for (it; it != end; ++it)
		{
			if ((*it) == component)
			{
				component->Shutdown();
				_components.erase(it);
				delete component;
				break;
			}
		}
	}
}