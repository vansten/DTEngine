#pragma once

#include "Core/Platform.h"
#include "Math.h"

struct BoundingBox final
{
private:
	Vector3 _min;
	Vector3 _max;

	DynamicArray<Vector3> _corners;

public:
	inline BoundingBox()
	{}
	BoundingBox(const Vector3& min, const Vector3& max);
	BoundingBox(const DynamicArray<Vector3>& positions);

private:
	void CalculateCorners();

	void SetMinMax(Vector3 min, Vector3 max);

public:
	void CalculateMinMax(const DynamicArray<Vector3>& positions);
	template<typename T>
	void CalculateMinMax(const T* dataArray, unsigned int dataCount, Function<const Vector3&(const T&)> positionGetter);

	inline Vector3 GetMin() const
	{
		return _min;
	}

	inline Vector3 GetMax() const
	{
		return _max;
	}

	inline const DynamicArray<Vector3>& GetCorners() const
	{
		return _corners;
	}

	inline Vector3 GetCenter() const
	{
		return (_min + _max) * 0.5f;
	}

	inline Vector3 GetHalfExtents() const
	{
		return _max - GetCenter();
	}

	inline BoundingBox& operator=(const BoundingBox& other)
	{
		_min = other._min;
		_max = other._max;
		CalculateCorners();

		return *this;
	}
};

template<typename T>
inline void BoundingBox::CalculateMinMax(const T* dataArray, unsigned int dataCount, Function<const Vector3&(const T&)> positionGetter)
{
	Vector3 minPosition(FLT_MAX, FLT_MAX, FLT_MAX);
	Vector3 maxPosition(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (unsigned int i = 0; i < dataCount; ++i)
	{
		minPosition = Vector3::Min(minPosition, positionGetter(dataArray[i]));
		maxPosition = Vector3::Max(maxPosition, positionGetter(dataArray[i]));
	}

	SetMinMax(minPosition, maxPosition);
}
