# Nocturna (Projekt- und Code-Name: Ewigfels)

Open-World-Zauberer-RPG in Unreal Engine 5.8 (C++). Budget 0 €, nur freie Tools und kommerziell nutzbare lokale KI.

- Gesamtspezifikation: [`GAME_PROMPT.md`](GAME_PROMPT.md)
- Einrichtung (Phase 0): [`docs/SETUP_WINDOWS.md`](docs/SETUP_WINDOWS.md)
- Lizenzen der Tools: [`docs/KI_LIZENZEN.md`](docs/KI_LIZENZEN.md)
- Fortschritt: [`PROGRESS.md`](PROGRESS.md) · Quellen: [`CREDITS.md`](CREDITS.md)

## Schnellstart (Windows)
```powershell
.\Scripts\Tools\Check-Environment.ps1 -EngineDir "D:\Epic\UE_5.8"
.\Scripts\Tools\Build-Ewigfels.ps1    -EngineDir "D:\Epic\UE_5.8"
.\Scripts\Tools\Test-Ewigfels.ps1     -EngineDir "D:\Epic\UE_5.8"
```

## Was liegt wo
- **Git/GitHub:** Code, Config, Skripte, JSON-Daten, Docs
- **Lokal + OneDrive-Backup:** `Content/**/*.uasset|umap`, `art/`, `raw/` (siehe `Scripts/Tools/Backup-ToOneDrive.ps1`)
