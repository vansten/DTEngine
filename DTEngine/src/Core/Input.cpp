#include "Input.h"

void Input::BindKeyDown(int keyCode, Event<bool>::Delegate::FunctionType function, int priority)
{
	_keyDownEvents[keyCode].Bind(function, priority);
}

void Input::UnbindKeyDown(int keyCode, Event<bool>::Delegate::FunctionType function)
{
	if(_keyDownEvents.find(keyCode) != _keyDownEvents.end())
	{
		_keyDownEvents[keyCode].Unbind(function);
	}
}

void Input::BindKeyUp(int keyCode, Event<bool>::Delegate::FunctionType function, int priority)
{
	_keyUpEvents[keyCode].Bind(function, priority);
}

void Input::UnbindKeyUp(int keyCode, Event<bool>::Delegate::FunctionType function)
{
	if(_keyUpEvents.find(keyCode) != _keyUpEvents.end())
	{
		_keyUpEvents[keyCode].Unbind(function);
	}
}

void Input::BindMouseDown(int mouseCode, Event<bool>::Delegate::FunctionType function, int priority)
{
	_mouseDownEvents[mouseCode].Bind(function, priority);
}

void Input::UnbindMouseDown(int mouseCode, Event<bool>::Delegate::FunctionType function)
{
	if(_mouseDownEvents.find(mouseCode) != _mouseDownEvents.end())
	{
		_mouseDownEvents[mouseCode].Unbind(function);
	}
}

void Input::BindMouseUp(int mouseCode, Event<bool>::Delegate::FunctionType function, int priority)
{
	_mouseUpEvents[mouseCode].Bind(function, priority);
}

void Input::UnbindMouseUp(int mouseCode, Event<bool>::Delegate::FunctionType function)
{
	if(_mouseUpEvents.find(mouseCode) != _mouseUpEvents.end())
	{
		_mouseUpEvents[mouseCode].Unbind(function);
	}
}

void Input::OnKeyDown(int keyCode)
{
	if(_keyDownEvents.find(keyCode) == _keyDownEvents.end())
	{
		return;
	}

	_keyDownEvents[keyCode].ExecuteUntil([](bool value) { return value; });
}

void Input::OnKeyUp(int keyCode)
{
	if(_keyUpEvents.find(keyCode) == _keyUpEvents.end())
	{
		return;
	}

	_keyUpEvents[keyCode].ExecuteUntil([](bool value) { return value; });
}

void Input::OnMouseDown(int mouseCode)
{
	if(_mouseDownEvents.find(mouseCode) == _mouseDownEvents.end())
	{
		return;
	}

	_mouseDownEvents[mouseCode].ExecuteUntil([](bool value) { return value; });
}

void Input::OnMouseUp(int mouseCode)
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
