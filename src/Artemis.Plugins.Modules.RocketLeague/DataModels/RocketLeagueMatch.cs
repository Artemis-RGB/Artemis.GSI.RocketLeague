using Artemis.Core.DataModelExpansions;
using SkiaSharp;

namespace Artemis.Plugins.Modules.RocketLeague
{
    public class RocketLeagueMatch
    {
        public RocketLeagueTeam Team_0 { get; set; }
        public RocketLeagueTeam Team_1 { get; set; }
        public int Time { get; set; }
        public RocketLeaguePlaylist Playlist { get; set; }
    }
}