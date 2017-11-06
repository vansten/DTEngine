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

public:
	CameraControl(GameObject* owner);
	CameraControl(const CameraControl& other);
	virtual ~CameraControl();

protected:
	virtual CameraControl* Copy(GameObject* newOwner) const override;

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
	virtual void Initialize() override;
	virtual void Update(float32 deltaTime) override;
};