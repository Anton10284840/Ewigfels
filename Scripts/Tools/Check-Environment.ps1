<#
.SYNOPSIS
  Prüft, ob alles für Ewigfels installiert ist. Gibt eine Ampel-Liste aus.
  Ausführen: .\Scripts\Tools\Check-Environment.ps1 [-EngineDir "C:\Program Files\Epic Games\UE_5.8"]
#>
param([string]$EngineDir = "C:\Program Files\Epic Games\UE_5.8")

function Show([string]$Name, [bool]$Ok, [string]$Info = '') {
    $tag = if ($Ok) { '[OK]  ' } else { '[FEHLT]' }
    $col = if ($Ok) { 'Green' } else { 'Red' }
    Write-Host ("{0} {1} {2}" -f $tag, $Name, $Info) -ForegroundColor $col
}

Show 'Git'       ([bool](Get-Command git -ErrorAction SilentlyContinue)) ((git --version) 2>$null)
Show 'Git LFS'   ([bool]((git lfs version) 2>$null)) ((git lfs version) 2>$null)
Show 'Python'    ([bool](Get-Command py -ErrorAction SilentlyContinue)) ((py -3.12 --version) 2>$null)

$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$vs = if (Test-Path $vswhere) { & $vswhere -latest -requires Microsoft.VisualStudio.Workload.NativeGame -property installationVersion } else { $null }
Show 'Visual Studio + Game-Workload' ([bool]$vs) $vs

Show "Unreal Engine ($EngineDir)" (Test-Path "$EngineDir\Engine\Binaries\Win64\UnrealEditor.exe")

$blender = Get-ChildItem "$env:ProgramFiles\Blender Foundation" -Filter blender.exe -Recurse -ErrorAction SilentlyContinue | Select-Object -First 1
Show 'Blender' ([bool]$blender) ($blender.FullName)

$smi = Get-Command nvidia-smi -ErrorAction SilentlyContinue
if ($smi) {
    $gpu = nvidia-smi --query-gpu=name,memory.total,driver_version --format=csv,noheader
    Show 'NVIDIA-GPU/Treiber' $true $gpu
} else { Show 'NVIDIA-Treiber (nvidia-smi)' $false }

Get-PSDrive -PSProvider FileSystem | Where-Object { $_.Used -ne $null } | ForEach-Object {
    $free = [math]::Round($_.Free / 1GB)
    Show ("Laufwerk {0}: frei" -f $_.Name) ($free -ge 250) ("$free GB (empfohlen: >= 250 GB auf NVMe)")
}

Show 'OneDrive-Ordner' ([bool]$env:OneDrive) $env:OneDrive
