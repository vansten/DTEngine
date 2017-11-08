#include "Asset.h"

Asset::Asset()
{

}

Asset::~Asset()
{

}

bool Asset::Initialize(const String& path)
{
	_path = path;
	return true;
}
