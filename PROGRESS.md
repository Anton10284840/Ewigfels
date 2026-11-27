# PROGRESS – Ewigfels

Stand: 27.11.2026 · Aktuelle Phase: **3 – Charakter, Zauber, Kampf** (Code fertig, Build auf dem PC offen) · Engine: UE 5.8

Legende: ⬜ offen · 🟨 in Arbeit · ✅ fertig · ❌ blockiert

## Phasen

| Phase | Inhalt | Status |
|---|---|---|
| 0 | Setup, Tools, Testprojekt | 🟨 Dateien fertig, Installation + erster Build beim Nutzer offen |
| 1 | Bibeln + Moodboards | 🟨 Bibeln + ComfyUI-Pipeline fertig, Bilder generieren + Grundsatzentscheidungen E1–E4 bestätigen beim Nutzer |
| 2 | Kernsysteme (Quest, Dialog, Entscheidung, Save) | ✅ Build + 9/9 Automation-Tests grün auf dem PC (26.11.2026). Offen: Test im Level (Phase 3/5). Entscheidung: SUDS + eigenes JSON (docs/PLUGINS.md §3) |
| 3 | Charakter & Animation | 🟨 C++ komplett (Figur, 5 Weisen, Kampf, 2 Gegnertypen, Dialogkamera, HUD, Testarena), Koop-fähig gebaut (GDD E11). Offen: Build + Tests auf dem PC, Game Animation Sample einbinden, Niagara-Effekte (docs/PHASE3_EDITOR.md) |
| 4 | Grafik-Benchmark-Szene | ⬜ |
| 5 | Vertical Slice Akt 1 | ⬜ |
| 6–10 | Akte 2–5 | ⬜ |
| 11 | Finale | ⬜ |

## Phase 0 im Detail

| Aufgabe | Status | Notiz |
|---|---|---|
| Hardware erfasst | ✅ | Ryzen 7 9700X, RTX 5070 12 GB, 32 GB DDR5, Win 11 |
| Freier Speicher | ⬜ | Nachfrage offen |
| Webcam / Android-Modell | ⬜ | Nachfrage offen (für FreeMoCap, MetaHuman Animator) |
| Lizenzprüfung KI-Tools | ✅ | Hunyuan3D raus (EU-Ausschluss), TRELLIS raus (VRAM + nvdiffrast), Cascadeur Free nicht kommerziell → TripoSG, Control Rig |
| Installationsanleitung | ✅ | docs/SETUP_WINDOWS.md |
| .gitignore / .gitattributes | ✅ | Content-Binärdateien nicht in Git, Backup per OneDrive |
| C++-Testprojekt | ✅ | erster Build beim Nutzer erfolgreich (26.11.2026) |
| GitHub-Repo | ⬜ | Nutzer legt `koelleprime-afk/ewigfels` (privat) an |
| OneDrive-Backup-Skript | ✅ | Scripts/Tools/Backup-ToOneDrive.ps1 |
| Plugins ausgewählt | ✅ | docs/PLUGINS.md (Runde 2: PCGEx, SUDS, Engine-Plugins für KI/UI/Wasser/Motion Matching); Installation per Install-Plugins.ps1 beim Nutzer offen |
| Unreal MCP + CLAUDE.md | 🟨 | in .uproject/.mcp.json eingerichtet, Verbindung beim Nutzer noch nicht getestet |

## Phase 1 im Detail

| Aufgabe | Status | Notiz |
|---|---|---|
| docs/GDD.md | ✅ | E1 entschieden: Akademie + Spieltitel **Nocturna**; E2–E5 mit Optionen (Empfehlung umgesetzt), 430 Weisen (E10), 25 Gegnertypen, 15 Bosse, Systeme, Steuerung |
| docs/STORY_BIBLE.md | ✅ | Welt Velmark, Zeitlinie, Magie „Resonanz“, Fraktionen, 50 Missionen (Liste), 6 Entscheidungen, 4 Enden, 6 Questreihen |
| docs/CHARACTER_BIBLE.md | ✅ | Pipeline, MetaHuman-Anleitung, 4 Türme, 15 Figurenblätter, NPC-Plan, 20 Kreaturen |
| docs/ART_BIBLE.md | ✅ | Paletten pro Region, Rendering, Post Process, Master-Materialien, Licht-Rigs, VFX, Presets, Budgets |
| Content/Data/Art/RegionLooks.json | ✅ | Look-Werte pro Region für Phase 4 |
| docs/ANIMATION_BIBLE.md | ✅ | AnimBP-Struktur, Checklisten aller Sets, FreeMoCap-Anleitung, Lip-Sync, Emotions-Tags, Reittier, Cutscene-Vorlage |
| ComfyUI-Workflows + Batch-Skript | ✅ | `run_comfy.py` gegen Test-Server geprüft, in echtem ComfyUI **noch nicht** gelaufen |
| Prompt-Listen | ✅ | 110 Moodboard-, 128 Figuren-, 48 Kreaturen-, 12 Turm-Bilder |
| Nutzer-Wünsche 26.11. | ✅ | Ton verspielt + gruselige Gegner, Magie aus den Fingern (kein Stab), Personalisierung über Läden (GDD §13), 6 eigene Reittiere, fröhliche/gruselige Dörfer + Missklinger. Weltgröße: Option A, ca. 64 km² mit dichten Kernen (GDD E9) |
| E10 Hunderte Weisen (Option C) + Akt-1-Schulvibe | ✅ | 430 Weisen (60 Kern, 358 Alltag, 12 Koop) in `Content/Data/Spells/Katalog/`, docs/WEISEN.md, docs/WEISEN_KATALOG.md, docs/AKT1_SCHULLEBEN.md (Tagesablauf, lebendige Akademie, 15 Geheimgänge, Feste, Glockenball, 18 Schulleben-Quests S101–S118), Akt-1-Missionen in STORY_BIBLE §8.1 überarbeitet, Prüfskript validate_spells.py. Offen Phase 5: M101/M110-JSON an die neue Beschreibung anpassen, S101–S118 als JSON |
| Bilder generiert | ⬜ | beim Nutzer (ca. 40–60 min), danach Key-Frames wählen |
| Missionsdesign-Dokumente | ⬜ | pro Akt in der Umsetzungsphase (Akt 1: Phase 5) |

## Systeme

| System | Geschrieben | Umgesetzt | Getestet |
|---|---|---|---|
| Weltstatus: Flags, Beziehungen, Ruf (`WorldState/`) | ✅ | ✅ | ✅ Automation-Tests |
| Quests aus JSON (`Quest/`) | ✅ | ✅ | ✅ Automation-Tests |
| Dialoge: SUDS-Brücke (`Dialogue/`) | ✅ | ✅ | ✅ Automation-Test · ⬜ Dialog im Editor abspielen |
| Speichern/Laden, Autosave, Saveable-Komponente (`Save/`) | ✅ | ✅ | ✅ Automation-Test · ⬜ PIE mit Level |
| Prüfskript Quests/Dialoge + Spielzeit (`Scripts/Tools/validate_data.py`) | ✅ | ✅ | ✅ 9 Selbsttests (Cloud) |
| Weisen-Katalog + Prüfskript (`Scripts/Tools/validate_spells.py`) | ✅ | 🟨 Kernweisen K01/K02/K13/K25/K26 spielbar (eigenes System, E5); Alltagsweisen + `IEwigfelsWeiseTarget` Phase 5 | ✅ 0 Fehler (Cloud) |
| Zaubersystem, eigenes (E5): Caster, Projektil, Strahl/Blitz, Effekte, Status (`Magic/`) | ✅ | 🟨 Bau grün bis Blitz-Umbau · ⬜ Bau Wirkungs-Baukasten | ⬜ Automation-Tests geschrieben |
| Individuelle Weisen: Heben/Schmettern, Kettenblitz, Klangband, Aufprall (`on_land`), Status-Looks, Akkord-Farben, Gesten, Zauberworte (9 Weisen im Spiel) | ✅ | ⬜ Build | ⬜ Test `Ewigfels.Magic.IndividualSpells` geschrieben |
| Kampf: Leben, Schaden, Status, Taumeln, Kampf-Regie (`Combat/`) | ✅ | ⬜ Build | ⬜ Automation-Test geschrieben |
| Spielfigur: Laufen, Sprint, Ausweichen, Zielaufschaltung, Platzhalter-Körper, AnimInstance (`Character/`) | ✅ | ⬜ Build · ⬜ Game Animation Sample | ⬜ |
| Gegner-KI Novize (Nahkampf) + Fluchsänger (Fernkampf) + Übungspuppe (`AI/`, `Content/Data/Creatures/`) | ✅ | ⬜ Build | ⬜ |
| Dialogkamera (Totale/Schulter/Close-up, 180°-Regel) (`Camera/`) | ✅ | ⬜ Build | ⬜ Automation-Test geschrieben |
| HUD Phase 3 (Leisten, Zauberleiste, Trefferzahlen, F1-Hilfe) + Testarena + Test-NPC Lio (`UI/`, `World/`) | ✅ | ⬜ Build | ⬜ |
| Charakter-Editor + Laden-Spiegel: Aussehen-Katalog, Regeln, Vorschau-Kamera, Slate-Fenster, Spielstand v2 (`Appearance/`, `UI/EwigfelsCharacterCreator`, `World/EwigfelsMirror`, docs/CHARACTER_CREATOR.md). Umbau 27.11.: 12 fertige Köpfe statt Regler, Kleiderschrank-Kacheln, bunt/magisch, Controller + Maus, geführte Erstellung | ✅ | ✅ Build v1 (PR #11) · ⬜ Build Umbau · ⬜ Assets (12 Köpfe, Vorschaubilder, Kleidung) Phase 5 | ✅ 19/19 Tests v1 · ⬜ Tests Umbau · ✅ validate_appearance.py 0 Fehler (Cloud) |
| Mehrspieler-Vorbereitung (server-autoritativ, repliziert, GDD E11) | ✅ | ⬜ | ⬜ PIE mit 2 Spielern |
| Prüfskript Weisen/Status/Gegner (`Scripts/Tools/validate_combat.py`, Format docs/SPELL_FORMAT.md) | ✅ | ✅ | ✅ 7 Selbsttests (Cloud) |
| Questlog/HUD/Lademenü-UI | ⬜ Phase 5 | | |
| Format-Doku | ✅ docs/QUEST_FORMAT.md | | |

## Quests (geschätzte Spielzeit gesamt: 0 / ~100 Std. umgesetzt · geplant: 50 Hauptmissionen ≈ 45–50 Std., Liste in STORY_BIBLE §8.1)

| ID | Name | Typ | Dauer | Geschrieben | Umgesetzt | Getestet |
|---|---|---|---|---|---|---|
| M101 | Das Boot über den Ilsensee | Main | 45 min | ✅ JSON + Dialog D_M101_Lio (Beispiel) | ⬜ Level fehlt | ⬜ |
| M110 | Die Doppelnacht ⚖ E1 | Main | 55 min | ✅ JSON + Dialog D_M110_Edda (Beispiel) | ⬜ Level fehlt | ⬜ |

## Charaktere

| Name | Rolle | Blatt | Concept | MetaHuman | Animiert |
|---|---|---|---|---|---|
| Edda Vornholt | Mentorin | ✅ | ⬜ | ⬜ | ⬜ |
| Lio Vandermeer | Begleiter | ✅ | ⬜ | ⬜ | ⬜ |
| Aurelia von Stauffen | Rivalin | ✅ | ⬜ | ⬜ | ⬜ |
| Corvin Hale | Lehrer, Verräter | ✅ | ⬜ | ⬜ | ⬜ |
| Tobiah Grell | Lehrer Duell | ✅ | ⬜ | ⬜ | ⬜ |
| Saphira Lindqvist | Lehrerin Kreaturen | ✅ | ⬜ | ⬜ | ⬜ |
| Ysolde Varga | Rektorin | ✅ | ⬜ | ⬜ | ⬜ |
| Theodric Vaal | Antagonist | ✅ | ⬜ | ⬜ | ⬜ |
| Nym | Nachhall | ✅ | ⬜ | ⬜ | ⬜ |
| Ambros Kettler | Fokusmacher | ✅ | ⬜ | ⬜ | ⬜ |
| Ruprecht von Stauffen | Kanzler | ✅ | ⬜ | ⬜ | ⬜ |
| Nell Tiedemann | Ermittlerin | ✅ | ⬜ | ⬜ | ⬜ |
| Fink | Widerstand | ✅ | ⬜ | ⬜ | ⬜ |
| Mara Vandermeer | Taktgeberin | ✅ | ⬜ | ⬜ | ⬜ |
| Mutter Quendel | Hüter | ✅ | ⬜ | ⬜ | ⬜ |
