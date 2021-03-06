#pragma once

#include "Core/Platform.h"
#include "Utility/Math.h"
#include "Utility/JSON.h"

class MaterialParametersCollection final
{
	friend class Material;
	friend struct ShaderVariable;

public:
	static MaterialParametersCollection GLOBAL;

private:
	Map<String, Matrix> _matrixParameters;
	Map<String, Vector4> _vector4Parameters;
	Map<String, Vector3> _vector3Parameters;
	Map<String, Vector2> _vector2Parameters;
	Map<String, float> _floatParameters;
	Map<String, int> _intParameters;

private:
	void const* GetMatrix(const String& name) const;
	void const* GetVector4(const String& name) const;
	void const* GetVector3(const String& name) const;
	void const* GetVector2(const String& name) const;
	void const* GetFloat(const String& name) const;
	void const* GetInt(const String& name) const;

	void const* Get(const String& name) const;

public:
	bool LoadFromJSON(const JSON& jsonData);

	inline void SetFloat(const String& name, float value)
	{
		_floatParameters[name] = value;
	}

	inline void SetInt(const String& name, int value)
	{
		_intParameters[name] = value;
	}

	inline void SetVector(const String& name, const Vector2& vector)
	{
		_vector2Parameters[name] = vector;
	}

	inline void SetVector(const String& name, const Vector3& vector)
	{
		_vector3Parameters[name] = vector;
	}

	inline void SetColor(const String& name, const Vector4& color)
	{
		_vector4Parameters[name] = color;
	}

	inline void SetMatrix(const String& name, const Matrix& matrix)
	{
		_matrixParameters[name] = matrix;
	}
};