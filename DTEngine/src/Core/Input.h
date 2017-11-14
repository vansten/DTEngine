#pragma once

#include "App.h"
#include "Platform.h"
#include "Utility/Math.h"
#include "Event.h"


class Input
{
protected:
	XMINT2 _mousePosition;
	
	Dictionary<int32, Event<bool>> _keyDownEvents;
	Dictionary<int32, Event<bool>> _keyUpEvents;
	Dictionary<int32, Event<bool>> _mouseDownEvents;
	Dictionary<int32, Event<bool>> _mouseUpEvents;

public:
	void BindKeyDown(int32 keyCode, Event<bool>::Delegate::FunctionType function, int32 priority = 0);
	void UnbindKeyDown(int32 keyCode, Event<bool>::Delegate::FunctionType function);
	void BindKeyUp(int32 keyCode, Event<bool>::Delegate::FunctionType function, int32 priority = 0);
	void UnbindKeyUp(int32 keyCode, Event<bool>::Delegate::FunctionType function);
	void BindMouseDown(int32 mouseCode, Event<bool>::Delegate::FunctionType function, int32 priority = 0);
	void UnbindMouseDown(int32 mouseCode, Event<bool>::Delegate::FunctionType function);
	void BindMouseUp(int32 mouseCode, Event<bool>::Delegate::FunctionType function, int32 priority = 0);
	void UnbindMouseUp(int32 mouseCode, Event<bool>::Delegate::FunctionType function);

	template<typename Class, typename FunctionType = Event<bool>::ClassDelegate<Class>::ClassFunctionType>
	void BindKeyDown(int32 keyCode, FunctionType function, SharedPtr<Class> object, int32 priority = 0);
	template<typename Class, typename FunctionType = Event<bool>::ClassDelegate<Class>::ClassFunctionType>
	void UnbindKeyDown(int32 keyCode, FunctionType function, SharedPtr<Class> object);

	template<typename Class, typename FunctionType = Event<bool>::ClassDelegate<Class>::ClassFunctionType>
	void BindKeyUp(int32 keyCode, FunctionType function, SharedPtr<Class> object, int32 priority = 0);
	template<typename Class, typename FunctionType = Event<bool>::ClassDelegate<Class>::ClassFunctionType>
	void UnbindKeyUp(int32 keyCode, FunctionType function, SharedPtr<Class> object);
	
	template<typename Class, typename FunctionType = Event<bool>::ClassDelegate<Class>::ClassFunctionType>
	void BindMouseDown(int32 mouseCode, FunctionType function, SharedPtr<Class> object, int32 priority = 0);
	template<typename Class, typename FunctionType = Event<bool>::ClassDelegate<Class>::ClassFunctionType>
	void UnbindMouseDown(int32 mouseCode, FunctionType function, SharedPtr<Class> object);

	template<typename Class, typename FunctionType = Event<bool>::ClassDelegate<Class>::ClassFunctionType>
	void BindMouseUp(int32 mouseCode, FunctionType function, SharedPtr<Class> object, int32 priority = 0);
	template<typename Class, typename FunctionType = Event<bool>::ClassDelegate<Class>::ClassFunctionType>
	void UnbindMouseUp(int32 mouseCode, FunctionType function, SharedPtr<Class> object);

	void OnKeyDown(int32 keyCode);
	void OnKeyUp(int32 keyCode);
	void OnMouseDown(int32 mouseCode);
	void OnMouseUp(int32 mouseCode);

	void SetMousePosition(const XMINT2& newMousePosition);
	XMINT2 GetMousePosition();

	void Shutdown();
};

template<typename Class, typename FunctionType>
inline void Input::BindKeyDown(int32 keyCode, FunctionType function, SharedPtr<Class> object, int32 priority)
{
	_keyDownEvents[keyCode].Bind(function, object, priority);
}

template<typename Class, typename FunctionType>
inline void Input::UnbindKeyDown(int32 keyCode, FunctionType function, SharedPtr<Class> object)
{
	if(_keyDownEvents.find(keyCode) != _keyDownEvents.end())
	{
		_keyDownEvents[keyCode].Unbind(function, object);
	}
}

template<typename Class, typename FunctionType>
inline void Input::BindKeyUp(int32 keyCode, FunctionType function, SharedPtr<Class> object, int32 priority)
{
	_keyUpEvents[keyCode].Bind(function, object, priority);
}

template<typename Class, typename FunctionType>
inline void Input::UnbindKeyUp(int32 keyCode, FunctionType function, SharedPtr<Class> object)
{
	if(_keyUpEvents.find(keyCode) != _keyUpEvents.end())
	{
		_keyUpEvents[keyCode].Unbind(function, object);
	}
}

template<typename Class, typename FunctionType>
inline void Input::BindMouseDown(int32 mouseCode, FunctionType function, SharedPtr<Class> object, int32 priority)
{
	_mouseDownEvents[mouseCode].Bind(function, object, priority);
}

template<typename Class, typename FunctionType>
inline void Input::UnbindMouseDown(int32 mouseCode, FunctionType function, SharedPtr<Class> object)
{
	if(_mouseDownEvents.find(mouseCode) != _mouseDownEvents.end())
	{
		_mouseDownEvents[mouseCode].Unbind(function, object);
	}
}

template<typename Class, typename FunctionType>
inline void Input::BindMouseUp(int32 mouseCode, FunctionType function, SharedPtr<Class> object, int32 priority)
{
	_mouseUpEvents[mouseCode].Bind(function, object, priority);
}

template<typename Class, typename FunctionType>
inline void Input::UnbindMouseUp(int32 mouseCode, FunctionType function, SharedPtr<Class> object)
{
	if(_mouseUpEvents.find(mouseCode) != _mouseUpEvents.end())
	{
		_mouseUpEvents[mouseCode].Unbind(function, object);
	}
}
