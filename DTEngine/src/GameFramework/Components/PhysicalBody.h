#pragma once

#include "Core/Event.h"
#include "GameFramework/Component.h"

class Collider;
struct BoundingBox;

class PhysicalBody : public Component
{
	friend class Physics;

private:
	physx::PxRigidActor* _rigidbody;
	float _mass;
	bool _isDynamic;
	bool _isKinematic;

	DynamicArray<UniquePtr<Collider>> _colliders;

public:
	Event<void(const CollisionInfo&)> OnHit;
	Event<void(const TriggerInfo&)> OnTriggerBegin;
	Event<void(const TriggerInfo&)> OnTriggerEnd;

public:
	inline PhysicalBody(SharedPtr<Entity> owner) : Component(owner)
	{}
	inline PhysicalBody(const PhysicalBody& other) : Component(other), _isKinematic(other._isKinematic), _mass(other._mass), _rigidbody(nullptr)
	{

	}

	DECLARE_SHARED_FROM_THIS(PhysicalBody)

protected:
	virtual SharedPtr<Component> Copy(SharedPtr<Entity> newOwner) const override;

	void OnPhysXCollision(const CollisionInfo& c) const
	{
		OnHit.Execute(c);
	}

	void OnPhysXTriggerBegin(const TriggerInfo& t) const
	{
		OnTriggerBegin.Execute(t);
	}

	void OnPhysXTriggerEnd(const TriggerInfo& t) const
	{
		OnTriggerEnd.Execute(t);
	}

public:
	virtual void OnInitialize() override;
	virtual void OnShutdown() override;

	virtual void Load(Archive& archive) override;
	virtual void Save(Archive& archive) override;

	void SetIsDynamic(bool isDynamic);

	void AddCollider(UniquePtr<Collider>&& collider);
};

class Collider
{
	friend class PhysicalBody;

protected:
	physx::PxShape* _shape;

public:
	Collider() : _shape(nullptr)
	{}
	virtual ~Collider()
	{}

protected:
	virtual void Load(Archive& archive) = 0;
	virtual void Initialize() = 0;
	virtual void Shutdown()
	{
		RELEASE_PHYSX(_shape);
	}

public:
	virtual void MatchToMeshBoundingBox(const BoundingBox& boundingBox) = 0;
};