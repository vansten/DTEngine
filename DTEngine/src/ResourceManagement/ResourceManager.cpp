#include "ResourceManager.h"

ResourceManager::ResourceManager()
{
}

bool ResourceManager::Initialize()
{
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