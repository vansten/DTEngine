#include "ResourceManager.h"

ResourceManager::ResourceManager()
{
}

bool ResourceManager::Initialize()
{
	Load<HexagonMesh>(HEXAGON_MESH);
	Load<TriangleMesh>(TRIANGLE_MESH);
	Load<QuadMesh>(QUAD_MESH);
	Load<PlaneMesh>(PLANE_MESH);
	Load<SphereMesh>(SPHERE_MESH);
	Load<CubeMesh>(CUBE_MESH);
	Load<CylinderMesh>(CYLINDER_MESH);
	Load<ConeMesh>(CONE_MESH);
	Load<CapsuleMesh>(CAPSULE_MESH);
	Load<Shader>(COLOR_SHADER);
	Load<Material>(WHITE_MATERIAL);

	return true;
}

void ResourceManager::Shutdown()
{
	for (auto pair : _assetsMap)
	{
		if (pair.second)
		{
			pair.second->Shutdown();
		}
	}
	_assetsMap.clear();
}