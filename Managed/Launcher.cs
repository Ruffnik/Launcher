using System;
using System.Diagnostics;
using System.IO;
using System.Windows.Forms;
using Launcher.Properties;

try
{
    Process.Start(new ProcessStartInfo(Settings.Default.FileName, Settings.Default.Arguments) { WorkingDirectory = Directory.GetParent(Environment.GetCommandLineArgs()[0]).ToString() }).WaitForExit();
}
catch (Exception Exception)
{
    MessageBox.Show($"{Exception}", Path.GetFileNameWithoutExtension(Environment.GetCommandLineArgs()[0]));
}