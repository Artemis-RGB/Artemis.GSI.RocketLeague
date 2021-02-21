#pragma once
#include "lib/json.hpp"

struct ArtemisColor
{
	unsigned char Red;
	unsigned char Green;
	unsigned char Blue;

	nlohmann::json GetJson();
	void SetValues(LinearColor source);
};

