#pragma once

#include "Core/App.h"
#include "Core/Event.h"
#include "Core/Platform.h"

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

class Debug
{
private:
	Map<LogVerbosity, String> _verbosityToString;

	Dictionary<String, Channel> _channels;
	Dictionary<Channel, DynamicArray<Log>, ChannelHasher> _logsPerChannel;

public:
	Event<void, const Channel&, const Log&> OnLogged;
	Event<void, const Channel&> OnChannelVisibilityChanged;

public:
	bool Initialize();
	void Shutdown();

	void Print(LogVerbosity verbosity, const String& channel, const String& message);
	void Printf(LogVerbosity verbosity, const String& channel, const Char* const format, ...);

	void RegisterChannel(const String& name);
	void SetChannelVisibility(const String& name, bool visibility);
};

#define CHANNEL_ENGINE DT_TEXT("Engine")
#define CHANNEL_GRAPHICS DT_TEXT("Graphics")
#define CHANNEL_AUDIO DT_TEXT("Audio")
#define CHANNEL_INPUT DT_TEXT("Input")
#define CHANNEL_CAMERA DT_TEXT("Camera")
#define CHANNEL_GAMEOBJECT DT_TEXT("GameObject")
#define CHANNEL_GENERAL DT_TEXT("General")
