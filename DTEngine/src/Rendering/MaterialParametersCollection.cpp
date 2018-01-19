#include "MaterialParametersCollection.h"

XMMATRIX MaterialParametersCollection::GetMatrix(const String& name) const
{
	auto& found = _matrixParameters.find(name);
	if(found == _matrixParameters.end())
	{
		return XMMATRIX();
	}

	return found->second;
}

XMFLOAT4 MaterialParametersCollection::GetColor(const String& name) const
{
	auto& found = _colorParameters.find(name);
	if(found == _colorParameters.end())
	{
		return XMFLOAT4();
	}

	return found->second;
}

XMFLOAT4 MaterialParametersCollection::GetVector4(const String& name) const
{
	auto& found = _vector4Parameters.find(name);
	if(found == _vector4Parameters.end())
	{
		return XMFLOAT4();
	}

	return found->second;
}

XMFLOAT3 MaterialParametersCollection::GetVector3(const String& name) const
{
	auto& found = _vector3Parameters.find(name);
	if(found == _vector3Parameters.end())
	{
		return XMFLOAT3();
	}

	return found->second;
}

XMFLOAT2 MaterialParametersCollection::GetVector2(const String& name) const
{
	auto& found = _vector2Parameters.find(name);
	if(found == _vector2Parameters.end())
	{
		return XMFLOAT2();
	}

	return found->second;
}

float32 MaterialParametersCollection::GetFloat(const String& name) const
{
	auto& found = _floatParameters.find(name);
	if(found == _floatParameters.end())
	{
		return 0.0f;
	}
	return found->second;
}

int32 MaterialParametersCollection::GetInt(const String& name) const
{
	auto& found = _intParameters.find(name);
	if(found == _intParameters.end())
	{
		return 0;
	}
	return found->second;
}

void const* MaterialParametersCollection::Get(const String& name) const
{
	auto& matrixFound = _matrixParameters.find(name);
	if(matrixFound != _matrixParameters.end())
	{
		return reinterpret_cast<void const*>(&(matrixFound->second));
	}

	auto& colorFound = _colorParameters.find(name);
	if(colorFound != _colorParameters.end())
	{
		return reinterpret_cast<void const*>(&(colorFound->second));
	}

	auto& vector4Found = _vector4Parameters.find(name);
	if(vector4Found != _vector4Parameters.end())
	{
		return reinterpret_cast<void const*>(&(vector4Found->second));
	}

	auto& vector3Found = _vector3Parameters.find(name);
	if(vector3Found != _vector3Parameters.end())
	{
		return reinterpret_cast<void const*>(&(vector3Found->second));
	}

	auto& vector2Found = _vector2Parameters.find(name);
	if(vector2Found != _vector2Parameters.end())
	{
		return reinterpret_cast<void const*>(&(vector2Found->second));
	}

	auto& floatFound = _floatParameters.find(name);
	if(floatFound != _floatParameters.end())
	{
		return reinterpret_cast<void const*>(&(floatFound->second));
	}

	auto& intFound = _intParameters.find(name);
	if(intFound != _intParameters.end())
	{
		return reinterpret_cast<void const*>(&(intFound->second));
	}

	return nullptr;
}
