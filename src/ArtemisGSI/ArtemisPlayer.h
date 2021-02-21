#pragma once

#include "lib/json.hpp"

class ArtemisPlayer
{
public:
	int Team = -1;
	int Score = -1;
	int Goals = -1;
	int Assists = -1;
	int Saves = -1;
	int Shots = -1;
	float Boost = -1;

	nlohmann::json GetJson();
};

