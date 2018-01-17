#include "ResourceManager.h"

ResourceManager::ResourceManager()
{
}

bool ResourceManager::Initialize()
{
	_missingMaterial = std::make_unique<Material>();
	_missingMaterial->SetColor(XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f));
	if(!_missingMaterial->Initialize())
	{
		return false;
	}
	return true;
}

void ResourceManager::Shutdown()
{
	if(_missingMaterial)
	{
		_missingMaterial->Shutdown();
	}
	_missingMaterial = nullptr;

	for (auto pair : _assetsMap)
	{
		if (pair.second)
		{
			pair.second->Shutdown();
		}
	}
	_assetsMap.clear();
}

Material* ResourceManager::GetDefaultMaterial() const
{
	return _missingMaterial.get();
}
