<#
.SYNOPSIS
  Erzeugt die VS-Projektdateien und baut den Editor (Development Editor, Win64).
  Ausführen im Projektordner: .\Scripts\Tools\Build-Ewigfels.ps1
#>
param(
    [string]$EngineDir = "C:\Program Files\Epic Games\UE_5.8",
    [switch]$SkipProjectFiles
)
$ErrorActionPreference = 'Stop'
$root = Resolve-Path "$PSScriptRoot\..\.."
$uproject = Join-Path $root 'Ewigfels.uproject'
$ubt = "$EngineDir\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe"
$build = "$EngineDir\Engine\Build\BatchFiles\Build.bat"

if (-not (Test-Path $build)) { throw "Engine nicht gefunden: $EngineDir. Pfad mit -EngineDir angeben." }

if (-not $SkipProjectFiles) {
    Write-Host '==> Visual-Studio-Projektdateien erzeugen' -ForegroundColor Cyan
    & $ubt -projectfiles -project="$uproject" -game -rocket -progress
    if ($LASTEXITCODE -ne 0) { throw "Projektdateien fehlgeschlagen ($LASTEXITCODE)" }
}

Write-Host '==> EwigfelsEditor Win64 Development bauen' -ForegroundColor Cyan
& $build EwigfelsEditor Win64 Development -Project="$uproject" -WaitMutex
if ($LASTEXITCODE -ne 0) { throw "Build fehlgeschlagen ($LASTEXITCODE). Log: $env:LOCALAPPDATA\UnrealBuildTool\Log.txt" }

Write-Host 'Build erfolgreich.' -ForegroundColor Green
