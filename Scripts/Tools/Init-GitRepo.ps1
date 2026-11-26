<#
.SYNOPSIS
  Verbindet den lokalen Projektordner mit deinem (leeren, privaten) GitHub-Repo und macht den ersten Commit.
.EXAMPLE
  .\Scripts\Tools\Init-GitRepo.ps1 -RemoteUrl https://github.com/koelleprime-afk/ewigfels.git
#>
param([Parameter(Mandatory)][string]$RemoteUrl)
$ErrorActionPreference = 'Stop'
Set-Location (Resolve-Path "$PSScriptRoot\..\..")

if (-not (Test-Path .git)) { git init -b main }
git add .
Write-Host '==> Diese Dateien kommen ins Repo (Kontrolle: keine .uasset/.umap!):' -ForegroundColor Cyan
git status --short
$big = git diff --cached --name-only | Where-Object { $_ -match '\.(uasset|umap|fbx|png|wav|blend)$' }
if ($big) { throw "Binärdateien im Commit gefunden: $($big -join ', '). .gitignore prüfen." }

git commit -m "Phase 0: Projektgeruest, UE-5.8-C++-Testprojekt, Setup-Doku"
if (-not (git remote | Select-String -Quiet origin)) { git remote add origin $RemoteUrl }
git push -u origin main
