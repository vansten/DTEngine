#pragma once

#include "App.h"
#include "Platform.h"
#include "Utility/Math.h"

#define MAKE_FUNCTION_OBJECT(object, function) Bind(function, object)

class Input
{
protected:
	struct InputFunction
	{
	protected:
		int32 _priority;

	public:
		Function<bool()> Callback;

	public:
		InputFunction(const Function<bool()>& function, int32 priority) : Callback(function), _priority(priority)
		{

		}

		InputFunction(const InputFunction& other) : Callback(other.Callback), _priority(other._priority)
		{

		}

		static bool Compare(const InputFunction& first, const InputFunction& second)
		{
			return first._priority > second._priority;
		}
	};

protected:
	XMINT2 _mousePosition;
	
	Dictionary<int32, DynamicArray<InputFunction>> _keyDownFunctions;
	Dictionary<int32, DynamicArray<InputFunction>> _keyUpFunctions;
	Dictionary<int32, DynamicArray<InputFunction>> _mouseDownFunctions;
	Dictionary<int32, DynamicArray<InputFunction>> _mouseUpFunctions;

protected:
	void SortFunctions(DynamicArray<InputFunction>& functions);

public:
	void BindKeyDownEvent(int32 keyCode, Function<bool()> function, int32 priority = 0);
	void BindKeyUpEvent(int32 keyCode, Function<bool()> function, int32 priority = 0);
	void BindMouseDownEvent(int32 mouseCode, Function<bool()> function, int32 priority = 0);
	void BindMouseUpEvent(int32 mouseCode, Function<bool()> function, int32 priority = 0);
	
	void OnKeyDown(int32 keyCode);
	void OnKeyUp(int32 keyCode);
	void OnMouseDown(int32 mouseCode);
	void OnMouseUp(int32 mouseCode);

	void SetMousePosition(const XMINT2& newMousePosition);
	XMINT2 GetMousePosition();

	void Shutdown();
};