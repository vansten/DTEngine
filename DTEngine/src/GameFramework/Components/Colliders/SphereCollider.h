#pragma once

#include "GameFramework/Components/PhysicalBody.h"

class SphereCollider : public Collider
{
private:
	float _radius;

public:
	inline SphereCollider() : Collider(), _radius(1.0f)
	{}
	inline SphereCollider(float radius) : Collider(), _radius(radius)
	{}
	inline SphereCollider(const SphereCollider& other) : Collider(other), _radius(other._radius)
	{}

protected:
	virtual void Load(Archive& archive) override
	{}
	virtual void Initialize(PhysicalBody* physicalBody) override;

	virtual void SetScale(const Vector3& scale) override;

public:
	virtual void MatchToMeshBoundingBox(const BoundingBox& boundingBox) override;
	void SetRadius(float newRadius);
};