#pragma once

#include "Math.h"

struct Plane final
{
private:
	Vector4 _planeVector;

public:
	inline Plane() : _planeVector() { }
	inline Plane(float a, float b, float c, float d) : _planeVector(a, b, c, d) { }

	inline float Dot(const Vector3& worldPoint) const
	{
		return _planeVector.X * worldPoint.X + _planeVector.Y * worldPoint.Y + _planeVector.Z * worldPoint.Z + _planeVector.W;
	}

	inline float Dot(const Vector4& worldVector) const
	{
		return _planeVector.X * worldVector.X + _planeVector.Y * worldVector.Y + _planeVector.Z * worldVector.Z  + _planeVector.W;
	}
};