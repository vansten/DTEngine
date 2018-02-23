#pragma once

#include <cassert>

#include "MathFunctions.h"

class Vector2;
class Vector3;
class Vector4;

inline Vector2 operator*(const Vector2& v, float scalar);
inline Vector2 operator*(float scalar, const Vector2& v);
inline Vector2 operator/(const Vector2& v, float scalar);
inline Vector2 operator+(const Vector2& v1, const Vector2& v2);
inline Vector2 operator-(const Vector2& v1, const Vector2& v2);

class Vector2 final
{
public:
	float X;
	float Y;

public:
	static const Vector2 ZERO;
	static const Vector2 UNIT_X;
	static const Vector2 UNIT_Y;
	static const Vector2 ONE;

public:
	inline Vector2() : X(0.0f), Y(0.0f)
	{}
	inline Vector2(float x, float y) : X(x), Y(y)
	{}
	inline Vector2(const Vector2& v) : X(v.X), Y(v.Y)
	{}
	explicit Vector2(const Vector3& v);
	explicit Vector2(const Vector4& v);

	inline float GetLength() const
	{
		return Math::Sqrt(X * X + Y * Y);
	}

	inline float GetLengthSquared() const
	{
		return X * X + Y * Y;
	}

	inline Vector2 GetNormalized() const
	{
		Vector2 v = *this;
		v.Normalize();
		return v;
	}

	inline Vector2 GetNormalizedSafe() const
	{
		Vector2 v = *this;
		v.NormalizeSafe();
		return v;
	}

	inline void Normalize()
	{
		float l = GetLength();
		X /= l;
		Y /= l;
	}

	inline void NormalizeSafe()
	{
		float l = GetLength();
		if (Math::Approximately(l, 0.0f))
		{
			return;
		}

		X /= l;
		Y /= l;
	}

	inline Vector2& operator=(const Vector2& v)
	{
		X = v.X;
		Y = v.Y;
		return *this;
	}

	inline Vector2& operator+=(const Vector2& v)
	{
		X += v.X;
		Y += v.Y;
		return *this;
	}

	inline Vector2& operator-=(const Vector2& v)
	{
		X -= v.X;
		Y -= v.Y;
		return *this;
	}

	inline Vector2& operator*=(float scalar)
	{
		X *= scalar;
		Y *= scalar;
		return *this;
	}

	inline Vector2& operator/=(float scalar)
	{
		X /= scalar;
		Y /= scalar;
		return *this;
	}

	inline float& operator[](size_t index)
	{
		assert(index >= 0 && index < 3);
		return *(&X + index);
	}

	inline const float& operator[](size_t index) const
	{
		assert(index >= 0 && index < 3);
		return *(&X + index);
	}

	inline bool operator==(const Vector2& v) const
	{
		return Math::Approximately(X, v.X) && Math::Approximately(Y, v.Y);
	}

	inline bool operator!=(const Vector2& v) const
	{
		return !Math::Approximately(X, v.X) || !Math::Approximately(Y, v.Y);
	}

public:
	inline static float DotProduct(const Vector2& v1, const Vector2& v2)
	{
		return v1.X * v2.X + v1.Y * v2.Y;
	}

	inline static Vector2 Lerp(const Vector2& from, const Vector2& to, float t)
	{
		return (1.0f - t) * from + t * to;
	}
};

inline Vector2 operator*(const Vector2& v, float scalar)
{
	return Vector2(v.X * scalar, v.Y * scalar);
}

inline Vector2 operator*(float scalar, const Vector2& v)
{
	return Vector2(v.X * scalar, v.Y * scalar);
}

inline Vector2 operator/(const Vector2& v, float scalar)
{
	return Vector2(v.X / scalar, v.Y / scalar);
}

inline Vector2 operator+(const Vector2& v1, const Vector2& v2)
{
	return Vector2(v1.X + v2.X, v1.Y + v2.Y);
}

inline Vector2 operator-(const Vector2& v1, const Vector2& v2)
{
	return Vector2(v1.X - v2.X, v1.Y - v2.Y);
}