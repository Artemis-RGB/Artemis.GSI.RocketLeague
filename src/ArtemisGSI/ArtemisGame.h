#pragma once
#include "ArtemisMatch.h"
#include "ArtemisPlayer.h"


enum class GameStatus
{
	Menu = -1,
	Replay,
	InGame,
	Freeplay,
	Training,
	Spectate
};

class ArtemisGame
{
public:
	ArtemisMatch Match;
	ArtemisPlayer Player;
	GameStatus Status;

	nlohmann::json GetJson();
};

