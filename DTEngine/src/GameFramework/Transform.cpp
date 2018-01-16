#include "Transform.h"

void Transform::CalculateModelMatrix(const Transform* parent)
{
	const XMFLOAT3 radianRotation = XMFLOAT3(XMConvertToRadians(_rotation.x), XMConvertToRadians(_rotation.y), XMConvertToRadians(_rotation.z));
	_modelMatrix = XMMatrixScalingFromVector(XMLoadFloat3(&_scale)) *
		XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&radianRotation)) *
		XMMatrixTranslationFromVector(XMLoadFloat3(&_position));

	if(parent)
	{
		_modelMatrix = XMMatrixMultiply(_modelMatrix, parent->GetModelMatrix());
	}

	_shouldCalculateMatrix = false;
}