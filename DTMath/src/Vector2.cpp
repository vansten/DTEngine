#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

const Vector2 Vector2::ZERO(0.0f, 0.0f);
const Vector2 Vector2::UNIT_X(1.0f, 0.0f);
const Vector2 Vector2::UNIT_Y(0.0f, 1.0f);
const Vector2 Vector2::ONE(1.0f, 1.0f);

Vector2::Vector2(const Vector3& v) : X(v.X), Y(v.Y)
{

}

Vector2::Vector2(const Vector4& v) : X(v.X), Y(v.Y)
{

}
