#include "BoxCollider.h"

#include "Debug/Debug.h"
#include "GameFramework/Entity.h"
#include "Utility/BoundingBox.h"

using namespace physx;

void BoxCollider::Initialize(PhysicalBody* physicalBody)
{
	Collider::Initialize(physicalBody);

	if (!gPhysics.CreateBoxShape(_size * 0.5f, &_shape))
	{
		gDebug.Print(LogVerbosity::Error, CHANNEL_PHYSICS, DT_TEXT("Cannot create physx::PxShape. Check log for more details!"));
		return;
	}
}

void BoxCollider::SetScale(const Vector3& scale)
{
	if (!_shape)
	{
		return;
	}

	PxBoxGeometry geometry;
	_shape->getBoxGeometry(geometry);
	Vector3 size = Vector3::Multiply(_size, scale);
	geometry.halfExtents = ToPxVec3(size * 0.5f);
	_shape->setGeometry(geometry);
}

void BoxCollider::MatchToMeshBoundingBox(const BoundingBox& boundingBox)
{
	SetSize(boundingBox.GetHalfExtents() * 2.0f);
}

void BoxCollider::SetSize(const Vector3& newSize)
{
	if (!_physicalBody)
	{
		return;
	}

	if (_size != newSize)
	{
		_size = newSize;
		SetScale(_physicalBody->GetOwner()->GetScale());
	}
}
