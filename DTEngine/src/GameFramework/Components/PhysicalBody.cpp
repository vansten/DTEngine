#include "PhysicalBody.h"

#include "Debug/Debug.h"
#include "GameFramework/Entity.h"

SharedPtr<Component> PhysicalBody::Copy(SharedPtr<Entity> newOwner) const
{
	IMPLEMENT_COPY(PhysicalBody);
}

void PhysicalBody::OnInitialize()
{
	if (!gPhysics.CreateRigidbody(_isDynamic, _mass, this, &_rigidbody))
	{
		gDebug.Print(LogVerbosity::Error, CHANNEL_PHYSICS, DT_TEXT("Cannot create PxRigidActor instance! Check log for more details"));
		return;
	}

	if (_isDynamic)
	{
		physx::PxRigidDynamic* rd = _rigidbody->is<physx::PxRigidDynamic>();
		if (rd)
		{
			rd->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, _isKinematic);
		}
	}

	for (const auto& collider : _colliders)
	{
		collider->Initialize(this);
		_rigidbody->attachShape(*(collider->_shape));
		collider->SetScale(GetOwner()->GetScale());
	}
}

void PhysicalBody::OnShutdown()
{
	for (const auto& collider : _colliders)
	{
		collider->Shutdown();
	}
	_colliders.clear();

	RELEASE_PHYSX(_rigidbody);
}

void PhysicalBody::Load(Archive& archive)
{

}

void PhysicalBody::Save(Archive& archive)
{

}

void PhysicalBody::OnOwnerTransformUpdated(const Transform& transform)
{
	for (const auto& collider : _colliders)
	{
		collider->SetScale(transform.GetScale());
	}
}

void PhysicalBody::SetIsDynamic(bool isDynamic)
{
	if (_isDynamic ^ isDynamic)
	{
		for (const auto& collider : _colliders)
		{
			_rigidbody->detachShape(*(collider->_shape));
		}

		RELEASE_PHYSX(_rigidbody);

		if (!gPhysics.CreateRigidbody(isDynamic, _mass, this, &_rigidbody))
		{
			gDebug.Print(LogVerbosity::Error, CHANNEL_PHYSICS, DT_TEXT("Cannot create PxRigidActor instance! Check log for more details"));
		}

		_isDynamic = isDynamic;

		if (_isDynamic)
		{
			physx::PxRigidDynamic* rd = _rigidbody->is<physx::PxRigidDynamic>();
			if (rd)
			{
				rd->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, _isKinematic);
			}
		}

		for (const auto& collider : _colliders)
		{
			_rigidbody->attachShape(*(collider->_shape));
		}
	}
}

void PhysicalBody::AddCollider(UniquePtr<Collider>&& collider)
{
	collider->Initialize(this);
	if (_rigidbody && collider->_shape)
	{
		_rigidbody->attachShape(*(collider->_shape));
	}
	collider->SetScale(GetOwner()->GetScale());
	_colliders.push_back(std::move(collider));
}
