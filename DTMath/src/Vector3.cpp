#include "Vector3.h"
#include "Vector4.h"

const Vector3 Vector3::ZERO(0.0f, 0.0f, 0.0f);
const Vector3 Vector3::UNIT_X(1.0f, 0.0f, 0.0f);
const Vector3 Vector3::UNIT_Y(0.0f, 1.0f, 0.0f);
const Vector3 Vector3::UNIT_Z(0.0f, 0.0f, 1.0f);
const Vector3 Vector3::ONE(1.0f, 1.0f, 1.0f);

Vector3::Vector3(const Vector4& v4) : X(v4.X), Y(v4.Y), Z(v4.Z)
{

}

Vector3 Vector3::ClampLength(const Vector3 & v, float length)
{
	if(v.LengthSquared() > length * length)
	{
		return v.GetNormalized() * length;
	}

	return v;
}