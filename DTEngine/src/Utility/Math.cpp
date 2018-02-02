#include "Math.h"

#include "Core/Platform.h"

XMVECTOR QuaternionBetweenVectors(XMVECTOR from, XMVECTOR to)
{
	from = XMVector3Normalize(from);
	to = XMVector3Normalize(to);
	float d = XMVector3Dot(from, to).m128_f32[0];

	if(d >= 1.0f)
	{
		return XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	}
	else if(d <= -1.0f)
	{
		XMVECTOR axis = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		axis = XMVector3Cross(axis, from);
		if(XMVector3Length(axis).m128_f32[0] == 0.0f)
		{
			axis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
			axis = XMVector3Cross(axis, from);
		}

		const float halfAngle = 0.5f * XM_PI;
		const float sinAngle = sinf(halfAngle);
		XMVECTOR quaternion = XMVectorSet(axis.m128_f32[0] * sinAngle, axis.m128_f32[1] * sinAngle, axis.m128_f32[2] * sinAngle, cosf(halfAngle));

		return XMQuaternionNormalize(quaternion);
	}

	const float s = sqrtf((1.0f + d) * 2.0f);
	const float invs = 1.0f / s;
	XMVECTOR c = XMVector3Cross(from, to);
	XMVectorMultiply(c, XMVectorSet(invs, invs, invs, invs));

	XMVECTOR quaternion = XMVectorSet(c.m128_f32[0], c.m128_f32[1], c.m128_f32[2], s * 0.5f);
	return XMQuaternionNormalize(quaternion);
}

XMFLOAT3 QuaternionToEuler(XMVECTOR quaternion)
{
	quaternion = XMQuaternionNormalize(quaternion);
	XMFLOAT4 q;
	XMStoreFloat4(&q, quaternion);
	const float sqw = q.w*q.w;
	const float sqx = q.x*q.x;
	const float sqy = q.y*q.y;
	const float sqz = q.z*q.z;
	const float test = 2.0f * (q.y*q.w - q.x*q.z);

	XMFLOAT3 euler;

	if(fabs(test - 1.0f) < 0.00001f)
	{
		// heading = rotation about z-axis
		euler.z = -2.0f * atan2(q.x, q.w);
		// bank = rotation about x-axis
		euler.x = 0.0f;
		// attitude = rotation about y-axis
		euler.y = XM_PIDIV2;
	}
	else if(fabs(test + 1.0f) < 0.00001f)
	{
		// heading = rotation about z-axis
		euler.z = 2.0f * atan2(q.x, q.w);
		// bank = rotation about x-axis
		euler.x = 0.0f;
		// attitude = rotation about y-axis
		euler.y = -XM_PIDIV2;
	}
	else
	{
		// heading = rotation about z-axis
		euler.z = atan2(2.0f * (q.x * q.y + q.z * q.w), (sqx - sqy - sqz + sqw));
		// bank = rotation about x-axis
		euler.x = atan2(2.0f * (q.y * q.z + q.x * q.w), (-sqx - sqy + sqz + sqw));
		// attitude = rotation about y-axis
		euler.y = asin(Clamp(test, -1.0f, 1.0f));
	}

	euler.x = XMConvertToDegrees(euler.x);
	euler.y = XMConvertToDegrees(euler.y);
	euler.z = XMConvertToDegrees(euler.z);

	return euler;
}

XMMATRIX RotationFromVector(const XMFLOAT3& vector)
{
	XMFLOAT3 normalizedVector = vector;
	Normalize(normalizedVector);

	XMFLOAT3 xAxis = Cross(VectorHelpers::Up, normalizedVector);
	Normalize(xAxis);
	XMFLOAT3 yAxis = Cross(normalizedVector, xAxis);
	if(Length(yAxis) < 0.0001f)
	{
		yAxis = Cross(normalizedVector, VectorHelpers::Right);
	}
	Normalize(yAxis);

	XMFLOAT3 eyePos(0, 0, 0);
	XMMATRIX rotation = XMMatrixLookToLH(XMLoadFloat3(&eyePos), XMLoadFloat3(&normalizedVector), XMLoadFloat3(&VectorHelpers::Up));
	return XMMatrixInverse(nullptr, rotation);
}
