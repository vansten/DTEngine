#pragma once

#include "Platform.h"

#define MAX_LAYERS 32

struct LayerID
{
private:
	unsigned int _value;

public:
	inline LayerID() : _value(0) 
	{}
	inline LayerID(unsigned int v) : _value(v)
	{}
	inline LayerID(int v) : _value((unsigned int)v)
	{}
	inline LayerID(const LayerID& layerID) : _value(layerID._value)
	{}

	inline operator unsigned int() const
	{
		return _value;
	}

	inline operator int() const
	{
		return (int)_value;
	}

	inline LayerID& operator&=(const LayerID& layer)
	{
		_value &= layer._value;
		return *this;
	}

	inline LayerID& operator|=(const LayerID& layer)
	{
		_value |= layer._value;
		return *this;
	}

	inline LayerID& operator=(const LayerID& layer)
	{
		_value = layer._value;
		return *this;
	}

	friend inline bool operator==(const LayerID& layerA, const LayerID& layerB);
	friend inline bool operator!=(const LayerID& layerA, const LayerID& layerB);
	friend inline bool operator==(const LayerID& layerA, unsigned int v);
	friend inline bool operator!=(const LayerID& layerA, unsigned int v);
	friend inline bool operator==(const LayerID& layerA, int v);
	friend inline bool operator!=(const LayerID& layerA, int v);
	friend inline LayerID operator&(const LayerID& layerA, const LayerID& layerB);
	friend inline LayerID operator|(const LayerID& layerA, const LayerID& layerB);
};

class LayerManager
{
private:
	static String _layers[MAX_LAYERS];

public:
	static LayerID ALL;

public:
	static void Initialize();
	static void Save();

	static LayerID NameToLayer(const String& layerName);
	static const String& LayerToName(LayerID layer);

	static LayerID GetMask(const DynamicArray<String>& layers);

	static void SetLayer(LayerID layer, const String& layerName);
};

inline bool operator==(const LayerID& layerA, const LayerID& layerB)
{
	return layerA._value == layerB._value;
}

inline bool operator!=(const LayerID& layerA, const LayerID& layerB)
{
	return layerA._value != layerB._value;
}

inline bool operator==(const LayerID& layerA, unsigned int v)
{
	return layerA._value == v;
}

inline bool operator!=(const LayerID& layerA, unsigned int v)
{
	return layerA._value != v;
}

inline bool operator==(const LayerID& layerA, int v)
{
	return layerA._value == (unsigned int)v;
}

inline bool operator!=(const LayerID& layerA, int v)
{
	return layerA._value != (unsigned int)v;
}

inline LayerID operator&(const LayerID& layerA, const LayerID& layerB)
{
	return LayerID(layerA._value & layerB._value);
}

inline LayerID operator|(const LayerID& layerA, const LayerID& layerB)
{
	return LayerID(layerA._value | layerB._value);
}
