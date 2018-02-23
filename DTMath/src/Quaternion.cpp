#include "Quaternion.h"

#include "Rotator.h"
#include "Matrix.h"

const float SINGULARITY_THRESHOLD = 0.499f;

const Quaternion Quaternion::IDENTITY(0.0f, 0.0f, 0.0f, 1.0f);

Rotator Quaternion::ToRotator() const
{
	float yaw;
	float pitch;
	float roll;

	const float singularity = X * Y + Z * W;
	if (singularity > SINGULARITY_THRESHOLD)
	{
		yaw = 2.0f * Math::Atan2(X, W);
		pitch = 0.0f;
		roll = Math::PI_DIV_2;
	}
	else if (singularity < -SINGULARITY_THRESHOLD)
	{
		yaw = -2.0f * Math::Atan2(X, W);
		pitch = 0.0f;
		roll = -Math::PI_DIV_2;
	}
	else
	{
		const float x2 = X * X;
		const float y2 = Y * Y;
		const float z2 = Z * Z;
		yaw = Math::Atan2(2.0f * Y * W - 2.0f * X * Z, 1.0f - 2.0f * y2 - 2.0f * z2);
		pitch = Math::Atan2(2.0f * X * W - 2.0f * Y * Z, 1.0f - 2.0f * x2 - 2.0f * z2);
		roll = Math::Asin(2.0f * singularity);
	}

	return Rotator(pitch * Math::RAD_TO_DEG, yaw * Math::RAD_TO_DEG, roll * Math::RAD_TO_DEG);
}

Matrix Quaternion::ToMatrix() const
{
	const float m11 = 1.0f - 2.0f * Y * Y - 2.0f * Z * Z;
	const float m12 = 2.0f * X * Y + 2.0f * Z * W;
	const float m13 = 2.0f * X * Z - 2.0f * Y * W;
	const float m14 = 0.0f;

	const float m21 = 2.0f * X * Y - 2.0f * Z * W;
	const float m22 = 1.0f - 2.0f * X * X - 2.0f * Z * Z;
	const float m23 = 2.0f * Y * Z + 2.0f * X * W;
	const float m24 = 0.0f;

	const float m31 = 2.0f * X * Z + 2.0f * Y * W;
	const float m32 = 2.0f * Y * Z - 2.0f * X * W;
	const float m33 = 1.0f - 2.0f * X * X - 2.0f * Y * Y;
	const float m34 = 0.0f;

	const float m41 = 0.0f;
	const float m42 = 0.0f;
	const float m43 = 0.0f;
	const float m44 = 1.0f;

	return Matrix(m11, m12, m13, m14,
				  m21, m22, m23, m24,
				  m31, m32, m33, m34,
				  m41, m42, m43, m44);
}
