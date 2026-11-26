<#
.SYNOPSIS
  Prüft alle Quest- und Dialogdaten und zeigt die geschätzte Spielzeit.
.EXAMPLE
  .\Scripts\Tools\Validate-Data.ps1           # Bericht
  .\Scripts\Tools\Validate-Data.ps1 -Strict   # Warnungen zählen als Fehler
#>
param([switch]$Strict)
$root = Resolve-Path "$PSScriptRoot\..\.."
$py = Get-Command python -ErrorAction SilentlyContinue
if (-not $py) { $py = Get-Command py -ErrorAction SilentlyContinue }
if (-not $py) { Write-Host 'Python fehlt. Installieren: winget install Python.Python.3.12' -ForegroundColor Red; exit 2 }
$pyArgs = @("$root\Scripts\Tools\validate_data.py")
if ($Strict) { $pyArgs += '--strict' }
& $py.Source @pyArgs
if ($LASTEXITCODE -eq 0) { Write-Host 'Daten in Ordnung.' -ForegroundColor Green }
else { Write-Host 'Daten haben Fehler (siehe oben).' -ForegroundColor Red; exit $LASTEXITCODE }
