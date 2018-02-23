#pragma once

#include "Vector4.h"

class Matrix;
class Quaternion;

inline Matrix operator*(const Matrix& m1, const Matrix& m2);
inline Matrix operator*(const Matrix& m, float scalar);
inline Matrix operator*(float scalar, const Matrix& m);
inline Vector4 operator*(const Vector4& v, const Matrix& m);
inline Vector3 operator*(const Vector3& v, const Matrix& m);

class Matrix final
{
public:
	union
	{
		Vector4 Columns[4];
		struct
		{
			float M11, M21, M31, M41;
			float M12, M22, M32, M42;
			float M13, M23, M33, M43;
			float M14, M24, M34, M44;
		};
	};

public:
	static const Matrix IDENTITY;

public:
	inline Matrix()
	{}
	inline Matrix(float m11, float m12, float m13, float m14,
				  float m21, float m22, float m23, float m24,
				  float m31, float m32, float m33, float m34,
				  float m41, float m42, float m43, float m44) :
		M11(m11), M21(m21), M31(m31), M41(m41),
		M12(m12), M22(m22), M32(m32), M42(m42),
		M13(m13), M23(m23), M33(m33), M43(m43),
		M14(m14), M24(m24), M34(m34), M44(m44)
	{}
	inline Matrix(const Matrix& m) :
		M11(m.M11), M21(m.M21), M31(m.M31), M41(m.M41),
		M12(m.M12), M22(m.M22), M32(m.M32), M42(m.M42),
		M13(m.M13), M23(m.M23), M33(m.M33), M43(m.M43),
		M14(m.M14), M24(m.M24), M34(m.M34), M44(m.M44)
	{}

	inline Matrix GetTransposed() const
	{
		return Matrix(M11, M21, M31, M41,
					  M12, M22, M32, M42,
					  M13, M23, M33, M43,
					  M14, M24, M34, M44);
	}

	inline float GetDeterminant() const
	{
		float v0 = M31 * M42 - M32 * M41;
		float v1 = M31 * M43 - M33 * M41;
		float v2 = M31 * M44 - M34 * M41;
		float v3 = M32 * M43 - M33 * M42;
		float v4 = M32 * M44 - M34 * M42;
		float v5 = M33 * M44 - M34 * M43;

		float t00 = +(v5 * M22 - v4 * M23 + v3 * M24);
		float t10 = -(v5 * M21 - v2 * M23 + v1 * M24);
		float t20 = +(v4 * M21 - v2 * M22 + v0 * M24);
		float t30 = -(v3 * M21 - v1 * M22 + v0 * M23);

		return (t00 * M11 + t10 * M12 + t20 * M13 + t30 * M14);
	}

	inline Matrix GetInversed() const
	{
		float v0 = M31 * M42 - M32 * M41;
		float v1 = M31 * M43 - M33 * M41;
		float v2 = M31 * M44 - M34 * M41;
		float v3 = M32 * M43 - M33 * M42;
		float v4 = M32 * M44 - M34 * M42;
		float v5 = M33 * M44 - M34 * M43;

		float t00 = +(v5 * M22 - v4 * M23 + v3 * M24);
		float t10 = -(v5 * M21 - v2 * M23 + v1 * M24);
		float t20 = +(v4 * M21 - v2 * M22 + v0 * M24);
		float t30 = -(v3 * M21 - v1 * M22 + v0 * M23);

		float invDet = 1 / (t00 * M11 + t10 * M12 + t20 * M13 + t30 * M14);

		float d00 = t00 * invDet;
		float d10 = t10 * invDet;
		float d20 = t20 * invDet;
		float d30 = t30 * invDet;

		float d01 = -(v5 * M12 - v4 * M13 + v3 * M14) * invDet;
		float d11 = +(v5 * M11 - v2 * M13 + v1 * M14) * invDet;
		float d21 = -(v4 * M11 - v2 * M12 + v0 * M14) * invDet;
		float d31 = +(v3 * M11 - v1 * M12 + v0 * M13) * invDet;

		v0 = M21 * M42 - M22 * M41;
		v1 = M21 * M43 - M23 * M41;
		v2 = M21 * M44 - M24 * M41;
		v3 = M22 * M43 - M23 * M42;
		v4 = M22 * M44 - M24 * M42;
		v5 = M23 * M44 - M24 * M43;

		float d02 = +(v5 * M12 - v4 * M13 + v3 * M14) * invDet;
		float d12 = -(v5 * M11 - v2 * M13 + v1 * M14) * invDet;
		float d22 = +(v4 * M11 - v2 * M12 + v0 * M14) * invDet;
		float d32 = -(v3 * M11 - v1 * M12 + v0 * M13) * invDet;

		v0 = M32 * M21 - M31 * M22;
		v1 = M33 * M21 - M31 * M23;
		v2 = M34 * M21 - M31 * M24;
		v3 = M33 * M22 - M32 * M23;
		v4 = M34 * M22 - M32 * M24;
		v5 = M34 * M23 - M33 * M24;

		float d03 = -(v5 * M12 - v4 * M13 + v3 * M14) * invDet;
		float d13 = +(v5 * M11 - v2 * M13 + v1 * M14) * invDet;
		float d23 = -(v4 * M11 - v2 * M12 + v0 * M14) * invDet;
		float d33 = +(v3 * M11 - v1 * M12 + v0 * M13) * invDet;

		return Matrix(
			d00, d01, d02, d03,
			d10, d11, d12, d13,
			d20, d21, d22, d23,
			d30, d31, d32, d33);
	}

	Quaternion ToQuaternion() const;

	inline const Vector4& operator[](size_t columnIndex) const
	{
		assert(columnIndex >= 0 && columnIndex < 4);
		return Columns[columnIndex];
	}

public:
	inline static Matrix FromScale(float uniformScale)
	{
		return FromScale(Vector3(uniformScale, uniformScale, uniformScale));
	}

	inline static Matrix FromScale(float x, float y, float z)
	{
		return FromScale(Vector3(x, y, z));
	}

	inline static Matrix FromScale(const Vector3& scale)
	{
		return Matrix(scale.X, 0.0f, 0.0f, 0.0f,
					  0.0f, scale.Y, 0.0f, 0.0f,
					  0.0f, 0.0f, scale.Z, 0.0f,
					  0.0f, 0.0f, 0.0f, 1.0f);
	}

	inline static Matrix FromTranslation(float x, float y, float z)
	{
		return FromTranslation(Vector3(x, y, z));
	}

	inline static Matrix FromTranslation(const Vector3& translation)
	{
		return Matrix(1.0f, 0.0f, 0.0f, 0.0f,
					  0.0f, 1.0f, 0.0f, 0.0f,
					  0.0f, 0.0f, 1.0f, 0.0f,
					  translation.X, translation.Y, translation.Z, 1.0f);
	}

	inline static Matrix FromDirection(const Vector3& direction, const Vector3& up = Vector3::UNIT_Y)
	{
		Vector3 normalized = direction.GetNormalized();
		Vector3 xAxis = Vector3::CrossProduct(up, normalized);
		Vector3 yAxis = Vector3::CrossProduct(normalized, xAxis);
		if (yAxis.LengthSquared() < 0.0001f)
		{
			yAxis = Vector3::CrossProduct(normalized, Vector3::UNIT_X);
		}
		if (xAxis.Length() < 0.0001f)
		{
			xAxis = Vector3::CrossProduct(yAxis, normalized);
		}

		return Matrix(xAxis.X, yAxis.X, normalized.X, 0.0f,
					  xAxis.Y, yAxis.Y, normalized.Y, 0.0f,
					  xAxis.Z, yAxis.Z, normalized.Z, 0.0f,
					  0.0f, 0.0f, 0.0f, 1.0f).GetTransposed();
	}

	inline static Matrix LookAt(const Vector3& eyePosition, const Vector3& eyeTarget, const Vector3& up)
	{
		return LookTo(eyePosition, (eyeTarget - eyePosition).GetNormalized(), up);
	}

	inline static Matrix LookTo(const Vector3& eyePosition, Vector3 eyeDirection, const Vector3& up)
	{
		if (!Math::Approximately(eyeDirection.Length(), 1.0f))
		{
			eyeDirection.Normalize();
		}

		Vector3 right = Vector3::CrossProduct(up, eyeDirection).GetNormalized();

		Vector3 upFromCross = Vector3::CrossProduct(eyeDirection, right).GetNormalized();

		Vector3 negEyePosition = -eyePosition;

		float dotForward = Vector3::DotProduct(eyeDirection, negEyePosition);
		float dotUp = Vector3::DotProduct(upFromCross, negEyePosition);
		float dotRight = Vector3::DotProduct(right, negEyePosition);

		return Matrix(right.X, upFromCross.X, eyeDirection.X, 0.0f,
					  right.Y, upFromCross.Y, eyeDirection.Y, 0.0f,
					  right.Z, upFromCross.Z, eyeDirection.Z, 0.0f,
					  dotRight, dotUp, dotForward, 1.0f);
	}

	inline static Matrix Perspective(float fov, float aspectRatio, float nearZ, float farZ)
	{
		float sinFov = Math::Sin(0.5f * fov * Math::DEG_TO_RAD);
		float cosFov = Math::Cos(0.5f * fov * Math::DEG_TO_RAD);
		float height = cosFov / sinFov;
		float width = height / aspectRatio;
		float fRange = farZ / (farZ - nearZ);

		return Matrix(width, 0.0f, 0.0f, 0.0f,
					  0.0f, height, 0.0f, 0.0f,
					  0.0f, 0.0f, fRange, 1.0f,
					  0.0f, 0.0f, -fRange * nearZ, 0.0f);
	}
};

inline Matrix operator*(const Matrix& m1, const Matrix& m2)
{
	const float m11 = m1.M11 * m2.M11 + m1.M12 * m2.M21 + m1.M13 * m2.M31 + m1.M14 * m2.M41;
	const float m12 = m1.M11 * m2.M12 + m1.M12 * m2.M22 + m1.M13 * m2.M32 + m1.M14 * m2.M42;
	const float m13 = m1.M11 * m2.M13 + m1.M12 * m2.M23 + m1.M13 * m2.M33 + m1.M14 * m2.M43;
	const float m14 = m1.M11 * m2.M14 + m1.M12 * m2.M24 + m1.M13 * m2.M34 + m1.M14 * m2.M44;

	const float m21 = m1.M21 * m2.M11 + m1.M22 * m2.M21 + m1.M23 * m2.M31 + m1.M24 * m2.M41;
	const float m22 = m1.M21 * m2.M12 + m1.M22 * m2.M22 + m1.M23 * m2.M32 + m1.M24 * m2.M42;
	const float m23 = m1.M21 * m2.M13 + m1.M22 * m2.M23 + m1.M23 * m2.M33 + m1.M24 * m2.M43;
	const float m24 = m1.M21 * m2.M14 + m1.M22 * m2.M24 + m1.M23 * m2.M34 + m1.M24 * m2.M44;

	const float m31 = m1.M31 * m2.M11 + m1.M32 * m2.M21 + m1.M33 * m2.M31 + m1.M34 * m2.M41;
	const float m32 = m1.M31 * m2.M12 + m1.M32 * m2.M22 + m1.M33 * m2.M32 + m1.M34 * m2.M42;
	const float m33 = m1.M31 * m2.M13 + m1.M32 * m2.M23 + m1.M33 * m2.M33 + m1.M34 * m2.M43;
	const float m34 = m1.M31 * m2.M14 + m1.M32 * m2.M24 + m1.M33 * m2.M34 + m1.M34 * m2.M44;

	const float m41 = m1.M41 * m2.M11 + m1.M42 * m2.M21 + m1.M43 * m2.M31 + m1.M44 * m2.M41;
	const float m42 = m1.M41 * m2.M12 + m1.M42 * m2.M22 + m1.M43 * m2.M32 + m1.M44 * m2.M42;
	const float m43 = m1.M41 * m2.M13 + m1.M42 * m2.M23 + m1.M43 * m2.M33 + m1.M44 * m2.M43;
	const float m44 = m1.M41 * m2.M14 + m1.M42 * m2.M24 + m1.M43 * m2.M34 + m1.M44 * m2.M44;

	return Matrix(m11, m12, m13, m14,
				  m21, m22, m23, m24,
				  m31, m32, m33, m34,
				  m41, m42, m43, m44);
}

inline Matrix operator*(const Matrix& m, float scalar)
{
	return Matrix(m.M11 * scalar, m.M12 * scalar, m.M13 * scalar, m.M14 * scalar,
				  m.M21 * scalar, m.M22 * scalar, m.M23 * scalar, m.M24 * scalar,
				  m.M31 * scalar, m.M32 * scalar, m.M33 * scalar, m.M34 * scalar,
				  m.M41 * scalar, m.M42 * scalar, m.M43 * scalar, m.M44 * scalar);
}

inline Matrix operator*(float scalar, const Matrix& m)
{
	return Matrix(m.M11 * scalar, m.M12 * scalar, m.M13 * scalar, m.M14 * scalar,
				  m.M21 * scalar, m.M22 * scalar, m.M23 * scalar, m.M24 * scalar,
				  m.M31 * scalar, m.M32 * scalar, m.M33 * scalar, m.M34 * scalar,
				  m.M41 * scalar, m.M42 * scalar, m.M43 * scalar, m.M44 * scalar);
}

inline Vector4 operator*(const Vector4& v, const Matrix& m)
{
	const float x = v.X * m.M11 + v.Y * m.M21 + v.Z * m.M31 + v.W * m.M41;
	const float y = v.X * m.M12 + v.Y * m.M22 + v.Z * m.M32 + v.W * m.M42;
	const float z = v.X * m.M13 + v.Y * m.M23 + v.Z * m.M33 + v.W * m.M43;
	const float w = v.X * m.M14 + v.Y * m.M24 + v.Z * m.M34 + v.W * m.M44;

	return Vector4(x, y, z, w);
}

inline Vector3 operator*(const Vector3& v, const Matrix& m)
{
	const Vector4 v4 = Vector4(v, 0.0f) * m;
	return Vector3(v4.X, v4.Y, v4.Z);
}