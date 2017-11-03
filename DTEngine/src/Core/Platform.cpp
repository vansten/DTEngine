#include "Platform.h"

#if DT_WINDOWS

#include <locale>
#include <codecvt>

DLL LoadDLL(const string& dllName)
{
	return LoadLibrary(dllName.c_str());
}

FUNCTION GetFunction(DLL* dll, const string& functionName)
{
	if(!dll)
	{
		return 0;
	}

	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	std::string functionNameString = converterX.to_bytes(functionName);
	return GetProcAddress(*dll, functionNameString.c_str());
}

#else

DLL LoadDLL(const string& dllName)
{
	OutputDebugString(DT_TEXT("Cannot load dll named "));
	OutputDebugString(dllName.c_str());
	OutputDebugString(DT_TEXT(". Platform undefined"));
}

FUNCTION GetFunction(DLL* dll, const string& functonName)
{

}

#endif