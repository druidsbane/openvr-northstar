using MahApps.Metro.Controls;
using Newtonsoft.Json.Linq;
using System;
using System.Diagnostics;
using System.IO;
using System.IO.Compression;
using System.Security.AccessControl;
using System.Windows;


namespace NorthstarLauncher
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : MetroWindow
    {

        string steamvrRoot { get; set; }
        string vrSettingsPath { get; set; }
        string systemVrSettingsPath { get; set; }

        public MainWindow()
        {
            InitializeComponent();

            steamvrRoot = FindSteamRoot(); // @"C:/Program Files (x86)/Steam/";
            vrSettingsPath = steamvrRoot + "config/steamvr.vrsettings";
            systemVrSettingsPath = steamvrRoot + "steamapps/common/SteamVR/resources/settings/default.vrsettings";

        }

        string FindSteamRoot()
        {
            DriveInfo[] allDrives = DriveInfo.GetDrives();

            foreach (DriveInfo d in allDrives)
            {
                if (d.IsReady && d.DriveType == DriveType.Fixed)
                {
                    string[] paths =
                    {
                        Path.Combine(new string[] { d.RootDirectory.FullName, "Steam"}),
                        Path.Combine(new string[] { d.RootDirectory.FullName, "Program Files", "Steam"}),
                        Path.Combine(new string[] { d.RootDirectory.FullName, "Program Files (x86)", "Steam"}),
                        Path.Combine(new string[] { d.RootDirectory.FullName, "SteamLibrary"}),
                        Path.Combine(new string[] { d.RootDirectory.FullName, "Program Files", "SteamLibrary"}),
                        Path.Combine(new string[] { d.RootDirectory.FullName, "Program Files (x86)", "SteamLibrary"})
                    };

                    foreach (var path in paths)
                    {
                        if (Directory.Exists(Path.Combine(path, @"steamapps/common/SteamVR/drivers")))
                        {
                            return path + "/";
                        }
                    }
                }
            }

            return "";
        }

        private void KillSteamVRButton_Click(object sender, RoutedEventArgs e)
        {
            KillSteamVR();
        }

        private void LaunchSteamVRButton_Click(object sender, RoutedEventArgs e)
        {
            KillSteamVR();
            UpdateSteamVRConfig();
            UpdateSteamVRSystemConfig();
            InstallSteamVRDriver();
            LaunchSteamVR();
        }

        private void UpdateSteamVRConfig()
        {
            BackupFile(vrSettingsPath);

            JObject northstarConfig = JObject.Parse(File.ReadAllText("steamvrconfig.json"));
            JObject steamvrConfig = JObject.Parse(File.ReadAllText(vrSettingsPath));
            steamvrConfig.Merge(northstarConfig, new JsonMergeSettings
            {
                MergeArrayHandling = MergeArrayHandling.Union
            });
            if (!steamvrConfig.ContainsKey("direct_mode"))
            {
                steamvrConfig["direct_mode"] = new JObject();
            }
            steamvrConfig["direct_mode"]["enable"] = false;

            WriteAllTextReadOnly(vrSettingsPath, steamvrConfig.ToString());
        }

        private void UpdateSteamVRSystemConfig()
        {
            BackupFile(systemVrSettingsPath);

            JObject northstarConfig = JObject.Parse(File.ReadAllText("steamvrconfig.json"));
            JObject steamvrConfig = JObject.Parse(File.ReadAllText(systemVrSettingsPath));
            steamvrConfig.Merge(northstarConfig, new JsonMergeSettings
            {
                MergeArrayHandling = MergeArrayHandling.Union
            });
            if (!steamvrConfig.ContainsKey("direct_mode"))
            {
                steamvrConfig["direct_mode"] = new JObject();
            }
            steamvrConfig["direct_mode"]["enable"] = false;

            WriteAllTextReadOnly(systemVrSettingsPath, steamvrConfig.ToString());
        }

        private void BackupFile(string path)
        {
            if (backupConfigs.IsChecked != true)
            {
                return;
            }

            var extension = Path.GetExtension(path);
            File.Copy(path, path + ".backup-" + DateTime.Now.Ticks + "." + extension);
        }

        private void InstallSteamVRDriver()
        {
            var driverDir = steamvrRoot + @"steamapps/common/SteamVR/drivers/northstar";

            var driverFiles = @"northstar";
            var driverFileDir = driverDir + @"/bin/win64";
            Directory.CreateDirectory(driverFileDir);
            foreach (string file in Directory.GetFiles(driverFiles))
            {
                string name = Path.GetFileName(file);
                string dest = Path.Combine(driverFileDir, name);
                File.Copy(file, dest, true);
            }

            driverFiles = @"Resources/resources/settings";
            driverFileDir = driverDir + @"/resources/settings";
            Directory.CreateDirectory(driverFileDir);
            foreach (string file in Directory.GetFiles(driverFiles))
            {
                string name = Path.GetFileName(file);
                string dest = Path.Combine(driverFileDir, name);
                File.Copy(file, dest, true);
            }

            File.Copy("Resources/driver.vrdrivermanifest", Path.Combine(driverDir, "driver.vrdrivermanifest"), true);

            var src = (string)global::Properties.Settings.Default["SteamVRCalibrationPath"];
            src = File.Exists(src) ? src : "Resources/headset_profile.json";
            CopyReadOnly(src, Path.Combine(driverDir, "headset_profile.json"));
        }

        private void CopyReadOnly(string src, string dest)
        {
            var attr = File.GetAttributes(dest);
            File.SetAttributes(dest, attr & ~FileAttributes.ReadOnly);
            File.Copy(src, dest, true);
            File.SetAttributes(dest, attr & FileAttributes.ReadOnly);
        }

        private void WriteAllTextReadOnly(string dest, string text)
        {
            File.SetAttributes(dest, File.GetAttributes(dest) & ~FileAttributes.ReadOnly);
            File.WriteAllText(dest, text);
            File.SetAttributes(dest, File.GetAttributes(dest) & FileAttributes.ReadOnly);
        }

        private void KillSteamVR()
        {
            KillProcesses(new string[] { "vrmonitor", "vrserver", "vrstartup" });
        }

        private void LaunchSteamVR()
        {
            System.Diagnostics.Process.Start(steamvrRoot + @"steamapps\common\SteamVR\bin\win64\vrstartup.exe");
        }
        private void KillProcesses(string[] names)
        {
            try
            {
                var procs = Process.GetProcesses();
                foreach (var proc in procs)
                {
                    Trace.WriteLine(proc.ProcessName);
                    foreach (var name in names)
                    {
                        if (proc.ProcessName.ToLower().Contains(name))
                        {
                            proc.Kill();
                        }
                    }
                }
                //Process[] proc = Process.GetProcessesByName(name);
                //proc[0].Kill();
            }
            catch { }
        }

        private void Window_Drop(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop))
            {
                string path = ((string[])e.Data.GetData(DataFormats.FileDrop))[0];
                global::Properties.Settings.Default["SteamVRCalibrationPath"] = path;
            }
        }
    }
}
