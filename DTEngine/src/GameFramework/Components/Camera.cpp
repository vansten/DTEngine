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
	OnResize();

	SharedPtr<Transform> ownerTransform = _owner->GetTransform();
	OnOwnerTransformUpdated(ownerTransform);
}

void Camera::OnOwnerTransformUpdated(SharedPtr<Transform> transform)
{
	const XMFLOAT3 forward = transform->GetForward();
	const XMFLOAT3 position = transform->GetPosition();
	_viewMatrix = XMMatrixLookToLH(XMLoadFloat3(&position), XMLoadFloat3(&forward), XMLoadFloat3(&VectorHelpers::Up));
}

void Camera::OnResize()
{
	Window& window = GetMainWindow();
	const float32 aspectRatio = window.GetAspectRatio();
	_projectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(_fov), aspectRatio, _near, _far);
}

// Converts from screen coordinates to world coordinates
// Returns a point on near camera plane with world coordinate matching passed screen coordinates
// Pass XMINT2 in range [(0, screenWidth), (0, screenHeight)]
XMFLOAT3 Camera::ConvertScreenToWorldPoint(const XMINT2& screenPoint) const
{
	Window& window = GetMainWindow();

	DT_ASSERT(window.GetHeight() * window.GetWidth() != 0);

	XMFLOAT3 viewPosition;
	viewPosition.x = window.GetAspectRatio() * (float32)screenPoint.x / window.GetWidth();
	viewPosition.y = (float32)screenPoint.y / window.GetHeight();
	viewPosition.z = 0.0f;

	const XMMATRIX viewInversed = XMMatrixInverse(&XMMatrixDeterminant(_viewMatrix), _viewMatrix);
	const XMVECTOR worldVector = XMVector3TransformCoord(XMLoadFloat3(&viewPosition), viewInversed);
	XMFLOAT3 worldPosition;
	XMStoreFloat3(&worldPosition, worldVector);
	return worldPosition;
}

// Converts from world coordinates to screen coordinates
// Returns screen coordinates in range [(0, screenWidth), (0, screenHeight)]
// Remember! When calling ConvertScreenToWorldPoint and ConvertWorldToScreenPoint right after
// There might be 1 pixel offset in each of the coordinates
// It's caused by floating point precision and converting from float to int
XMINT2 Camera::ConvertWorldToScreenPoint(const XMFLOAT3& worldPoint) const
{
	Window& window = GetMainWindow();
	DT_ASSERT(window.GetAspectRatio() != 0);

	const XMVECTOR viewVector = XMVector3TransformCoord(XMLoadFloat3(&worldPoint), _viewMatrix);
	XMFLOAT3 viewPosition;
	XMStoreFloat3(&viewPosition, viewVector);
	XMINT2 screenPosition;
	screenPosition.x = (uint32)(viewPosition.x * window.GetWidth() / window.GetAspectRatio());
	screenPosition.y = (uint32)(viewPosition.y * window.GetHeight());

	return screenPosition;
}

SharedPtr<Camera> Camera::GetMainCamera()
{
	return _main;
}
