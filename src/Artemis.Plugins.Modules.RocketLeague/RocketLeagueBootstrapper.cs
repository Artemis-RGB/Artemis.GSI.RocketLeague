using Artemis.Core;
using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Artemis.Plugins.Modules.RocketLeague
{
    public class RocketLeagueBootstrapper : PluginBootstrapper
    {
        public override void OnPluginLoaded(Plugin plugin)
        {
            AddPluginPrerequisite(new BakkesModPrerequisite(plugin));
            AddPluginPrerequisite(new BakkesModPluginPrerequisite(plugin));
        }
    }

    internal class BakkesModPluginPrerequisite : PluginPrerequisite
    {
        public override string Name => "Artemis GSI BakkesMod Plugin";

        public override string Description => "Plugin for BakkesMod responsible for sending data to Artemis";

        public override bool RequiresElevation => false;

        public override List<PluginPrerequisiteAction> InstallActions { get; }

        public override List<PluginPrerequisiteAction> UninstallActions { get; }

        public override bool IsMet()
        {
            if (!BakkesModUtils.TryGetBakkesModPath(out var bakkesModPath))
                return false;

            var pluginsPath = Path.Combine(bakkesModPath, "plugins");
            if (!Directory.Exists(pluginsPath))
                return false;

            var pluginFilename = Path.Combine(pluginsPath, "ArtemisGSI.dll");

            return File.Exists(pluginFilename);
        }

        public BakkesModPluginPrerequisite(Plugin plugin)
        {
            if(!BakkesModUtils.TryGetBakkesModPath(out var path))
            {
                //bruh
            }

            string pluginPath = Path.Combine(path, "plugins", "ArtemisGSI.dll");

            InstallActions = new List<PluginPrerequisiteAction>
            {
                new DownloadFileAction("Download ArtemisGSI plugin", "https://github.com/diogotr7/Artemis.GSI.RocketLeague/releases/latest/download/Artemis.GSI.dll", pluginPath),
            };
        }


    }

    internal class BakkesModPrerequisite : PluginPrerequisite
    {
        public override string Name => "BakkesMod";

        public override string Description => "Mod for Rocket League required for game state integration.";

        public override bool RequiresElevation => false;

        public override List<PluginPrerequisiteAction> InstallActions { get; }

        public override List<PluginPrerequisiteAction> UninstallActions { get; }

        public override bool IsMet()
        {
            return BakkesModUtils.TryGetBakkesModPath(out var _);
        }

        public BakkesModPrerequisite(Plugin plugin)
        {
            string installerFilename = Path.Combine(plugin.Directory.FullName, "BakkesModSetup.exe");

            InstallActions = new List<PluginPrerequisiteAction>
            {
                new DownloadFileAction("Download BakkesMod setup", "https://github.com/bakkesmodorg/BakkesModInjectorCpp/releases/latest/download/BakkesModSetup.exe", installerFilename),
                new ExecuteFileAction("Install BakkesMod", installerFilename),
                new DeleteFileAction("Cleaning up...", installerFilename),
            };
        }
    }

    internal static class BakkesModUtils
    {
        internal static bool TryGetBakkesModPath(out string path)
        {
            path = null;
            RegistryKey key = Registry.CurrentUser.OpenSubKey(@"SOFTWARE\BakkesMod\AppPath", false);
            if (key == null)
                return false;

            path = key?.GetValue("BakkesModPath")?.ToString();
            if (path == null)
                return false;

            return true;
        }
    }
}
