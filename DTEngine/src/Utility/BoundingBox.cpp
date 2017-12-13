#include "BoundingBox.h"

BoundingBox::BoundingBox(const XMFLOAT3& min, const XMFLOAT3& max)
{
	_min = XMLoadFloat3(&min);
	_max = XMLoadFloat3(&max);
}

BoundingBox::BoundingBox(const DynamicArray<XMFLOAT3>& positions)
{
	CalculateMinMax(positions);
}

void BoundingBox::CalculateCorners()
{
	// Pairs of min and max multipliers for each corner
	// Sum of multipliers[i] and multipliers[i + 1] must be equal to (1, 1, 1, 1) vector to correctly determine corner
	// For example:
	// multipliers[4] -> (0, 1, 0, 0)
	// multipliers[5] -> (1, 0, 1, 1)
	// sum -> (1, 1, 1, 1)
	static XMVECTOR multipliers[16] =
	{
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f),
		XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 1.0f, 1.0f),
		XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMVectorSet(1.0f, 0.0f, 1.0f, 1.0f),
		XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMVectorSet(1.0f, 1.0f, 0.0f, 1.0f),
		XMVectorSet(0.0f, 1.0f, 1.0f, 0.0f), XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f),
		XMVectorSet(1.0f, 1.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f),
		XMVectorSet(1.0f, 0.0f, 1.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f),
		XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
	};

	for(int8 i = 0; i < 8; ++i)
	{
		// Get min and max multiplier
		const XMVECTOR& minMultiplier = multipliers[2 * i];
		const XMVECTOR& maxMultiplier = multipliers[2 * i + 1];

		// Calculate min and max part
		XMVECTOR minPart = XMVectorMultiply(_min, minMultiplier);
		XMVECTOR maxPart = XMVectorMultiply(_max, maxMultiplier);

		// Push corner which is sum of calculated min and max parts
		_corners.push_back(XMVectorAdd(minPart, maxPart));
	}
}

void BoundingBox::SetMinMax(XMFLOAT3 min, XMFLOAT3 max)
{
	// Ensure that distances in every axis is above 0
	static const float TOLERANCE = 0.0001f;
	static const float HALF_TOLERANCE = 0.5f * TOLERANCE;

	if(abs(min.x - max.x) < TOLERANCE)
	{
		min.x -= HALF_TOLERANCE;
		max.x += HALF_TOLERANCE;
	}
	if(abs(min.y - max.y) < TOLERANCE)
	{
		min.y -= HALF_TOLERANCE;
		max.y += HALF_TOLERANCE;
	}
	if(abs(min.z - max.z) < TOLERANCE)
	{
		min.z -= HALF_TOLERANCE;
		max.z += HALF_TOLERANCE;
	}

	_min = XMLoadFloat3(&min);
	_max = XMLoadFloat3(&max);

	CalculateCorners();
}

void BoundingBox::CalculateMinMax(const DynamicArray<XMFLOAT3>& positions)
{
	XMFLOAT3 minPosition(FLT_MAX, FLT_MAX, FLT_MAX);
	XMFLOAT3 maxPosition(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for(auto& position : positions)
	{
		minPosition = Min(minPosition, position);
		maxPosition = Max(maxPosition, position);
	}

	SetMinMax(minPosition, maxPosition);
}
