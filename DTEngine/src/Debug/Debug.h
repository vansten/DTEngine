#pragma once

#include "Core/App.h"
#include "Core/Event.h"
#include "Core/Platform.h"
#include "Utility/EnumUtility.h"
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
	{ DT_TEXT("Error"), LogVerbosity::Error },
	{ DT_TEXT("Exception"), LogVerbosity::Exception },
	{ DT_TEXT("Log"), LogVerbosity::Log },
	{ DT_TEXT("Warning"), LogVerbosity::Warning }
};

struct Channel
{
public:
	String Name;
	bool Visible;

public:
	Channel() { }
	Channel(const String& name, bool visible) : Name(name), Visible(visible) { }

	inline bool operator==(const Channel& other) const
	{
		return std::equal(Name.begin(), Name.end(), other.Name.begin(), other.Name.end());
	}
};

struct ChannelHasher
{
	inline std::size_t operator()(const Channel& key) const
	{
		return std::hash<String>()(key.Name);
	}
};

struct Log
{
public:
	LogVerbosity Verbosity;
	String Message;

	Log(LogVerbosity verbosity, const String& message) : Verbosity(verbosity), Message(message) { }
};

struct DebugDrawGeometry
{
	friend class Debug;

private:
	SharedPtr<MeshBase> _mesh;
	UniquePtr<Material> _material;
	XMMATRIX _worldMatrix;

	float _lifetime;

public:
	DebugDrawGeometry(SharedPtr<MeshBase> mesh, XMFLOAT3 position, XMFLOAT3 rotation = XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3 scale = XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4 color = XMFLOAT4(1, 1, 1, 1), float lifetime = 0.0f);
	DebugDrawGeometry(SharedPtr<MeshBase> mesh, XMFLOAT3 position, XMMATRIX rotation = XMMATRIX(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT3 scale = XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4 color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), float lifetime = 0.0f);
	DebugDrawGeometry(DebugDrawGeometry&& other);
	~DebugDrawGeometry();

	void Render(Graphics& graphics) const;

	DebugDrawGeometry& operator=(const DebugDrawGeometry& other);
};

class Debug
{
private:
	Dictionary<String, Channel> _channels;
	Dictionary<Channel, DynamicArray<Log>, ChannelHasher> _logsPerChannel;

	DynamicArray<DebugDrawGeometry> _draws;

	SharedPtr<MeshBase> _cube;
	SharedPtr<MeshBase> _sphere;

public:
	Event<void, const Channel&, const Log&> OnLogged;
	Event<void, const Channel&> OnChannelVisibilityChanged;

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

	void DrawCube(XMFLOAT3 center, XMFLOAT3 size, XMFLOAT3 rotation = XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4 color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), float lifetime = -1.0f);
	void DrawSphere(XMFLOAT3 center, float radius, XMFLOAT4 color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), float lifetime = -1.0f);
	void DrawLine(XMFLOAT3 start, XMFLOAT3 end, XMFLOAT4 color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), float thickness = 1.0f, float lifetime = -1.0f);

	inline const DynamicArray<DebugDrawGeometry>& GetDraws() const { return _draws; }
};

extern const String CHANNEL_ENGINE;
extern const String CHANNEL_GRAPHICS;
extern const String CHANNEL_AUDIO;
extern const String CHANNEL_INPUT;
extern const String CHANNEL_CAMERA;
extern const String CHANNEL_GAMEOBJECT;
extern const String CHANNEL_GENERAL;
