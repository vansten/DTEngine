#include "ResourceManager.h"

#define CREATE_NEW_MATERIAL(pointer, path, shader, color)	\
pointer = Load<Material>(path);								\
pointer->SetShader(shader);									\
pointer->SetColor(color);									\

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

	SharedPtr<Shader> colorShader = Load<Shader>(COLOR_SHADER);
	SharedPtr<Material> material;
	XMFLOAT4 color;
	color.x = color.y = color.z = color.w = 1.0f;
	CREATE_NEW_MATERIAL(material, WHITE_MATERIAL, colorShader, color);
	color.y = color.z = 0.0f;
	CREATE_NEW_MATERIAL(material, RED_MATERIAL, colorShader, color);
	color.x = 0.0f; color.y = 1.0f;
	CREATE_NEW_MATERIAL(material, GREEN_MATERIAL, colorShader, color);
	color.y = 0.0f; color.z = 1.0f;
	CREATE_NEW_MATERIAL(material, BLUE_MATERIAL, colorShader, color);
	color.x = 1.0f; color.y = 1.0f; color.z = 0.0f;
	CREATE_NEW_MATERIAL(material, YELLOW_MATERIAL, colorShader, color);
	color.x = 0.0f; color.y = 1.0f; color.z = 1.0f;
	CREATE_NEW_MATERIAL(material, CYAN_MATERIAL, colorShader, color);
	color.x = 1.0f; color.y = 0.0f; color.z = 1.0f;
	CREATE_NEW_MATERIAL(material, MAGENTA_MATERIAL, colorShader, color);
	color.x = 0.0f; color.y = 0.0f; color.z = 0.0f;
	CREATE_NEW_MATERIAL(material, BLACK_MATERIAL, colorShader, color);

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