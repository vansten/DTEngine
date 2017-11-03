#pragma once

#include "Rendering/MeshBase.h"
#include "Rendering/Material.h"
#include "Rendering/Shader.h"
#include "Rendering/Meshes/HexagonMesh.h"
#include "Rendering/Meshes/TriangleMesh.h"

#include <map>

#define HEXAGON_MESH DT_TEXT("Hidden/Primitives/Hexagon")
#define TRIANGLE_MESH DT_TEXT("Hidden/Primitives/Triangle")
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