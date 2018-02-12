#include "Transform.h"

void Transform::CalculateModelMatrix(const Transform* parent)
{
	_modelMatrix = Matrix::FromScale(_scale) * _rotation.ToMatrix() * Matrix::FromTranslation(_position);

	if(parent)
	{
		_modelMatrix = _modelMatrix * parent->GetModelMatrix();
	}

	_shouldCalculateMatrix = false;
}