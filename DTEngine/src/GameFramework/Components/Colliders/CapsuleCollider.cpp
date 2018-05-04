#include "CapsuleCollider.h"

#include "Debug/Debug.h"
#include "GameFramework/Entity.h"
#include "Utility/BoundingBox.h"

using namespace physx;

static const Dictionary<eCapsuleDirection, Rotator> Poses
{
	{ eCapsuleDirection::X, Rotator(0.0f, 0.0f, 0.0f)},
	{ eCapsuleDirection::Y, Rotator(0.0f, 0.0f, 90.0f)},
	{ eCapsuleDirection::Z, Rotator(0.0f, 90.0f, 0.0f)}
};

void CapsuleCollider::Initialize(PhysicalBody* physicalBody)
{
	Collider::Initialize(physicalBody);

	if (!gPhysics.CreateCapsuleShape(_radius, _halfHeight, &_shape))
	{
		gDebug.Print(LogVerbosity::Error, CHANNEL_PHYSICS, DT_TEXT("Cannot create physx::PxShape. Check log for more details!"));
		return;
	}

	if (_shape)
	{
		const Rotator& rotation = Poses.at(_capsuleDirection);
		_shape->setLocalPose(PxTransform(ToPxQuat(rotation.ToQuaternion())));
	}
}

void CapsuleCollider::SetScale(const Vector3& scale)
{
	if (!_shape)
	{
		return;
	}

	PxCapsuleGeometry geometry;
	_shape->getCapsuleGeometry(geometry);
	geometry.radius = _radius * Math::Max(scale.X, scale.Z);
	geometry.halfHeight = _halfHeight * scale.Y;
	_shape->setGeometry(geometry);
}

void CapsuleCollider::MatchToMeshBoundingBox(const BoundingBox& boundingBox)
{
	Vector3 size = boundingBox.GetHalfExtents() * 2.0f;
	float radius = Math::Max(size.X, size.Z);
	_halfHeight = size.Y;
	SetRadius(radius);
}

void CapsuleCollider::SetRadius(float newRadius)
{
	if (newRadius != _radius)
	{
		_radius = newRadius;
		SetScale(_physicalBody->GetOwner()->GetScale());
	}
}

void CapsuleCollider::SetHalfHeight(float newHalfHeight)
{
	if (_halfHeight != newHalfHeight)
	{
		_halfHeight = newHalfHeight;
		SetScale(_physicalBody->GetOwner()->GetScale());
	}
}

void CapsuleCollider::SetCapsuleDirection(eCapsuleDirection newDirection)
{
	if (_capsuleDirection != newDirection)
	{
		_capsuleDirection = newDirection;
		
		if (_shape)
		{
			const Rotator& rotation = Poses.at(_capsuleDirection);
			_shape->setLocalPose(PxTransform(ToPxQuat(rotation.ToQuaternion())));
		}
	}
}
