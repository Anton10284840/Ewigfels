# Ewigfels – Kurzinfo für Claude

Open-World-Zauberer-RPG, **UE 5.8, C++**. Eigenständige Welt, keine Begriffe aus fremden Marken. Budget 0 €: nur Gratis-/CC0-Assets und kommerziell nutzbare Lizenzen.
Antworten auf **Deutsch**, direkt, ohne Floskeln. Vor großen Designentscheidungen 2–3 Optionen mit Vor-/Nachteilen.

## Wo was liegt
- `GAME_PROMPT.md` – Gesamtspezifikation (23 KB). **Nicht komplett lesen**, gezielt Abschnitt suchen (`grep -n "^#" GAME_PROMPT.md`).
- `PROGRESS.md` – Status Phasen/Systeme/Quests. Nach jeder Phase/jedem Feature aktualisieren.
- `CREDITS.md` – jede externe Ressource (Asset, Plugin, KI-Modell). Ohne Eintrag kommt nichts rein.
- `docs/` – `SETUP_WINDOWS.md` (Installation), `KI_LIZENZEN.md` (Tool-Lizenzen), `PLUGINS.md` (Plugin-Bewertung), `QUEST_FORMAT.md` (Quest-JSON, SUDS-Dialoge, Weltstatus-Syntax, Speichern).
- Kernsysteme (Phase 2, `Source/Ewigfels/{WorldState,Quest,Dialogue,Save}`): Weltstatus-Subsystem (Flags/Beziehungen/Ruf), Quest-Subsystem (lädt `Content/Data/Quests/**/*.json`), SUDS-Brücke, Save-Subsystem. Neue Quest = JSON + `.sud`, danach `python Scripts/Tools/validate_data.py` (läuft auch in der Cloud). Erlaubte Flags/NPCs/Regionen: `Content/Data/World/WorldState.json`.
- Kampf (Phase 3, `Source/Ewigfels/{Magic,Combat,Character,Camera,AI,UI,World}`): eigenes Zaubersystem (kein GAS), Kampfwerte in `Content/Data/Spells/*.json` + Gegner in `Content/Data/Creatures/`, Format `docs/SPELL_FORMAT.md`, prüfen mit `python Scripts/Tools/validate_combat.py`. Leere Karte startet automatisch die Testarena.
- Weisen (E10, 430 Zauber): `docs/WEISEN.md` (System), `docs/WEISEN_KATALOG.md` (automatisch erzeugt, nicht von Hand ändern), Daten `Content/Data/Spells/Katalog/*.json`, prüfen + Katalog neu schreiben: `python Scripts/Tools/validate_spells.py --write-doc`. Akt-1-Schulleben: `docs/AKT1_SCHULLEBEN.md`.
- Bibeln (Phase 1, groß, gezielt per `grep -n "^#"` lesen): `GDD.md` (Systeme, Weisen, Gegner), `STORY_BIBLE.md` (Lore, Missionen, Flags, Enden), `CHARACTER_BIBLE.md` (Figuren, Türme, Kreaturen), `ART_BIBLE.md` (Look, Budgets), `ANIMATION_BIBLE.md` (Checklisten).
- Kernbegriffe: Spiel- und Akademiename **Nocturna** (Code, Repo, Pfade, Klassen-Präfix bleiben `Ewigfels`), Kontinent **Velmark**, Magie = Klang aus den Fingern, kein Stab (**Weisen**, **Stimmringe**, **Atem**), Schwarzmagier = **Missklinger**, verbotene Magie **Urton**, Schattenreich **Nachhall**, Antagonisten **Stille Hand**/**Dirigent**, Häuser = 4 **Türme**.
- `Source/Ewigfels/` – C++ (`Public/` Header, `Private/` Implementierung, `Private/Tests/` Automation-Tests).
- `Config/` – `DefaultEngine.ini` (Rendering: Lumen HW-RT, Nanite, VSM, TSR).
- `Content/Data/**` – JSON/CSV-Daten (Quests, Dialoge, Items, Zauber). Einziger versionierter Teil von `Content/`.
- `Scripts/Tools/` – PowerShell: Build, Test, Install, Backup. `Scripts/Python/` – Editor-Skripte. `Scripts/ComfyUI/` – Workflows, Prompt-Listen, `run_comfy.py` (Batch).
- `Plugins/ThirdParty/` – Drittanbieter-Plugins, **nicht im Repo**, Liste in `Scripts/Tools/Plugins.json`.

## Konventionen
- C++: Systeme über Komponenten und Interfaces, keine Monster-Klassen. Klassen-Präfix `Ewigfels` (z. B. `AEwigfelsGameMode`, `UEwigfelsQuestComponent`). Kommentare auf Deutsch, knapp.
- Modulplan: Core, Magic, Combat, Quest, Dialogue, AI, UI, Save, World (je eigenes Modul, sobald gebraucht).
- Gameplay-Inhalte datengetrieben (JSON in `Content/Data`), Texte über String Tables (DE + EN).
- Binär-Assets (`*.uasset`, `*.umap`, `art/`) nie in Git. Backup über OneDrive.
- **Mehrspieler-Regel (GDD E11):** Koop 2–4 Spieler kommt später, darum alles server-autoritativ bauen: Spielwerte nur mit `HasAuthority()` ändern, Clients schicken Server-RPCs, Zustand per `DOREPLIFETIME`/OnRep, Optik per Multicast, Bewegung über die CMC, nie „Spieler 0“ annehmen.
- Vor C++-Arbeit `docs/UNREAL_REGELN.md` beachten (Stolperfallen: Zeiger/GC, Komponenten, Netzwerk, Enhanced Input, Niagara, KI, Bauen/Testen).
- Commits: ein funktionierendes Feature pro Commit, Nachricht auf Deutsch.

## Bauen und testen (Windows, beim Nutzer)
```powershell
.\Scripts\Tools\Build-Ewigfels.ps1 -EngineDir "D:\Epic\UE_5.8"
.\Scripts\Tools\Test-Ewigfels.ps1  -EngineDir "D:\Epic\UE_5.8"
```
In der Cloud gibt es keine Engine: C++ dort nur schreiben, nicht bauen. Bei Build-Fehlern nur die `error`-Zeilen auswerten, nicht das ganze Log.

## Editor steuern (Unreal MCP, eingebaut ab UE 5.8, experimentell)
Editor offen → Server läuft auf `http://127.0.0.1:8000/mcp` (`.mcp.json` im Repo). Claude Code im Projektordner starten.
Tools zuerst mit `list_toolsets`/`describe_tools` suchen statt raten. Nur lokal, nie nach außen öffnen.
