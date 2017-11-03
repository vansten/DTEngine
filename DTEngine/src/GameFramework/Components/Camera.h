#pragma once

#include "GameFramework/Component.h"
#include "Utility/Math.h"

class Camera : public Component
{
protected:
	static Camera* _main;

	XMMATRIX _viewMatrix;
	XMMATRIX _projectionMatrix;

	float32 _fov;
	float32 _near;
	float32 _far;

public:
	Camera(GameObject* owner);
	Camera(const Camera& other);

	virtual void Initialize() override;
	virtual void PostLoad() override;

	virtual void OnOwnerTransformUpdated(const Transform& transform) override;

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

	static Camera const* GetMainCamera();
};