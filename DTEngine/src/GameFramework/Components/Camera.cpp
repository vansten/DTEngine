#include "Camera.h"

#include "GameFramework/GameObject.h"
#include "Core/Window.h"

Camera* Camera::_main = nullptr;

Camera::Camera(GameObject* owner) : Component(owner)
{
	if (!_main)
	{
		_main = this;
	}
}

Camera::Camera(const Camera& other) : Component(other), _fov(other._fov), _near(other._near), _far(other._far)
{

}

Camera::~Camera()
{

}

Camera* Camera::Copy(GameObject* newOwner) const
{
	Camera* copy = new Camera(*this);
	copy->_owner = newOwner;
	return copy;
}

void Camera::Initialize()
{
	_fov = 60.0f;
	_near = 0.01f;
	_far = 100.0f;

	PostLoad();
}

void Camera::PostLoad()
{
	float32 aspectRatio = 16.0f / 9.0f;
	if (gWindow)
	{
		aspectRatio = gWindow->GetAspectRatio();
	}
	_projectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(_fov), aspectRatio, _near, _far);

	const Transform& ownerTransform = _owner->GetTransform();
	OnOwnerTransformUpdated(ownerTransform);
}

void Camera::OnOwnerTransformUpdated(const Transform& transform)
{
	XMFLOAT3 forward = transform.GetForward();
	XMFLOAT3 position = transform.GetPosition();
	_viewMatrix = XMMatrixLookToLH(XMLoadFloat3(&position), XMLoadFloat3(&forward), XMLoadFloat3(&VectorHelpers::Up));
}

// Converts from screen coordinates to world coordinates
// Returns a point on near camera plane with world coordinate matching passed screen coordinates
// Pass XMINT2 in range [(0, screenWidth), (0, screenHeight)]
XMFLOAT3 Camera::ConvertScreenToWorldPoint(const XMINT2& screenPoint) const
{
	// Cause screenPoint is in [(0, screenWidth), (0, screenHeight)] ranges
	// Offset must be applied to make sure that middle of screen is (0,0) point
	XMINT2 screenPointWithOffset(
								 (uint32)(screenPoint.x - gWindow->GetWidth() * 0.5f),
								 (uint32)(gWindow->GetHeight() * 0.5f - screenPoint.y)
								);

	XMVECTOR screenVector = XMLoadSInt2(&screenPointWithOffset);
	XMMATRIX vp = XMMatrixMultiply(_viewMatrix, _projectionMatrix);
	XMMATRIX pvInversed = XMMatrixInverse(nullptr, vp);
	XMVECTOR worldVector = XMVector2Transform(screenVector, pvInversed);
	float32 w = worldVector.m128_f32[3];
	if (w != 0.0f)
	{
		XMFLOAT4 divide(w, w, w, w);
		worldVector = XMVectorDivide(worldVector, XMLoadFloat4(&divide));
	}

	XMFLOAT3 worldPoint;
	XMStoreFloat3(&worldPoint, worldVector);
	return worldPoint;
}

// Converts from world coordinates to screen coordinates
// Returns screen coordinates in range [(0, screenWidth), (0, screenHeight)]
// Remember! When calling ConvertScreenToWorldPoint and ConvertWorldToScreenPoint right after
// There might be 1 pixel offset in each of the coordinates
// It's caused by floating point precision and converting from float to int
XMINT2 Camera::ConvertWorldToScreenPoint(const XMFLOAT3& worldPoint) const
{
	XMVECTOR worldVector = XMLoadFloat3(&worldPoint);
	XMMATRIX vp = XMMatrixMultiply(_viewMatrix, _projectionMatrix);
	XMVECTOR screenVector = XMVector3Transform(worldVector, vp);
	float32 w = screenVector.m128_f32[3];	// NOTE: If w is 0 maybe function should return (0,0) and display a warning?
	if (w != 0.0f)
	{
		XMFLOAT4 divide(w, w, w, w);
		screenVector = XMVectorDivide(screenVector, XMLoadFloat4(&divide));
	}


	XMINT2 screenPoint;
	XMStoreSInt2(&screenPoint, screenVector);
	// Applying offset just to make sure that returned screen point is in
	// [(0, screenWidth), (0, screenHeight)] range
	screenPoint.x = (uint32)(screenPoint.x + gWindow->GetWidth() * 0.5f);
	screenPoint.y = (uint32)(gWindow->GetHeight() * 0.5f - screenPoint.y);

	return screenPoint;
}

Camera const* Camera::GetMainCamera()
{
	return _main;
}
