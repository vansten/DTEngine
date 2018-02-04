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

	XMMATRIX _viewMatrix;
	XMMATRIX _projectionMatrix;

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
	virtual void PostLoad() override;

	virtual void OnOwnerTransformUpdated(const Transform& transform) override;

	void Render(Graphics& graphics, const DynamicArray<SharedPtr<MeshRenderer>>& renderers);
	void RenderDebug(Graphics& graphics);
	void RenderSky(Graphics& graphics);
	void RenderUI(Graphics& graphics, const DynamicArray<SharedPtr<UIRenderer>>& uiRenderers) = delete;

	// Returns true if a worldPoint is inside a frustum
	bool IsInsideFrustum(const XMFLOAT3& worldPoint) const;
	// Returns true if a worldPoint is inside frustum
	bool IsInsideFrustum(const XMVECTOR& worldPoint) const;
	// Returns true if one of boundingBox corners is inside frustum
	// NOTE: this function variant assumes that boundingBox has corners in world space already
	bool IsInsideFrustum(const BoundingBox& boundingBox) const;
	// Returns true if one of boundingBox corners transformed by modelToWorld matrix is inside frustum
	// NOTE: this function variant assumes that boundingBox has corners in model/object space
	bool IsInsideFrustum(const BoundingBox& boundingBox, const XMMATRIX& modelToWorld) const;

	XMFLOAT3 ConvertWorldToViewPoint(const XMFLOAT3& worldPoint) const;
	XMFLOAT3 ConvertScreenToWorldPoint(const XMINT2& screenPoint) const;
	XMINT2 ConvertWorldToScreenPoint(const XMFLOAT3& worldPoint) const;

	inline const XMMATRIX& GetViewMatrix() const
	{
		return _viewMatrix;
	}

	inline const XMMATRIX& GetProjectionMatrix() const
	{
		return _projectionMatrix;
	}

public:
	inline static SharedPtr<Camera> GetMainCamera()
	{
		SharedPtr<Camera> mainShared = _main.lock();
		if(mainShared)
		{
			return mainShared;
		}

		for(auto& camera : _allCameras)
		{
			if(camera)
			{
				_main = camera;
				return camera;
			}
		}

		return nullptr;
	}

	inline static const DynamicArray<SharedPtr<Camera>>& GetAllCameras()
	{
		return _allCameras;
	}

	inline static void OnResize()
	{
		for(auto& camera : _allCameras)
		{
			if(camera)
			{
				camera->Resize();
			}
		}
	}
};