#pragma once

#include "Platform.h"
#include "Utility/Math.h"
#include "Event.h"

class Input final
{
protected:
	Vector2 _mousePosition;

	Dictionary<int, Event<bool>> _keyDownEvents;
	Dictionary<int, Event<bool>> _keyUpEvents;
	Dictionary<int, Event<bool>> _mouseDownEvents;
	Dictionary<int, Event<bool>> _mouseUpEvents;

public:
	void BindKeyDown(int keyCode, Event<bool>::Delegate::FunctionType function, int priority = 0);
	void UnbindKeyDown(int keyCode, Event<bool>::Delegate::FunctionType function);
	void BindKeyUp(int keyCode, Event<bool>::Delegate::FunctionType function, int priority = 0);
	void UnbindKeyUp(int keyCode, Event<bool>::Delegate::FunctionType function);
	void BindMouseDown(int mouseCode, Event<bool>::Delegate::FunctionType function, int priority = 0);
	void UnbindMouseDown(int mouseCode, Event<bool>::Delegate::FunctionType function);
	void BindMouseUp(int mouseCode, Event<bool>::Delegate::FunctionType function, int priority = 0);
	void UnbindMouseUp(int mouseCode, Event<bool>::Delegate::FunctionType function);

	template<typename Class, typename FunctionType = Event<bool>::ClassDelegate<Class>::ClassFunctionType>
	void BindKeyDown(int keyCode, FunctionType function, SharedPtr<Class> object, int priority = 0);
	template<typename Class, typename FunctionType = Event<bool>::ClassDelegate<Class>::ClassFunctionType>
	void UnbindKeyDown(int keyCode, FunctionType function, SharedPtr<Class> object);

	template<typename Class, typename FunctionType = Event<bool>::ClassDelegate<Class>::ClassFunctionType>
	void BindKeyUp(int keyCode, FunctionType function, SharedPtr<Class> object, int priority = 0);
	template<typename Class, typename FunctionType = Event<bool>::ClassDelegate<Class>::ClassFunctionType>
	void UnbindKeyUp(int keyCode, FunctionType function, SharedPtr<Class> object);

	template<typename Class, typename FunctionType = Event<bool>::ClassDelegate<Class>::ClassFunctionType>
	void BindMouseDown(int mouseCode, FunctionType function, SharedPtr<Class> object, int priority = 0);
	template<typename Class, typename FunctionType = Event<bool>::ClassDelegate<Class>::ClassFunctionType>
	void UnbindMouseDown(int mouseCode, FunctionType function, SharedPtr<Class> object);

	template<typename Class, typename FunctionType = Event<bool>::ClassDelegate<Class>::ClassFunctionType>
	void BindMouseUp(int mouseCode, FunctionType function, SharedPtr<Class> object, int priority = 0);
	template<typename Class, typename FunctionType = Event<bool>::ClassDelegate<Class>::ClassFunctionType>
	void UnbindMouseUp(int mouseCode, FunctionType function, SharedPtr<Class> object);

	void OnKeyDown(int keyCode);
	void OnKeyUp(int keyCode);
	void OnMouseDown(int mouseCode);
	void OnMouseUp(int mouseCode);

	void SetMousePosition(const Vector2& newMousePosition);
	Vector2 GetMousePosition();

	void Shutdown();
};

extern Input gInput;

template<typename Class, typename FunctionType>
inline void Input::BindKeyDown(int keyCode, FunctionType function, SharedPtr<Class> object, int priority)
{
	_keyDownEvents[keyCode].Bind(function, object, priority);
}

template<typename Class, typename FunctionType>
inline void Input::UnbindKeyDown(int keyCode, FunctionType function, SharedPtr<Class> object)
{
	if (_keyDownEvents.find(keyCode) != _keyDownEvents.end())
	{
		_keyDownEvents[keyCode].Unbind(function, object);
	}
}

template<typename Class, typename FunctionType>
inline void Input::BindKeyUp(int keyCode, FunctionType function, SharedPtr<Class> object, int priority)
{
	_keyUpEvents[keyCode].Bind(function, object, priority);
}

template<typename Class, typename FunctionType>
inline void Input::UnbindKeyUp(int keyCode, FunctionType function, SharedPtr<Class> object)
{
	if (_keyUpEvents.find(keyCode) != _keyUpEvents.end())
	{
		_keyUpEvents[keyCode].Unbind(function, object);
	}
}

template<typename Class, typename FunctionType>
inline void Input::BindMouseDown(int mouseCode, FunctionType function, SharedPtr<Class> object, int priority)
{
	_mouseDownEvents[mouseCode].Bind(function, object, priority);
}

template<typename Class, typename FunctionType>
inline void Input::UnbindMouseDown(int mouseCode, FunctionType function, SharedPtr<Class> object)
{
	if (_mouseDownEvents.find(mouseCode) != _mouseDownEvents.end())
	{
		_mouseDownEvents[mouseCode].Unbind(function, object);
	}
}

template<typename Class, typename FunctionType>
inline void Input::BindMouseUp(int mouseCode, FunctionType function, SharedPtr<Class> object, int priority)
{
	_mouseUpEvents[mouseCode].Bind(function, object, priority);
}

template<typename Class, typename FunctionType>
inline void Input::UnbindMouseUp(int mouseCode, FunctionType function, SharedPtr<Class> object)
{
	if (_mouseUpEvents.find(mouseCode) != _mouseUpEvents.end())
	{
		_mouseUpEvents[mouseCode].Unbind(function, object);
	}
}
