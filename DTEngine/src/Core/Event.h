#pragma once

#include "Core/Platform.h"

template<typename ReturnType, typename ...Args>
class Event
{
private:
	typedef bool(*Predicate)(ReturnType);

	class DelegateBase
	{
		friend class Event<ReturnType, Args...>;

	protected:
		int32 _priority;

	public:
		DelegateBase(int32 priority) : _priority(priority) { }
		virtual ~DelegateBase()	{ }

		virtual ReturnType Execute(Args... args) const = 0;
		inline virtual bool IsBound() const = 0;

		inline int32 GetPriority() const { return _priority; }
		
		static bool Compare(const UniquePtr<DelegateBase>& first, const UniquePtr<DelegateBase>& second)
		{
			if(!first || !second)
			{
				return false;
			}

			return first->GetPriority() > second->GetPriority();
		}
	};

public:
	class Delegate : public DelegateBase
	{
		friend class Event<ReturnType, Args...>;

	public:
		typedef ReturnType(*FunctionType)(Args...);

	private:
		FunctionType _function;

	public:
		Delegate(FunctionType function, int32 priority) : DelegateBase(priority), _function(function) { }

		virtual ReturnType Execute(Args... args) const override
		{
			if(_function)
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
	class ClassDelegate : public DelegateBase
	{
		friend class Event<ReturnType, Args...>;

	public:
		typedef ReturnType(Class::*ClassFunctionType)(Args...);

	private:
		ClassFunctionType _function;
		WeakPtr<Class> _object;

	public:
		ClassDelegate(SharedPtr<Class> object, ClassFunctionType function, int32 priority) : DelegateBase(priority), _object(object), _function(function) { }

		virtual ReturnType Execute(Args... args) const override
		{
			SharedPtr<Class> sharedObject = _object.lock();
			if(_function && sharedObject)
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

public:
	template<typename FunctionType = Delegate::FunctionType>
	void Bind(FunctionType function, int32 priority = 0)
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
			if(!db || !d)
			{
				return false;
			}

			return d->_function == function;
		});

		if(found != _delegates.end())
		{
			_delegates.erase(found);
		}
	}

	template<typename Class, typename FunctionType = ClassDelegate<Class>::ClassFunctionType>
	void Bind(FunctionType function, SharedPtr<Class> object, int32 priority = 0)
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
			if(!db || !cd)
			{
				return false;
			}

			return cd->_function == function && cd->_object == object;
		});

		if(found != _delegates.end())
		{
			_delegates.erase(found);
		}
	}

	ReturnType Execute(Args... args) const
	{
		const size_t functionsSize = _delegates.size();
		size_t i = 0;
		for(auto& function : _delegates)
		{
			++i;
			if(function->IsBound())
			{
				if(i == functionsSize)
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
		for(auto& function : _delegates)
		{
			++i;
			if(function->IsBound())
			{
				const ReturnType returnValue = function->Execute(args...);
				if(predicate(returnValue) || i == functionsSize)
				{
					return returnValue;
				}
			}
		}
		return ReturnType();
	}
};