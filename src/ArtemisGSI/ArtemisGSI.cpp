#include "pch.h"
#include "ArtemisGSI.h"

BAKKESMOD_PLUGIN(ArtemisGSI, "Artemis RGB integration", plugin_version, PLUGINTYPE_THREADED)

void ArtemisGSI::onLoad()
{
	const std::string WEBSERVER_FILE = "C:\\ProgramData\\Artemis\\webserver.txt";
	if (!std::filesystem::exists(WEBSERVER_FILE)) {
		cvarManager->log("Artemis webserver file not found, exiting...");
		cvarManager->executeCommand("sleep 1; plugin unload artemisgsi");
		return;
	}

	std::ifstream file(WEBSERVER_FILE);
	if (!file.good()) {
		cvarManager->log("Artemis webserver file read error, exiting...");
		cvarManager->executeCommand("sleep 1; plugin unload artemisgsi");
		return;
	}

	std::string line;
	if (!std::getline(file, line)) {
		cvarManager->log("Artemis webserver file was empty, exiting...");
		cvarManager->executeCommand("sleep 1; plugin unload artemisgsi");
		return;
	}

	//remove trailing slash
	if(line.back() == '/')
		line.pop_back();

	cvarManager->log("Artemis client starting with url " + line);

	artemisClient = new httplib::Client(line.c_str());
	artemisClient->set_connection_timeout(0, 50000);

	canSendUpdates = true;
	std::thread t(&ArtemisGSI::StartLoop, this);
	t.detach();
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
}

void ArtemisGSI::SendToArtemis(std::string data) {
	auto response = artemisClient->Post("/plugins/945dc0aa-7ee3-47ec-9be6-f378fb7cb7b0/update", data, "application/json");

	if (!response) {
		cvarManager->log("Error sending data to Artemis, stopping...");
		cvarManager->executeCommand("sleep 1; plugin unload artemisgsi");
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

	auto teams = wrapper.GetTeams();
	for (int i = 0; i < teams.Count(); i++) {

		auto team = teams.Get(i);
		if (team.IsNull()) continue;

		GameState.Match.Teams[i].Goals = team.GetScore();

		if (!team.GetSanitizedTeamName().IsNull())
			GameState.Match.Teams[i].Name = team.GetSanitizedTeamName().ToString();
		else
			GameState.Match.Teams[i].Name = i == 0 ? "Blue" : "Orange";

		GameState.Match.Teams[i].PrimaryColor.SetValues(team.GetPrimaryColor());
		GameState.Match.Teams[i].SecondaryColor.SetValues(team.GetSecondaryColor());
		GameState.Match.Teams[i].FontColor.SetValues(team.GetFontColor());
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
	if (!playlistWrapper.IsNull())
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