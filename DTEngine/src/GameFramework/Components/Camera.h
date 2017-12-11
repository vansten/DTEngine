#pragma once

#include "GameFramework/Component.h"
#include "Utility/Math.h"

class MeshRenderer;
class UIRenderer;

class Camera : public Component
{
private:
	struct Plane
	{
	private:
		XMVECTOR _planeVector;

	public:
		inline Plane() { }
		inline Plane(float32 a, float32 b, float32 c, float32 d) 
		{
			XMFLOAT4 tmp(a, b, c, d);
			_planeVector = XMLoadFloat4(&tmp);
			_planeVector = XMPlaneNormalize(_planeVector);
		}

		inline float32 Dot(const XMFLOAT3& worldPoint) const
		{
			return XMPlaneDotCoord(_planeVector, XMLoadFloat3(&worldPoint)).m128_f32[0];
		}
	};

private:
	static WeakPtr<Camera> _main;
	static DynamicArray<SharedPtr<Camera>> _allCameras;

	// Order: left, right, top, bottom, near, far
	Plane _frustum[6];

	XMMATRIX _viewMatrix;
	XMMATRIX _projectionMatrix;

	float32 _fov;
	float32 _near;
	float32 _far;
	int16 _order;

public:
	Camera(SharedPtr<GameObject> owner);
	Camera(const Camera& other);
	virtual ~Camera();

	DECLARE_SHARED_FROM_THIS(Camera)

private:
	void Resize();
	void DivideVisibleRenderersByRenderQueue(const DynamicArray<SharedPtr<MeshRenderer>>& allRenderers, DynamicArray<SharedPtr<MeshRenderer>>& opaqueRenderers, DynamicArray<SharedPtr<MeshRenderer>>& transparentRenderers);
	void ConstructFrustum();

	bool IsVisible(SharedPtr<MeshRenderer> renderer);

	static void RegisterCamera(SharedPtr<Camera> camera);
	static void UnregisterCamera(SharedPtr<Camera> camera);

protected:
	virtual SharedPtr<Component> Copy(SharedPtr<GameObject> newOwner) const override;

public:
	virtual void Initialize() override;
	virtual void Shutdown() override;
	virtual void PostLoad() override;

	virtual void OnOwnerTransformUpdated(SharedPtr<Transform> transform) override;

	void Render(Graphics& graphics, const DynamicArray<SharedPtr<MeshRenderer>>& renderers);
	void RenderDebug(Graphics& graphics);
	void RenderSky(Graphics& graphics);
	void RenderUI(Graphics& graphics, const DynamicArray<SharedPtr<UIRenderer>>& uiRenderers) = delete;

	bool IsInsideFrustum(const XMFLOAT3& viewSpacePosition) const;

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