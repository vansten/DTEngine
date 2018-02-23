#pragma once

#define APP_SUCCESSFUL 0x000
#define APP_INITIALIZATION_FAILED 0x001
#define APP_MEMORY_LEAKS 0x002

#include "Core/Platform.h"
#include "Utility/UniqueSingleton.h"

class Game;

class App final : public UniqueSingleton<App>
{
	friend class UniqueSingleton<App>;

protected:
	UniquePtr<Game> _game;

	bool _isRunning;

protected:
	App();

public:
	~App();

protected:
	bool Initialize(UniquePtr<Game>&& game);
	void Loop();
	void Shutdown();

public:
	int Run(UniquePtr<Game>&& game);

	inline bool IsRunning() const
	{
		return _isRunning;
	}

	inline Game& GetGame() const
	{
		DT_ASSERT(_game, DT_TEXT("Cannot dereference null game pointer"));
		return *_game;
	}
};

inline Game& GetGame()
{
	return App::GetInstance()->GetGame();
}