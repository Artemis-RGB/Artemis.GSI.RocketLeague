#include "pch.h"
#include "ArtemisGSI.h"

BAKKESMOD_PLUGIN(ArtemisGSI, "Artemis RGB integration", plugin_version, PLUGINTYPE_THREADED)


void ArtemisGSI::onLoad()
{
	const std::string WEBSERVER_FILE = "C:\\ProgramData\\Artemis\\webserver.txt";
	if (!std::filesystem::exists(WEBSERVER_FILE)) {
		cvarManager->log("Artemis webserver file not found, exiting...");
		cvarManager->executeCommand(fmt::format("sleep 2; plugin unload \"{}\"", "ArtemisGSI"));
		return;
	}

	std::ifstream file(WEBSERVER_FILE);
	if (!file.good()) {
		cvarManager->log("Artemis webserver file read error, exiting...");
		cvarManager->executeCommand(fmt::format("sleep 1; plugin unload \"{}\"", "ArtemisGSI"));
		return;
	}

	std::string line;
	if (!std::getline(file, line)) {
		cvarManager->log("Artemis webserver file was empty, exiting...");
		cvarManager->executeCommand(fmt::format("sleep 1; plugin unload \"{}\"", "ArtemisGSI"));
		return;
	}

	//remove trailing slash
	if(line.back() == '/')
		line.pop_back();

	cvarManager->log(fmt::format("Artemis webserver file contained {}, starting", line));

	artemisClient = new httplib::Client(line.c_str());
	artemisClient->set_connection_timeout(0, 50000);

	canSendUpdates = true;
	std::thread t(&ArtemisGSI::StartLoop, this);
	t.detach();

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
	canSendUpdates = false;
}

void ArtemisGSI::StartLoop() {
	cvarManager->log("Starting dedicated thread...");

	while (canSendUpdates) {
		gameWrapper->Execute(std::bind(&ArtemisGSI::Update, this));
		std::string newJson = GameState.GetJson().dump();
		if (newJson != json) {
			json = newJson;
			SendToArtemis(json);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 30));
	}

	cvarManager->log("Stopping dedicated thread...");
}

void ArtemisGSI::Update() {
	ServerWrapper wrapper = GetCurrentGameWrapperType();

	if (!wrapper.IsNull())
		this->UpdateGameState(wrapper);
	else
		GameState.Reset();

	return;
}

void ArtemisGSI::SendToArtemis(std::string data) {
	auto response = artemisClient->Post("/plugins/945dc0aa-7ee3-47ec-9be6-f378fb7cb7b0/update", data, "application/json");

	if (!response) {
		cvarManager->log("Error sending data to Artemis, stopping...");
		canSendUpdates = false;
	}
}

void ArtemisGSI::UpdateGameState(ServerWrapper wrapper)
{
	GameState.Match.Overtime = wrapper.GetbOverTime();
	GameState.Match.UnlimitedTime = wrapper.GetbUnlimitedTime();
	if (!GameState.Match.Overtime && !GameState.Match.UnlimitedTime)
		GameState.Match.Time = wrapper.GetSecondsRemaining();
	else
		GameState.Match.Time = -1;

	ArrayWrapper<TeamWrapper> teams = wrapper.GetTeams();
	for (int i = 0; i < teams.Count(); i++) {

		auto team = teams.Get(i);
		if (team.IsNull()) continue;

		GameState.Match.Teams[i].Goals = team.GetScore();

		if (!team.GetSanitizedTeamName().IsNull())
			GameState.Match.Teams[i].Name = team.GetSanitizedTeamName().ToString();
		else
			GameState.Match.Teams[i].Name = i == 0 ? "Blue" : "Orange";

		auto primaryColor = team.GetPrimaryColor();
		auto secondaryColor = team.GetSecondaryColor();
		auto fontColor = team.GetFontColor();

		GameState.Match.Teams[i].PrimaryColor.SetValues(primaryColor);
		GameState.Match.Teams[i].SecondaryColor.SetValues(secondaryColor);
		GameState.Match.Teams[i].FontColor.SetValues(fontColor);
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
			GameState.Player.BallTouches = local.GetBallTouches();
			GameState.Player.CarTouches = local.GetCarTouches();
			GameState.Player.Demolishes = local.GetMatchDemolishes();

			if (!local.GetTeam().IsNull())
				GameState.Player.Team = local.GetTeam().GetTeamIndex();
			else
				GameState.Player.Team = -1;

			auto car = local.GetCar();
			if (!car.IsNull()) {
				auto boostComponent = car.GetBoostComponent();
				if (!boostComponent.IsNull()) {
					GameState.Car.Boost = boostComponent.GetCurrentBoostAmount();
				}
				else {
					GameState.Car.Boost = -1;
				}

				GameState.Car.SuperSonic = car.GetbSuperSonic();
				GameState.Car.IsOnWall = car.IsOnWall();
				GameState.Car.IsOnGround = car.IsOnGround();
				GameState.Car.SpeedKph = (car.GetVelocity().magnitude() * 0.036f) + 0.5f;
			}
		}
		else {
			GameState.Player.Score = -1;
			GameState.Player.Goals = -1;
			GameState.Player.Assists = -1;
			GameState.Player.Saves = -1;
			GameState.Player.Shots = -1;
			GameState.Player.Team = -1;
		}
	}

	GameSettingPlaylistWrapper playlistWrapper = wrapper.GetPlaylist();
	if (playlistWrapper.memory_address != NULL)
		GameState.Match.Playlist = playlistWrapper.GetPlaylistId();
	else
		GameState.Match.Playlist = -1;
}

ServerWrapper ArtemisGSI::GetCurrentGameWrapperType() {
	if (gameWrapper->IsInOnlineGame()) {
		GameState.Status = GameStatus::InGame;
		return gameWrapper->GetOnlineGame();
	}
	else if (gameWrapper->IsSpectatingInOnlineGame()) {
		GameState.Status = GameStatus::Spectate;
		return gameWrapper->GetOnlineGame();
	}
	else if (gameWrapper->IsInReplay()) {
		GameState.Status = GameStatus::Replay;
		return gameWrapper->GetGameEventAsReplay();
	}
	else if (gameWrapper->IsInFreeplay()) {
		GameState.Status = GameStatus::Freeplay;
		return gameWrapper->GetGameEventAsServer();
	}
	else if (gameWrapper->IsInCustomTraining()) {
		GameState.Status = GameStatus::Training;
		return gameWrapper->GetGameEventAsServer();
	}
	else if (gameWrapper->IsInGame()) {
		GameState.Status = GameStatus::InGame;
		return gameWrapper->GetGameEventAsServer();
	}
	else {
		GameState.Status = GameStatus::Menu;
		return NULL;
	}
}