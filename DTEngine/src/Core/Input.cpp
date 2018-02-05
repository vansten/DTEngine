#include "Input.h"

Input gInput;

void Input::BindKeyDown(int keyCode, Event<bool>::Delegate::FunctionType function, int priority)
{
	_keyDownEvents[keyCode].Bind(function, priority);
}

void Input::UnbindKeyDown(int keyCode, Event<bool>::Delegate::FunctionType function)
{
	const auto& keyDownEvent = _keyDownEvents.find(keyCode);
	if(keyDownEvent != _keyDownEvents.end())
	{
		keyDownEvent->second.Unbind(function);
	}
}

void Input::BindKeyUp(int keyCode, Event<bool>::Delegate::FunctionType function, int priority)
{
	_keyUpEvents[keyCode].Bind(function, priority);
}

void Input::UnbindKeyUp(int keyCode, Event<bool>::Delegate::FunctionType function)
{
	const auto& keyUpEvent = _keyUpEvents.find(keyCode);
	if(keyUpEvent != _keyUpEvents.end())
	{
		keyUpEvent->second.Unbind(function);
	}
}

void Input::BindMouseDown(int mouseCode, Event<bool>::Delegate::FunctionType function, int priority)
{
	_mouseDownEvents[mouseCode].Bind(function, priority);
}

void Input::UnbindMouseDown(int mouseCode, Event<bool>::Delegate::FunctionType function)
{
	const auto& mouseDownEvent = _mouseDownEvents.find(mouseCode);
	if(mouseDownEvent != _mouseDownEvents.end())
	{
		mouseDownEvent->second.Unbind(function);
	}
}

void Input::BindMouseUp(int mouseCode, Event<bool>::Delegate::FunctionType function, int priority)
{
	_mouseUpEvents[mouseCode].Bind(function, priority);
}

void Input::UnbindMouseUp(int mouseCode, Event<bool>::Delegate::FunctionType function)
{
	const auto& mouseUpEvent = _mouseUpEvents.find(mouseCode);
	if(mouseUpEvent != _mouseUpEvents.end())
	{
		mouseUpEvent->second.Unbind(function);
	}
}

void Input::OnKeyDown(int keyCode)
{
	const auto& keyDownEvent = _keyDownEvents.find(keyCode);
	if(keyDownEvent != _keyDownEvents.end())
	{
		keyDownEvent->second.ExecuteUntil([](bool value) { return value; });
	}

}

void Input::OnKeyUp(int keyCode)
{
	const auto& keyUpEvent = _keyUpEvents.find(keyCode);
	if(keyUpEvent != _keyUpEvents.end())
	{
		keyUpEvent->second.ExecuteUntil([](bool value) { return value; });
	}
}

void Input::OnMouseDown(int mouseCode)
{
	const auto& mouseDownEvent = _mouseDownEvents.find(mouseCode);
	if(mouseDownEvent != _mouseDownEvents.end())
	{
		mouseDownEvent->second.ExecuteUntil([](bool value) { return value; });
	}
}

void Input::OnMouseUp(int mouseCode)
{
	const auto& mouseUpEvent = _mouseUpEvents.find(mouseCode);
	if(mouseUpEvent != _mouseUpEvents.end())
	{
		mouseUpEvent->second.ExecuteUntil([](bool value) { return value; });
	}
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
