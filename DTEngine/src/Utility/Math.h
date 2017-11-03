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

static float32 Clamp(float32 value, float32 minValue = 0.0f, float32 maxValue = 1.0f)
{
	return min(max(value, minValue), maxValue);
}