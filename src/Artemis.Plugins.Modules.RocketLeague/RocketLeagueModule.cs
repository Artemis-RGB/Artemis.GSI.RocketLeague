using Artemis.Core.Modules;
using Artemis.Core.Services;
using SkiaSharp;

namespace Artemis.Plugins.Modules.RocketLeague
{
    public class RocketLeagueModule : Module<RocketLeagueDataModel>
    {
        private readonly IWebServerService _webServerService;

        private DataModelJsonPluginEndPoint<RocketLeagueDataModel> _updateEndpoint;

        public RocketLeagueModule(IWebServerService webServerService)
        {
            DisplayName = "Rocket League";
            DisplayIcon = "RocketLeague.svg";
            ActivationRequirements.Add(new ProcessActivationRequirement("RocketLeague"));

            _webServerService = webServerService;
        }

        public override void Enable()
        {
            _updateEndpoint = _webServerService.AddDataModelJsonEndPoint(this, "update");
            _updateEndpoint.ProcessedRequest += OnProcessedRequest;
        }

        private void OnProcessedRequest(object sender, EndpointRequestEventArgs e)
        {
            if (DataModel.Player == null)
            {
                DataModel.Match.MyTeam = null;
                DataModel.Match.OpponentTeam = null;
            }

            if (DataModel.Player.Team == 0)
            {
                DataModel.Match.MyTeam = DataModel.Match.Team_0;
                DataModel.Match.OpponentTeam = DataModel.Match.Team_1;
            }
            else
            {
                DataModel.Match.MyTeam = DataModel.Match.Team_1;
                DataModel.Match.OpponentTeam = DataModel.Match.Team_0;
            }
        }

        public override void Disable()
        {
            _updateEndpoint.ProcessedRequest -= OnProcessedRequest;
            _webServerService.RemovePluginEndPoint(_updateEndpoint);
        }

        public override void ModuleActivated(bool isOverride)
        {
        }

        public override void ModuleDeactivated(bool isOverride)
        {
        }

        public override void Update(double deltaTime)
        {
        }
    }
}