#pragma once

#include "Core/Platform.h"
#include "Utility/Math.h"

#include "Physics/Physics.h"

struct Transform final
{
	friend class Entity;

protected:
	Matrix _modelMatrix;

	union
	{
		struct
		{
			Quaternion _rotation;
			Vector3 _position;
		};
		physx::PxTransform _physicsTransform;
	};
	Vector3 _scale;

	bool _shouldCalculateMatrix;

public:
	inline Transform() : _position(0.0f, 0.0f, 0.0f), _rotation(0.0f, 0.0f, 0.0f, 1.0f), _scale(1.0f, 1.0f, 1.0f), _shouldCalculateMatrix(false)
	{

	}

	inline Transform(const Transform& other) : _position(other._position), _rotation(other._rotation), _scale(other._scale), _modelMatrix(other._modelMatrix), _shouldCalculateMatrix(other._shouldCalculateMatrix)
	{

	}

	inline Transform(const physx::PxTransform& transform) : _physicsTransform(transform), _scale(1.0f, 1.0f, 1.0f), _shouldCalculateMatrix(true)
	{

	}

protected:
	void CalculateModelMatrix(const Transform* parent);

public:
	inline void SetPosition(const Vector3& newPosition)
	{
		_position = newPosition;
		_shouldCalculateMatrix = true;
	}

	inline void SetRotation(const Quaternion& newRotation)
	{
		_rotation = newRotation;
		_shouldCalculateMatrix = true;
	}

	inline void SetScale(const Vector3& newScale)
	{
		_scale = newScale;
		_shouldCalculateMatrix = true;
	}

	inline const Vector3& GetPosition() const
	{
		return _position;
	}

	inline const Quaternion& GetRotation() const
	{
		return _rotation;
	}

	inline const Vector3& GetScale() const
	{
		return _scale;
	}

	inline const Matrix& GetModelMatrix() const
	{
		return _modelMatrix;
	}

	inline Vector3 TransformDirection(const Vector3& direction) const
	{
		return direction * _modelMatrix;
	}

	inline Vector3 GetForward() const
	{
		return TransformDirection(Vector3::UNIT_Z);
	}

	inline Vector3 GetUp() const
	{
		return TransformDirection(Vector3::UNIT_Y);
	}

	inline Vector3 GetRight() const
	{
		return TransformDirection(Vector3::UNIT_X);
	}

	inline Transform& operator=(const Transform& t)
	{
		_rotation = t._rotation;
		_position = t._position;
		_scale = t._scale;
		_modelMatrix = t._modelMatrix;
		_shouldCalculateMatrix = t._shouldCalculateMatrix;
		return *this;
	}

	inline Transform& operator=(const physx::PxTransform& t)
	{
		_physicsTransform = t;
		_shouldCalculateMatrix = true;
		return *this;
	}

	inline operator physx::PxTransform() const
	{
		return _physicsTransform;
	}
};