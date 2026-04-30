# Registers the Optris Monitor as a Windows Task Scheduler task.
# Runs at logon for the current user, restarts automatically on failure.
# Run this script once as Administrator.
#
# To remove the task later:  Unregister-ScheduledTask -TaskName "OptrisMonitor" -Confirm:$false

$TaskName   = "OptrisMonitor"
$Python     = "$PSScriptRoot\.venv\Scripts\pythonw.exe"   # pythonw = no console window
$Script     = "$PSScriptRoot\watchdog.py"
$Config     = "$PSScriptRoot\config\monitor.json"
$WorkingDir = $PSScriptRoot
$LogonUser  = "$env:USERDOMAIN\$env:USERNAME"

$action = New-ScheduledTaskAction `
    -Execute $Python `
    -Argument "`"$Script`" `"$Config`"" `
    -WorkingDirectory $WorkingDir

# Trigger: run when this user logs on
$trigger = New-ScheduledTaskTrigger -AtLogOn -User $LogonUser

$settings = New-ScheduledTaskSettingsSet `
    -RestartCount 10 `
    -RestartInterval (New-TimeSpan -Minutes 1) `
    -ExecutionTimeLimit ([TimeSpan]::Zero) `
    -MultipleInstances IgnoreNew

$principal = New-ScheduledTaskPrincipal `
    -UserId $LogonUser `
    -LogonType Interactive `
    -RunLevel Limited

$task = New-ScheduledTask `
    -Action $action `
    -Trigger $trigger `
    -Settings $settings `
    -Principal $principal `
    -Description "Optris Thermal Monitor — restarts automatically on crash"

Register-ScheduledTask -TaskName $TaskName -InputObject $task -Force

Write-Host "Task '$TaskName' registered."
Write-Host "It will start automatically at next logon."
Write-Host "To start it now: Start-ScheduledTask -TaskName '$TaskName'"
