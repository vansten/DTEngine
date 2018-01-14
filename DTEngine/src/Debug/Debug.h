#pragma once

#include "Core/App.h"
#include "Core/Event.h"
#include "Core/Platform.h"
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
	SharedPtr<Material> _material;
	XMMATRIX _worldMatrix;

	float32 _lifetime;

public:
	DebugDrawGeometry(SharedPtr<MeshBase> mesh, XMFLOAT3 position, XMFLOAT3 rotation = XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3 scale = XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4 color = XMFLOAT4(1, 1, 1, 1), float32 lifetime = 0.0f);
	DebugDrawGeometry(DebugDrawGeometry&& other);
	~DebugDrawGeometry();

	void Render(Graphics& graphics) const;
};

class Debug
{
private:
	Map<LogVerbosity, String> _verbosityToString;

	Dictionary<String, Channel> _channels;
	Dictionary<Channel, DynamicArray<Log>, ChannelHasher> _logsPerChannel;

	DynamicArray<DebugDrawGeometry> _draws;

	SharedPtr<MeshBase> _cube;
	SharedPtr<MeshBase> _sphere;

public:
	Event<void, const Channel&, const Log&> OnLogged;
	Event<void, const Channel&> OnChannelVisibilityChanged;

private:
	void UpdateDraws(float32 deltaTime);

public:
	bool Initialize();
	bool InitializeDraws();
	void Shutdown();

	void Update(float32 deltaTime);

	void Print(LogVerbosity verbosity, const String& channel, const String& message);
	void Printf(LogVerbosity verbosity, const String& channel, const Char* const format, ...);

	void RegisterChannel(const String& name);
	void SetChannelVisibility(const String& name, bool visibility);

	void DrawCube(XMFLOAT3 center, XMFLOAT3 size, XMFLOAT3 rotation = XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4 color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), float32 lifetime = -1.0f);
	void DrawSphere(XMFLOAT3 center, float32 radius, XMFLOAT4 color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), float32 lifetime = -1.0f);

	inline const DynamicArray<DebugDrawGeometry>& GetDraws() const { return _draws; }
};

#define CHANNEL_ENGINE DT_TEXT("Engine")
#define CHANNEL_GRAPHICS DT_TEXT("Graphics")
#define CHANNEL_AUDIO DT_TEXT("Audio")
#define CHANNEL_INPUT DT_TEXT("Input")
#define CHANNEL_CAMERA DT_TEXT("Camera")
#define CHANNEL_GAMEOBJECT DT_TEXT("GameObject")
#define CHANNEL_GENERAL DT_TEXT("General")
