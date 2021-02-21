#include "pch.h"
#include "ArtemisGSI.h"

BAKKESMOD_PLUGIN(ArtemisGSI, "Artemis RGB integration", plugin_version, PLUGINTYPE_THREADED)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;


void ArtemisGSI::onLoad()
{
	_globalCvarManager = cvarManager;

	artemisClient = new httplib::Client("http://localhost:9696");
	artemisClient->set_connection_timeout(0, 100000);

	this->StartLoop();
	//cvarManager->log("Plugin loaded!");

	//cvarManager->registerNotifier("my_aweseome_notifier", [&](std::vector<std::string> args) {
	//	cvarManager->log("Hello notifier!");
	//}, "", 0);

	//auto cvar = cvarManager->registerCvar("template_cvar", "hello-cvar", "just a example of a cvar");
	//auto cvar2 = cvarManager->registerCvar("template_cvar2", "0", "just a example of a cvar with more settings", true, true, -10, true, 10 );

	//cvar.addOnValueChanged([this](std::string cvarName, CVarWrapper newCvar) {
	//	cvarManager->log("the cvar with name: " + cvarName + " changed");
	//	cvarManager->log("the new value is:" + newCvar.getStringValue());
	//});

	//cvar2.addOnValueChanged(std::bind(&ArtemisGSI::YourPluginMethod, this, _1, _2));

	// enabled decleared in the header
	//enabled = std::make_shared<bool>(false);
	//cvarManager->registerCvar("TEMPLATE_Enabled", "0", "Enable the TEMPLATE plugin", true, true, 0, true, 1).bindTo(enabled);

	//cvarManager->registerNotifier("NOTIFIER", [this](std::vector<std::string> params){FUNCTION();}, "DESCRIPTION", PERMISSION_ALL);
	//cvarManager->registerCvar("CVAR", "DEFAULTVALUE", "DESCRIPTION", true, true, MINVAL, true, MAXVAL);//.bindTo(CVARVARIABLE);
	//gameWrapper->HookEvent("FUNCTIONNAME", std::bind(&TEMPLATE::FUNCTION, this));
	//gameWrapper->HookEventWithCallerPost<ActorWrapper>("FUNCTIONNAME", std::bind(&ArtemisGSI::FUNCTION, this, _1, _2, _3));
	//gameWrapper->RegisterDrawable(bind(&TEMPLATE::Render, this, std::placeholders::_1));


	//gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode", [this](std::string eventName) {
	//	cvarManager->log("Your hook got called and the ball went POOF");
	//});
	// You could also use std::bind here
	//gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode", std::bind(&ArtemisGSI::YourPluginMethod, this);
}

void ArtemisGSI::onUnload()
{
	ok = false;
}

void ArtemisGSI::StartLoop() {
	if (ok) {
		gameWrapper->Execute(std::bind(&ArtemisGSI::UpdateMatchState, this));
		std::string newJson = GameState.GetJson().dump();
		if (newJson != json) {
			json = newJson;
			SendToArtemis(json);
		}
		this->gameWrapper->SetTimeout(std::bind(&ArtemisGSI::StartLoop, this), 0.1f);
	}
}

void ArtemisGSI::UpdateMatchState() {
	ServerWrapper wrapper = GetCurrentGameType();//this sets type (-1 for menu)

	if (!wrapper.IsNull())
		this->UpdateState(wrapper);
	else
		this->ResetStates();

	return;
}

void ArtemisGSI::SendToArtemis(std::string data) {
	try {
		artemisClient->Post("/plugins/945dc0aa-7ee3-47ec-9be6-f378fb7cb7b0/update", data, "application/json");
	}
	catch (...) {
		cvarManager->log("stopping...");
		ok = false;
	}
}

void ArtemisGSI::UpdateState(ServerWrapper wrapper)
{
	if (!wrapper.GetbUnlimitedTime() && !wrapper.GetbOverTime())
		GameState.Match.Time = wrapper.GetSecondsRemaining();
	else
		GameState.Match.Time = -1;

	ArrayWrapper<TeamWrapper> teams = wrapper.GetTeams();
	for (int i = 0; i < teams.Count(); i++) {
		if (teams.Get(i).IsNull()) continue;

		GameState.Match.Teams[i].Goals = teams.Get(i).GetScore();

		if (!teams.Get(i).GetSanitizedTeamName().IsNull())
			GameState.Match.Teams[i].Name = teams.Get(i).GetSanitizedTeamName().ToString();
		else
			GameState.Match.Teams[i].Name = i == 0 ? "Blue" : "Orange";

		auto primaryColor = teams.Get(i).GetPrimaryColor();
		auto secondaryColor = teams.Get(i).GetSecondaryColor();

		GameState.Match.Teams[i].PrimaryColor.SetValues(primaryColor);
		GameState.Match.Teams[i].SecondaryColor.SetValues(secondaryColor);
	}

	auto localController = this->gameWrapper->GetPlayerController();
	if (!localController.IsNull()) {
		auto local = localController.GetPRI();
		if (!local.IsNull()) {
			GameState.Player.Score = local.GetMatchScore();
			GameState.Player.Goals = local.GetMatchGoals();
			GameState.Player.Assists = local.GetMatchAssists();
			GameState.Player.Saves = local.GetMatchSaves();
			GameState.Player.Shots = local.GetMatchShots();

			if (!local.GetTeam().IsNull())
				GameState.Player.Team = local.GetTeam().GetTeamIndex();
			else
				GameState.Player.Team = -1;

			if (!local.GetCar().IsNull() && !local.GetCar().GetBoostComponent().IsNull())
				GameState.Player.Boost = local.GetCar().GetBoostComponent().GetCurrentBoostAmount();
			else
				GameState.Player.Boost = -1;
		}
		else {
			GameState.Player.Score = -1;
			GameState.Player.Goals = -1;
			GameState.Player.Assists = -1;
			GameState.Player.Saves = -1;
			GameState.Player.Shots = -1;
			GameState.Player.Team = -1;
			GameState.Player.Boost = -1;
		}
	}

	GameSettingPlaylistWrapper playlistWrapper = wrapper.GetPlaylist();
	if (playlistWrapper.memory_address != NULL)
		GameState.Match.Playlist = playlistWrapper.GetPlaylistId();
	else
		GameState.Match.Playlist = -1;
}

void ArtemisGSI::ResetStates()
{
	GameState.Status = GameStatus::Menu;

	GameState.Match.Playlist = -1;
	GameState.Match.Time = -1;
	GameState.Match.Teams[0].Goals = -1;
	GameState.Match.Teams[0].PrimaryColor.Red = -1;
	GameState.Match.Teams[0].PrimaryColor.Green = -1;
	GameState.Match.Teams[0].PrimaryColor.Blue = -1;
	GameState.Match.Teams[0].SecondaryColor.Red = -1;
	GameState.Match.Teams[0].SecondaryColor.Green = -1;
	GameState.Match.Teams[0].SecondaryColor.Blue = -1;
	GameState.Match.Teams[0].Name = "";
	GameState.Match.Teams[1].Goals = -1;
	GameState.Match.Teams[1].PrimaryColor.Red = -1;
	GameState.Match.Teams[1].PrimaryColor.Green = -1;
	GameState.Match.Teams[1].PrimaryColor.Blue = -1;
	GameState.Match.Teams[1].SecondaryColor.Red = -1;
	GameState.Match.Teams[1].SecondaryColor.Green = -1;
	GameState.Match.Teams[1].SecondaryColor.Blue = -1;
	GameState.Match.Teams[1].Name = "";

	GameState.Player.Team = -1;
	GameState.Player.Boost = -1;
	GameState.Player.Assists = -1;
	GameState.Player.Goals = -1;
	GameState.Player.Saves = -1;
	GameState.Player.Score = -1;
	GameState.Player.Shots = -1;
}

ServerWrapper ArtemisGSI::GetCurrentGameType() {
	if (this->gameWrapper->IsInOnlineGame()) {
		GameState.Status = GameStatus::InGame;
		return this->gameWrapper->GetOnlineGame();
	}
	else if (this->gameWrapper->IsSpectatingInOnlineGame()) {
		GameState.Status = GameStatus::Spectate;
		return this->gameWrapper->GetOnlineGame();
	}
	else if (this->gameWrapper->IsInReplay()) {
		GameState.Status = GameStatus::Replay;
		return this->gameWrapper->GetGameEventAsReplay();
	}
	else if (this->gameWrapper->IsInFreeplay()) {
		GameState.Status = GameStatus::Freeplay;
		return this->gameWrapper->GetGameEventAsServer();
	}
	else if (this->gameWrapper->IsInCustomTraining()) {
		GameState.Status = GameStatus::Training;
		return this->gameWrapper->GetGameEventAsServer();
	}
	else if (this->gameWrapper->IsInGame()) {
		GameState.Status = GameStatus::InGame;
		return this->gameWrapper->GetGameEventAsServer();
	}
	else {
		GameState.Status = GameStatus::Menu;
		return NULL;
	}
}