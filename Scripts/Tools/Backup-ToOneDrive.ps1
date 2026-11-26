<#
.SYNOPSIS
  Sichert alles, was NICHT auf GitHub liegt, nach OneDrive.
.DESCRIPTION
  - Content\ (uasset/umap), art\, raw\  -> Spiegel in OneDrive\Ewigfels-Backup\current
  - Zusätzlich ein datiertes 7z-Archiv (Standard: nur sonntags oder mit -Snapshot)
  - Git-Bundle des kompletten Repos (falls GitHub mal weg ist)
  Das Projekt selbst liegt NICHT im OneDrive-Ordner (Dateisperren/Sync-Last durch UE).
  Editor vorher schließen!
.EXAMPLE
  .\Scripts\Tools\Backup-ToOneDrive.ps1 -Snapshot
#>
param(
    [string]$Target = (Join-Path $env:OneDrive 'Ewigfels-Backup'),
    [switch]$Snapshot,
    [int]$KeepSnapshots = 6
)
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path "$PSScriptRoot\..\..").Path
if (-not $env:OneDrive) { throw 'OneDrive ist nicht eingerichtet ($env:OneDrive leer).' }
if (Get-Process UnrealEditor -ErrorAction SilentlyContinue) { throw 'Bitte zuerst den Unreal Editor schließen.' }

$current = Join-Path $Target 'current'
New-Item -ItemType Directory -Force -Path $current | Out-Null

foreach ($dir in 'Content', 'art', 'raw') {
    $src = Join-Path $root $dir
    if (Test-Path $src) {
        Write-Host "==> Spiegle $dir" -ForegroundColor Cyan
        # /MIR spiegelt (löscht im Ziel, was lokal gelöscht wurde) – darum zusätzlich Snapshots
        robocopy $src (Join-Path $current $dir) /MIR /R:2 /W:2 /NFL /NDL /NP | Out-Null
        if ($LASTEXITCODE -ge 8) { throw "robocopy-Fehler bei $dir ($LASTEXITCODE)" }
    }
}

Write-Host '==> Git-Bundle' -ForegroundColor Cyan
git -C $root bundle create (Join-Path $Target 'ewigfels.bundle') --all

if ($Snapshot -or (Get-Date).DayOfWeek -eq 'Sunday') {
    $seven = "$env:ProgramFiles\7-Zip\7z.exe"
    $stamp = Get-Date -Format 'yyyy-MM-dd'
    $archive = Join-Path $Target "snapshots\Ewigfels-$stamp.7z"
    New-Item -ItemType Directory -Force -Path (Split-Path $archive) | Out-Null
    Write-Host "==> Snapshot $archive" -ForegroundColor Cyan
    & $seven a -t7z -mx=3 $archive (Join-Path $current '*') | Out-Null
    Get-ChildItem (Split-Path $archive) -Filter 'Ewigfels-*.7z' | Sort-Object Name -Descending |
        Select-Object -Skip $KeepSnapshots | Remove-Item -Force
}
Write-Host "Backup fertig: $Target" -ForegroundColor Green
