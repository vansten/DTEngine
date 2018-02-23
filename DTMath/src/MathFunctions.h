#pragma once

#include <cmath>

namespace Math
{
	static const float EPSILON = 1e-5f;
	static const float PI = 3.14159f;
	static const float TWO_PI = 2.0f * PI;
	static const float PI_DIV_2 = PI * 0.5f;
	static const float PI_DIV_4 = PI * 0.25f;
	static const float RAD_TO_DEG = 180.0f / PI;
	static const float DEG_TO_RAD = PI / 180.0f;

	template<typename T>
	inline std::enable_if_t<std::is_arithmetic_v<T>, T> Abs(T a)
	{
		return a >= 0 ? a : -a;
	}

	inline bool Approximately(float a, float b, float tollerance = EPSILON)
	{
		return abs(a - b) < EPSILON;
	}

	inline float Sqrt(float x)
	{
		return sqrt(x);
	}

	// Returned value is given in radians
	inline float Acos(float x)
	{
		return acos(x);
	}

	// Returned value is given in radians
	inline float Asin(float x)
	{
		return asin(x);
	}

	// Returned value is given in radians
	inline float Atan2(float y, float x)
	{
		return atan2(y, x);
	}

	inline float Cos(float radians)
	{
		return cos(radians);
	}

	inline float Sin(float radians)
	{
		return sin(radians);
	}

	inline float Max(float a, float b)
	{
		return a > b ? a : b;
	}

	inline float Min(float a, float b)
	{
		return a < b ? a : b;
	}

	inline float Clamp(float value, float min = 0.0f, float max = 1.0f)
	{
		return Min(max, Max(value, min));
	}

	inline float Wrap(float value, float min, float max)
	{
		float result = value;
		float step = Abs(max) - Abs(min);
		while (result < min)
		{
			result += step;
		}
		while (result > max)
		{
			result -= step;
		}

		return result;
	}

	inline float Round(float value)
	{
		return round(value);
	}
}