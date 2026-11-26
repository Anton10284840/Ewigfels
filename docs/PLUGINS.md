# Plugins – Bewertung (Stand 26.11.2026)

Kriterien: kommerziell nutzbare Lizenz, läuft mit **UE 5.8**, wird gepflegt, brauchen wir wirklich (nicht nur „nice to have“).
Installation: `.\Scripts\Tools\Install-Plugins.ps1` (Liste mit gepinnten Versionen: `Scripts/Tools/Plugins.json`). Plugins landen in `Plugins/ThirdParty/` und sind nicht im Repo.

## 1. Eingebaut in UE 5.8 (nur aktivieren, nichts herunterladen)

| Plugin | Status | Wofür | Bewertung |
|---|---|---|---|
| **Unreal MCP** (`ModelContextProtocol`) + **All Toolsets** (`AllToolsets`) | in `Ewigfels.uproject` aktiviert (nur Editor) | Claude steuert den Editor direkt: Actors, Blueprints, Materialien, PCG, Niagara, Sequencer, Automation-Tests | **Größter Token-Sparer.** Statt Klick-Anleitungen zu schreiben und Screenshots zu deuten, ruft Claude Editor-Funktionen auf. Von Epic, experimentell: APIs können sich ändern. Nur lokal (127.0.0.1:8000), keine Authentifizierung. |
| PythonScriptPlugin, EditorScriptingUtilities | schon aktiv | Massen-Import, Weltaufbau, Validierung | Bleibt, ergänzt MCP für große Batch-Jobs. |
| PCG, EnhancedInput | schon aktiv | Weltaufbau, Steuerung | – |
| Json/JsonUtilities (Module) | in der Engine | JSON für Quests/Dialoge | Kein Plugin wie VaRest nötig. |
| StateTree, GameplayAbilities, CommonUI, PoseSearch (Motion Matching) | noch aus | KI, Zauber, UI, Locomotion | Einschalten in der jeweiligen Phase (2/3). GAS vs. eigenes Zaubersystem ist eine Designentscheidung → Phase 3 mit Optionen. |

## 2. Von GitHub – empfohlen

| Plugin | Lizenz | UE 5.8 | Gepflegt | Wofür | Bewertung |
|---|---|---|---|---|---|
| [Not Yet: DlgSystem](https://github.com/NotYetGames/DlgSystem) | MIT | ja (5.6–5.8 laut README) | letzter Commit 22.06.2026 | Dialogsystem: Verzweigungen, Bedingungen, Events, Graph-Editor | Passt gut zu „Dialoge datengetrieben“: **JSON-Import/-Export** eingebaut. Kamera-/Emotions-Integration bauen wir selbst. |
| [Flow Graph](https://github.com/MothCocoon/FlowGraph) | MIT | ja (Tag `v2.3-5.8`, 11.07.2026) | aktiv | Quest- und Story-Ablauf als Graph | Bewährt in großen Produktionen. Nachteil: Flows sind `.uasset`, nicht JSON. Deshalb ist **Flow vs. eigenes JSON-Questsystem** die Entscheidung zu Beginn von Phase 2. Standardmäßig deaktiviert, bis wir uns entschieden haben. |

## 3. Von GitHub – optional, erst testen

| Plugin | Lizenz | UE 5.8 | Bewertung |
|---|---|---|---|
| [SPUD](https://github.com/sinbad/SPUD) | MIT | offiziell nur bis 5.5 | Gepflegt (Commit 03.11.2026). Aber **World Partition nur in einem Neben-Branch** – wir nutzen World Partition. Darum Kandidat für Phase 2, kein fester Bestandteil. Plan B: eigenes Save-System auf `USaveGame`. |
| [UnrealImGui](https://github.com/IDI-Systems/UnrealImGui) | MIT | offiziell nur bis 5.5 | Debug-Overlays. Praktisch, aber nicht nötig; UE hat Gameplay Debugger und Rewind Debugger eingebaut. |

## 4. Geprüft und verworfen

| Kandidat | Grund |
|---|---|
| Fremde MCP-Brücken (chongdashu/unreal-mcp, flopperam, VibeUE u. a.) | Überflüssig, seit UE 5.8 einen eigenen MCP-Server mitbringt. Zusätzlicher C++-Code, der bei jedem Engine-Update brechen kann. |
| Mountea Dialogue System | MPL-2.0 (Datei-Copyleft) und `.uplugin` auf 5.7. DlgSystem ist für uns einfacher. |
| VaRest | JSON können die Engine-Module selbst. |
| ALS Community | Motion Matching (Game Animation Sample) ist moderner und von Epic. |
| Logic Driver, GAS Companion, Easy Multi Save u. Ä. | Kostenpflichtig. |

## 5. Claude-Seite (Token sparen)

| Was | Wo | Effekt |
|---|---|---|
| `CLAUDE.md` | Repo-Wurzel | Architektur, Pfade, Konventionen in ~50 Zeilen. Claude muss nicht jedes Mal `GAME_PROMPT.md` (23 KB) lesen. |
| Lese-Sperren | `.claude/settings.json` | Binaries, Intermediate, Saved, DDC, `.uasset`/`.umap`, Bilder, Modelle werden nie gelesen. (`.claudeignore` gibt es in Claude Code nicht.) |
| Skills | `.claude/skills/` | `ue-build-test` (Log gefiltert lesen), `credits-eintrag` (Lizenzcheck), `phase-abschluss` (PROGRESS + Zusammenfassung). Wiederkehrende Abläufe ohne neue Erklärung. |
| `.mcp.json` | Repo-Wurzel | Claude Code verbindet sich automatisch mit dem Unreal-MCP-Server. |
| Claude-Plugin „Unreal Engine MCP“ von Epic | claude.com/plugins | Optional. Baut auf denselben eingebauten Plugins auf; unser `.mcp.json` reicht. |
