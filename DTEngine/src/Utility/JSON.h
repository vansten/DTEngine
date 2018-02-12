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

inline void to_json(JSON& json, const Vector2& value)
{
	json["x"] = value.X;
	json["y"] = value.Y;
}

inline void from_json(const JSON& json, Vector2& value)
{
	value.X = json["x"].get<float>();
	value.Y = json["y"].get<float>();
}

inline void to_json(JSON& json, const Vector3& value)
{
	json["x"] = value.X;
	json["y"] = value.Y;
	json["z"] = value.Z;
}

inline void from_json(const JSON& json, Vector3& value)
{
	value.X = json["x"];
	value.Y = json["y"];
	value.Z = json["z"];
}

inline void to_json(JSON& json, const Vector4& value)
{
	json["x"] = value.X;
	json["y"] = value.Y;
	json["z"] = value.Z;
	json["w"] = value.W;
}

inline void from_json(const JSON& json, Vector4& value)
{
	value.X = json["x"];
	value.Y = json["y"];
	value.Z = json["z"];
	value.W = json["w"];
}