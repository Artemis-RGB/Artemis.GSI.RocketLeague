#pragma once
#include "lib/json.hpp"
#include "ArtemisTeam.h"

class ArtemisMatch
{
public:
	ArtemisTeam Teams[2];
	int Time;
	int Playlist;
	nlohmann::json GetJson();
};

