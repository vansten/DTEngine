#pragma once

#include "Core/Platform.h"
#include "Utility/Math.h"

class MaterialParametersCollection
{
private:
	Map<String, XMMATRIX> _matrixParameters;
	Map<String, XMFLOAT4> _colorParameters;
	Map<String, XMFLOAT4> _vector4Parameters;
	Map<String, XMFLOAT3> _vector3Parameters;
	Map<String, XMFLOAT2> _vector2Parameters;
	Map<String, float32> _floatParameters;
	Map<String, int32> _intParameters;

public:
	inline void SetFloat(const String& name, float value)
	{
		_floatParameters[name] = value;
	}

	inline void SetInt(const String& name, int value)
	{
		_intParameters[name] = value;
	}

	inline void SetVector(const String& name, const XMFLOAT2& vector)
	{
		_vector2Parameters[name] = vector;
	}

	inline void SetVector(const String& name, const XMFLOAT3& vector)
	{
		_vector3Parameters[name] = vector;
	}

	inline void SetVector(const String& name, const XMFLOAT4& vector)
	{
		_vector4Parameters[name] = vector;
	}

	inline void SetColor(const String& name, const XMFLOAT4& color)
	{
		_colorParameters[name] = color;
	}

	inline void SetMatrix(const String& name, const XMMATRIX& matrix)
	{
		_matrixParameters[name] = matrix;
	}

	XMMATRIX GetMatrix(const String& name) const;
	XMFLOAT4 GetColor(const String& name) const;
	XMFLOAT4 GetVector4(const String& name) const;
	XMFLOAT3 GetVector3(const String& name) const;
	XMFLOAT2 GetVector2(const String& name) const;
	float32 GetFloat(const String& name) const;
	int32 GetInt(const String& name) const;

	void const* Get(const String& name) const;
};