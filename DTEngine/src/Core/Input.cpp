#include "Input.h"

#include <algorithm>

Input* Input::_instance = nullptr;

void Input::SortFunctions(std::vector<InputFunction>& functions)
{
	std::sort(functions.begin(), functions.end(), &InputFunction::Compare);
}

void Input::BindKeyDownEvent(int32 keyCode, std::function<bool()> function, int32 priority)
{
	if (!_instance)
	{
		return;
	}

	std::vector<InputFunction>& functionsVector = _instance->_keyDownsFunctions[keyCode];
	functionsVector.push_back(InputFunction(function, priority));
	
	if(functionsVector.size() > 1)
	{
		SortFunctions(functionsVector);
	}
}

void Input::BindKeyUpEvent(int32 keyCode, std::function<bool()> function, int32 priority)
{
	if (!_instance)
	{
		return;
	}

	std::vector<InputFunction>& functionsVector = _instance->_keyUpFunctions[keyCode];
	functionsVector.push_back(InputFunction(function, priority));
	
	if(functionsVector.size() > 1)
	{
		SortFunctions(functionsVector);
	}
}

void Input::BindMouseDownEvent(int32 mouseCode, std::function<bool()> function, int32 priority)
{
	if (!_instance)
	{
		return;
	}

	std::vector<InputFunction>& functionsVector = _instance->_mouseDownFunctions[mouseCode];
	functionsVector.push_back(InputFunction(function, priority));
	
	if(functionsVector.size() > 1)
	{
		SortFunctions(functionsVector);
	}
}

void Input::BindMouseUpEvent(int32 mouseCode, std::function<bool()> function, int32 priority)
{
	if (!_instance)
	{
		return;
	}

	std::vector<InputFunction>& functionsVector = _instance->_mouseUpFunctions[mouseCode];
	functionsVector.push_back(InputFunction(function, priority));
	
	if(functionsVector.size() > 1)
	{
		SortFunctions(functionsVector);
	}
}

void Input::OnKeyDown(int32 keyCode)
{
	if (!_instance)
	{
		return;
	}

	if(_instance->_keyDownsFunctions.find(keyCode) == _instance->_keyDownsFunctions.end())
	{
		return;
	}
	
	std::vector<InputFunction>& functions = _instance->_keyDownsFunctions[keyCode];
	std::vector<InputFunction>::iterator& it = functions.begin();
	std::vector<InputFunction>::iterator& end = functions.end();
	for(it; it != end; ++it)
	{
		if((*it).Function())
		{
			break;
		}
	}
}

void Input::OnKeyUp(int32 keyCode)
{
	if (!_instance)
	{
		return;
	}

	if(_instance->_keyUpFunctions.find(keyCode) == _instance->_keyUpFunctions.end())
	{
		return;
	}
	
	std::vector<InputFunction>& functions = _instance->_keyUpFunctions[keyCode];
	std::vector<InputFunction>::iterator& it = functions.begin();
	std::vector<InputFunction>::iterator& end = functions.end();
	for(it; it != end; ++it)
	{
		if((*it).Function())
		{
			break;
		}
	}
}

void Input::OnMouseDown(int32 mouseCode)
{
	if (!_instance)
	{
		return;
	}

	if(_instance->_mouseDownFunctions.find(mouseCode) == _instance->_mouseDownFunctions.end())
	{
		return;
	}
	
	std::vector<InputFunction>& functions = _instance->_mouseDownFunctions[mouseCode];
	std::vector<InputFunction>::iterator& it = functions.begin();
	std::vector<InputFunction>::iterator& end = functions.end();
	for(it; it != end; ++it)
	{
		if((*it).Function())
		{
			break;
		}
	}
}

void Input::OnMouseUp(int32 mouseCode)
{
	if (!_instance)
	{
		return;
	}

	if(_instance->_mouseUpFunctions.find(mouseCode) == _instance->_mouseUpFunctions.end())
	{
		return;
	}
	
	std::vector<InputFunction>& functions = _instance->_mouseUpFunctions[mouseCode];
	std::vector<InputFunction>::iterator& it = functions.begin();
	std::vector<InputFunction>::iterator& end = functions.end();
	for(it; it != end; ++it)
	{
		if((*it).Function())
		{
			break;
		}
	}
}

void Input::SetMousePosition(const XMINT2& newMousePosition)
{
	if (!_instance)
	{
		return;
	}

	_instance->_mousePosition = newMousePosition;
}

XMINT2 Input::GetMousePosition()
{
	if (!_instance)
	{
		return XMINT2();
	}

	return _instance->_mousePosition;
}

void Input::Initialize()
{
	if (_instance)
	{
		// Prevent double initialization
		return;
	}

	_instance = new Input();
}

void Input::Shutdown()
{
	if (_instance)
	{
		delete _instance;
		_instance = nullptr;
	}
}
