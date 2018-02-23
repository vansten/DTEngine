#include "BoundingBox.h"

BoundingBox::BoundingBox(const Vector3& min, const Vector3& max)
{
	SetMinMax(min, max);
}

BoundingBox::BoundingBox(const DynamicArray<Vector3>& positions)
{
	CalculateMinMax(positions);
}

void BoundingBox::CalculateCorners()
{
	_corners.clear();
	_corners.push_back(Vector3(_min.X, _min.Y, _min.Z));
	_corners.push_back(Vector3(_min.X, _max.Y, _min.Z));
	_corners.push_back(Vector3(_max.X, _min.Y, _min.Z));
	_corners.push_back(Vector3(_max.X, _max.Y, _min.Z));
	_corners.push_back(Vector3(_min.X, _min.Y, _max.Z));
	_corners.push_back(Vector3(_min.X, _max.Y, _max.Z));
	_corners.push_back(Vector3(_max.X, _min.Y, _max.Z));
	_corners.push_back(Vector3(_max.X, _max.Y, _max.Z));
}

void BoundingBox::SetMinMax(Vector3 min, Vector3 max)
{
	// Ensure that distances in every axis is above 0
	static const float TOLERANCE = 0.0001f;
	static const float HALF_TOLERANCE = 0.5f * TOLERANCE;

	if (abs(min.X - max.X) < TOLERANCE)
	{
		min.X -= HALF_TOLERANCE;
		max.X += HALF_TOLERANCE;
	}
	if (abs(min.Y - max.Y) < TOLERANCE)
	{
		min.Y -= HALF_TOLERANCE;
		max.Y += HALF_TOLERANCE;
	}
	if (abs(min.Z - max.Z) < TOLERANCE)
	{
		min.Z -= HALF_TOLERANCE;
		max.Z += HALF_TOLERANCE;
	}

	_min = min;
	_max = max;

	CalculateCorners();
}

void BoundingBox::CalculateMinMax(const DynamicArray<Vector3>& positions)
{
	Vector3 minPosition(FLT_MAX, FLT_MAX, FLT_MAX);
	Vector3 maxPosition(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (auto& position : positions)
	{
		minPosition = Vector3::Min(minPosition, position);
		maxPosition = Vector3::Max(maxPosition, position);
	}

	SetMinMax(minPosition, maxPosition);
}
