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
$build = "$EngineDir\Engine\Build\BatchFiles\Build.bat"

if (-not (Test-Path $build)) { throw "Engine nicht gefunden: $EngineDir. Pfad mit -EngineDir angeben." }

# Offener Editor sperrt die DLLs (Live Coding) -> Build schlägt fehl oder wird übersprungen.
if (Get-Process UnrealEditor -ErrorAction SilentlyContinue) {
    throw 'Der Unreal Editor ist offen. Bitte schließen und das Skript erneut starten.'
}

if (-not $SkipProjectFiles) {
    Write-Host '==> Visual-Studio-Projektdateien erzeugen' -ForegroundColor Cyan
    # Über Build.bat statt UnrealBuildTool.exe: Build.bat nutzt das .NET, das mit der Engine kommt
    # (UnrealBuildTool.exe direkt verlangt ein systemweit installiertes .NET 10).
    & $build -projectfiles -project="$uproject" -game -rocket -progress
    if ($LASTEXITCODE -ne 0) { throw "Projektdateien fehlgeschlagen ($LASTEXITCODE)" }
}

Write-Host '==> EwigfelsEditor Win64 Development bauen' -ForegroundColor Cyan
& $build EwigfelsEditor Win64 Development -Project="$uproject" -WaitMutex
if ($LASTEXITCODE -ne 0) { throw "Build fehlgeschlagen ($LASTEXITCODE). Log: $env:LOCALAPPDATA\UnrealBuildTool\Log.txt" }

Write-Host 'Build erfolgreich.' -ForegroundColor Green
