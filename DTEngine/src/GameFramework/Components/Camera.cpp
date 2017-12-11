#include "Camera.h"

#include "GameFramework/GameObject.h"
#include "Core/Window.h"
#include "Debug/Debug.h"

#include "MeshRenderer.h"
#include "Rendering/Material.h"

#include "Utility/Math.h"

WeakPtr<Camera> Camera::_main;
DynamicArray<SharedPtr<Camera>> Camera::_allCameras;

Camera::Camera(SharedPtr<GameObject> owner) : Component(owner)
{

}

Camera::Camera(const Camera& other) : Component(other), _fov(other._fov), _near(other._near), _far(other._far)
{

}

Camera::~Camera()
{

}

void Camera::Resize()
{
	Window& window = GetMainWindow();
	const float32 aspectRatio = window.GetAspectRatio();
	_projectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(_fov), aspectRatio, _near, _far);

	ConstructFrustum();

	GetDebug().Printf(LogVerbosity::Log, CHANNEL_CAMERA, DT_TEXT("Resizing camera for object: %s"), GetOwner()->GetName().c_str());
}

void Camera::DivideVisibleRenderersByRenderQueue(const DynamicArray<SharedPtr<MeshRenderer>>& allRenderers, DynamicArray<SharedPtr<MeshRenderer>>& opaqueRenderers, DynamicArray<SharedPtr<MeshRenderer>>& transparentRenderers)
{
	for(auto renderer : allRenderers)
	{
		if(IsVisible(renderer))
		{
			if(renderer->GetQueue() == RenderQueue::Opaque)
			{
				opaqueRenderers.push_back(renderer);
			}
			else
			{
				transparentRenderers.push_back(renderer);
			}
		}
	}
}

void Camera::ConstructFrustum()
{
	XMMATRIX vp = _viewMatrix * _projectionMatrix;

	// Order: left, right, bottom, top, near, far
	_frustum[0] = Plane(GetMatrixComponentAt(vp, 0, 3) + GetMatrixComponentAt(vp, 0, 0),
						GetMatrixComponentAt(vp, 1, 3) + GetMatrixComponentAt(vp, 1, 0),
						GetMatrixComponentAt(vp, 2, 3) + GetMatrixComponentAt(vp, 2, 0),
						GetMatrixComponentAt(vp, 3, 3) + GetMatrixComponentAt(vp, 3, 0));

	_frustum[1] = Plane(GetMatrixComponentAt(vp, 0, 3) - GetMatrixComponentAt(vp, 0, 0),
						GetMatrixComponentAt(vp, 1, 3) - GetMatrixComponentAt(vp, 1, 0),
						GetMatrixComponentAt(vp, 2, 3) - GetMatrixComponentAt(vp, 2, 0),
						GetMatrixComponentAt(vp, 3, 3) - GetMatrixComponentAt(vp, 3, 0));

	_frustum[2] = Plane(GetMatrixComponentAt(vp, 0, 3) + GetMatrixComponentAt(vp, 0, 1),
						GetMatrixComponentAt(vp, 1, 3) + GetMatrixComponentAt(vp, 1, 1),
						GetMatrixComponentAt(vp, 2, 3) + GetMatrixComponentAt(vp, 2, 1),
						GetMatrixComponentAt(vp, 3, 3) + GetMatrixComponentAt(vp, 3, 1));

	_frustum[3] = Plane(GetMatrixComponentAt(vp, 0, 3) - GetMatrixComponentAt(vp, 0, 1),
						GetMatrixComponentAt(vp, 1, 3) - GetMatrixComponentAt(vp, 1, 1),
						GetMatrixComponentAt(vp, 2, 3) - GetMatrixComponentAt(vp, 2, 1),
						GetMatrixComponentAt(vp, 3, 3) - GetMatrixComponentAt(vp, 3, 1));

	_frustum[4] = Plane(GetMatrixComponentAt(vp, 0, 3) + GetMatrixComponentAt(vp, 0, 2),
						GetMatrixComponentAt(vp, 1, 3) + GetMatrixComponentAt(vp, 1, 2),
						GetMatrixComponentAt(vp, 2, 3) + GetMatrixComponentAt(vp, 2, 2),
						GetMatrixComponentAt(vp, 3, 3) + GetMatrixComponentAt(vp, 3, 2));

	_frustum[5] = Plane(GetMatrixComponentAt(vp, 0, 3) - GetMatrixComponentAt(vp, 0, 2),
						GetMatrixComponentAt(vp, 1, 3) - GetMatrixComponentAt(vp, 1, 2),
						GetMatrixComponentAt(vp, 2, 3) - GetMatrixComponentAt(vp, 2, 2),
						GetMatrixComponentAt(vp, 3, 3) - GetMatrixComponentAt(vp, 3, 2));
}

bool Camera::IsVisible(SharedPtr<MeshRenderer> renderer)
{
	// TODO: Replace GetPosition by renderer->GetBoundingBox();
	return IsInsideFrustum(renderer->GetOwner()->GetTransform()->GetPosition());
}

void Camera::RegisterCamera(SharedPtr<Camera> camera)
{
	_allCameras.push_back(camera);

	static auto sortPredicate = [](const WeakPtr<Camera>& cam1, const WeakPtr<Camera>& cam2)
	{
		SharedPtr<Camera> cam1Shared = cam1.lock();
		SharedPtr<Camera> cam2Shared = cam2.lock();
		if(!cam1Shared)
		{
			return true;
		}
		if(!cam2Shared)
		{
			return false;
		}

		return cam2Shared->_order < cam1Shared->_order;
	};

	std::sort(_allCameras.begin(), _allCameras.end(), sortPredicate);
}

void Camera::UnregisterCamera(SharedPtr<Camera> camera)
{
	auto& found = std::find(_allCameras.begin(), _allCameras.end(), camera);
	if(found != _allCameras.end())
	{
		_allCameras.erase(found);
	}
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

	if(!_main.lock())
	{
		_main = SharedFromThis();
	}

	_fov = 60.0f;
	_near = 0.01f;
	_far = 30.0f;

	_order = 0;
}

void Camera::Shutdown()
{
	Component::Shutdown();

	SharedPtr<Camera> cam = SharedFromThis();
	UnregisterCamera(SharedFromThis());
}

void Camera::PostLoad()
{
	Resize();

	SharedPtr<Transform> ownerTransform = _owner->GetTransform();
	OnOwnerTransformUpdated(ownerTransform);

	RegisterCamera(SharedFromThis());
}

void Camera::OnOwnerTransformUpdated(SharedPtr<Transform> transform)
{
	const XMFLOAT3 forward = transform->GetForward();
	const XMFLOAT3 position = transform->GetPosition();
	_viewMatrix = XMMatrixLookToLH(XMLoadFloat3(&position), XMLoadFloat3(&forward), XMLoadFloat3(&VectorHelpers::Up));

	ConstructFrustum();
}

void Camera::Render(Graphics& graphics, const DynamicArray<SharedPtr<MeshRenderer>>& renderers)
{
	static DynamicArray<SharedPtr<MeshRenderer>> opaqueRenderers;
	static DynamicArray<SharedPtr<MeshRenderer>> transparentRenderers;
	opaqueRenderers.clear();
	transparentRenderers.clear();

	DivideVisibleRenderersByRenderQueue(renderers, opaqueRenderers, transparentRenderers);

	for(auto meshRenderer : opaqueRenderers)
	{
		meshRenderer->GetOwner()->OnWillRender(graphics);
		meshRenderer->OnRender(graphics);
	}
}

void Camera::RenderDebug(Graphics& graphics)
{

}

void Camera::RenderSky(Graphics& graphics)
{

}

bool Camera::IsInsideFrustum(const XMFLOAT3& worldPoint) const
{
	for(uint8 i = 0; i < 6; ++i)
	{
		if(_frustum[i].Dot(worldPoint) < 0.0f)
		{
			return false;
		}
	}

	return true;
}

// Converts from world coordinates to view (camera) space coordinates
XMFLOAT3 Camera::ConvertWorldToViewPoint(const XMFLOAT3& worldPoint) const
{
	XMVECTOR worldVector = XMLoadFloat3(&worldPoint);
	XMVECTOR viewSpaceVector = XMVector3Transform(worldVector, _viewMatrix);
	XMFLOAT3 viewSpacePoint;
	XMStoreFloat3(&viewSpacePoint, viewSpaceVector);

	return viewSpacePoint;
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
