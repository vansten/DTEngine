#include "MeshCollider.h"

#include "Debug/Debug.h"

void MeshCollider::Initialize()
{
	if (!gPhysics.CreateMeshShape(_mesh, &_shape))
	{
		gDebug.Print(LogVerbosity::Error, CHANNEL_PHYSICS, DT_TEXT("Cannot create physx::PxShape. Check log for more details!"));
		return;
	}
}

void MeshCollider::MatchToMeshBoundingBox(const BoundingBox& boundingBox)
{

}
