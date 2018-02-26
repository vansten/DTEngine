#pragma once

#include "Core/Event.h"
#include "Core/Platform.h"
#include "Utility/EnumInfo.h"
#include "Utility/Math.h"

class Graphics;
class MeshBase;
class Material;

enum class LogVerbosity
{
	Log,
	Warning,
	Error,
	Exception
};

DECLARE_ENUM_NAMES(LogVerbosity)
{
	{
		DT_TEXT("Error"), LogVerbosity::Error
	},
		{DT_TEXT("Exception"), LogVerbosity::Exception},
		{DT_TEXT("Log"), LogVerbosity::Log},
		{DT_TEXT("Warning"), LogVerbosity::Warning}
};

struct Channel final
{
public:
	String Name;
	bool Visible;

public:
	Channel()
	{}
	Channel(const String& name, bool visible) : Name(name), Visible(visible)
	{}

	inline bool operator==(const Channel& other) const
	{
		return std::equal(Name.begin(), Name.end(), other.Name.begin(), other.Name.end());
	}
};

struct ChannelHasher final
{
	inline std::size_t operator()(const Channel& key) const
	{
		return std::hash<String>()(key.Name);
	}
};

struct Log final
{
public:
	LogVerbosity Verbosity;
	String Message;

	Log(LogVerbosity verbosity, const String& message) : Verbosity(verbosity), Message(message)
	{}
};

struct DebugDrawGeometry final
{
	friend class Debug;

private:
	SharedPtr<MeshBase> _mesh;
	UniquePtr<Material> _material;
	Matrix _worldMatrix;

	float _lifetime;

public:
	DebugDrawGeometry(SharedPtr<MeshBase> mesh, Vector3 position, Quaternion rotation = Quaternion::IDENTITY, Vector3 scale = Vector3(1.0f, 1.0f, 1.0f), Vector4 color = Vector4(1, 1, 1, 1), float lifetime = 0.0f);
	DebugDrawGeometry(DebugDrawGeometry&& other);
	~DebugDrawGeometry();

	void Render(Graphics& graphics) const;

	DebugDrawGeometry& operator=(const DebugDrawGeometry& other);
};

class Debug final
{
private:
	Dictionary<String, Channel> _channels;
	Dictionary<Channel, DynamicArray<Log>, ChannelHasher> _logsPerChannel;

	DynamicArray<DebugDrawGeometry> _draws;

	SharedPtr<MeshBase> _cube;
	SharedPtr<MeshBase> _sphere;

public:
	Event<void(const Channel&, const Log&)> OnLogged;
	Event<void(const Channel&)> OnChannelVisibilityChanged;

private:
	void UpdateDraws(float deltaTime);

public:
	bool Initialize();
	bool InitializeDraws();
	void Shutdown();

	void Update(float deltaTime);

	void Print(LogVerbosity verbosity, const String& channel, const String& message);
	void Printf(LogVerbosity verbosity, const String& channel, const Char* const format, ...);

	void RegisterChannel(const String& name);
	void SetChannelVisibility(const String& name, bool visibility);

	void DrawMesh(const Vector3& position, SharedPtr<MeshBase> mesh, const Vector3& size = Vector3::ONE, const Quaternion& rotation = Quaternion::IDENTITY, const Vector4& color = Vector4(1.0f, 1.0f, 1.0f, 1.0f), float lifetime = -1.0f);
	void DrawCube(const Vector3& center, const Vector3& size, const Quaternion& rotation = Quaternion::IDENTITY, const Vector4& color = Vector4(1.0f, 1.0f, 1.0f, 1.0f), float lifetime = -1.0f);
	void DrawSphere(const Vector3& center, float radius, const Vector4& color = Vector4(1.0f, 1.0f, 1.0f, 1.0f), float lifetime = -1.0f);
	void DrawLine(const Vector3& start, const Vector3& end, const Vector4& color = Vector4(1.0f, 1.0f, 1.0f, 1.0f), float thickness = 1.0f, float lifetime = -1.0f);

	inline const DynamicArray<DebugDrawGeometry>& GetDraws() const
	{
		return _draws;
	}
};

extern const String CHANNEL_ENGINE;
extern const String CHANNEL_GRAPHICS;
extern const String CHANNEL_AUDIO;
extern const String CHANNEL_INPUT;
extern const String CHANNEL_CAMERA;
extern const String CHANNEL_GAMEOBJECT;
extern const String CHANNEL_GENERAL;

extern Debug gDebug;