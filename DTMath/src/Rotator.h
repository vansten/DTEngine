#pragma once

class Quaternion;
class Rotator;

inline Rotator operator+(const Rotator& r1, const Rotator& r2);
inline Rotator operator-(const Rotator& r1, const Rotator& r2);
inline Rotator operator*(const Rotator& r, float scalar);
inline Rotator operator*(float scalar, const Rotator& r);
inline Rotator operator/(const Rotator& r, float scalar);

// Representation of Euler angles
// Rotation order: Roll->Pitch->Yaw(ZXY) (I guess)
class Rotator final
{
public:
	// Rotation around X axis
	float Pitch;
	// Rotation around Y axis
	float Yaw;
	// Rotation around Z axis
	float Roll;

public:
	inline Rotator() : Pitch(0.0f), Yaw(0.0f), Roll(0.0f)
	{}
	inline Rotator(float pitch, float yaw, float roll) : Pitch(pitch), Yaw(yaw), Roll(roll)
	{}
	inline Rotator(const Rotator& r) : Pitch(r.Pitch), Yaw(r.Yaw), Roll(r.Roll)
	{}

	Quaternion ToQuaternion() const;

public:
	inline static Rotator Lerp(const Rotator& from, const Rotator& to, float t)
	{
		return (1.0f - t) * from + t * to;
	}
};

inline Rotator operator+(const Rotator& r1, const Rotator& r2)
{
	return Rotator(r1.Pitch + r2.Pitch, r1.Yaw + r2.Yaw, r1.Roll + r2.Roll);
}

inline Rotator operator-(const Rotator& r1, const Rotator& r2)
{
	return Rotator(r1.Pitch - r2.Pitch, r1.Yaw - r2.Yaw, r1.Roll - r2.Roll);
}

inline Rotator operator*(const Rotator& r, float scalar)
{
	return Rotator(r.Pitch * scalar, r.Yaw * scalar, r.Roll * scalar);
}

inline Rotator operator*(float scalar, const Rotator& r)
{
	return Rotator(r.Pitch * scalar, r.Yaw * scalar, r.Roll * scalar);
}

inline Rotator operator/(const Rotator& r, float scalar)
{
	return Rotator(r.Pitch / scalar, r.Yaw / scalar, r.Roll / scalar);
}