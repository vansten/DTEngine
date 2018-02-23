#include "LayerManager.h"

#include <fstream>

String LayerManager::_layers[MAX_LAYERS];
LayerID LayerManager::ALL = 0;

const String LAYER_MANAGER_FILENAME = DT_TEXT("Resources/Layers.dtl");

void LayerManager::Initialize()
{
	_layers[0] = DT_TEXT("Default");

	for (unsigned int i = 0; i < MAX_LAYERS; ++i)
	{
		ALL |= (1 << i);
	}

	std::ifstream file(LAYER_MANAGER_FILENAME);
	if (file.is_open())
	{
		std::string lineStr;
		int i = 0;
		while (std::getline(file, lineStr) && i < MAX_LAYERS)
		{
			String line(lineStr.begin(), lineStr.end());
			_layers[i] = line;
			++i;
		}
	}
	file.close();
}

void LayerManager::Save()
{
	String data;
	for (int i = 0; i < MAX_LAYERS; ++i)
	{
		data += _layers[i];

		if (i < MAX_LAYERS - 1)
		{
			data += DT_TEXT("\n");
		}
	}

	std::ofstream file(LAYER_MANAGER_FILENAME);
	if (file.is_open())
	{
		std::string dataStr(data.begin(), data.end());
		file << dataStr;
	}
	file.close();
}

LayerID LayerManager::NameToLayer(const String& layerName)
{
	for(unsigned int i = 0; i < MAX_LAYERS; ++i)
	{
		if (_layers[i].size() == layerName.size() && std::equal(_layers[i].begin(), _layers[i].end(), layerName.begin(), layerName.end()))
		{
			return i;
		}
	}

	return -1;
}

const String& LayerManager::LayerToName(LayerID layer)
{
	DT_ASSERT((unsigned int)layer >= 0 && (unsigned int)layer < MAX_LAYERS, DT_TEXT("Cannot obtain layer name. Layer is out of range"));
	return _layers[(unsigned int)layer];
}

LayerID LayerManager::GetMask(const DynamicArray<String>& layers)
{
	unsigned int mask = 0;
	for (const auto& layerName : layers)
	{
		unsigned int layer = NameToLayer(layerName);
		if (layer >= 0)
		{
			mask |= (1 << layer);
		}
	}

	return mask;
}

void LayerManager::SetLayer(LayerID layer, const String& layerName)
{
	DT_ASSERT((unsigned int)layer >= 0 && (unsigned int)layer < MAX_LAYERS, DT_TEXT("Cannot obtain layer name. Layer is out of range"));
	_layers[(unsigned int)layer] = layerName;
}
