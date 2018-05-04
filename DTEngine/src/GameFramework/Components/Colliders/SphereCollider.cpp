#include "SphereCollider.h"

#include "Debug/Debug.h"
#include "GameFramework/Entity.h"
#include "Utility/BoundingBox.h"

using namespace physx;

void SphereCollider::Initialize(PhysicalBody* physicalBody)
{
	Collider::Initialize(physicalBody);

	if (!gPhysics.CreateSphereShape(_radius, &_shape))
	{
		gDebug.Print(LogVerbosity::Error, CHANNEL_PHYSICS, DT_TEXT("Cannot create physx::PxShape. Check log for more details!"));
		return;
	}
}

void SphereCollider::SetScale(const Vector3& scale)
{
	if (!_shape)
	{
		return;
	}

	PxSphereGeometry geometry;
	_shape->getSphereGeometry(geometry);
	geometry.radius = _radius * Math::Max(Math::Max(scale.X, scale.Y), scale.Z);
	_shape->setGeometry(geometry);
}

void SphereCollider::MatchToMeshBoundingBox(const BoundingBox& boundingBox)
{
	Vector3 size = boundingBox.GetHalfExtents() * 2.0f;
	float radius = Math::Max(Math::Max(size.X, size.Y), size.Z);
	SetRadius(radius);
}

void SphereCollider::SetRadius(float newRadius)
{
	if (_radius != newRadius)
	{
		_radius = newRadius;
		SetScale(_physicalBody->GetOwner()->GetScale());
	}
}
