#pragma once

#include <cassert>

#include "MathFunctions.h"

class Vector3;
class Vector4;

inline Vector3 operator+(const Vector3& v);
inline Vector3 operator-(const Vector3& v);
inline Vector3 operator+(const Vector3& v1, const Vector3& v2);
inline Vector3 operator-(const Vector3& v1, const Vector3& v2);
inline Vector3 operator*(const Vector3& v, float scalar);
inline Vector3 operator*(float scalar, const Vector3& v);
inline Vector3 operator/(const Vector3& v, float scalar);

// Representation of 3D vector
class Vector3 final
{
public:
	float X;
	float Y;
	float Z;

public:
	static const Vector3 ZERO;
	static const Vector3 UNIT_X;
	static const Vector3 UNIT_Y;
	static const Vector3 UNIT_Z;
	static const Vector3 ONE;

public:
	inline Vector3() : X(0.0f), Y(0.0f), Z(0.0f) { }
	inline Vector3(float x, float y, float z) : X(x), Y(y), Z(z) { }
	inline Vector3(const Vector3& v) : X(v.X), Y(v.Y), Z(v.Z) { }
	explicit Vector3(const Vector4& v4);

	inline float Length() const
	{
		return Math::Sqrt(X * X + Y * Y + Z * Z);
	}

	inline float LengthSquared() const
	{
		return X * X + Y * Y + Z * Z;
	}

	inline bool IsNormalized() const
	{
		return Math::Approximately(LengthSquared(), 1.0f);
	}

	// Returns normalized (unit length) version of this vector
	inline Vector3 GetNormalized() const
	{
		Vector3 v = *this;
		v.Normalize();
		return v;
	}

	// Returns normalized (unit length) version of this vector
	// This version performs a check whether vector's length is equal to 0
	// to avoid division by zero
	inline Vector3 GetNormalizedSafe() const
	{
		Vector3 v = *this;
		v.NormalizeSafe();
		return v;
	}

	inline void Normalize()
	{
		*this /= Length();
	}

	inline void NormalizeSafe()
	{
		float length = Length();
		if(Math::Approximately(length, 0.0f))
		{
			return;
		}

		*this /= length;
	}

	inline Vector3& operator=(const Vector3& v)
	{
		X = v.X;
		Y = v.Y;
		Z = v.Z;
		return *this;
	}

	inline Vector3& operator+=(const Vector3& v)
	{
		X += v.X;
		Y += v.Y;
		Z += v.Z;
		return *this;
	}

	inline Vector3& operator-=(const Vector3& v)
	{
		X -= v.X;
		Y -= v.Y;
		Z -= v.Z;
		return *this;
	}

	inline Vector3& operator*=(float scalar)
	{
		X *= scalar;
		Y *= scalar;
		Z *= scalar;
		return *this;
	}

	inline Vector3& operator/=(float scalar)
	{
		X /= scalar;
		Y /= scalar;
		Z /= scalar;
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

	inline bool operator==(const Vector3& v) const
	{
		return Math::Approximately(X, v.X) && Math::Approximately(Y, v.Y) && Math::Approximately(Z, v.Z);
	}

	inline bool operator!=(const Vector3& v) const
	{
		return !Math::Approximately(X, v.X) || !Math::Approximately(Y, v.Y) || !Math::Approximately(Z, v.Z);
	}

public:
	inline static float DotProduct(const Vector3& v1, const Vector3& v2)
	{
		return v1.X * v2.X + v1.Y * v2.Y + v1.Z * v2.Z;
	}

	// Returns closest angle (in degrees) between two vectors
	inline static float AngleBetween(const Vector3& v1, const Vector3& v2)
	{
		float lengths = v1.Length() * v2.Length();
		if(Math::Approximately(lengths, 0.0f))
		{
			return 0.0f;
		}
		return Math::Acos(DotProduct(v1, v2) / lengths) * Math::RAD_TO_DEG;
	}

	inline static Vector3 CrossProduct(const Vector3& v1, const Vector3& v2)
	{
		Vector3 result;

		result.X = v1.Y * v2.Z - v2.Y * v1.Z;
		result.Y = v1.Z * v2.X - v2.Z * v1.X;
		result.Z = v1.X * v2.Y - v2.X * v1.Y;

		return result;
	}

	inline static Vector3 Lerp(const Vector3& from, const Vector3& to, float t)
	{
		return (1.0f - t) * from + t * to;
	}

	// Returns vector constructed from minimum of given vectors' components
	static inline Vector3 Min(const Vector3& v1, const Vector3& v2)
	{
		Vector3 result;

		result.X = Math::Min(v1.X, v2.X);
		result.Y = Math::Min(v1.Y, v2.Y);
		result.Z = Math::Min(v1.Z, v2.Z);

		return result;
	}

	// Returns vector constructed from maximum of given vectors' components
	static inline Vector3 Max(const Vector3& v1, const Vector3& v2)
	{
		Vector3 result;

		result.X = Math::Max(v1.X, v2.X);
		result.Y = Math::Max(v1.Y, v2.Y);
		result.Z = Math::Max(v1.Z, v2.Z);

		return result;
	}

	// Returns vector with length less or equal to given one
	static Vector3 ClampLength(const Vector3& v, float length);
};

inline Vector3 operator+(const Vector3& v)
{
	return Vector3(v.X, v.Y, v.Z);
}

inline Vector3 operator-(const Vector3& v)
{
	return Vector3(-v.X, -v.Y, -v.Z);
}

inline Vector3 operator+(const Vector3& v1, const Vector3& v2)
{
	return Vector3(v1.X + v2.X, v1.Y + v2.Y, v1.Z + v2.Z);
}

inline Vector3 operator-(const Vector3& v1, const Vector3& v2)
{
	return Vector3(v1.X - v2.X, v1.Y - v2.Y, v1.Z - v2.Z);
}

inline Vector3 operator*(const Vector3& v, float scalar)
{
	return Vector3(v.X * scalar, v.Y * scalar, v.Z * scalar);
}

inline Vector3 operator*(float scalar, const Vector3& v)
{
	return Vector3(v.X * scalar, v.Y * scalar, v.Z * scalar);
}

inline Vector3 operator/(const Vector3& v, float scalar)
{
	return Vector3(v.X / scalar, v.Y / scalar, v.Z / scalar);
}