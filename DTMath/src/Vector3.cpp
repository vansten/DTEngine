#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "Rotator.h"
#include "Matrix.h"

const Vector3 Vector3::ZERO(0.0f, 0.0f, 0.0f);
const Vector3 Vector3::UNIT_X(1.0f, 0.0f, 0.0f);
const Vector3 Vector3::UNIT_Y(0.0f, 1.0f, 0.0f);
const Vector3 Vector3::UNIT_Z(0.0f, 0.0f, 1.0f);
const Vector3 Vector3::ONE(1.0f, 1.0f, 1.0f);

Vector3::Vector3(const Vector4& v4) : X(v4.X), Y(v4.Y), Z(v4.Z)
{

}

Rotator Vector3::ToRotator() const
{
	// NOTE: Taken from UE4 and replaced:
	// X -> Z
	// Y -> X
	// Z -> -Y (TODO: Figure out why there is a minus)

	Rotator r;

	r.Yaw = Math::Atan2(X, Z) * Math::RAD_TO_DEG;
	r.Pitch = Math::Atan2(-Y, Math::Sqrt(Z * Z + X * X)) * Math::RAD_TO_DEG;

	return r;
}

Quaternion Vector3::ToQuaternion() const
{
	// TODO: Implement proper finding of axis-angle representation
	// and conversion to quaternion

	static const float ALMOST_EQUAL_DOT = 0.998f;
	
	Vector3 direction = GetNormalized();
	Vector3 initialDirection = Vector3::UNIT_Z;

	float dot = Vector3::DotProduct(direction, initialDirection);
	
	if(dot > ALMOST_EQUAL_DOT)
	{
		return Quaternion(0.0f, 1.0f, 0.0f, 0.0f);
	}
	else if(dot < -ALMOST_EQUAL_DOT)
	{
		direction = direction * Rotator(Math::RAD_TO_DEG * 0.5f, 0.0f, 0.0f).ToQuaternion();
	}

	Vector3 axis = Vector3::CrossProduct(initialDirection, direction);

	float angle = Math::Acos(dot);
	float sinus = Math::Sin(angle * 0.5f);
	return Quaternion(axis * sinus, Math::Cos(angle * 0.5f));
}

Vector3 Vector3::ClampLength(const Vector3& v, float length)
{
	if(v.LengthSquared() > length * length)
	{
		return v.GetNormalized() * length;
	}

	return v;
}