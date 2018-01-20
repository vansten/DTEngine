#pragma once

#include <cctype>

#include "Core/Platform.h"

static String GetExtension(const String& path)
{
	const size_t lastDotIndex = path.find_last_of('.');
	if(lastDotIndex == -1)
	{
		return false;
	}

	String extension = path.substr(lastDotIndex + 1);
	const size_t length = extension.length();
	for(uint64 i = 0; i < length; ++i)
	{
		extension[i] = tolower(extension[i]);
	}

	return extension;
}

static bool Contains(const String& string, const String& testString, bool caseSensitive = true)
{
	const size_t testStringSize = testString.size();
	const size_t stringSize = string.size();
	if(testStringSize > stringSize)
	{
		return false;
	}

	const size_t firstCharIndex = string.find_first_of(testString[0]);
	const size_t lastCharIndex = firstCharIndex + (testStringSize - 1);
	if(firstCharIndex == -1 || lastCharIndex >= stringSize)
	{
		return false;
	}

	if(caseSensitive)
	{
		for(size_t i = firstCharIndex; i < lastCharIndex; ++i)
		{
			if(string[i] != testString[i - firstCharIndex])
			{
				return false;
			}
		}
	}
	else
	{
		for(size_t i = firstCharIndex; i < lastCharIndex; ++i)
		{
			if(std::tolower(string[i]) != std::tolower(testString[i - firstCharIndex]))
			{
				return false;
			}
		}
	}

	return true;
}