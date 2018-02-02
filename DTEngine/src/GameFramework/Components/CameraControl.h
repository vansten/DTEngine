#pragma once

#include "GameFramework/Component.h"
#include "Utility/Math.h"

class CameraControl : public Component
{
private:
	static const float _xRotationMax;
	static const float _xRotationMin;

private:
	float _movementSpeed;
	float _shiftMultiplier;
	float _mouseSensitivity;

	XMFLOAT3 _movementVector;
	XMINT2 _previousMousePosition;

	bool _isRMBPressed;

	float _timer;

public:
	CameraControl(SharedPtr<Entity> owner);
	CameraControl(const CameraControl& other);
	virtual ~CameraControl();

	DECLARE_SHARED_FROM_THIS(CameraControl)

protected:
	virtual SharedPtr<Component> Copy(SharedPtr<Entity> newOwner) const override;

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
	virtual void OnUpdate(float deltaTime) override;
};