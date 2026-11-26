# Plugins – Bewertung (Stand 26.11.2026)

Kriterien: kommerziell nutzbare Lizenz, läuft mit **UE 5.8**, wird gepflegt, bringt echten Mehrwert für `GAME_PROMPT.md`. Lieber weglassen als aufblähen.
Installation der GitHub-Plugins: `.\Scripts\Tools\Install-Plugins.ps1` (gepinnte Versionen in `Scripts/Tools/Plugins.json`, Ziel `Plugins/ThirdParty/`, nicht im Repo).

## 1. Eingebaut in UE 5.8 – in `Ewigfels.uproject` aktiviert

| Plugin | Wofür (GAME_PROMPT) | Bemerkung |
|---|---|---|
| **Unreal MCP** (`ModelContextProtocol`) + `AllToolsets` | Claude steuert den Editor direkt | Größter Token-Sparer. Experimentell, nur lokal (127.0.0.1:8000), keine Anmeldung. Nur Editor. |
| `DataValidation` | Asset-Prüfung (Namen, fehlende Referenzen, Budgets) | Basis für eigene Validatoren; ergänzt das JSON-Prüfskript aus Kap. 9. Nur Editor. |
| `StateTree` + `GameplayStateTree` | Gegner-, Begleiter-, NPC-KI (Kap. 7.3, 7.10) | Epics moderner Ersatz für viele Behavior-Tree-Fälle. |
| `CommonUI` | Menüs, HUD, Gamepad + Tastatur (Kap. 7.15) | Volle Eingabe-Weiterleitung braucht später `CommonGameViewportClient` in der Config (Phase 2/UI). |
| `Water` | Seen, Flüsse, Küste, Schwimmen (Kap. 6, 7.1) | Zieht `Landmass` automatisch mit. |
| `PoseSearch`, `MotionWarping`, `Chooser` | Motion Matching wie im Game Animation Sample (Kap. 5.1) | Genau die Plugins, die das Sample voraussetzt. |
| PythonScriptPlugin, EditorScriptingUtilities, PCG, EnhancedInput | Automatisierung, Weltaufbau, Steuerung | waren schon aktiv |

**Bewusst noch nicht aktiviert** (Entscheidung in der jeweiligen Phase, sonst Ballast):
- `GameplayAbilities` (GAS) – Zaubersystem: GAS vs. eigenes System → Phase 3 mit Optionen.
- `Mutable` – Kleidung/Transmog (Kap. 7.7) → Phase 3/5.
- MetaHuman-Plugins inkl. **MetaHuman Animator** – kommen mit dem MetaHuman-Import (Phase 3). Lip-Sync aus reiner Audiodatei geht damit offline im Editor (Performance-Asset, Input Type = Audio), kein iPhone und keine Webcam nötig. Ob deutsche Sprache gleich gut klappt, sagt Epic nicht – testen wir in Phase 3.
- Movie Render Queue – erst Phase 4 (Trailer-Shot).

## 2. Von GitHub – fester Bestandteil (`set: empfohlen`)

| Plugin | Lizenz | UE 5.8 | Gepflegt | Wofür |
|---|---|---|---|---|
| [PCG Extended Toolkit (PCGEx)](https://github.com/Nebukam/PCGExtendedToolkit) | MIT | ja (`.uplugin` 5.8.0) | sehr aktiv (Commit 26.11.2026) | Wege, Straßen, Flussufer, Siedlungslayouts, Graphen, bessere Verteilung. Hebt PCG für die 7 Regionen und Dörfer auf ein anderes Niveau. Nachteil: großer Quellcode, erster Build dauert länger. |

## 3. Dialog- und Questsystem: entschieden (26.11.2026)

**SUDS für Dialoge + eigenes JSON-Questsystem.** SUDS steht jetzt im Set `empfohlen`, der Ewigfels-Code hängt davon ab. DlgSystem und Flow sind aus `Plugins.json` geflogen (`Install-Plugins.ps1 -Prune` löscht sie lokal). Format und Regeln: [QUEST_FORMAT.md](QUEST_FORMAT.md).

| Plugin | Lizenz | UE 5.8 | Ergebnis |
|---|---|---|---|
| [SUDS](https://github.com/sinbad/SUDS) | MIT | ja (Commit „UE 5.8 compatibility“, 19.06.2026) | ✅ genommen: Dialoge als Textskripte (`.sud`), Entscheidungen, Bedingungen, Variablen, Events, Lokalisierung, Test-Tool im Editor. Claude schreibt und prüft Dialoge direkt als Text. |
| [Not Yet: DlgSystem](https://github.com/NotYetGames/DlgSystem) | MIT | ja (5.6–5.8) | ❌ raus: Graph-Editor, schwerer zu überblicken und von Claude schlechter direkt zu schreiben. |
| [Flow Graph](https://github.com/MothCocoon/FlowGraph) | MIT | ja (Tag `v2.3-5.8`) | ❌ raus: Flows sind `.uasset`, nicht JSON. Quests sind bei uns reine Daten (`Content/Data/Quests/*.json`). |

Hinweis zu SUDS und SPUD: Der Autor lehnt KI-generierte Pull Requests an *sein* Repo ab. Das betrifft nur Beiträge dorthin, nicht die Nutzung (MIT).
Speichern: SPUD wird nicht gebraucht, das eigene `UEwigfelsSaveSubsystem` (auf `USaveGame`) kann World Partition.

## 4. Von GitHub – optional (`set: optional`, nur mit `-All`)

| Plugin | Lizenz | UE 5.8 | Bewertung |
|---|---|---|---|
| [SPUD](https://github.com/sinbad/SPUD) | MIT | offiziell bis 5.5 | Save-System-Kandidat, aber **World Partition nur im Neben-Branch**. Plan A: eigenes Save-System auf `USaveGame`. |
| [UnrealImGui](https://github.com/IDI-Systems/UnrealImGui) | MIT | offiziell bis 5.5 | Debug-Overlays. Gameplay Debugger und Rewind Debugger sind eingebaut. |

## 5. Außerhalb von GitHub – kostenlos, manuell

| Tool | Quelle | Lizenz | Wofür | Bewertung |
|---|---|---|---|---|
| **NVIDIA DLSS 4.5** (Plugin 8.7.2 für UE 5.8, 21.07.2026) | [NVIDIA Developer](https://developer.nvidia.com/rtx/dlss/get-started) – kostenloser NVIDIA-Entwickler-Account nötig | NVIDIA RTX SDK-Lizenz (kostenlos, kommerziell erlaubt; Hinweis-/Logo-Pflichten in der `license.txt` im ZIP vor Release lesen) | Upscaling + Multi-Frame-Generation auf deiner RTX 5070 | Lohnt sich sehr für 1440p mit Hardware-Lumen. Download geht nur mit Login, daher kein Skript. Entpacken nach `Plugins/ThirdParty/DLSS/`. **Erst Phase 4**, damit wir TSR-Werte als Vergleich haben. |
| **Send to Unreal** (Blender-Add-on) | [EpicGamesExt/BlenderTools](https://github.com/EpicGamesExt/BlenderTools) | MIT | Ein-Klick-Export Blender → UE (Meshes, LODs, Kollision, Rigs) über Python Remote Execution | Spart bei jedem Prop den FBX-Umweg. Installation in Blender: *Edit → Preferences → Add-ons → Install from Disk* (ZIP aus den Releases). In UE: *Project Settings → Python → Enable Remote Execution* an. |

## 6. Geprüft und verworfen

| Kandidat | Grund |
|---|---|
| Fremde MCP-Brücken (chongdashu/unreal-mcp, flopperam, VibeUE u. a.) | Überflüssig dank eingebautem Unreal MCP. |
| Mountea Dialogue System | MPL-2.0 (Datei-Copyleft), `.uplugin` auf 5.7. |
| UEGitPlugin (Project Borealis) | Gut gepflegt (MIT), aber unsere Assets liegen bewusst nicht in Git – im Editor bringt es uns nichts. |
| RuntimeAudioImporter | Letzter Commit Feb. 2025; Piper-Sprachdateien importieren wir offline als SoundWave. |
| VaRest | JSON können die Engine-Module selbst. |
| ALS Community | Motion Matching (Game Animation Sample) ist moderner. |
| Ultra Dynamic Sky, Logic Driver, GAS Companion, Easy Multi Save, Runtime MetaHuman Lip Sync u. Ä. | Kostenpflichtig. Tag/Nacht + Wetter bauen wir mit Sky Atmosphere, Volumetric Clouds, Niagara und einem eigenen Zeit-Subsystem. |
| Inkpot (Ink) | Gleiche Rolle wie SUDS, aber zusätzliche Skriptsprache außerhalb von UE. |

## 7. Claude-Seite (Token sparen)

| Was | Wo | Effekt |
|---|---|---|
| `CLAUDE.md` | Repo-Wurzel | Architektur, Pfade, Konventionen kompakt; `GAME_PROMPT.md` nur abschnittsweise. |
| Lese-Sperren | `.claude/settings.json` | Build-Ordner, `.uasset`/`.umap`, Bilder, Modelle werden nie gelesen (`.claudeignore` gibt es nicht). |
| Skills | `.claude/skills/` | `ue-build-test`, `credits-eintrag`, `phase-abschluss`. |
| `.mcp.json` | Repo-Wurzel | Claude Code verbindet sich automatisch mit dem Unreal-MCP-Server. |
