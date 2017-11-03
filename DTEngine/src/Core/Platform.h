#pragma once

// Platform independent includes
#include <string>
#include <functional>

#if defined(_WIN32) || defined(_WIN64)

	#define DT_WINDOWS 1
	#define DT_UNDEFINEDPLATFORM 0
	
using uint8 = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;
using uint64 = unsigned long;
using int8 = char;
using int16 = short;
using int32 = int;
using int64 = long;
using float32 = float;
using double64 = double;

#else

	#define DT_WINDOWS 0
	#define DT_UNDEFINEDPLATFORM 1

using uint8 = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;
using uint64 = unsigned long;
using int8 = char;
using int16 = short;
using int32 = int;
using int64 = long;
using float32 = float;
using double64 = double;

#endif

#if DT_WINDOWS

	#if defined(_WIN64)

		// Defining unicode to get wide characters in Win32 API strings
		#define UNICODE

using string = std::wstring;

		#define DT_TEXT(string) L##string

	#else

using string = std::string;

		#define DT_TEXT(string) string

	#endif
	
	// Windows specific includes
	#include <Windows.h>
	#include <windowsx.h>

	// Windows aliases (DLL and FUNCTION are used in LoadDLL and GetFunction functions)
using DLL = HMODULE;
using FUNCTION = FARPROC;

#else

	typedef std::string string;

	#define DT_TEXT(string) string

using DLL = void*;
using FUNCTION = void*;

#endif

// Loads DLL with given name and returns it (0/NULL if DLL was not found)
DLL LoadDLL(const string& dllName);
// Looks for address of functon with given name in given DLL and returns that function (0/NULL if function was not found or not marked as extern "C" __declspec(dllexport))
FUNCTION GetFunction(DLL* dll, const string& functionName);

#if defined(DEBUG) || defined(_DEBUG)

	#define DT_DEBUG 1
	#define DT_RELEASE 0

#else

	#define DT_DEBUG 0
	#define DT_RELEASE 1

#endif

// TODO: See this and implement
// https://stackoverflow.com/questions/910172/track-c-memory-allocations