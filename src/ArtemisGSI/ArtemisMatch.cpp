#include "pch.h"
#include "ArtemisMatch.h"

nlohmann::json ArtemisMatch::GetJson()
{
	nlohmann::json json_match;
	json_match["team_0"] = Teams[0].GetJson();
	json_match["team_1"] = Teams[1].GetJson();
	json_match["playlist"] = Playlist;
	json_match["time"] = Time;
	return json_match;
}
