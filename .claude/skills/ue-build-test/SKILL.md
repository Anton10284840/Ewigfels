---
name: ue-build-test
description: Ewigfels bauen und Automation-Tests ausführen bzw. einen Build-Fehler aus dem Log auswerten. Nutzen, wenn der Nutzer einen Build/Test will oder ein Fehlerlog schickt.
---

# Bauen und testen

1. Nur auf dem Windows-Rechner des Nutzers möglich (Engine unter `D:\Epic\UE_5.8`). In der Cloud nicht versuchen.
2. Befehle:
   ```powershell
   .\Scripts\Tools\Build-Ewigfels.ps1 -EngineDir "D:\Epic\UE_5.8" -SkipProjectFiles
   .\Scripts\Tools\Test-Ewigfels.ps1  -EngineDir "D:\Epic\UE_5.8"
   ```
   `-SkipProjectFiles` weglassen, wenn Dateien/Module/Plugins neu dazugekommen sind.
3. Log sparsam lesen. Nicht das ganze Log öffnen, sondern filtern:
   ```powershell
   Select-String -Path "$env:LOCALAPPDATA\UnrealBuildTool\Log.txt" -Pattern "error|fatal" | Select-Object -First 30
   Select-String -Path ".\Saved\Logs\Ewigfels.log" -Pattern "Error|Test Completed|Fail" | Select-Object -First 40
   ```
4. Erste echte Fehlerursache beheben (Folgefehler ignorieren), dann nur neu bauen.
5. Ergebnis in einem Satz melden; bei Erfolg passenden Eintrag in `PROGRESS.md`.
