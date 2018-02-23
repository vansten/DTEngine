#pragma once

#include "GameFramework/Component.h"
#include "Utility/Math.h"
#include "Utility/BoundingBox.h"
#include "Utility/GeometryUtils.h"

class MeshRenderer;
class UIRenderer;

class Camera final : public Component
{
private:

private:
	static WeakPtr<Camera> _main;
	static DynamicArray<SharedPtr<Camera>> _allCameras;

	// Order: left, right, top, bottom, near, far
	Plane _frustum[6];

	Matrix _projectionMatrix;
	Matrix _viewMatrix;

	float _fov;
	float _near;
	float _far;
	short _order;

public:
	Camera(SharedPtr<Entity> owner);
	Camera(const Camera& other);
	virtual ~Camera();

	DECLARE_SHARED_FROM_THIS(Camera)

private:
	void Resize();
	void DetermineVisibleRenderers(const DynamicArray<SharedPtr<MeshRenderer>>& allRenderers, DynamicArray<SharedPtr<MeshRenderer>>& visibleRenderers);
	void DivideRenderersByRenderQueue(const DynamicArray<SharedPtr<MeshRenderer>>& allRenderers, DynamicArray<SharedPtr<MeshRenderer>>& opaqueRenderers, DynamicArray<SharedPtr<MeshRenderer>>& transparentRenderers);
	void ConstructFrustum();

	bool IsVisible(SharedPtr<MeshRenderer> renderer);

	static void RegisterCamera(SharedPtr<Camera> camera);
	static void UnregisterCamera(SharedPtr<Camera> camera);

protected:
	virtual SharedPtr<Component> Copy(SharedPtr<Entity> newOwner) const override;

public:
	virtual void OnInitialize() override;
	virtual void OnShutdown() override;

	virtual void OnOwnerTransformUpdated(const Transform& transform) override;

	void Render(Graphics& graphics, const DynamicArray<SharedPtr<MeshRenderer>>& renderers);
	void RenderDebug(Graphics& graphics);
	void RenderSky(Graphics& graphics);
	void RenderUI(Graphics& graphics, const DynamicArray<SharedPtr<UIRenderer>>& uiRenderers) = delete;

	// Returns true if a worldPoint is inside a frustum
	bool IsInsideFrustum(const Vector3& worldPoint) const;
	// Returns true if one of boundingBox corners is inside frustum
	// NOTE: this function variant assumes that boundingBox has corners in world space already
	bool IsInsideFrustum(const BoundingBox& boundingBox) const;
	// Returns true if one of boundingBox corners transformed by modelToWorld matrix is inside frustum
	// NOTE: this function variant assumes that boundingBox has corners in model/object space
	bool IsInsideFrustum(const BoundingBox& boundingBox, const Matrix& modelToWorld) const;

	// Converts from world coordinates to view (camera) space coordinates
	Vector3 ConvertWorldToViewPoint(const Vector3& worldPoint) const;

	// Converts from screen coordinates to world coordinates
	// Returns a point on near camera plane with world coordinate matching passed screen coordinates
	// Pass XMINT2 in range [(0, screenWidth), (0, screenHeight)]
	Vector3 ConvertScreenToWorldPoint(const Vector2& screenPoint) const;

	// Converts from world coordinates to screen coordinates
	// Returns screen coordinates in range [(0, screenWidth), (0, screenHeight)]
	// Remember! When calling ConvertScreenToWorldPoint and ConvertWorldToScreenPoint right after
	// There might be 1 pixel offset in each of the coordinates
	// It's caused by floating point precision and converting from float to int
	Vector2 ConvertWorldToScreenPoint(const Vector3& worldPoint) const;

	inline const Matrix& GetViewMatrix() const
	{
		return _viewMatrix;
	}

	inline const Matrix& GetProjectionMatrix() const
	{
		return _projectionMatrix;
	}

public:
	inline static SharedPtr<Camera> GetMainCamera()
	{
		return _main.lock();
	}

	inline static const DynamicArray<SharedPtr<Camera>>& GetAllCameras()
	{
		return _allCameras;
	}

	inline static void OnResize()
	{
		for (auto& camera : _allCameras)
		{
			if (camera)
			{
				camera->Resize();
			}
		}
	}
};