#include "CameraControl.h"

#include "Core/Input.h"
#include "GameFramework/GameObject.h"
#include "GameFramework/Components/Camera.h"

const float32 CameraControl::_xRotationMax = 89.99f;
const float32 CameraControl::_xRotationMin = -89.99f;

CameraControl::CameraControl(GameObject* owner) : Component(owner)
{

}

CameraControl::CameraControl(const CameraControl& other) : Component(other), _movementSpeed(other._movementSpeed), _shiftMultiplier(other._shiftMultiplier), _mouseSensitivity(other._mouseSensitivity)
{

}

CameraControl::~CameraControl()
{

}

CameraControl* CameraControl::Copy(GameObject* newOwner) const
{
	CameraControl* copy = new CameraControl(*this);
	copy->_owner = newOwner;
	return copy;
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
	_previousMousePosition = Input::GetMousePosition();
	return false;
}

bool CameraControl::ONRMBReleased()
{
	_isRMBPressed = false;
	return false;
}

bool CameraControl::OnLMBReleased()
{
	XMINT2 mousePosition = Input::GetMousePosition();
	XMFLOAT3 worldPosition = Camera::GetMainCamera()->ConvertScreenToWorldPoint(mousePosition);
	XMINT2 backToScreen = Camera::GetMainCamera()->ConvertWorldToScreenPoint(worldPosition);
	string s = DT_TEXT("");
	s += std::to_wstring(mousePosition.x);
	s += DT_TEXT(" ");
	s += std::to_wstring(mousePosition.y);
	s += DT_TEXT(" ");
	s += std::to_wstring(worldPosition.x);
	s += DT_TEXT(" ");
	s += std::to_wstring(worldPosition.y);
	s += DT_TEXT(" ");
	s += std::to_wstring(worldPosition.z);
	s += DT_TEXT(" ");
	s += std::to_wstring(backToScreen.x);
	s += DT_TEXT(" ");
	s += std::to_wstring(backToScreen.y);
	s += DT_TEXT("\n");
	OutputDebugString(s.c_str());
	// TODO: Cast a ray through scene and pick hexagon if found
	return false;
}

void CameraControl::Initialize()
{
	Input::BindKeyDownEvent('W', MAKE_FUNCTION_OBJECT(this, &CameraControl::OnWPressed));
	Input::BindKeyUpEvent('W', MAKE_FUNCTION_OBJECT(this, &CameraControl::OnWReleased));
	Input::BindKeyDownEvent('S', MAKE_FUNCTION_OBJECT(this, &CameraControl::OnSPressed));
	Input::BindKeyUpEvent('S', MAKE_FUNCTION_OBJECT(this, &CameraControl::OnSReleased));
	Input::BindKeyDownEvent('A', MAKE_FUNCTION_OBJECT(this, &CameraControl::OnAPressed));
	Input::BindKeyUpEvent('A', MAKE_FUNCTION_OBJECT(this, &CameraControl::OnAReleased));
	Input::BindKeyDownEvent('D', MAKE_FUNCTION_OBJECT(this, &CameraControl::OnDPressed));
	Input::BindKeyUpEvent('D', MAKE_FUNCTION_OBJECT(this, &CameraControl::OnDReleased));
	Input::BindKeyDownEvent(VK_SHIFT, MAKE_FUNCTION_OBJECT(this, &CameraControl::OnShiftPressed));
	Input::BindKeyUpEvent(VK_SHIFT, MAKE_FUNCTION_OBJECT(this, &CameraControl::OnShiftReleased));
	Input::BindMouseDownEvent(VK_RBUTTON, MAKE_FUNCTION_OBJECT(this, &CameraControl::OnRMBPressed));
	Input::BindMouseUpEvent(VK_RBUTTON, MAKE_FUNCTION_OBJECT(this, &CameraControl::ONRMBReleased));
	Input::BindMouseUpEvent(VK_LBUTTON, MAKE_FUNCTION_OBJECT(this, &CameraControl::OnLMBReleased));

	_movementSpeed = 4.0f;
	_shiftMultiplier = 1.0f;
	_mouseSensitivity = 20.0f;

	_isRMBPressed = false;
}

void CameraControl::Update(float32 deltaTime)
{
	if (_isRMBPressed)
	{
		XMFLOAT3 direction = _owner->GetTransform().TransformDirection(_movementVector);
		XMFLOAT3 currentPosition = _owner->GetTransform().GetPosition();
		float32 speedMulDeltaTime = _movementSpeed * deltaTime * _shiftMultiplier;
		currentPosition.x += direction.x * speedMulDeltaTime;
		currentPosition.y += direction.y * speedMulDeltaTime;
		currentPosition.z += direction.z * speedMulDeltaTime;
		_owner->GetTransform().SetPosition(currentPosition);

		XMINT2 mousePosition = Input::GetMousePosition();
		XMINT2 mouseDeltaPosition = mousePosition;
		mouseDeltaPosition.x -= _previousMousePosition.x;
		mouseDeltaPosition.y -= _previousMousePosition.y;

		XMFLOAT3 currentRotation = _owner->GetTransform().GetRotation();
		currentRotation.x += (mouseDeltaPosition.y * deltaTime * _mouseSensitivity);
		currentRotation.x = Clamp(currentRotation.x, _xRotationMin, _xRotationMax);
		currentRotation.y = currentRotation.y + (mouseDeltaPosition.x * deltaTime * _mouseSensitivity);
		_owner->GetTransform().SetRotation(currentRotation);

		_previousMousePosition = mousePosition;
	}
}