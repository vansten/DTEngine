#pragma once

// Platform independent includes
#include <string>
#include <sstream>
#include <functional>
#include <memory>
#include <type_traits>
#include <vector>
#include <unordered_map>
#include <list>
#include <map>
#include <queue>

#include <functional>

#include <assert.h>

template<typename T>
using SharedPtr = std::shared_ptr<T>;
template<typename T>
using UniquePtr = std::unique_ptr<T, std::default_delete<T>>;
template<typename T>
using WeakPtr = std::weak_ptr<T>;
template<typename T>
using EnableSharedFromThis = std::enable_shared_from_this<T>;

template<typename T>
using IsPointer = std::is_pointer<T>;

template<typename T>
using Less = std::less<T>;
template<typename T>
using Greater = std::greater<T>;
template<typename T>
using Hash = std::hash<T>;
template<typename T>
using Queue = std::queue<T>;
template<typename T, typename UnderlyingType = DynamicArray<T>, typename LessType = Less<T>>
using PriorityQueue = std::priority_queue<T, UnderlyingType, LessType>;
template<typename T1, typename T2>
using Pair = std::pair<T1, T2>;

template<typename T>
using Function = std::function<T>;

template<typename To, typename From>
inline SharedPtr<To> StaticPointerCast(SharedPtr<From>& pointer)
{
	return std::static_pointer_cast<To>(pointer);
}

template<typename To, typename From>
inline SharedPtr<To> DynamicPointerCast(SharedPtr<From>& pointer)
{
	return std::dynamic_pointer_cast<To>(pointer);
}

template<typename To, typename From>
inline SharedPtr<To> ConstPointerCast(SharedPtr<From>& pointer)
{
	return std::const_pointer_cast<To>(pointer);
}

template<typename FunctionT, typename ClassT>
inline decltype(auto) Bind(FunctionT&& function, ClassT&& object)
{
	return std::bind(function, object);
}

template<typename T>
bool operator==(const WeakPtr<T>& weakPtr, const SharedPtr<T>& sharedPtr)
{
	return !weakPtr.owner_before(sharedPtr) && !sharedPtr.owner_before(weakPtr);
}

template<typename T>
using DynamicArray = std::vector<T>;
template<typename Key, typename Value, typename Hasher = Hash<Key>>
using Dictionary = std::unordered_map<Key, Value, Hasher>;
template<typename T>
using LinkedList = std::list<T>;
template<typename Key, typename Value>
using Map = std::map<Key, Value>;

#if defined(_WIN32) || defined(_WIN64)

#define DT_WINDOWS 1
#define DT_UNDEFINEDPLATFORM 0

#else

#define DT_WINDOWS 0
#define DT_UNDEFINEDPLATFORM 1

#endif

#if DT_WINDOWS

#if defined(_WIN64)

// Defining unicode to get wide characters in Win32 API strings
#define UNICODE

using Char = wchar_t;
using String = std::wstring;
using StringStream = std::wstringstream;

template<int Size>
inline int Sprintf(Char(&buffer)[Size], const Char* const format, va_list argList)
{
	return vswprintf_s(buffer, format, argList);
}

#define DT_TEXT(string) L##string

#else

using Char = char;
using String = std::string;
using StringStream = std::stringstream;

template<int Size>
inline int Sprintf(Char(&buffer)[Size], char Char* const format, va_list argList)
{
	return sprintf_s(buffer, format, argList);
}

#define DT_TEXT(string) string

#endif

// Windows specific includes
#include <Windows.h>
#include <windowsx.h>


#else

using String = std::string;

#define DT_TEXT(string) string

#endif

#if defined(DEBUG) || defined(_DEBUG)

#define DT_DEBUG 1
#define DT_RELEASE 0

#define DT_ASSERT(cond, message) assert((cond) && message)

#else

#define DT_DEBUG 0
#define DT_RELEASE 1

#define DT_ASSERT(cond, message)

#endif

// TODO: See this and implement
// https://stackoverflow.com/questions/910172/track-c-memory-allocations