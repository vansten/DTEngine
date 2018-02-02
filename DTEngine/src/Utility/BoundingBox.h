#pragma once

#include "Core/Platform.h"
#include "Math.h"

struct BoundingBox
{
private:
	XMVECTOR _min;
	XMVECTOR _max;

	DynamicArray<XMVECTOR> _corners;

public:
	inline BoundingBox() { }
	BoundingBox(const XMFLOAT3& min, const XMFLOAT3& max);
	BoundingBox(const DynamicArray<XMFLOAT3>& positions);

private:
	void CalculateCorners();

	void SetMinMax(XMFLOAT3 min, XMFLOAT3 max);

public:
	void CalculateMinMax(const DynamicArray<XMFLOAT3>& positions);
	template<typename T>
	void CalculateMinMax(const T* dataArray, unsigned int dataCount, Function<const XMFLOAT3&(const T&)> positionGetter);

	inline XMFLOAT3 GetMin() const
	{
		XMFLOAT3 min;
		XMStoreFloat3(&min, _min);
		return min;
	}

	inline XMFLOAT3 GetMax() const
	{
		XMFLOAT3 max;
		XMStoreFloat3(&max, _max);
		return max;
	}

	inline const DynamicArray<XMVECTOR>& GetCorners() const
	{
		return _corners;
	}

	inline XMFLOAT3 GetCenter() const
	{
		static const XMVECTOR halfVector = XMVectorSet(0.5f, 0.5f, 0.5f, 0.5f);
		XMVECTOR centerVector = XMVectorAdd(_min, _max);
		centerVector = XMVectorMultiply(centerVector, halfVector);
		XMFLOAT3 center;
		XMStoreFloat3(&center, centerVector);
		return center;
	}

	inline XMFLOAT3 GetHalfExtents() const
	{
		static const XMVECTOR halfVector = XMVectorSet(0.5f, 0.5f, 0.5f, 0.5f);
		XMVECTOR centerVector = XMVectorAdd(_min, _max);
		centerVector = XMVectorMultiply(centerVector, halfVector);
		XMVECTOR halfExtentsVector = XMVectorSubtract(_max, centerVector);
		XMFLOAT3 halfExtents;
		XMStoreFloat3(&halfExtents, halfExtentsVector);
		return halfExtents;
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
inline void BoundingBox::CalculateMinMax(const T* dataArray, unsigned int dataCount, Function<const XMFLOAT3&(const T&)> positionGetter)
{
	XMFLOAT3 minPosition(FLT_MAX, FLT_MAX, FLT_MAX);
	XMFLOAT3 maxPosition(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for(unsigned int i = 0; i < dataCount; ++i)
	{
		minPosition = Min(minPosition, positionGetter(dataArray[i]));
		maxPosition = Max(maxPosition, positionGetter(dataArray[i]));
	}

	SetMinMax(minPosition, maxPosition);
}
