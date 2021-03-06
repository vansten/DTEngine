#pragma once

#include "Debug/Debug.h"

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

class Shader;
class MeshBase;
class Material;

class Resources final
{
protected:
	Map<String, SharedPtr<Asset>> _assetsMap;
	UniquePtr<Material> _missingMaterial;

public:
	bool Initialize();
	void Shutdown();

	Material* GetDefaultMaterial() const;

	template<typename T>
	SharedPtr<T> Get();
	template<typename T>
	SharedPtr<T> Get(const String& path);
	template<typename T>
	SharedPtr<T> GetCopy(const T& original);
};

extern Resources gResources;

template<typename T>
SharedPtr<T> Resources::Get()
{
	String path = DT_TEXT("Hidden");
	std::string typeNameStr = typeid(T).name();
	String typeName(typeNameStr.begin(), typeNameStr.end());
	path += typeName;
	if (_assetsMap.find(path) != _assetsMap.end())
	{
		return StaticPointerCast<T>(_assetsMap[path]);
	}

	SharedPtr<T> nAsset(new T());
	bool result = nAsset->Initialize();
	if (!result)
	{
		gDebug.Printf(LogVerbosity::Error, CHANNEL_ENGINE, DT_TEXT("Cannot initialize %s"), typeName.c_str());
		nAsset->Shutdown();
		return SharedPtr<T>(nullptr);
	}

	gDebug.Printf(LogVerbosity::Log, CHANNEL_ENGINE, DT_TEXT("Initialized asset of type %s"), typeName.c_str());

	_assetsMap.insert(Pair<String, SharedPtr<Asset>>(path, nAsset));

	return StaticPointerCast<T>(_assetsMap[path]);
}

template<typename T>
SharedPtr<T> Resources::Get(const String& path)
{
	if (_assetsMap.find(path) != _assetsMap.end())
	{
		return StaticPointerCast<T>(_assetsMap[path]);
	}

	std::string typeNameStr = typeid(T).name();
	String typeName(typeNameStr.begin(), typeNameStr.end());
	SharedPtr<T> nAsset(new T());
	bool result = nAsset->Load(path);
	if (!result)
	{
		gDebug.Printf(LogVerbosity::Error, CHANNEL_ENGINE, DT_TEXT("Cannot load %s at path: %s"), typeName.c_str(), path.c_str());
		nAsset->Shutdown();
		return SharedPtr<T>(nullptr);
	}

	gDebug.Printf(LogVerbosity::Log, CHANNEL_ENGINE, DT_TEXT("Loaded asset of type %s at path: %s"), typeName.c_str(), path.c_str());

	result = nAsset->Initialize();
	if (!result)
	{
		gDebug.Printf(LogVerbosity::Error, CHANNEL_ENGINE, DT_TEXT("Cannot initialize %s at path: %s"), typeName.c_str(), path.c_str());
		nAsset->Shutdown();
		return SharedPtr<T>(nullptr);
	}

	gDebug.Printf(LogVerbosity::Log, CHANNEL_ENGINE, DT_TEXT("Initialized asset of type %s at path: %s"), typeName.c_str(), path.c_str());

	_assetsMap.insert(Pair<String, SharedPtr<Asset>>(path, nAsset));

	return StaticPointerCast<T>(_assetsMap[path]);
}

template<typename T>
inline SharedPtr<T> Resources::GetCopy(const T& original)
{
	SharedPtr<T> nAsset(new T(original));
	std::string typeNameStr = typeid(T).name();
	String typeName(typeNameStr.begin(), typeNameStr.end());
	String path = original.GetPath() + DT_TEXT("_copy");

	bool result = nAsset->Initialize();
	if (!result)
	{
		gDebug.Printf(LogVerbosity::Error, CHANNEL_ENGINE, DT_TEXT("Cannot initialize copy of a %s"), typeName.c_str());
		nAsset->Shutdown();
		return SharedPtr<T>(nullptr);
	}

	gDebug.Printf(LogVerbosity::Log, CHANNEL_ENGINE, DT_TEXT("Initialized asset copy of type %s"), typeName.c_str());

	_assetsMap.insert(Pair<String, SharedPtr<Asset>>(path, nAsset));

	return StaticPointerCast<T>(_assetsMap[path]);
}
