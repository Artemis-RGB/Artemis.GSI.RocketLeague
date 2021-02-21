﻿using Artemis.Core.DataModelExpansions;
using SkiaSharp;

namespace Artemis.Plugins.Modules.RocketLeague
{
    public class RocketLeaguePlayer
    {
        public int Team { get; set; }
        public int Score { get; set; }
        public int Goals{ get; set; }
        public int Assists { get; set; }
        public int Saves { get; set; }
        public int Shots { get; set; }
        public float Boost { get; set; }
    }
}