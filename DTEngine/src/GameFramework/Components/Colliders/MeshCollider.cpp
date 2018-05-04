#include "MeshCollider.h"

#include "Debug/Debug.h"

using namespace physx;

void MeshCollider::Initialize(PhysicalBody* physicalBody)
{
	Collider::Initialize(physicalBody);

	if (!gPhysics.CreateMeshShape(_mesh, &_shape))
	{
		gDebug.Print(LogVerbosity::Error, CHANNEL_PHYSICS, DT_TEXT("Cannot create physx::PxShape. Check log for more details!"));
		return;
	}
}

void MeshCollider::SetScale(const Vector3& scale)
{
	if (!_shape)
	{
		return;
	}
	PxTriangleMeshGeometry geometry;
	_shape->getTriangleMeshGeometry(geometry);
	geometry.scale.scale = ToPxVec3(scale);
	_shape->setGeometry(geometry);
}

void MeshCollider::MatchToMeshBoundingBox(const BoundingBox& boundingBox)
{
	if (!_mesh)
	{
		return;
	}

	Vector3 targetScale = boundingBox.GetHalfExtents();
	const DynamicArray<MeshBase::VertexType>& vertices = _mesh->GetVertices();
	Vector3 meshSize = Vector3::ZERO;
	for (int i = 0; i < vertices.size(); ++i)
	{
		for (int j = i; j < vertices.size(); ++i)
		{
			Vector3 diff = vertices[j].Position - vertices[i].Position;
			meshSize = Vector3::Max(meshSize, diff);
		}
	}

	if (meshSize.X != 0.0f)
	{
		targetScale.X /= meshSize.X;
	}
	if (meshSize.Y != 0.0f)
	{
		targetScale.Y /= meshSize.Y;
	}
	if (meshSize.Z != 0.0f)
	{
		targetScale.Z /= meshSize.Z;
	}

	SetScale(targetScale);
}
