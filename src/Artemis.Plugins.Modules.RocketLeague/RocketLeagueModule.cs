using Artemis.Core;
using Artemis.Core.Modules;
using Artemis.Core.Services;
using SkiaSharp;

namespace Artemis.Plugins.Modules.RocketLeague
{
    public class RocketLeagueModule : ProfileModule<RocketLeagueDataModel>
    {
        private readonly IWebServerService _webServerService;

        public RocketLeagueModule(IWebServerService webServerService)
        {
            _webServerService = webServerService;
        }

        public override void Enable()
        {
            DisplayName = "Rocket League";
            DisplayIcon = "RocketLeague.svg";
            DefaultPriorityCategory = ModulePriorityCategory.Application;
            ActivationRequirements.Add(new ProcessActivationRequirement("RocketLeague"));
            //_webServerService.AddStringEndPoint(this, "update", s => 
            //{

            //});

            _webServerService.AddJsonEndPoint<RocketLeagueDataModel>(this, "update", s =>
            {
                DataModel.Status = s.Status;
                DataModel.Match = s.Match;
                DataModel.Player = s.Player;
            });
        }

        public override void Disable()
        {
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

        public override void Render(double deltaTime, SKCanvas canvas, SKImageInfo canvasInfo)
        {
        }
    }
}