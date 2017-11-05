#pragma once


#include "Rendering/Material.h"
#include "Rendering/Shader.h"

#include "Rendering/MeshBase.h"
#include "Rendering/Meshes/HexagonMesh.h"
#include "Rendering/Meshes/TriangleMesh.h"
#include "Rendering/Meshes/QuadMesh.h"
#include "Rendering/Meshes/PlaneMesh.h"
#include "Rendering/Meshes/SphereMesh.h"
#include "Rendering/Meshes/CubeMesh.h"
#include "Rendering/Meshes/CylinderMesh.h"
#include "Rendering/Meshes/ConeMesh.h"
#include "Rendering/Meshes/CapsuleMesh.h"
#include "Rendering/Meshes/StaticMesh.h"

#include <map>

#define HEXAGON_MESH DT_TEXT("Hidden/Primitives/Hexagon")
#define TRIANGLE_MESH DT_TEXT("Hidden/Primitives/Triangle")
#define QUAD_MESH DT_TEXT("Hidden/Primitives/Quad")
#define PLANE_MESH DT_TEXT("Hidden/Primitives/Plane")
#define SPHERE_MESH DT_TEXT("Hidden/Primitives/Sphere")
#define CUBE_MESH DT_TEXT("Hidden/Primitives/Cube")
#define CYLINDER_MESH DT_TEXT("Hidden/Primitives/Cylinder")
#define CAPSULE_MESH DT_TEXT("Hidden/Primitives/Capsule")
#define CONE_MESH DT_TEXT("Hidden/Primitives/Cone")

#define WHITE_MATERIAL DT_TEXT("Hidden/Materials/White")
#define RED_MATERIAL DT_TEXT("Hidden/Materials/Red")
#define GREEN_MATERIAL DT_TEXT("Hidden/Materials/Green")
#define BLUE_MATERIAL DT_TEXT("Hidden/Materials/Blue")
#define YELLOW_MATERIAL DT_TEXT("Hidden/Materials/Yellow")
#define CYAN_MATERIAL DT_TEXT("Hidden/Materials/Cyan")
#define MAGENTA_MATERIAL DT_TEXT("Hidden/Materials/Magenta")
#define BLACK_MATERIAL DT_TEXT("Hidden/Materials/Black")

#define COLOR_SHADER DT_TEXT("Resources/Shaders/Color")

class Shader;
class MeshBase;
class Material;

class ResourceManager
{
protected:
	std::map<string, Asset*> _assetsMap;

public:
	ResourceManager();

	bool Initialize();
	void Shutdown();

	template<typename T>
	T* Load(const string& path);
};

extern ResourceManager* gResourceManager;

template<typename T>
T* ResourceManager::Load(const string& path)
{
	if(_assetsMap.find(path) != _assetsMap.end())
	{
		return (T*)_assetsMap[path];
	}

	T* nAsset = new T();
	bool result = nAsset->Initialize(path);
	if(!result)
	{
		nAsset->Shutdown();
		delete nAsset;
		return nullptr;
	}

	_assetsMap.insert(std::pair<string, Asset*>(path, nAsset));

	return (T*)nAsset;
}