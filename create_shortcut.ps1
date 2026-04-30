# Run this once to create a Desktop shortcut for the Optris Thermal Monitor
$WshShell = New-Object -ComObject WScript.Shell
$Shortcut = $WshShell.CreateShortcut("$env:USERPROFILE\Desktop\Optris Monitor.lnk")
$Shortcut.TargetPath = "cmd.exe"
$Shortcut.Arguments = "/c `"$PSScriptRoot\start_monitor.bat`""
$Shortcut.WorkingDirectory = $PSScriptRoot
$Shortcut.WindowStyle = 1
$Shortcut.IconLocation = "C:\Program Files\Optris\otcsdk\Optris.ico"
$Shortcut.Description = "Optris Thermal Monitor - live feed and alarm daemon"
$Shortcut.Save()
Write-Host "Shortcut created: $env:USERPROFILE\Desktop\Optris Monitor.lnk"
