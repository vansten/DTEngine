#include "Debug.h"

bool Debug::Initialize()
{
#if DT_DEBUG
	_verbosityToString = Map<LogVerbosity, String>
	{
		{LogVerbosity::Error, DT_TEXT("Error")},
		{LogVerbosity::Exception, DT_TEXT("Exception")},
		{LogVerbosity::Log, DT_TEXT("Log")},
		{LogVerbosity::Warning, DT_TEXT("Warning")}
	};
	RegisterChannel(CHANNEL_ENGINE);
	RegisterChannel(CHANNEL_GRAPHICS);
	RegisterChannel(CHANNEL_AUDIO);
	RegisterChannel(CHANNEL_INPUT);
	RegisterChannel(CHANNEL_CAMERA);
	RegisterChannel(CHANNEL_GAMEOBJECT);
	RegisterChannel(CHANNEL_GENERAL);
#endif

	return true;
}

void Debug::Shutdown()
{
	_channels.clear();
	_logsPerChannel.clear();
}

void Debug::Print(LogVerbosity verbosity, const String& channel, const String& message)
{
#if DT_DEBUG
	DT_ASSERT(_channels.find(channel) != _channels.end() && DT_TEXT("Channel does not exist!"));

	Log l(verbosity, message);
	const Channel& channelRef = _channels[channel];

	_logsPerChannel[channelRef].push_back(l);
	OnLogged.Execute(channelRef, l);

	if(channelRef.Visible)
	{
		String s = channel;
		s += DT_TEXT(": ");
		s += _verbosityToString[verbosity];
		s += DT_TEXT(" - ");
		s += message;
		s += DT_TEXT("\n");
		OutputDebugString(s.c_str());
	}
#endif
}

void Debug::Printf(LogVerbosity verbosity, const String& channel, const Char* const format, ...)
{
#if DT_DEBUG
	Char buffer[1000];
	va_list argList;
	__crt_va_start(argList, format);
	Sprintf(buffer, format, argList);
	__crt_va_end(argList);

	Print(verbosity, channel, buffer);
#endif
}

void Debug::RegisterChannel(const String& name)
{
#if DT_DEBUG
	DT_ASSERT(_channels.find(name) == _channels.end() && DT_TEXT("Channel already exists"));

	_channels.emplace(name, Channel(name, true));
#endif
}

void Debug::SetChannelVisibility(const String& name, bool visibility)
{
#if DT_DEBUG
	DT_ASSERT(_channels.find(name) != _channels.end() && DT_TEXT("Channel does not exist!"));

	Channel& channel = _channels[name];
	channel.Visible = visibility;
	OnChannelVisibilityChanged.Execute(channel);
#endif
}