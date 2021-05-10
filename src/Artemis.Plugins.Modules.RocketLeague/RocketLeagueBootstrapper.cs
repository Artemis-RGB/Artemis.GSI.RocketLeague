using Artemis.Core;
using Artemis.Plugins.Modules.RocketLeague.Prerequisites;

namespace Artemis.Plugins.Modules.RocketLeague
{
    public class RocketLeagueBootstrapper : PluginBootstrapper
    {
        public override void OnPluginLoaded(Plugin plugin)
        {
            AddPluginPrerequisite(new BakkesModPrerequisite(plugin));
            AddPluginPrerequisite(new BakkesModPluginPrerequisite());
        }
    }
}
