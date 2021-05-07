#pragma once
#include "ArtemisMatch.h"
#include "ArtemisPlayer.h"
#include "ArtemisCar.h"

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
	ArtemisCar Car;
	GameStatus Status;

	nlohmann::json GetJson();
};

