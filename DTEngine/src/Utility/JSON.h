#pragma once

#include "Core/Platform.h"
#include "Utility/Math.h"
#include "json.hpp"

using JSON = nlohmann::json;

namespace std
{
	inline void to_json(JSON& json, const wstring& value)
	{
		string str(value.begin(), value.end());
		json = str;
	}

	inline void from_json(const JSON& json, wstring& value)
	{
		string str = json.get<string>();
		value = wstring(str.begin(), str.end());
	}
}

namespace DirectX
{
	inline void to_json(JSON& json, const XMFLOAT4& value)
	{
		json["x"] = value.x;
		json["y"] = value.y;
		json["z"] = value.z;
		json["w"] = value.w;
	}

	inline void from_json(const JSON& json, XMFLOAT4& value)
	{
		value.x = json["x"].get<float>();
		value.y = json["y"].get<float>();
		value.z = json["z"].get<float>();
		value.w = json["w"].get<float>();
	}

	inline void to_json(JSON& json, const XMFLOAT3& value)
	{
		json["x"] = value.x;
		json["y"] = value.y;
		json["z"] = value.z;
	}

	inline void from_json(const JSON& json, XMFLOAT3& value)
	{
		value.x = json["x"].get<float>();
		value.y = json["y"].get<float>();
		value.z = json["z"].get<float>();
	}

	inline void to_json(JSON& json, const XMFLOAT2& value)
	{
		json["x"] = value.x;
		json["y"] = value.y;
	}

	inline void from_json(const JSON& json, XMFLOAT2& value)
	{
		value.x = json["x"].get<float>();
		value.y = json["y"].get<float>();
	}
}