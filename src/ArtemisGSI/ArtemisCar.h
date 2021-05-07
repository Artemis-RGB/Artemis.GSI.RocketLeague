#pragma once
#include "lib/json.hpp"
class ArtemisCar
{
public:
	float Boost = -1;
	float SpeedKph = -1;
	bool SuperSonic = false;
	bool IsOnWall = false;
	bool IsOnGround = false;

	nlohmann::json GetJson();
};

