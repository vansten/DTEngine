#pragma once

#include <DirectXMath.h>
#include "Core/Platform.h"

using namespace DirectX;

namespace VectorHelpers
{
	const XMFLOAT3 Forward = XMFLOAT3(0.0f, 0.0f, 1.0f);
	const XMFLOAT3 Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	const XMFLOAT3 Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
}

static void Normalize(XMFLOAT3& vector)
{
	float32 length = sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
	if(length == 0.0f || length == 1.0f)
	{
		return;
	}
	vector.x /= length;
	vector.y /= length;
	vector.z /= length;
}

static float32 Clamp(float32 value, float32 minValue = 0.0f, float32 maxValue = 1.0f)
{
	return min(max(value, minValue), maxValue);
}