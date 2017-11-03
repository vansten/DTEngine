#pragma once

#include <functional>
#include <map>
#include <vector>

#include "Utility/Math.h"

#define MAKE_FUNCTION_OBJECT(object, function) std::bind(function, object)

class Input
{
protected:
	struct InputFunction
	{
	protected:
		int32 _priority;

	public:
		std::function<bool()> Function;

	public:
		InputFunction(const std::function<bool()>& function, int32 priority) : Function(function), _priority(priority)
		{

		}

		InputFunction(const InputFunction& other) : Function(other.Function), _priority(other._priority)
		{

		}

		static bool Compare(const InputFunction& first, const InputFunction& second)
		{
			return first._priority >= second._priority;
		}
	};

protected:
	XMINT2 _mousePosition;
	
	std::map<int32, std::vector<InputFunction>> _keyDownsFunctions;
	std::map<int32, std::vector<InputFunction>> _keyUpFunctions;
	std::map<int32, std::vector<InputFunction>> _mouseDownFunctions;
	std::map<int32, std::vector<InputFunction>> _mouseUpFunctions;

	static Input* _instance;

protected:
	static void SortFunctions(std::vector<InputFunction>& functions);

public:
	static void BindKeyDownEvent(int32 keyCode, std::function<bool()> function, int32 priority = 0);
	static void BindKeyUpEvent(int32 keyCode, std::function<bool()> function, int32 priority = 0);
	static void BindMouseDownEvent(int32 mouseCode, std::function<bool()> function, int32 priority = 0);
	static void BindMouseUpEvent(int32 mouseCode, std::function<bool()> function, int32 priority = 0);
	
	static void OnKeyDown(int32 keyCode);
	static void OnKeyUp(int32 keyCode);
	static void OnMouseDown(int32 mouseCode);
	static void OnMouseUp(int32 mouseCode);

	static void SetMousePosition(const XMINT2& newMousePosition);
	static XMINT2 GetMousePosition();

	static void Initialize();
	static void Shutdown();
};