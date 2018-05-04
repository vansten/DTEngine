#pragma once

#include "GameFramework/Components/PhysicalBody.h"

enum class eCapsuleDirection
{
	X,
	Y,
	Z
};

DECLARE_ENUM_NAMES(eCapsuleDirection)
{
	{DT_TEXT("X"), eCapsuleDirection::X },
	{DT_TEXT("Y"), eCapsuleDirection::Y },
	{DT_TEXT("Z"), eCapsuleDirection::Z }
};

class CapsuleCollider : public Collider
{
private:
	float _radius;
	float _halfHeight;
	eCapsuleDirection _capsuleDirection;

public:
	inline CapsuleCollider() : Collider(), _radius(0.25f), _halfHeight(0.5f), _capsuleDirection(eCapsuleDirection::Y)
	{}
	inline CapsuleCollider(float radius, float halfHeight, eCapsuleDirection capsuleDirection) : Collider(), _radius(radius), _halfHeight(halfHeight), _capsuleDirection(capsuleDirection)
	{}
	inline CapsuleCollider(const CapsuleCollider& other) : Collider(other), _radius(other._radius), _halfHeight(other._halfHeight), _capsuleDirection(other._capsuleDirection)
	{}

protected:
	virtual void Load(Archive& archive) override
	{}
	virtual void Initialize(PhysicalBody* physicalBody) override;

	virtual void SetScale(const Vector3& scale) override;

public:
	virtual void MatchToMeshBoundingBox(const BoundingBox& boundingBox) override;
	void SetRadius(float newRadius);
	void SetHalfHeight(float newHalfHeight);
	void SetCapsuleDirection(eCapsuleDirection newDirection);
};