#pragma once

#include "Vector3.h"

class Vector4 final
{
public:
	float X;
	float Y;
	float Z;
	float W;

public:
	Vector4() : X(0.0f), Y(0.0f), Z(0.0f), W(0.0f) { }
	Vector4(float x, float y, float z, float w) : X(x), Y(y), Z(z), W(w) { }
	Vector4(const Vector3& v, float w) : X(v.X), Y(v.Y), Z(v.Z), W(w) { }
	Vector4(const Vector4& v) : X(v.X), Y(v.Y), Z(v.Z), W(v.W) { }

	inline float& operator[](size_t index)
	{
		assert(index >= 0 && index < 4);
		return *(&X + index);
	}

	inline const float& operator[](size_t index) const
	{
		assert(index >= 0 && index < 4);
		return *(&X + index);
	}

	inline bool operator==(const Vector4& v) const
	{
		return Math::Approximately(X, v.X) && Math::Approximately(Y, v.Y) && Math::Approximately(Z, v.Z) && Math::Approximately(W, v.W);
	}

	inline bool operator!=(const Vector4& v) const
	{
		return !Math::Approximately(X, v.X) || !Math::Approximately(Y, v.Y) || !Math::Approximately(Z, v.Z) || !Math::Approximately(W, v.W);
	}

public:
	inline static float DotProduct(const Vector4& v1, const Vector4& v2)
	{
		return v1.X * v2.X + v1.Y * v2.Y + v1.Z * v2.Z + v1.W * v2.W;
	}
};