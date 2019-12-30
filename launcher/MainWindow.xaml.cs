using MahApps.Metro.Controls;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Net;
using System.Runtime.InteropServices;
using System.Security.AccessControl;
using System.Windows;
using System.Xml.Linq;

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
            InstallLeapSteamVRDriver();
            InstallStructureCoreSteamVRDriver();
            LaunchSteamVR();
            AdjustNorthstarResolution();
        }

        private void AdjustNorthstarResolution()
        {

            foreach (var screen in System.Windows.Forms.Screen.AllScreens)
            {
                Console.WriteLine(screen);
                //// For each screen, add the screen properties to a list box.
                //listBox1.Items.Add("Device Name: " + screen.DeviceName);
                //listBox1.Items.Add("Bounds: " + screen.Bounds.ToString());
                //listBox1.Items.Add("Type: " + screen.GetType().ToString());
                //listBox1.Items.Add("Working Area: " + screen.WorkingArea.ToString());
                //listBox1.Items.Add("Primary Screen: " + screen.Primary.ToString());
            }

            WaitForProcesses(new string[] { "vrmonitor", "vrserver", "vrstartup" });
            Display.MonitorChanger.UpdateNorthstarResolution();
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
            steamvrConfig["steamvr"]["activateMultipleDrivers"] = true;

            applyTrackingOverrides(steamvrConfig);

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

            applyTrackingOverrides(steamvrConfig);

            WriteAllTextReadOnly(systemVrSettingsPath, steamvrConfig.ToString());
        }

        private void applyTrackingOverrides(JObject steamvrConfig)
        {
            steamvrConfig["TrackingOverrides"] = new JObject();
            if (installStructureCoreDriver.IsChecked == true)
            {
                steamvrConfig["TrackingOverrides"]["/devices/structurecore/1337821"] = "/user/head";
            }
            else if (installT265Driver.IsChecked == true)
            {
                //steamvrConfig["TrackingOverrides"]["/devices/t265/<serial>"] = "/user/head";
            }
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

        private void InstallLeapSteamVRDriver()
        {
            if (installLeapDriver.IsChecked != true)
            {
                return;
            }

            var driverDir = steamvrRoot + @"steamapps/common/SteamVR/drivers/leap";
            if (Directory.Exists(driverDir))
            {
                Directory.Delete(driverDir, true);
            }

            var leapDriverZip = @"driver_leap_1.0-157_x86.zip";
            var leapDriverZipLocal = leapDriverZip;
            var leapDriverZipUrl = @"https://github.com/SDraw/driver_leap/releases/download/1.0/" + leapDriverZip;
            if (!File.Exists(leapDriverZipLocal))
            {
                using (var client = new WebClient())
                {
                    client.DownloadFile(leapDriverZipUrl, leapDriverZipLocal);
                }
            }

            if (!File.Exists(leapDriverZipLocal)) { MessageBox.Show(@"Failed to download Leap SteamVR Driver"); }

            ZipFile.ExtractToDirectory(leapDriverZipLocal, driverDir);

            var config = driverDir + @"/cfg/control_config.xml";
            var xdoc = XDocument.Load(config);
            XElement item = (from el in xdoc.Root.Descendants()
                             where (string)el.Attribute("name") == "emulated_controller"
                             select el).FirstOrDefault();
            item.SetAttributeValue("value", "index");
            xdoc.Save(config);
        }

        private void InstallStructureCoreSteamVRDriver()
        {
            if (installStructureCoreDriver.IsChecked != true)
            {
                return;
            }

            var driverDir = steamvrRoot + @"steamapps/common/SteamVR/drivers/structurecore";
            if (Directory.Exists(driverDir))
            {
                Directory.Delete(driverDir, true);
            }

            var structureCoreDriverZip = @"openvr-structurecore.zip";
            if (!File.Exists(structureCoreDriverZip))
            {
                return;
            }

            ZipFile.ExtractToDirectory(structureCoreDriverZip, driverDir);
        }

        private void CopyReadOnly(string src, string dest)
        {
            if (File.Exists(dest))
            {
                var attr = File.GetAttributes(dest);
                File.SetAttributes(dest, attr & ~FileAttributes.ReadOnly);
            }

            File.Copy(src, dest, true);

            if (File.Exists(dest))
            {
                var attr = File.GetAttributes(dest);
                File.SetAttributes(dest, attr & FileAttributes.ReadOnly);
            }
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

        private void WaitForProcesses(string[] names)
        {
            var allNames = new HashSet<string>(names);
            try
            {
                while (allNames.Count > 0)
                {
                    var procs = Process.GetProcesses();
                    foreach (var proc in procs)
                    {
                        Trace.WriteLine(proc.ProcessName);
                        foreach (var name in names)
                        {
                            if (proc.ProcessName.ToLower().Contains(name))
                            {
                                allNames.Remove(name);
                            }
                        }
                    }
                }
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


// based on https://stackoverflow.com/questions/195267/use-windows-api-from-c-sharp-to-set-primary-monitor
namespace Display
{
    public class MonitorChanger
    {
        public static void UpdateNorthstarResolution()
        {
            var device = new DISPLAY_DEVICE();
            device.cb = Marshal.SizeOf(device);

            // Update remaining devices
            for (uint otherid = 0; NativeMethods.EnumDisplayDevices(null, otherid, ref device, 0); otherid++)
            {
                if (device.StateFlags.HasFlag(DisplayDeviceStateFlags.AttachedToDesktop))
                {
                    device.cb = Marshal.SizeOf(device);
                    var otherDeviceMode = new DEVMODE();

                    NativeMethods.EnumDisplaySettings(device.DeviceName, -1, ref otherDeviceMode);
                    if (otherDeviceMode.dmPelsWidth == 2880 && otherDeviceMode.dmPelsHeight == 1600)
                    {
                        otherDeviceMode.dmDisplayFrequency = 120;

                        if (NativeMethods.ChangeDisplaySettingsEx(
                            device.DeviceName,
                            ref otherDeviceMode,
                            (IntPtr)null,
                            ChangeDisplaySettingsFlags.CDS_TEST,
                            IntPtr.Zero) == DISP_CHANGE.Successful)
                        {
                            NativeMethods.ChangeDisplaySettingsEx(
                                device.DeviceName,
                                ref otherDeviceMode,
                                (IntPtr)null,
                                (ChangeDisplaySettingsFlags.CDS_UPDATEREGISTRY | ChangeDisplaySettingsFlags.CDS_NORESET),
                                IntPtr.Zero);
                        }

                        otherDeviceMode.dmDisplayFrequency = 90;

                        if (NativeMethods.ChangeDisplaySettingsEx(
                            device.DeviceName,
                            ref otherDeviceMode,
                            (IntPtr)null,
                            ChangeDisplaySettingsFlags.CDS_TEST,
                            IntPtr.Zero) == DISP_CHANGE.Successful)
                        {
                            NativeMethods.ChangeDisplaySettingsEx(
                                device.DeviceName,
                                ref otherDeviceMode,
                                (IntPtr)null,
                                (ChangeDisplaySettingsFlags.CDS_UPDATEREGISTRY | ChangeDisplaySettingsFlags.CDS_NORESET),
                                IntPtr.Zero);
                        }
                    }
                }

                device.cb = Marshal.SizeOf(device);
            }
        }

        public static void SetAsPrimaryMonitor(uint id)
        {
            var device = new DISPLAY_DEVICE();
            var deviceMode = new DEVMODE();
            device.cb = Marshal.SizeOf(device);

            NativeMethods.EnumDisplayDevices(null, id, ref device, 0);
            NativeMethods.EnumDisplaySettings(device.DeviceName, -1, ref deviceMode);
            var offsetx = deviceMode.dmPosition.x;
            var offsety = deviceMode.dmPosition.y;
            deviceMode.dmPosition.x = 0;
            deviceMode.dmPosition.y = 0;

            NativeMethods.ChangeDisplaySettingsEx(
                device.DeviceName,
                ref deviceMode,
                (IntPtr)null,
                (ChangeDisplaySettingsFlags.CDS_SET_PRIMARY | ChangeDisplaySettingsFlags.CDS_UPDATEREGISTRY | ChangeDisplaySettingsFlags.CDS_NORESET),
                IntPtr.Zero);

            device = new DISPLAY_DEVICE();
            device.cb = Marshal.SizeOf(device);

            // Update remaining devices
            for (uint otherid = 0; NativeMethods.EnumDisplayDevices(null, otherid, ref device, 0); otherid++)
            {
                if (device.StateFlags.HasFlag(DisplayDeviceStateFlags.AttachedToDesktop) && otherid != id)
                {
                    device.cb = Marshal.SizeOf(device);
                    var otherDeviceMode = new DEVMODE();

                    NativeMethods.EnumDisplaySettings(device.DeviceName, -1, ref otherDeviceMode);

                    otherDeviceMode.dmPosition.x -= offsetx;
                    otherDeviceMode.dmPosition.y -= offsety;

                    NativeMethods.ChangeDisplaySettingsEx(
                        device.DeviceName,
                        ref otherDeviceMode,
                        (IntPtr)null,
                        (ChangeDisplaySettingsFlags.CDS_UPDATEREGISTRY | ChangeDisplaySettingsFlags.CDS_NORESET),
                        IntPtr.Zero);

                }

                device.cb = Marshal.SizeOf(device);
            }

            // Apply settings
            NativeMethods.ChangeDisplaySettingsEx(null, IntPtr.Zero, (IntPtr)null, ChangeDisplaySettingsFlags.CDS_NONE, (IntPtr)null);
        }
    }

    [StructLayout(LayoutKind.Explicit, CharSet = CharSet.Ansi)]
    public struct DEVMODE
    {
        public const int CCHDEVICENAME = 32;
        public const int CCHFORMNAME = 32;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = CCHDEVICENAME)]
        [System.Runtime.InteropServices.FieldOffset(0)]
        public string dmDeviceName;
        [System.Runtime.InteropServices.FieldOffset(32)]
        public Int16 dmSpecVersion;
        [System.Runtime.InteropServices.FieldOffset(34)]
        public Int16 dmDriverVersion;
        [System.Runtime.InteropServices.FieldOffset(36)]
        public Int16 dmSize;
        [System.Runtime.InteropServices.FieldOffset(38)]
        public Int16 dmDriverExtra;
        [System.Runtime.InteropServices.FieldOffset(40)]
        public UInt32 dmFields;

        [System.Runtime.InteropServices.FieldOffset(44)]
        Int16 dmOrientation;
        [System.Runtime.InteropServices.FieldOffset(46)]
        Int16 dmPaperSize;
        [System.Runtime.InteropServices.FieldOffset(48)]
        Int16 dmPaperLength;
        [System.Runtime.InteropServices.FieldOffset(50)]
        Int16 dmPaperWidth;
        [System.Runtime.InteropServices.FieldOffset(52)]
        Int16 dmScale;
        [System.Runtime.InteropServices.FieldOffset(54)]
        Int16 dmCopies;
        [System.Runtime.InteropServices.FieldOffset(56)]
        Int16 dmDefaultSource;
        [System.Runtime.InteropServices.FieldOffset(58)]
        Int16 dmPrintQuality;

        [System.Runtime.InteropServices.FieldOffset(44)]
        public POINTL dmPosition;
        [System.Runtime.InteropServices.FieldOffset(52)]
        public Int32 dmDisplayOrientation;
        [System.Runtime.InteropServices.FieldOffset(56)]
        public Int32 dmDisplayFixedOutput;

        [System.Runtime.InteropServices.FieldOffset(60)]
        public short dmColor; // See note below!
        [System.Runtime.InteropServices.FieldOffset(62)]
        public short dmDuplex; // See note below!
        [System.Runtime.InteropServices.FieldOffset(64)]
        public short dmYResolution;
        [System.Runtime.InteropServices.FieldOffset(66)]
        public short dmTTOption;
        [System.Runtime.InteropServices.FieldOffset(68)]
        public short dmCollate; // See note below!
        [System.Runtime.InteropServices.FieldOffset(72)]
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = CCHFORMNAME)]
        public string dmFormName;
        [System.Runtime.InteropServices.FieldOffset(102)]
        public Int16 dmLogPixels;
        [System.Runtime.InteropServices.FieldOffset(104)]
        public Int32 dmBitsPerPel;
        [System.Runtime.InteropServices.FieldOffset(108)]
        public Int32 dmPelsWidth;
        [System.Runtime.InteropServices.FieldOffset(112)]
        public Int32 dmPelsHeight;
        [System.Runtime.InteropServices.FieldOffset(116)]
        public Int32 dmDisplayFlags;
        [System.Runtime.InteropServices.FieldOffset(116)]
        public Int32 dmNup;
        [System.Runtime.InteropServices.FieldOffset(120)]
        public Int32 dmDisplayFrequency;
    }

    public enum DISP_CHANGE : int
    {
        Successful = 0,
        Restart = 1,
        Failed = -1,
        BadMode = -2,
        NotUpdated = -3,
        BadFlags = -4,
        BadParam = -5,
        BadDualView = -6
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct DISPLAY_DEVICE
    {
        [MarshalAs(UnmanagedType.U4)]
        public int cb;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
        public string DeviceName;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)]
        public string DeviceString;
        [MarshalAs(UnmanagedType.U4)]
        public DisplayDeviceStateFlags StateFlags;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)]
        public string DeviceID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)]
        public string DeviceKey;
    }

    [Flags()]
    public enum DisplayDeviceStateFlags : int
    {
        /// <summary>The device is part of the desktop.</summary>
        AttachedToDesktop = 0x1,
        MultiDriver = 0x2,
        /// <summary>The device is part of the desktop.</summary>
        PrimaryDevice = 0x4,
        /// <summary>Represents a pseudo device used to mirror application drawing for remoting or other purposes.</summary>
        MirroringDriver = 0x8,
        /// <summary>The device is VGA compatible.</summary>
        VGACompatible = 0x10,
        /// <summary>The device is removable; it cannot be the primary display.</summary>
        Removable = 0x20,
        /// <summary>The device has more display modes than its output devices support.</summary>
        ModesPruned = 0x8000000,
        Remote = 0x4000000,
        Disconnect = 0x2000000,
    }

    [Flags()]
    public enum ChangeDisplaySettingsFlags : uint
    {
        CDS_NONE = 0,
        CDS_UPDATEREGISTRY = 0x00000001,
        CDS_TEST = 0x00000002,
        CDS_FULLSCREEN = 0x00000004,
        CDS_GLOBAL = 0x00000008,
        CDS_SET_PRIMARY = 0x00000010,
        CDS_VIDEOPARAMETERS = 0x00000020,
        CDS_ENABLE_UNSAFE_MODES = 0x00000100,
        CDS_DISABLE_UNSAFE_MODES = 0x00000200,
        CDS_RESET = 0x40000000,
        CDS_RESET_EX = 0x20000000,
        CDS_NORESET = 0x10000000
    }

    public class NativeMethods
    {
        [DllImport("user32.dll")]
        public static extern DISP_CHANGE ChangeDisplaySettingsEx(string lpszDeviceName, ref DEVMODE lpDevMode, IntPtr hwnd, ChangeDisplaySettingsFlags dwflags, IntPtr lParam);

        [DllImport("user32.dll")]
        // A signature for ChangeDisplaySettingsEx with a DEVMODE struct as the second parameter won't allow you to pass in IntPtr.Zero, so create an overload
        public static extern DISP_CHANGE ChangeDisplaySettingsEx(string lpszDeviceName, IntPtr lpDevMode, IntPtr hwnd, ChangeDisplaySettingsFlags dwflags, IntPtr lParam);

        [DllImport("user32.dll")]
        public static extern bool EnumDisplayDevices(string lpDevice, uint iDevNum, ref DISPLAY_DEVICE lpDisplayDevice, uint dwFlags);

        [DllImport("user32.dll")]
        public static extern bool EnumDisplaySettings(string deviceName, int modeNum, ref DEVMODE devMode);
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct POINTL
    {
        public int x;
        public int y;
    }
}
