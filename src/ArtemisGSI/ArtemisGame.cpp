#include "pch.h"
#include "ArtemisGame.h"

nlohmann::json ArtemisGame::GetJson()
{
	nlohmann::json data;
	data["status"] = Status;
	data["player"] = Player.GetJson();
	data["match"] = Match.GetJson();

	return data;
}
