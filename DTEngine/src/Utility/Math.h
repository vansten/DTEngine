#pragma once

#include <DirectXMath.h>
#include <cmath>

#include "Core/Platform.h"

using namespace DirectX;

namespace VectorHelpers
{
	const XMFLOAT3 Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	const XMFLOAT3 Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	const XMFLOAT3 Forward = XMFLOAT3(0.0f, 0.0f, 1.0f);
}

XMVECTOR QuaternionBetweenVectors(XMVECTOR from, XMVECTOR to);
XMFLOAT3 QuaternionToEuler(XMVECTOR q);

static void Normalize(XMFLOAT3& vector)
{
	const float32 length = sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
	if(length == 0.0f || length == 1.0f)
	{
		return;
	}
	vector.x /= length;
	vector.y /= length;
	vector.z /= length;
}

static float32 Length(XMFLOAT3& vector)
{
	return sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

static float32 Clamp(float32 value, float32 minValue = 0.0f, float32 maxValue = 1.0f)
{
	return min(max(value, minValue), maxValue);
}

static float32 ToRadians(float32 degrees)
{
	return (float32)(degrees * XM_PI / 180.0f);
}

static float32 ToDegrees(float32 radians)
{
	return (float32)(radians * 180.0f / XM_PI);
}

static inline XMFLOAT3 Cross(const XMFLOAT3& a, const XMFLOAT3& b)
{
	XMVECTOR aVec = XMLoadFloat3(&a);
	XMVECTOR bVec = XMLoadFloat3(&b);

	XMVECTOR crossVec = XMVector3Cross(aVec, bVec);
	XMFLOAT3 cross;
	XMStoreFloat3(&cross, crossVec);

	return cross;
}

static inline float32 Dot(const XMFLOAT3& a, const XMFLOAT3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

XMMATRIX RotationFromVector(const XMFLOAT3& vector);

static inline XMFLOAT3 Min(const XMFLOAT3& a, const XMFLOAT3& b)
{
	return XMFLOAT3(
		min(a.x, b.x),
		min(a.y, b.y),
		min(a.z, b.z)
	);
}

static inline XMFLOAT3 Max(const XMFLOAT3& a, const XMFLOAT3& b)
{
	return XMFLOAT3(
		max(a.x, b.x),
		max(a.y, b.y),
		max(a.z, b.z)
	);
}

static inline float32 GetMatrixComponentAt(XMMATRIX& matrix, uint32 rowIndex, uint32 columnIndex)
{
	assert(rowIndex < 4 && columnIndex < 4);
	return matrix.r[rowIndex].m128_f32[columnIndex];
}

static inline XMFLOAT3 operator+(const XMFLOAT3& a, const XMFLOAT3& b)
{
	return XMFLOAT3(a.x + b.x, a.y + b.y, a.z + b.z);
}

static inline XMINT2 operator-(const XMINT2& a, const XMINT2& b)
{
	return XMINT2(a.x - b.x, a.y - b.y);
}

static inline XMFLOAT3 operator-(const XMFLOAT3& a, const XMFLOAT3& b)
{
	return XMFLOAT3(a.x - b.x, a.y - b.y, a.z - b.z);
}

static inline XMFLOAT3& operator+=(XMFLOAT3& a, const XMFLOAT3& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;

	return a;
}

static inline XMFLOAT3& operator-=(XMFLOAT3& a, const XMFLOAT3& b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;

	return a;
}

static inline XMFLOAT3 operator*(const XMFLOAT3& a, const float32 scalar)
{
	return XMFLOAT3(a.x * scalar, a.y * scalar, a.z * scalar);
}

static inline XMFLOAT3 operator*(const float32 scalar, const XMFLOAT3& a)
{
	return XMFLOAT3(a.x * scalar, a.y * scalar, a.z * scalar);
}