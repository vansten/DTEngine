#pragma once

#include <cmath>
#include <type_traits>

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

	template<typename T>
	inline std::enable_if_t<std::is_arithmetic_v<T>, T> Wrap(T value, T min, T max)
	{
		T result = value;
		T step = Abs(max) - Abs(min);
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

	template<typename T>
	inline std::enable_if_t<std::is_arithmetic_v<T>, T> Round(T value)
	{
		return round(value);
	}

	template<typename T>
	inline std::enable_if_t<std::is_trivially_constructible_v<T>, T> Max(T a, T b)
	{
		return a > b ? a : b;
	}

	template<typename T>
	inline std::enable_if_t<!std::is_trivially_constructible_v<T>, const T&> Max(const T& a, const T& b)
	{
		return a > b ? a : b;
	}

	template<typename T>
	inline std::enable_if_t<std::is_trivially_constructible_v<T>, T> Min(T a, T b)
	{
		return a < b ? a : b;
	}

	template<typename T>
	inline std::enable_if_t<!std::is_trivially_constructible_v<T>, const T&> Min(const T& a, const T& b)
	{
		return a < b ? a : b;
	}

	template<typename T>
	inline std::enable_if_t<std::is_trivially_constructible_v<T>, T> Clamp(T value, T min, T max)
	{
		return Min(max, Max(min, value));
	}

	template<typename T>
	inline std::enable_if_t<!std::is_trivially_constructible_v<T>, T> Clamp(const T& value, const T& min, const T& max)
	{
		return Min(max, Max(min, value));
	}
}