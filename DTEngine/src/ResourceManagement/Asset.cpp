#include "Asset.h"

Asset::Asset()
{}

Asset::~Asset()
{}

bool Asset::Load(const String& path)
{
	_path = path;
	return true;
}

bool Asset::Save(const String& path)
{
	DT_ASSERT(false, DT_TEXT("This asset does not support saving"));
	return false;
}

bool Asset::Initialize()
{
	return true;
}

void Asset::Shutdown()
{}
