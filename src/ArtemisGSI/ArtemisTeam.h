#pragma once
#include "lib/json.hpp"
#include "ArtemisColor.h"

class ArtemisTeam
{
public:
	int Goals = -1;
	ArtemisColor PrimaryColor;
	ArtemisColor SecondaryColor;
	std::string Name = "";

	nlohmann::json GetJson();
};

