using Artemis.Core.DataModelExpansions;
using SkiaSharp;

namespace Artemis.Plugins.Modules.RocketLeague
{
    public class RocketLeagueTeam
    {
        public int Goals { get; set; }
        public string Name { get; set; }

        [DataModelIgnore]
        internal RocketLeagueColor PrimaryColor { get; set; }
        public SKColor Primary => new(PrimaryColor.Red, PrimaryColor.Green, PrimaryColor.Blue);

        [DataModelIgnore]
        internal RocketLeagueColor SecondaryColor { get; set; }
        public SKColor Secondary => new(SecondaryColor.Red, SecondaryColor.Green, SecondaryColor.Blue);
    }
}