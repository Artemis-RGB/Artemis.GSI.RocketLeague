﻿using Artemis.Core;
using Artemis.Core.Modules;
using Artemis.Core.Services;
using SkiaSharp;
using System;

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

            var request = _webServerService.AddDataModelJsonEndPoint(this, "update");
            request.ProcessedRequest += OnProcessedRequest;
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