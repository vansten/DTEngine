#pragma once

#include "Core/Platform.h"
#include "Utility/Math.h"

struct Transform final
{
	friend class Entity;

protected:
	XMMATRIX _modelMatrix;

	XMFLOAT3 _position;
	XMFLOAT3 _rotation;
	XMFLOAT3 _scale;

	bool _shouldCalculateMatrix;

public:
	inline Transform() : _position(0.0f, 0.0f, 0.0f), _rotation(0.0f, 0.0f, 0.0f), _scale(1.0f, 1.0f, 1.0f), _shouldCalculateMatrix(false)
	{

	}

	inline Transform(const Transform& other) : _position(other._position), _rotation(other._rotation), _scale(other._scale), _shouldCalculateMatrix(false)
	{

	}

protected:
	void CalculateModelMatrix(const Transform* parent);

public:
	inline void SetPosition(const XMFLOAT3& newPosition)
	{
		_position = newPosition;
		_shouldCalculateMatrix = true;
	}

	inline void SetRotation(const XMFLOAT3& newRotation)
	{
		_rotation = newRotation;
		_shouldCalculateMatrix = true;
	}

	inline void SetScale(const XMFLOAT3& newScale)
	{
		_scale = newScale;
		_shouldCalculateMatrix = true;
	}

	inline const XMFLOAT3& GetPosition() const
	{
		return _position;
	}

	inline const XMFLOAT3& GetRotation() const
	{
		return _rotation;
	}

	inline const XMFLOAT3& GetScale() const
	{
		return _scale;
	}

	inline const XMMATRIX& GetModelMatrix() const
	{
		return _modelMatrix;
	}

	inline XMFLOAT3 TransformDirection(XMFLOAT3 direction) const
	{
		const XMFLOAT4 direction4(direction.x, direction.y, direction.z, 0.0f);
		const XMVECTOR directionVector = XMLoadFloat4(&direction4);
		const XMVECTOR transformedDirectionVector = XMVector4Transform(directionVector, _modelMatrix);
		XMFLOAT3 transformedDirection;
		XMStoreFloat3(&transformedDirection, transformedDirectionVector);
		return transformedDirection;
	}

	inline XMFLOAT3 GetForward() const
	{
		return TransformDirection(VectorHelpers::Forward);
	}

	inline XMFLOAT3 GetUp() const
	{
		return TransformDirection(VectorHelpers::Up);
	}

	inline XMFLOAT3 GetRight() const
	{
		return TransformDirection(VectorHelpers::Right);
	}
};