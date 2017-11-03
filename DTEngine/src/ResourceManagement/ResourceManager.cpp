#include "ResourceManager.h"

#include "Rendering/Material.h"
#include "Rendering/Meshes/HexagonMesh.h"
#include "Rendering/Meshes/TriangleMesh.h"

#define CREATE_NEW_MATERIAL(shaderPath, color, path, pointer, map)	\
pointer = new Material();											\
pointer->Initialize(shaderPath);									\
pointer->SetColor(color);											\
map.insert(std::pair<string, Asset*>(path, pointer));				\

ResourceManager* gResourceManager = nullptr;

ResourceManager::ResourceManager()
{
}

bool ResourceManager::Initialize()
{
	if (!gResourceManager)
	{
		gResourceManager = this;
	}

	Load<HexagonMesh>(HEXAGON_MESH);
	Load<TriangleMesh>(TRIANGLE_MESH);
	Load<QuadMesh>(QUAD_MESH);
	Load<PlaneMesh>(PLANE_MESH);
	Load<SphereMesh>(SPHERE_MESH);
	Load<CubeMesh>(CUBE_MESH);

	Shader* colorShader = Load<Shader>(COLOR_SHADER);
	Material* material = nullptr;
	XMFLOAT4 color;
	color.x = color.y = color.z = color.w = 1.0f;
	CREATE_NEW_MATERIAL(COLOR_SHADER, color, WHITE_MATERIAL, material, _assetsMap);
	color.y = color.z = 0.0f;
	CREATE_NEW_MATERIAL(COLOR_SHADER, color, RED_MATERIAL, material, _assetsMap);
	color.x = 0.0f; color.y = 1.0f;
	CREATE_NEW_MATERIAL(COLOR_SHADER, color, GREEN_MATERIAL, material, _assetsMap);
	color.y = 0.0f; color.z = 1.0f;
	CREATE_NEW_MATERIAL(COLOR_SHADER, color, BLUE_MATERIAL, material, _assetsMap);
	color.x = 1.0f; color.y = 1.0f; color.z = 0.0f;
	CREATE_NEW_MATERIAL(COLOR_SHADER, color, YELLOW_MATERIAL, material, _assetsMap);
	color.x = 0.0f; color.y = 1.0f; color.z = 1.0f;
	CREATE_NEW_MATERIAL(COLOR_SHADER, color, CYAN_MATERIAL, material, _assetsMap);
	color.x = 1.0f; color.y = 0.0f; color.z = 1.0f;
	CREATE_NEW_MATERIAL(COLOR_SHADER, color, MAGENTA_MATERIAL, material, _assetsMap);
	color.x = 0.0f; color.y = 0.0f; color.z = 0.0f;
	CREATE_NEW_MATERIAL(COLOR_SHADER, color, BLACK_MATERIAL, material, _assetsMap);

	return true;
}

void ResourceManager::Shutdown()
{
	std::map<string, Asset*>::iterator it = _assetsMap.begin();
	std::map<string, Asset*>::iterator end = _assetsMap.end();
	for (it; it != end; ++it)
	{
		if ((*it).second)
		{
			(*it).second->Shutdown();
			delete (*it).second;
			(*it).second = nullptr;
		}
	}
	_assetsMap.clear();
}