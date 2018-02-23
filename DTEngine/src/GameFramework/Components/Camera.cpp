#include "Camera.h"

#include "GameFramework/Entity.h"
#include "Core/Window.h"
#include "Debug/Debug.h"

#include "MeshRenderer.h"
#include "Rendering/Material.h"

#include "Utility/Math.h"

WeakPtr<Camera> Camera::_main;
DynamicArray<SharedPtr<Camera>> Camera::_allCameras;

Camera::Camera(SharedPtr<Entity> owner) : Component(owner), _fov(60.0f), _near(0.01f), _far(1000.0f), _order(0)
{}

Camera::Camera(const Camera& other) : Component(other), _fov(other._fov), _near(other._near), _far(other._far)
{}

Camera::~Camera()
{}

void Camera::Resize()
{
	_projectionMatrix = Matrix::Perspective(_fov, gWindow.GetAspectRatio(), _near, _far);

	ConstructFrustum();

	gDebug.Printf(LogVerbosity::Log, CHANNEL_CAMERA, DT_TEXT("Resizing camera for object: %s"), GetOwner()->GetName().c_str());
}

void Camera::DetermineVisibleRenderers(const DynamicArray<SharedPtr<MeshRenderer>>& allRenderers, DynamicArray<SharedPtr<MeshRenderer>>& visibleRenderers)
{
	for (auto renderer : allRenderers)
	{
		if (IsVisible(renderer))
		{
			visibleRenderers.push_back(renderer);
		}
	}
}

void Camera::DivideRenderersByRenderQueue(const DynamicArray<SharedPtr<MeshRenderer>>& allRenderers, DynamicArray<SharedPtr<MeshRenderer>>& opaqueRenderers, DynamicArray<SharedPtr<MeshRenderer>>& transparentRenderers)
{
	for (auto renderer : allRenderers)
	{
		if (renderer->GetQueue() == RenderQueue::Opaque)
		{
			opaqueRenderers.push_back(renderer);
		}
		else
		{
			transparentRenderers.push_back(renderer);
		}
	}
}

void Camera::ConstructFrustum()
{
	Matrix vp = _viewMatrix * _projectionMatrix;

	// Order: left, right, bottom, top, near, far
	_frustum[0] = Plane(vp[3][0] + vp[0][0],
						vp[3][1] + vp[0][1],
						vp[3][2] + vp[0][2],
						vp[3][3] + vp[0][3]);

	_frustum[1] = Plane(vp[3][0] - vp[0][0],
						vp[3][1] - vp[0][1],
						vp[3][2] - vp[0][2],
						vp[3][3] - vp[0][3]);

	_frustum[2] = Plane(vp[3][0] + vp[1][0],
						vp[3][1] + vp[1][1],
						vp[3][2] + vp[1][2],
						vp[3][3] + vp[1][3]);

	_frustum[3] = Plane(vp[3][0] - vp[1][0],
						vp[3][1] - vp[1][1],
						vp[3][2] - vp[1][2],
						vp[3][3] - vp[1][3]);

	_frustum[4] = Plane(vp[3][0] + vp[2][0],
						vp[3][1] + vp[2][1],
						vp[3][2] + vp[2][2],
						vp[3][3] + vp[2][3]);

	_frustum[5] = Plane(vp[3][0] - vp[2][0],
						vp[3][1] - vp[2][1],
						vp[3][2] - vp[2][2],
						vp[3][3] - vp[2][3]);
}

bool Camera::IsVisible(SharedPtr<MeshRenderer> renderer)
{
	// Check if bounding box of given renderer is inside frustum
	// Pass also a model-to-world matrix
	return IsInsideFrustum(renderer->GetBoundingBox(), renderer->GetOwner()->GetTransform().GetModelMatrix());
}

void Camera::RegisterCamera(SharedPtr<Camera> camera)
{
	_allCameras.push_back(camera);

	const static auto sortPredicate = [](const WeakPtr<Camera>& cam1, const WeakPtr<Camera>& cam2)
	{
		SharedPtr<Camera> cam1Shared = cam1.lock();
		SharedPtr<Camera> cam2Shared = cam2.lock();
		if (!cam1Shared)
		{
			return true;
		}
		if (!cam2Shared)
		{
			return false;
		}

		return cam2Shared->_order < cam1Shared->_order;
	};

	std::sort(_allCameras.begin(), _allCameras.end(), sortPredicate);

	if (_allCameras.size() > 0)
	{
		_main = _allCameras[0];
	}
}

void Camera::UnregisterCamera(SharedPtr<Camera> camera)
{
	auto& found = std::find(_allCameras.begin(), _allCameras.end(), camera);
	if (found != _allCameras.end())
	{
		_allCameras.erase(found);
	}

	if (_allCameras.size() > 0)
	{
		_main = _allCameras[0];
	}
}

SharedPtr<Component> Camera::Copy(SharedPtr<Entity> newOwner) const
{
	SharedPtr<Camera> copy = SharedPtr<Camera>(new Camera(*this));
	copy->_owner = newOwner;
	return StaticPointerCast<Component>(copy);
}

void Camera::OnInitialize()
{
	Component::OnInitialize();

	Resize();

	OnOwnerTransformUpdated(_owner->GetTransform());

	RegisterCamera(SharedFromThis());
}

void Camera::OnShutdown()
{
	Component::OnShutdown();

	SharedPtr<Camera> cam = SharedFromThis();
	UnregisterCamera(SharedFromThis());
}

void Camera::OnOwnerTransformUpdated(const Transform& transform)
{
	_viewMatrix = Matrix::LookTo(transform.GetPosition(), transform.GetForward(), Vector3::UNIT_Y);

	ConstructFrustum();
}

void Camera::Render(Graphics& graphics, const DynamicArray<SharedPtr<MeshRenderer>>& renderers)
{
	static DynamicArray<SharedPtr<MeshRenderer>> opaqueRenderers;
	static DynamicArray<SharedPtr<MeshRenderer>> transparentRenderers;
	static DynamicArray<SharedPtr<MeshRenderer>> visibleRenderers;
	opaqueRenderers.clear();
	transparentRenderers.clear();
	visibleRenderers.clear();

	DetermineVisibleRenderers(renderers, visibleRenderers);
	DivideRenderersByRenderQueue(visibleRenderers, opaqueRenderers, transparentRenderers);

	for (auto& meshRenderer : opaqueRenderers)
	{
		meshRenderer->GetOwner()->Render(graphics);
	}

	for (auto& meshRenderer : transparentRenderers)
	{
		meshRenderer->GetOwner()->Render(graphics);
	}
}

void Camera::RenderDebug(Graphics& graphics)
{
#if DT_DEBUG

	// Set render state to debug wireframe
	graphics.SetRenderState(Graphics::CommonRenderStates::WireframeRenderState);
	graphics.SetObject(nullptr);

	// Draw all debug draws
	const DynamicArray<DebugDrawGeometry>& debugDraws = gDebug.GetDraws();
	for (auto& draw : debugDraws)
	{
		draw.Render(graphics);
	}

#endif
}

void Camera::RenderSky(Graphics& graphics)
{

}

bool Camera::IsInsideFrustum(const Vector3& worldPoint) const
{
	for (unsigned char i = 0; i < 6; ++i)
	{
		// Check if worldPoint is outside a frustum ith plane
		if (_frustum[i].Dot(worldPoint) < 0.0f)
		{
			// Return false if condition is met
			return false;
		}
	}

	return true;
}

bool Camera::IsInsideFrustum(const BoundingBox& boundingBox) const
{
	return IsInsideFrustum(boundingBox, Matrix::IDENTITY);
}

bool Camera::IsInsideFrustum(const BoundingBox& boundingBox, const Matrix& modelToWorld) const
{
	const DynamicArray<Vector3>& corners = boundingBox.GetCorners();
	for (unsigned char i = 0; i < 6; ++i)
	{
		bool liesBehind = true;
		for (auto& corner : corners)
		{
			// Calculate world position of a corner
			Vector4 worldCorner = Vector4(corner, 1) * modelToWorld;
			if (_frustum[i].Dot(worldCorner) > 0.0f)
			{
				liesBehind = false;
				break;
			}
		}

		if (liesBehind)
		{
			return false;
		}
	}

	return true;
}

Vector3 Camera::ConvertWorldToViewPoint(const Vector3& worldPoint) const
{
	return Vector3(Vector4(worldPoint, 1) * _viewMatrix);
}

Vector3 Camera::ConvertScreenToWorldPoint(const Vector2& screenPoint) const
{
	DT_ASSERT(gWindow.GetHeight() * gWindow.GetWidth() != 0, "");

	Vector3 viewPosition;
	viewPosition.X = gWindow.GetAspectRatio() * screenPoint.X / gWindow.GetWidth();
	viewPosition.Y = screenPoint.Y / gWindow.GetHeight();
	viewPosition.Z = 0.0f;

	Matrix viewInversed = _viewMatrix.GetInversed();
	return Vector3(Vector4(viewPosition, 1.0f) * viewInversed);
}

Vector2 Camera::ConvertWorldToScreenPoint(const Vector3& worldPoint) const
{
	DT_ASSERT(gWindow.GetAspectRatio() != 0, "");

	Vector3 viewPoint = Vector3(Vector4(worldPoint, 1) * _viewMatrix);

	Vector2 screenPosition;
	screenPosition.X = viewPoint.X * gWindow.GetWidth() / gWindow.GetAspectRatio();
	screenPosition.Y = viewPoint.Y * gWindow.GetHeight();

	return screenPosition;
}
