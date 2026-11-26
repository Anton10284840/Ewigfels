<#
.SYNOPSIS
  Installiert die Basis-Tools für Ewigfels per winget (Phase 0).
.DESCRIPTION
  Ausführen in PowerShell ALS ADMINISTRATOR:
    Set-ExecutionPolicy -Scope Process Bypass
    .\Scripts\Tools\Install-Tools.ps1
  Unreal Engine selbst wird über den Epic Games Launcher installiert (siehe docs/SETUP_WINDOWS.md).
#>
$ErrorActionPreference = 'Stop'

function Install-WingetPackage([string]$Id, [string]$ExtraArgs = '') {
    Write-Host "==> $Id" -ForegroundColor Cyan
    $cmd = "winget install --id $Id -e --accept-source-agreements --accept-package-agreements $ExtraArgs"
    Invoke-Expression $cmd
}

# 1) Git (enthält Git LFS) + 7-Zip (zum Entpacken von ComfyUI Portable)
Install-WingetPackage 'Git.Git'
Install-WingetPackage '7zip.7zip'

# 2) Epic Games Launcher
Install-WingetPackage 'EpicGames.EpicGamesLauncher'

# 3) Visual Studio 2022 Community mit allen Workloads/Komponenten für UE 5.8
$vsComponents = @(
    'Microsoft.VisualStudio.Workload.NativeGame',        # Spieleentwicklung mit C++
    'Microsoft.VisualStudio.Workload.NativeDesktop',     # Desktopentwicklung mit C++
    'Microsoft.VisualStudio.Workload.ManagedDesktop',    # .NET-Desktopentwicklung
    'Microsoft.VisualStudio.Component.VC.Tools.x86.x64', # MSVC v143
    'Microsoft.VisualStudio.Component.Windows11SDK.22621',
    'Microsoft.VisualStudio.Component.VC.Llvm.Clang',    # optional, schadet nicht
    'Microsoft.VisualStudio.Component.Unreal',           # Unreal-Engine-Installer-Integration
    'Microsoft.VisualStudio.Component.Unreal.Ide',       # IDE-Unterstützung für Unreal
    'Microsoft.Net.Component.4.6.2.TargetingPack'
) | ForEach-Object { "--add $_" }
$override = "--wait --passive --norestart --includeRecommended " + ($vsComponents -join ' ')
Install-WingetPackage 'Microsoft.VisualStudio.2022.Community' "--override `"$override`""

# 4) Blender
Install-WingetPackage 'BlenderFoundation.Blender'

# 5) Python 3.12 (für FreeMoCap und eigene Tools; ComfyUI Portable bringt eigenes Python mit)
Install-WingetPackage 'Python.Python.3.12'

# 6) Git-Grundkonfiguration
git lfs install
git config --global core.longpaths true
git config --global init.defaultBranch main

Write-Host "`nFertig. Jetzt: Neustart, dann docs/SETUP_WINDOWS.md ab Schritt 3 (Unreal Engine)." -ForegroundColor Green
