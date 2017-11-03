#include "Asset.h"

Asset::Asset()
{

}

Asset::~Asset()
{

}

bool Asset::Initialize(const string& path)
{
	_path = path;
	return true;
}
