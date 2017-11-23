#include "CameraControl.h"

#include "Core/Input.h"
#include "GameFramework/GameObject.h"
#include "GameFramework/Components/Camera.h"

#include "Debug/Debug.h"

const float32 CameraControl::_xRotationMax = 89.99f;
const float32 CameraControl::_xRotationMin = -89.99f;

CameraControl::CameraControl(SharedPtr<GameObject> owner) : Component(owner)
{
}

CameraControl::CameraControl(const CameraControl& other) : Component(other), _movementSpeed(other._movementSpeed), _shiftMultiplier(other._shiftMultiplier), _mouseSensitivity(other._mouseSensitivity)
{

}

CameraControl::~CameraControl()
{

}

SharedPtr<Component> CameraControl::Copy(SharedPtr<GameObject> newOwner) const
{
	SharedPtr<CameraControl> copy = SharedPtr<CameraControl>(new CameraControl(*this));
	copy->_owner = newOwner;
	return StaticPointerCast<Component>(copy);
}

bool CameraControl::OnWPressed()
{
	_movementVector.z += 1.0f;
	return false;
}

bool CameraControl::OnWReleased()
{
	_movementVector.z -= 1.0f;
	return false;
}

bool CameraControl::OnSPressed()
{
	_movementVector.z -= 1.0f;
	return false;
}

bool CameraControl::OnSReleased()
{
	_movementVector.z += 1.0f;
	return false;
}

bool CameraControl::OnAPressed()
{
	_movementVector.x -= 1.0f;
	return false;
}

bool CameraControl::OnAReleased()
{
	_movementVector.x += 1.0f;
	return false;
}

bool CameraControl::OnDPressed()
{
	_movementVector.x += 1.0f;
	return false;
}

bool CameraControl::OnDReleased()
{
	_movementVector.x -= 1.0f;
	return false;
}

bool CameraControl::OnShiftPressed()
{
	_shiftMultiplier = 3.0f;
	return false;
}

bool CameraControl::OnShiftReleased()
{
	_shiftMultiplier = 1.0f;
	return false;
}

bool CameraControl::OnRMBPressed()
{
	_isRMBPressed = true;
	_previousMousePosition = GetInput().GetMousePosition();
	return false;
}

bool CameraControl::ONRMBReleased()
{
	_isRMBPressed = false;
	return false;
}

bool CameraControl::OnLMBReleased()
{
	XMINT2 mousePosition = GetInput().GetMousePosition();
	XMFLOAT3 worldPosition = Camera::GetMainCamera()->ConvertScreenToWorldPoint(mousePosition);
	XMINT2 backToScreen = Camera::GetMainCamera()->ConvertWorldToScreenPoint(worldPosition);

	GetDebug().Printf(LogVerbosity::Log, CHANNEL_CAMERA, DT_TEXT("%i %i %.2f %.2f %.2f %i %i"), mousePosition.x, mousePosition.y, worldPosition.x, worldPosition.y, worldPosition.z, backToScreen.x, backToScreen.y);
	
	// TODO: Cast a ray through scene and pick hexagon if found
	return false;
}

void CameraControl::Initialize()
{
	Input& input = GetInput();

	input.BindKeyDown('W', &CameraControl::OnWPressed, SharedFromThis());
	input.BindKeyUp('W', &CameraControl::OnWReleased, SharedFromThis());
	input.BindKeyDown('S', &CameraControl::OnSPressed, SharedFromThis());
	input.BindKeyUp('S', &CameraControl::OnSReleased, SharedFromThis());
	input.BindKeyDown('A', &CameraControl::OnAPressed, SharedFromThis());
	input.BindKeyUp('A', &CameraControl::OnAReleased, SharedFromThis());
	input.BindKeyDown('D', &CameraControl::OnDPressed, SharedFromThis());
	input.BindKeyUp('D', &CameraControl::OnDReleased, SharedFromThis());
	input.BindKeyDown(VK_SHIFT, &CameraControl::OnShiftPressed, SharedFromThis());
	input.BindKeyUp(VK_SHIFT, &CameraControl::OnShiftReleased, SharedFromThis());

	input.BindMouseDown(VK_RBUTTON, &CameraControl::OnRMBPressed, SharedFromThis());
	input.BindMouseUp(VK_RBUTTON, &CameraControl::ONRMBReleased, SharedFromThis());
	input.BindMouseUp(VK_LBUTTON, &CameraControl::OnLMBReleased, SharedFromThis());

	_movementSpeed = 4.0f;
	_shiftMultiplier = 1.0f;
	_mouseSensitivity = 20.0f;

	_isRMBPressed = false;
}

void CameraControl::Update(float32 deltaTime)
{
	if (_isRMBPressed)
	{
		const XMFLOAT3 direction = _owner->GetTransform()->TransformDirection(_movementVector);
		const float32 speedMulDeltaTime = _movementSpeed * deltaTime * _shiftMultiplier;

		XMFLOAT3 currentPosition = _owner->GetTransform()->GetPosition();
		currentPosition += direction * speedMulDeltaTime;
		_owner->GetTransform()->SetPosition(currentPosition);

		const XMINT2 mousePosition = GetInput().GetMousePosition();
		XMINT2 mouseDeltaPosition = mousePosition - _previousMousePosition;

		XMFLOAT3 currentRotation = _owner->GetTransform()->GetRotation();
		currentRotation.x += (mouseDeltaPosition.y * deltaTime * _mouseSensitivity);
		currentRotation.x = Clamp(currentRotation.x, _xRotationMin, _xRotationMax);
		currentRotation.y = currentRotation.y + (mouseDeltaPosition.x * deltaTime * _mouseSensitivity);
		_owner->GetTransform()->SetRotation(currentRotation);

		_previousMousePosition = mousePosition;
	}
}