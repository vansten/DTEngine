#include "Input.h"

#include <algorithm>

void Input::SortFunctions(DynamicArray<InputFunction>& functions)
{
	std::sort(functions.begin(), functions.end(), &InputFunction::Compare);
}

void Input::BindKeyDownEvent(int32 keyCode, Function<bool()> function, int32 priority)
{
	DynamicArray<InputFunction>& functions = _keyDownFunctions[keyCode];
	functions.push_back(InputFunction(function, priority));

	if(functions.size() > 1)
	{
		SortFunctions(functions);
	}
}

void Input::BindKeyUpEvent(int32 keyCode, Function<bool()> function, int32 priority)
{
	DynamicArray<InputFunction>& functions = _keyUpFunctions[keyCode];
	functions.push_back(InputFunction(function, priority));

	if(functions.size() > 1)
	{
		SortFunctions(functions);
	}
}

void Input::BindMouseDownEvent(int32 mouseCode, Function<bool()> function, int32 priority)
{
	DynamicArray<InputFunction>& functions = _mouseDownFunctions[mouseCode];
	functions.push_back(InputFunction(function, priority));

	if(functions.size() > 1)
	{
		SortFunctions(functions);
	}
}

void Input::BindMouseUpEvent(int32 mouseCode, Function<bool()> function, int32 priority)
{
	DynamicArray<InputFunction>& functions = _mouseUpFunctions[mouseCode];
	functions.push_back(InputFunction(function, priority));

	if(functions.size() > 1)
	{
		SortFunctions(functions);
	}
}

void Input::OnKeyDown(int32 keyCode)
{
	if(_keyDownFunctions.find(keyCode) == _keyDownFunctions.end())
	{
		return;
	}

	DynamicArray<InputFunction>& functions = _keyDownFunctions[keyCode];
	for(auto function : functions)
	{
		if(function.Callback())
		{
			break;
		}
	}
}

void Input::OnKeyUp(int32 keyCode)
{
	if(_keyUpFunctions.find(keyCode) == _keyUpFunctions.end())
	{
		return;
	}

	DynamicArray<InputFunction>& functions = _keyUpFunctions[keyCode];
	for(auto function : functions)
	{
		if(function.Callback())
		{
			break;
		}
	}
}

void Input::OnMouseDown(int32 mouseCode)
{
	if(_mouseDownFunctions.find(mouseCode) == _mouseDownFunctions.end())
	{
		return;
	}

	DynamicArray<InputFunction>& functions = _mouseDownFunctions[mouseCode];
	for(auto function : functions)
	{
		if(function.Callback())
		{
			break;
		}
	}
}

void Input::OnMouseUp(int32 mouseCode)
{
	if(_mouseUpFunctions.find(mouseCode) == _mouseUpFunctions.end())
	{
		return;
	}

	DynamicArray<InputFunction>& functions = _mouseUpFunctions[mouseCode];
	for(auto function : functions)
	{
		if(function.Callback())
		{
			break;
		}
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
	for(auto pair : _keyDownFunctions)
	{
		pair.second.clear();
	}
	_keyDownFunctions.clear();

	for(auto pair : _keyUpFunctions)
	{
		pair.second.clear();
	}
	_keyUpFunctions.clear();

	for(auto pair : _mouseDownFunctions)
	{
		pair.second.clear();
	}
	_mouseDownFunctions.clear();

	for(auto pair : _mouseUpFunctions)
	{
		pair.second.clear();
	}
	_mouseUpFunctions.clear();
}
