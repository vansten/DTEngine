#pragma once

#include <Core/Platform.h>

template<typename Enum>
struct EnumStringPair
{
public:
	String Name;
	Enum Value;
};

template<typename Enum>
struct EnumInfo
{
private:
	static EnumStringPair<Enum> EnumInfo<Enum>::_pairList[];
	static const String _emptyName;

public:
	static Enum FromString(const String& name)
	{
		const EnumStringPair<Enum>* pairs = _pairList;
		for(; pairs->Name[0] != 0; ++pairs)
		{
			if(name.compare(pairs->Name) == 0)
			{
				return pairs->Value;
			}
		}

		DT_ASSERT(false, "Given name does not exist in enum or the conversion was not defined");
		return (Enum)0;
	}

	static const String& ToString(Enum value)
	{
		const EnumStringPair<Enum>* pairs = _pairList;
		for(; pairs->Name[0] != 0; ++pairs)
		{
			if(pairs->Value == value)
			{
				return pairs->Name;
			}
		}

		DT_ASSERT(false, "Given value does not exist in enum or the conversion was not defined");
		return _emptyName;
	}
};

template<typename Enum>
const String EnumInfo<Enum>::_emptyName = DT_TEXT("");

#define DECLARE_ENUM_NAMES(enumType) \
EnumStringPair<enumType> EnumInfo<enumType>::_pairList[] =
