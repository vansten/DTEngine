#pragma once

#include "Core/Platform.h"

String GetExtension(const String& path)
{
	size_t lastDotIndex = path.find_last_of('.');
	if(lastDotIndex == -1)
	{
		return false;
	}

	String extension = path.substr(lastDotIndex + 1);
	size_t length = extension.length();
	for(uint64 i = 0; i < length; ++i)
	{
		extension[i] = tolower(extension[i]);
	}

	return extension;
}