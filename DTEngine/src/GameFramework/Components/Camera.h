#pragma once

#include "GameFramework/Component.h"
#include "Utility/Math.h"

class Camera : public Component
{
protected:
	static SharedPtr<Camera> _main;

	XMMATRIX _viewMatrix;
	XMMATRIX _projectionMatrix;

	float32 _fov;
	float32 _near;
	float32 _far;

public:
	Camera(SharedPtr<GameObject> owner);
	Camera(const Camera& other);
	virtual ~Camera();

	DECLARE_SHARED_FROM_THIS(Camera)

protected:
	virtual SharedPtr<Component> Copy(SharedPtr<GameObject> newOwner) const override;

public:
	virtual void Initialize() override;
	virtual void Shutdown() override;
	virtual void PostLoad() override;

	virtual void OnOwnerTransformUpdated(SharedPtr<Transform> transform) override;

	void OnResize();

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

	static SharedPtr<Camera> GetMainCamera();
};