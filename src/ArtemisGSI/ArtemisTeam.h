#pragma once
#include "lib/json.hpp"
#include "ArtemisColor.h"

class ArtemisTeam
{
public:
	std::string Name = "";
	int Goals = -1;
	ArtemisColor PrimaryColor;
	ArtemisColor SecondaryColor;
	ArtemisColor FontColor;

	nlohmann::json GetJson();
};