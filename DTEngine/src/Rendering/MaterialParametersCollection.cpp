#include "MaterialParametersCollection.h"

#include "Debug/Debug.h"

MaterialParametersCollection MaterialParametersCollection::GLOBAL;

void const* MaterialParametersCollection::GetMatrix(const String& name) const
{
	auto& found = _matrixParameters.find(name);
	if(found == _matrixParameters.end())
	{
		return nullptr;
	}

	return &(found->second);
}

void const* MaterialParametersCollection::GetVector4(const String& name) const
{
	auto& found = _vector4Parameters.find(name);
	if(found == _vector4Parameters.end())
	{
		return nullptr;
	}

	return &(found->second);
}

void const* MaterialParametersCollection::GetVector3(const String& name) const
{
	auto& found = _vector3Parameters.find(name);
	if(found == _vector3Parameters.end())
	{
		return nullptr;
	}

	return &(found->second);
}

void const* MaterialParametersCollection::GetVector2(const String& name) const
{
	auto& found = _vector2Parameters.find(name);
	if(found == _vector2Parameters.end())
	{
		return nullptr;
	}

	return &(found->second);
}

void const* MaterialParametersCollection::GetFloat(const String& name) const
{
	auto& found = _floatParameters.find(name);
	if(found == _floatParameters.end())
	{
		return nullptr;
	}
	return &(found->second);
}

void const* MaterialParametersCollection::GetInt(const String& name) const
{
	auto& found = _intParameters.find(name);
	if(found == _intParameters.end())
	{
		return nullptr;
	}
	return &(found->second);
}

void const* MaterialParametersCollection::Get(const String& name) const
{
	void const* ptr = GetMatrix(name);
	if(ptr != nullptr)
	{
		return ptr;
	}

	ptr = GetVector4(name);
	if(ptr != nullptr)
	{
		return ptr;
	}

	ptr = GetVector3(name);
	if(ptr != nullptr)
	{
		return ptr;
	}

	ptr = GetVector2(name);
	if(ptr != nullptr)
	{
		return ptr;
	}

	ptr = GetFloat(name);
	if(ptr != nullptr)
	{
		return ptr;
	}

	ptr = GetInt(name);
	if(ptr != nullptr)
	{
		return ptr;
	}

	return nullptr;
}

bool MaterialParametersCollection::LoadFromJSON(const JSON& jsonData)
{
	if(!jsonData.is_array())
	{
		return false;
	}

	for(auto& it = jsonData.begin(); it != jsonData.end(); ++it)
	{
		auto& element = *it;
		if(element.is_object())
		{
			for(auto& elementIt = element.begin(); elementIt != element.end(); ++elementIt)
			{
				std::string key = elementIt.key();
				String name(key.begin(), key.end());
				auto& value = elementIt.value();
				if(value.is_number())
				{
					if(value.is_number_float())
					{
						float32 v = value;
						SetFloat(name, v);
					}
					else if(value.is_number_integer())
					{
						int32 v = value;
						SetInt(name, v);
					}
					else if(value.is_number_unsigned())
					{
						uint32 v = value;
						SetInt(name, (int32)v);
					}
				}
				else if(value.is_string())
				{
					std::string v = value;
					String texturePath(v.begin(), v.end());
					// TODO: SetTexture!
				}
				else if(value.is_object())
				{
					uint64 size = (uint64)value.size();
					if(size == 4)
					{
						XMFLOAT4 v = value;
						SetVector(name, v);
					}
					else if(size == 3)
					{
						XMFLOAT3 v = value;
						SetVector(name, v);
					}
					else if(size == 2)
					{
						XMFLOAT2 v = value;
						SetVector(name, v);
					}
				}
			}
		}
	}

	return true;
}
