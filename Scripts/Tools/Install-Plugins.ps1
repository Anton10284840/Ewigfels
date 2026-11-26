<#
.SYNOPSIS
  Klont die Drittanbieter-Plugins aus Plugins.json nach Plugins\ThirdParty\ (fest gepinnte Version).
.DESCRIPTION
  Plugins liegen NICHT im Repo (.gitignore), nur diese Liste. Danach neu bauen (Build-Ewigfels.ps1),
  weil die Plugins als C++-Quellcode kommen und mit dem Projekt kompiliert werden.
.EXAMPLE
  .\Scripts\Tools\Install-Plugins.ps1                 # Set "empfohlen" (DlgSystem, Flow)
  .\Scripts\Tools\Install-Plugins.ps1 -All            # auch optionale (SPUD, ImGui)
  .\Scripts\Tools\Install-Plugins.ps1 -Name SPUD      # nur bestimmte
  .\Scripts\Tools\Install-Plugins.ps1 -Update         # vorhandene auf den Stand der Liste bringen
#>
param(
    [string[]]$Name,
    [switch]$All,
    [switch]$Update
)
$ErrorActionPreference = 'Stop'
$root     = Resolve-Path "$PSScriptRoot\..\.."
$manifest = Get-Content (Join-Path $PSScriptRoot 'Plugins.json') -Raw -Encoding UTF8 | ConvertFrom-Json
$target   = Join-Path $root 'Plugins\ThirdParty'
New-Item -ItemType Directory -Force -Path $target | Out-Null

$list = $manifest.plugins
if ($Name)        { $list = $list | Where-Object { $Name -contains $_.name } }
elseif (-not $All) { $list = $list | Where-Object { $_.set -eq 'empfohlen' } }
if (-not $list) { throw "Kein Plugin ausgewählt. Namen: $($manifest.plugins.name -join ', ')" }

foreach ($p in $list) {
    $dir = Join-Path $target $p.name
    Write-Host "==> $($p.name) @ $($p.ref)  [$($p.license)]" -ForegroundColor Cyan
    if (Test-Path (Join-Path $dir '.git')) {
        if (-not $Update) { Write-Host '    schon vorhanden (mit -Update aktualisieren)'; continue }
    } else {
        git clone --filter=blob:none --no-checkout $p.repo $dir
        if ($LASTEXITCODE -ne 0) { throw "Klonen fehlgeschlagen: $($p.repo)" }
    }
    git -C $dir fetch --tags origin
    git -C $dir -c advice.detachedHead=false checkout --force $p.ref
    if ($LASTEXITCODE -ne 0) { throw "Checkout fehlgeschlagen: $($p.name) $($p.ref)" }
}

Write-Host "`nFertig. Jetzt neu bauen: .\Scripts\Tools\Build-Ewigfels.ps1 -EngineDir <Pfad>" -ForegroundColor Green
Write-Host 'Hinweis: Flow ist standardmäßig deaktiviert. Wir schalten es in Ewigfels.uproject ein, sobald die Phase-2-Entscheidung steht.' -ForegroundColor Yellow
