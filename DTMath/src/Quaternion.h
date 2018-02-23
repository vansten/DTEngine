#pragma once

#include "Vector3.h"

class Matrix;
class Rotator;

// Representation of a quaternion
class Quaternion final
{
public:
	union
	{
		struct
		{
			float X;
			float Y;
			float Z;
		};
		Vector3 V;
	};
	float W;

public:
	static const Quaternion IDENTITY;

public:
	inline Quaternion() : W(1.0f), X(0.0f), Y(0.0f), Z(0.0f)
	{}
	inline Quaternion(float x, float y, float z, float w) : W(w), X(x), Y(y), Z(z)
	{}
	inline Quaternion(const Vector3& v, float w) : W(w), V(v)
	{}
	inline Quaternion(const Quaternion& q) : W(q.W), V(q.V)
	{}

	inline Quaternion Conjugate() const
	{
		return Quaternion(-V, W);
	}

	inline float Length() const
	{
		return Math::Sqrt(X * X + Y * Y + Z * Z + W * W);
	}

	inline Quaternion GetNormalized() const
	{
		Quaternion q(*this);
		q.Normalize();
		return q;
	}

	inline Quaternion GetNormalizedSafe() const
	{
		Quaternion q(*this);
		q.NormalizeSafe();
		return q;
	}

	inline void Normalize()
	{
		float l = Length();
		V /= l;
		W /= l;
	}

	inline void NormalizeSafe()
	{
		float l = Length();
		if (Math::Approximately(l, 0.0f))
		{
			return;
		}
		V /= l;
		W /= l;
	}

	Rotator ToRotator() const;
	Matrix ToMatrix() const;

	inline Quaternion& operator=(const Quaternion& q)
	{
		V = q.V;
		W = q.W;
		return *this;
	}

public:
	// Forms a quaternion from given rotation axis and angle (in degrees)
	inline static Quaternion FromAxisAngle(const Vector3& axis, float angle)
	{
		float halfCosAngle = 0.5f * Math::Cos(angle * Math::DEG_TO_RAD);
		float halfSinAngle = 0.5f * Math::Sin(angle * Math::DEG_TO_RAD);

		return Quaternion(axis.GetNormalized() * halfSinAngle, halfCosAngle);
	}

	inline static Quaternion Slerp(const Quaternion& from, const Quaternion& to, float t)
	{
		if (Math::Approximately(t, 0.0f))
		{
			return from;
		}
		if (Math::Approximately(t, 1.0f))
		{
			return to;
		}

		float cosHalfTheta = from.W * to.W + from.X * to.X + from.Y * to.Y + from.Z * to.Z;
		float halfTheta = Math::Acos(cosHalfTheta);
		float sinHalfTheta = Math::Sqrt(1.0f - cosHalfTheta);

		if (Math::Approximately(sinHalfTheta, 0.0f))
		{
			return Quaternion(from.V * 0.5f + to.V * 0.5f, from.W * 0.5f + to.W * 0.5f);
		}

		float ratioA = Math::Sin((1.0f - t) * halfTheta) / sinHalfTheta;
		float ratioB = Math::Sin(t * halfTheta) / sinHalfTheta;

		return Quaternion(from.V * ratioA + to.V * ratioB, from.W * ratioA + to.W * ratioB);
	}
};

inline Quaternion operator*(const Quaternion& q1, const Quaternion& q2)
{
	return Quaternion(
		-q1.X * q2.Z + q1.Y * q2.W + q1.Z * q2.X + q1.W * q2.Y,
		q1.X * q2.Y - q1.Y * q2.X + q1.Z * q2.W + q1.W * q2.Z,
		-q1.X * q2.X - q1.Y * q2.Y - q1.Z * q2.Z + q1.W * q2.W,
		q1.X * q2.W + q1.Y * q2.Z - q1.Z * q2.Y + q1.W * q2.X
	);
}

inline Vector3 operator*(const Vector3& v, const Quaternion& q)
{
	const float x = q.W * q.W * v.X + 2.0f * q.Y * q.W * v.Z - 2.0f * q.Z * q.W * v.Y + q.X * q.X *v.X + 2.0f * q.Y * q.X * v.Y + 2.0f * q.Z * q.X * v.Z - q.Z * q.Z * v.X - q.Y * q.Y * v.X;

	const float y = 2.0f * q.X * q.Y * v.X + q.Y  *  q.Y * v.Y + 2.0f * q.Z * q.Y * v.Z + 2.0f * q.W * q.Z * v.X - q.Z * q.Z * v.Y + q.W * q.W * v.Y - 2.0f * q.X * q.W * v.Z - q.X * q.X * v.Y;

	const float z = 2.0f * q.X * q.Z * v.X + 2.0f * q.Y * q.Z * v.Y + q.Z * q.Z * v.Z - 2.0f * q.W * q.Y * v.X - q.Y * q.Y * v.Z + 2.0f * q.W * q.X * v.Y - q.X * q.X * v.Z + q.W * q.W * v.Z;

	return Vector3(x, y, z);
}