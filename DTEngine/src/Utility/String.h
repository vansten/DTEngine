#pragma once

#include "Core/Platform.h"

string GetExtension(const string& path)
{
	size_t lastDotIndex = path.find_last_of('.');
	if(lastDotIndex == -1)
	{
		return false;
	}

	string extension = path.substr(lastDotIndex + 1);
	size_t length = extension.length();
	for(uint64 i = 0; i < length; ++i)
	{
		extension[i] = tolower(extension[i]);
	}

	return extension;
}