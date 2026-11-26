<#
.SYNOPSIS
  Führt die Automation-Tests "Ewigfels.*" headless aus (nach erfolgreichem Build).
#>
param([string]$EngineDir = "C:\Program Files\Epic Games\UE_5.8")
$root = Resolve-Path "$PSScriptRoot\..\.."
$uproject = Join-Path $root 'Ewigfels.uproject'
$cmd = "$EngineDir\Engine\Binaries\Win64\UnrealEditor-Cmd.exe"

& $cmd "$uproject" -ExecCmds="Automation RunTests Ewigfels;Quit" -unattended -nopause -nosplash -nullrhi -log -stdout -TestExit="Automation Test Queue Empty"
if ($LASTEXITCODE -eq 0) { Write-Host 'Tests bestanden.' -ForegroundColor Green }
else { Write-Host "Tests fehlgeschlagen ($LASTEXITCODE). Details: Saved\Logs\Ewigfels.log" -ForegroundColor Red; exit $LASTEXITCODE }
