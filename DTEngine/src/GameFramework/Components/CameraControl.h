#pragma once

#include "GameFramework/Component.h"
#include "Utility/Math.h"

class CameraControl : public Component
{
private:
	static const float32 _xRotationMax;
	static const float32 _xRotationMin;

private:
	float32 _movementSpeed;
	float32 _shiftMultiplier;
	float32 _mouseSensitivity;

	XMFLOAT3 _movementVector;
	XMINT2 _previousMousePosition;

	bool _isRMBPressed;

	float32 _timer;

public:
	CameraControl(SharedPtr<GameObject> owner);
	CameraControl(const CameraControl& other);
	virtual ~CameraControl();

	DECLARE_SHARED_FROM_THIS(CameraControl)

protected:
	virtual SharedPtr<Component> Copy(SharedPtr<GameObject> newOwner) const override;

	bool OnWPressed();
	bool OnWReleased();
	bool OnSPressed();
	bool OnSReleased();
	bool OnAPressed();
	bool OnAReleased();
	bool OnDPressed();
	bool OnDReleased();
	bool OnShiftPressed();
	bool OnShiftReleased();
	bool OnRMBPressed();
	bool ONRMBReleased();
	bool OnLMBReleased();

public:
	virtual void OnInitialize() override;
	virtual void OnUpdate(float32 deltaTime) override;
};