#include "Rotator.h"

#include "Quaternion.h"
#include "Matrix.h"

Quaternion Rotator::ToQuaternion() const
{
	const float cosYaw = Math::Cos(Yaw * Math::DEG_TO_RAD * 0.5f);
	const float cosPitch = Math::Cos(Pitch * Math::DEG_TO_RAD * 0.5f);
	const float cosRoll = Math::Cos(Roll * Math::DEG_TO_RAD * 0.5f);
	const float sinYaw = Math::Sin(Yaw * Math::DEG_TO_RAD * 0.5f);
	const float sinPitch = Math::Sin(Pitch * Math::DEG_TO_RAD * 0.5f);
	const float sinRoll = Math::Sin(Roll * Math::DEG_TO_RAD * 0.5f);

	const float x = sinPitch * cosYaw * cosRoll + cosPitch * sinYaw * sinRoll;
	const float y = cosPitch * sinYaw * cosRoll - sinPitch * cosYaw * sinRoll;
	const float z = cosPitch * cosYaw * sinRoll - sinPitch * sinYaw * cosRoll;
	const float w = cosPitch * cosYaw * cosRoll + sinPitch * sinYaw * sinRoll;

	return Quaternion(x, y, z, w);
}
