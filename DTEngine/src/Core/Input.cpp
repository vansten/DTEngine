#include "Input.h"

void Input::BindKeyDown(int32 keyCode, Event<bool>::Delegate::FunctionType function, int32 priority)
{
	_keyDownEvents[keyCode].Bind(function, priority);
}

void Input::UnbindKeyDown(int32 keyCode, Event<bool>::Delegate::FunctionType function)
{
	if(_keyDownEvents.find(keyCode) != _keyDownEvents.end())
	{
		_keyDownEvents[keyCode].Unbind(function);
	}
}

void Input::BindKeyUp(int32 keyCode, Event<bool>::Delegate::FunctionType function, int32 priority)
{
	_keyUpEvents[keyCode].Bind(function, priority);
}

void Input::UnbindKeyUp(int32 keyCode, Event<bool>::Delegate::FunctionType function)
{
	if(_keyUpEvents.find(keyCode) != _keyUpEvents.end())
	{
		_keyUpEvents[keyCode].Unbind(function);
	}
}

void Input::BindMouseDown(int32 mouseCode, Event<bool>::Delegate::FunctionType function, int32 priority)
{
	_mouseDownEvents[mouseCode].Bind(function, priority);
}

void Input::UnbindMouseDown(int32 mouseCode, Event<bool>::Delegate::FunctionType function)
{
	if(_mouseDownEvents.find(mouseCode) != _mouseDownEvents.end())
	{
		_mouseDownEvents[mouseCode].Unbind(function);
	}
}

void Input::BindMouseUp(int32 mouseCode, Event<bool>::Delegate::FunctionType function, int32 priority)
{
	_mouseUpEvents[mouseCode].Bind(function, priority);
}

void Input::UnbindMouseUp(int32 mouseCode, Event<bool>::Delegate::FunctionType function)
{
	if(_mouseUpEvents.find(mouseCode) != _mouseUpEvents.end())
	{
		_mouseUpEvents[mouseCode].Unbind(function);
	}
}

void Input::OnKeyDown(int32 keyCode)
{
	if(_keyDownEvents.find(keyCode) == _keyDownEvents.end())
	{
		return;
	}

	_keyDownEvents[keyCode].ExecuteUntil([](bool value) { return value; });
}

void Input::OnKeyUp(int32 keyCode)
{
	if(_keyUpEvents.find(keyCode) == _keyUpEvents.end())
	{
		return;
	}

	_keyUpEvents[keyCode].ExecuteUntil([](bool value) { return value; });
}

void Input::OnMouseDown(int32 mouseCode)
{
	if(_mouseDownEvents.find(mouseCode) == _mouseDownEvents.end())
	{
		return;
	}

	_mouseDownEvents[mouseCode].ExecuteUntil([](bool value) { return value; });
}

void Input::OnMouseUp(int32 mouseCode)
{
	if(_mouseUpEvents.find(mouseCode) == _mouseUpEvents.end())
	{
		return;
	}

	_mouseUpEvents[mouseCode].ExecuteUntil([](bool value) { return value; });
}

void Input::SetMousePosition(const XMINT2& newMousePosition)
{
	_mousePosition = newMousePosition;
}

XMINT2 Input::GetMousePosition()
{
	return _mousePosition;
}

void Input::Shutdown()
{
	_keyDownEvents.clear();
	_keyUpEvents.clear();
	_mouseDownEvents.clear();
	_mouseUpEvents.clear();
}
