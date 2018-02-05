#pragma once

#include "Core/Platform.h"

template<typename T>
class UniqueSingleton
{
protected:
	static UniquePtr<T> _instance;

public:
	static const UniquePtr<T>& GetInstance()
	{
		if(!_instance)
		{
			_instance = UniquePtr<T>(new T());
		}
		return _instance;
	}

	static void FreeInstance()
	{
		if(_instance)
		{
			_instance.reset();
		}
	}
};

template<typename T>
UniquePtr<T> UniqueSingleton<T>::_instance = nullptr;