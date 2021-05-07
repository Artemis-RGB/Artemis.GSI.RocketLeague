using Artemis.Core.DataModelExpansions;
using SkiaSharp;

namespace Artemis.Plugins.Modules.RocketLeague
{
    public class RocketLeagueDataModel : DataModel
    {
        public RocketLeagueGameStatus Status { get; set; }
        public RocketLeagueMatch Match { get; set; }
        public RocketLeaguePlayer Player { get; set; }
        public RocketLeagueCar Car { get; set; }
    }
}