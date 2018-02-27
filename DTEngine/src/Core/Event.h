#pragma once

#include "Core/Platform.h"

template<typename T>
class Function;

template<typename ReturnType, typename ...Args>
class Function<ReturnType(Args...)>
{
private:
	class ICallable
	{
	public:
		virtual ~ICallable() = default;
		virtual ReturnType Invoke(Args...) const = 0;
	};

	template<typename T>
	class CallableT : public ICallable
	{
	private:
		T _t;

	public:
		CallableT(const T& t) : _t(t)
		{}
		CallableT(const CallableT& ct) : _t(ct._t)
		{}

		~CallableT() override = default;

		virtual ReturnType Invoke(Args... args) const override
		{
			return _t(std::forward<Args>(args)...);
		}
	};

	template<typename Class, typename T>
	class CallableClassT : public ICallable
	{
	private:
		WeakPtr<Class> _object;
		T _t;

	public:
		CallableClassT(SharedPtr<Class> object, const T& t) : _object(object), _t(t)
		{}
		CallableClassT(const CallableClassT& callable) : _object(callable._object), _t(callable._t)
		{}

		~CallableClassT() override = default;

		virtual ReturnType Invoke(Args... args) const override
		{
			SharedPtr<Class> obj = _object.lock();
			if (obj)
			{
				Class* rawPtr = obj.get();
				return (rawPtr->*_t)(std::forward<Args>(args)...);
			}

			return ReturnType();
		}
	};

private:
	UniquePtr<ICallable> _callable;

public:
	Function()
	{
		_callable = nullptr;
	}

	template<typename T>
	Function(T t)
	{
		_callable = std::make_unique<CallableT<T>>(t);
	}

	template<typename Class, typename T>
	Function(SharedPtr<Class> object, const T& t)
	{
		_callable = std::make_unique<CallableClassT<Class, T>>(object, t);
	}

	template<typename Class, typename T>
	void Bind(SharedPtr<Class> object, const T& t)
	{
		_callable = std::make_unique<CallableClassT<Class, T>>(object, t);
	}

	template<typename T>
	Function& operator=(T t)
	{
		_callable = std::make_unique<CallableT<T>>(t);
		return *this;
	}

	Function& operator=(nullptr_t)
	{
		_callable = nullptr;
		return *this;
	}

	ReturnType operator()(Args... args) const
	{
		DT_ASSERT(_callable, DT_TEXT("Cannot invoke function that is not bound to anything"));
		return _callable->Invoke(std::forward<Args>(args)...);
	}
};

template<typename T>
class Event;

template<typename ReturnType, typename ...Args>
class Event<ReturnType(Args...)>
{
private:
	typedef bool(*Predicate)(ReturnType);

	class DelegateBase
	{
		friend class EventBase<ReturnType, Args...>;

	protected:
		int _priority;

	public:
		DelegateBase(int priority) : _priority(priority)
		{}
		virtual ~DelegateBase()
		{}

		virtual ReturnType Execute(Args... args) const = 0;
		inline virtual bool IsBound() const = 0;

		inline int GetPriority() const
		{
			return _priority;
		}

		static bool Compare(const UniquePtr<DelegateBase>& first, const UniquePtr<DelegateBase>& second)
		{
			if (!first || !second)
			{
				return false;
			}

			return first->GetPriority() > second->GetPriority();
		}
	};

public:
	class Delegate final : public DelegateBase
	{
		friend class EventBase<ReturnType, Args...>;

	public:
		typedef ReturnType(*FunctionType)(Args...);

	private:
		FunctionType _function;

	public:
		Delegate(FunctionType function, int priority) : DelegateBase(priority), _function(function)
		{}

		virtual ReturnType Execute(Args... args) const override
		{
			if (_function)
			{
				return _function(args...);
			}

			return ReturnType();
		}

		inline virtual bool IsBound() const override
		{
			return _function != nullptr;
		}
	};

	template<typename Class>
	class ClassDelegate final : public DelegateBase
	{
		friend class EventBase<ReturnType, Args...>;

	public:
		typedef ReturnType(Class::*ClassFunctionType)(Args...);

	private:
		ClassFunctionType _function;
		WeakPtr<Class> _object;

	public:
		ClassDelegate(SharedPtr<Class> object, ClassFunctionType function, int priority) : DelegateBase(priority), _object(object), _function(function)
		{}

		virtual ReturnType Execute(Args... args) const override
		{
			SharedPtr<Class> sharedObject = _object.lock();
			if (_function && sharedObject)
			{
				Class* rawPtr = sharedObject.get();
				return (rawPtr->*_function)(args...);
			}

			DT_ASSERT(false, "Shouldn't be here");
			return ReturnType();
		}

		inline virtual bool IsBound() const override
		{
			return _object.lock() != nullptr && _function != nullptr;
		}
	};

private:
	DynamicArray<UniquePtr<DelegateBase>> _delegates;

private:
	void SortDelegates()
	{
		std::sort(_delegates.begin(), _delegates.end(), &DelegateBase::Compare);
	}

protected:
	// This simple pure virtual function will do nothing in derived classes
	// But it's super important to make sure nobody instantiates EventBase class directly
	virtual void PreventFromInstantiating() const = 0;

public:
	template<typename FunctionType = Delegate::FunctionType>
	void Bind(FunctionType function, int priority = 0)
	{
		_delegates.push_back(UniquePtr<DelegateBase>(new Delegate(function, priority)));
		SortDelegates();
	}

	template<typename FunctionType = Delegate::FunctionType>
	void Unbind(FunctionType function)
	{
		auto& found = std::find_if(_delegates.begin(), _delegates.end(), [function](UniquePtr<DelegateBase>& del)
		{
			const DelegateBase* const db = del.get();
			const Delegate* const d = dynamic_cast<const Delegate* const>(db);
			if (!d)
			{
				return false;
			}

			return d->_function == function;
		});

		if (found != _delegates.end())
		{
			_delegates.erase(found);
		}
	}

	template<typename Class, typename FunctionType = ClassDelegate<Class>::ClassFunctionType>
	void Bind(FunctionType function, SharedPtr<Class> object, int priority = 0)
	{
		_delegates.push_back(UniquePtr<DelegateBase>(new ClassDelegate<Class>(object, function, priority)));
		SortDelegates();
	}

	template<typename Class, typename FunctionType = ClassDelegate<Class>::ClassFunctionType>
	void Unbind(FunctionType function, SharedPtr<Class> object)
	{
		auto& found = std::find_if(_delegates.begin(), _delegates.end(), [function, object](UniquePtr<DelegateBase>& del)
		{
			const DelegateBase* const db = del.get();
			const ClassDelegate<Class>* const cd = dynamic_cast<const ClassDelegate<Class>* const>(db);
			if (!cd)
			{
				return false;
			}

			return cd->_function == function && cd->_object == object;
		});

		if (found != _delegates.end())
		{
			_delegates.erase(found);
		}
	}

	ReturnType Execute(Args... args) const
	{
		const size_t functionsSize = _delegates.size();
		size_t i = 0;
		for (auto& function : _delegates)
		{
			++i;
			if (function->IsBound())
			{
				if (i == functionsSize)
				{
					return function->Execute(args...);
				}
				else
				{
					function->Execute(args...);
				}
			}
		}
		return ReturnType();
	}

	// DOES NOT WORK for ReturnType == void!
	ReturnType ExecuteUntil(Args... args, Predicate predicate) const
	{
		const size_t functionsSize = _delegates.size();
		size_t i = 0;
		for (auto& function : _delegates)
		{
			++i;
			if (function->IsBound())
			{
				const ReturnType returnValue = function->Execute(args...);
				if (predicate(returnValue) || i == functionsSize)
				{
					return returnValue;
				}
			}
		}
		return ReturnType();
	}
};