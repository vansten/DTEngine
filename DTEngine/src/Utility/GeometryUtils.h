#pragma once

#include "Math.h"

struct Plane final
{
private:
	XMVECTOR _planeVector;

public:
	inline Plane() : _planeVector() { }
	inline Plane(float a, float b, float c, float d)
	{
		XMFLOAT4 tmp(a, b, c, d);
		_planeVector = XMLoadFloat4(&tmp);
		_planeVector = XMPlaneNormalize(_planeVector);
	}

	inline float Dot(const XMFLOAT3& worldPoint) const
	{
		return XMPlaneDotCoord(_planeVector, XMLoadFloat3(&worldPoint)).m128_f32[0];
	}

	inline float Dot(const XMVECTOR& worldVector) const
	{
		return XMPlaneDotCoord(_planeVector, worldVector).m128_f32[0];
	}
};