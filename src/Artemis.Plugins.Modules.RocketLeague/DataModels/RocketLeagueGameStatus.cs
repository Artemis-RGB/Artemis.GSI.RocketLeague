﻿using Artemis.Core.DataModelExpansions;
using SkiaSharp;

namespace Artemis.Plugins.Modules.RocketLeague
{
    public enum RocketLeagueGameStatus
    {
        Menu = -1,
	    Replay = 0,
	    InGame = 1,
	    Freeplay = 2,
	    Training = 3,
	    Spectate = 4
    };
}