#include "Camera.h"

#include "GameFramework/GameObject.h"
#include "Core/Window.h"

SharedPtr<Camera> Camera::_main = nullptr;

Camera::Camera(SharedPtr<GameObject> owner) : Component(owner)
{

}

Camera::Camera(const Camera& other) : Component(other), _fov(other._fov), _near(other._near), _far(other._far)
{

}

Camera::~Camera()
{

}

SharedPtr<Component> Camera::Copy(SharedPtr<GameObject> newOwner) const
{
	SharedPtr<Camera> copy = SharedPtr<Camera>(new Camera(*this));
	copy->_owner = newOwner;
	return StaticPointerCast<Component>(copy);
}

void Camera::Initialize()
{
	Component::Initialize();

	if(!_main)
	{
		_main = DynamicPointerCast<Camera>(shared_from_this());
	}

	_fov = 60.0f;
	_near = 0.01f;
	_far = 100.0f;

	PostLoad();
}

void Camera::Shutdown()
{
	if(_main == shared_from_this())
	{
		_main = nullptr;
	}

	Component::Shutdown();
}

void Camera::PostLoad()
{
	Window& window = GetMainWindow();
	float32 aspectRatio = window.GetAspectRatio();
	_projectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(_fov), aspectRatio, _near, _far);

	SharedPtr<Transform> ownerTransform = _owner->GetTransform();
	OnOwnerTransformUpdated(ownerTransform);
}

void Camera::OnOwnerTransformUpdated(SharedPtr<Transform> transform)
{
	XMFLOAT3 forward = transform->GetForward();
	XMFLOAT3 position = transform->GetPosition();
	_viewMatrix = XMMatrixLookToLH(XMLoadFloat3(&position), XMLoadFloat3(&forward), XMLoadFloat3(&VectorHelpers::Up));
}

// Converts from screen coordinates to world coordinates
// Returns a point on near camera plane with world coordinate matching passed screen coordinates
// Pass XMINT2 in range [(0, screenWidth), (0, screenHeight)]
XMFLOAT3 Camera::ConvertScreenToWorldPoint(const XMINT2& screenPoint) const
{
	Window& window = GetMainWindow();
	// Cause screenPoint is in [(0, screenWidth), (0, screenHeight)] ranges
	// Offset must be applied to make sure that middle of screen is (0,0) point
	XMINT2 screenPointWithOffset(
								 (uint32)(screenPoint.x - window.GetWidth() * 0.5f),
								 (uint32)(window.GetHeight() * 0.5f - screenPoint.y)
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
	Window& window = GetMainWindow();
	// Applying offset just to make sure that returned screen point is in
	// [(0, screenWidth), (0, screenHeight)] range
	screenPoint.x = (uint32)(screenPoint.x + window.GetWidth() * 0.5f);
	screenPoint.y = (uint32)(window.GetHeight() * 0.5f - screenPoint.y);

	return screenPoint;
}

SharedPtr<Camera> Camera::GetMainCamera()
{
	return _main;
}
