using Artemis.Core.DataModelExpansions;
using SkiaSharp;

namespace Artemis.Plugins.Modules.RocketLeague
{
    public class RocketLeagueTeam
    {
        public int Goals { get; set; }
        public string Name { get; set; }

        [DataModelIgnore]
        public RocketLeagueColor PrimaryColor { get; set; }
        public SKColor Primary => new(PrimaryColor.Red, PrimaryColor.Green, PrimaryColor.Blue);

        [DataModelIgnore]
        public RocketLeagueColor SecondaryColor { get; set; }
        public SKColor Secondary => new(SecondaryColor.Red, SecondaryColor.Green, SecondaryColor.Blue);
    }
}