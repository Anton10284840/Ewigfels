# PROGRESS – Ewigfels

Stand: 26.11.2026 · Aktuelle Phase: **3 – Charakter, Zauber, Kampf** (Code fertig, Build auf dem PC offen) · Engine: UE 5.8

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
| Freier Speicher | ✅ | 2 TB SSD |
| Webcam / Android-Modell | 🟨 | Pixel 10 Pro (kein Echtzeit-Tracking in Live Link Face, Nutzung als USB-Webcam für Mono-Video, siehe SETUP 8a). Webcam-Modell offen |
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
| Missionsdesign-Dokumente | ✅ | alle 50 Missionen + 5 Story-Bosse in docs/missions/ (27.11.2026); Akt 1 komplett als JSON + SUDS, Stimmen-Leitfaden docs/DIALOG_STIMMEN.md |

## Systeme

| System | Geschrieben | Umgesetzt | Getestet |
|---|---|---|---|
| Weltstatus: Flags, Beziehungen, Ruf (`WorldState/`) | ✅ | ✅ | ✅ Automation-Tests |
| Quests aus JSON (`Quest/`) | ✅ | ✅ | ✅ Automation-Tests |
| Dialoge: SUDS-Brücke (`Dialogue/`) | ✅ | ✅ | ✅ Automation-Test · ⬜ Dialog im Editor abspielen |
| Speichern/Laden, Autosave, Saveable-Komponente (`Save/`) | ✅ | ✅ | ✅ Automation-Test · ⬜ PIE mit Level |
| Prüfskript Quests/Dialoge + Spielzeit (`Scripts/Tools/validate_data.py`) | ✅ | ✅ | ✅ 9 Selbsttests (Cloud) |
| Weisen-Katalog + Prüfskript (`Scripts/Tools/validate_spells.py`) | ✅ | 🟨 Kernweisen K01/K02/K13/K25/K26 spielbar (eigenes System, E5); Alltagsweisen + `IEwigfelsWeiseTarget` Phase 5 | ✅ 0 Fehler (Cloud) |
| Zaubersystem, eigenes (E5): Caster, Projektil, Effekte, Status (`Magic/`) | ✅ | ⬜ Build | ⬜ 5 Automation-Tests geschrieben |
| Kampf: Leben, Schaden, Status, Taumeln, Kampf-Regie (`Combat/`) | ✅ | ⬜ Build | ⬜ Automation-Test geschrieben |
| Spielfigur: Laufen, Sprint, Ausweichen, Zielaufschaltung, Platzhalter-Körper, AnimInstance (`Character/`) | ✅ | ⬜ Build · ⬜ Game Animation Sample | ⬜ |
| Gegner-KI Novize (Nahkampf) + Fluchsänger (Fernkampf) + Übungspuppe (`AI/`, `Content/Data/Creatures/`) | ✅ | ⬜ Build | ⬜ |
| Dialogkamera (Totale/Schulter/Close-up, 180°-Regel) (`Camera/`) | ✅ | ⬜ Build | ⬜ Automation-Test geschrieben |
| HUD Phase 3 (Leisten, Zauberleiste, Trefferzahlen, F1-Hilfe) + Testarena + Test-NPC Lio (`UI/`, `World/`) | ✅ | ⬜ Build | ⬜ |
| Mehrspieler-Vorbereitung (server-autoritativ, repliziert, GDD E11) | ✅ | ⬜ | ⬜ PIE mit 2 Spielern |
| Prüfskript Weisen/Status/Gegner (`Scripts/Tools/validate_combat.py`, Format docs/SPELL_FORMAT.md) | ✅ | ✅ | ✅ 7 Selbsttests (Cloud) |
| Questlog/HUD/Lademenü-UI | ⬜ Phase 5 | | |
| Format-Doku | ✅ docs/QUEST_FORMAT.md | | |

## Quests (geschätzte Spielzeit geschrieben: 44,3 Std. · Ziel ~100 Std. · Missionsdesign aller 50 Hauptmissionen: docs/missions/)

| ID | Name | Typ | Dauer | Geschrieben | Umgesetzt | Getestet |
|---|---|---|---|---|---|---|
| M101 | Das Boot über den Ilsensee | Main | 45 min | ✅ JSON + 2 Dialoge | ⬜ Level fehlt | ⬜ |
| M102 | Die Turmwahl | Main | 50 min | ✅ JSON + 2 Dialoge | ⬜ Level fehlt | ⬜ |
| M103 | Der erste Ton | Main | 50 min | ✅ JSON + 3 Dialoge | ⬜ Level fehlt | ⬜ |
| M104 | Deine ersten Ringe | Main | 50 min | ✅ JSON + 3 Dialoge | ⬜ Level fehlt | ⬜ |
| M105 | Das Flüstern im Raunwald | Main | 55 min | ✅ JSON + 3 Dialoge | ⬜ Level fehlt | ⬜ |
| M106 | Ilsenlicht | Main | 55 min | ✅ JSON + 4 Dialoge | ⬜ Level fehlt | ⬜ |
| M107 | Die Stumme Kammer | Main | 50 min | ✅ JSON + 3 Dialoge | ⬜ Level fehlt | ⬜ |
| M108 | Erwachen | Main | 60 min | ✅ JSON + 2 Dialoge | ⬜ Level fehlt | ⬜ |
| M109 | Was hast du getan? | Main | 50 min | ✅ JSON + 3 Dialoge | ⬜ Level fehlt | ⬜ |
| M110 | Die Doppelnacht ⚖ E1 | Main | 55 min | ✅ JSON + 3 Dialoge | ⬜ Level fehlt | ⬜ |
| S101 | Der erste Brief | Neben | 20 min | ✅ JSON | ⬜ Level fehlt | ⬜ |
| S102 | Kessel Klaus ist beleidigt | Neben | 25 min | ✅ JSON | ⬜ Level fehlt | ⬜ |
| S103 | Ottilie | Neben | 25 min | ✅ JSON | ⬜ Level fehlt | ⬜ |
| S104 | Die verlorenen Bücher | Neben | 25 min | ✅ JSON | ⬜ Level fehlt | ⬜ |
| S105 | Speisefluss in Not | Neben | 20 min | ✅ JSON | ⬜ Level fehlt | ⬜ |
| S106 | Schiefe Töne | Neben | 35 min | ✅ JSON | ⬜ Level fehlt | ⬜ |
| S107 | Barnabas | Neben | 30 min | ✅ JSON | ⬜ Level fehlt | ⬜ |
| S108 | Herr Quaak | Neben | 30 min | ✅ JSON | ⬜ Level fehlt | ⬜ |
| S109 | Die fliegende Badewanne | Neben | 35 min | ✅ JSON | ⬜ Level fehlt | ⬜ |
| S110 | Glockenball: Turmrunde | Neben | 40 min | ✅ JSON | ⬜ Level fehlt | ⬜ |
| S111 | Nachklänge | Neben | 35 min | ✅ JSON | ⬜ Level fehlt | ⬜ |
| S112 | Die Pfannkuchen-Meisterschaft | Neben | 25 min | ✅ JSON | ⬜ Level fehlt | ⬜ |
| S113 | Firnnacht | Neben | 30 min | ✅ JSON | ⬜ Level fehlt | ⬜ |
| S114 | Jupps Melodie | Neben | 30 min | ✅ JSON | ⬜ Level fehlt | ⬜ |
| S115 | Wachsfest | Neben | 30 min | ✅ JSON | ⬜ Level fehlt | ⬜ |
| S116 | Turmmelodien | Neben | 35 min | ✅ JSON | ⬜ Level fehlt | ⬜ |
| S117 | Die Stimmprobe | Neben | 60 min | ✅ JSON | ⬜ Level fehlt | ⬜ |
| S118 | Das Gründerzimmer | Neben | 25 min | ✅ JSON | ⬜ Level fehlt | ⬜ |
| N101 | Das Boot, das allein fährt (Ilsensee) | Neben | 30 min | ✅ JSON + 2 Dialoge | ⬜ Level fehlt | ⬜ |
| N102 | Die Mutprobe der Kaimöwen | Neben | 25 min | ✅ JSON + 3 Dialoge | ⬜ Level fehlt | ⬜ |
| N103 | Der Glockenwels | Neben | 30 min | ✅ JSON + 2 Dialoge | ⬜ Level fehlt | ⬜ |
| N104 | Das Flüsterdorf (Stillwasser befreien, Senta Grau) | Neben | 50 min | ✅ JSON + 4 Dialoge | ⬜ Level fehlt | ⬜ |
| N105 | Die Seeglocke (Seegrotte, Regionsboss) | Neben | 45 min | ✅ JSON | ⬜ Level fehlt | ⬜ |
| N106 | Die Robbe im Netz (Akt 2, Reittier Gischtrobbe) | Neben | 35 min | ✅ JSON + 2 Dialoge | ⬜ Level fehlt | ⬜ |
| N111 | Der Fuchs, der im Kreis läuft (Raunwald) | Neben | 25 min | ✅ JSON + 2 Dialoge | ⬜ Level fehlt | ⬜ |
| N112 | Der Mottenreigen | Neben | 25 min | ✅ JSON + 1 Dialog | ⬜ Level fehlt | ⬜ |
| N113 | Die Spiegelhexe (Laden Gesicht ändern) | Neben | 30 min | ✅ JSON + 2 Dialoge | ⬜ Level fehlt | ⬜ |
| N114 | Krähenhain (Mauritz Knochenfeld) | Neben | 50 min | ✅ JSON + 3 Dialoge | ⬜ Level fehlt | ⬜ |
| N115 | Der Wurzelvater (Regionsboss, nach M108) | Neben | 45 min | ✅ JSON + 2 Dialoge | ⬜ Level fehlt | ⬜ |
| N116 | Der Bock mit dem Garten (Reittier Moosbock) | Neben | 35 min | ✅ JSON + 2 Dialoge | ⬜ Level fehlt | ⬜ |
| N121 | Das Dach vom Warmen Ofen (Kesselbrunn) | Neben | 30 min | ✅ JSON + 2 Dialoge | ⬜ Level fehlt | ⬜ |
| N122 | Die kalten Quellen | Neben | 30 min | ✅ JSON + 2 Dialoge | ⬜ Level fehlt | ⬜ |
| N123 | Heddas Geheimrezept | Neben | 25 min | ✅ JSON + 3 Dialoge | ⬜ Level fehlt | ⬜ |
| N124 | Die Rauchgesichter (Russgrube, Egon Ruß) | Neben | 50 min | ✅ JSON + 5 Dialoge | ⬜ Level fehlt | ⬜ |
| N125 | Der Schmelzer (Alte Mine, Regionsboss) | Neben | 45 min | ✅ JSON | ⬜ Level fehlt | ⬜ |
| N126 | Der summende Quarz (Ambros) | Neben | 25 min | ✅ JSON + 2 Dialoge | ⬜ Level fehlt | ⬜ |
| N211 | Der Freund im Schacht (Glastiefen) | Neben | 25 min | ✅ JSON + 2 Dialoge | ⬜ Level fehlt | ⬜ |
| N212 | Großmutter Prismas Augen (Glastiefen) | Neben | 25 min | ✅ JSON + 2 Dialoge | ⬜ Level fehlt | ⬜ |
| N213 | Die Laterne, die nie ausgeht (Glastiefen) | Neben | 35 min | ✅ JSON + 2 Dialoge | ⬜ Level fehlt | ⬜ |
| N214 | Hohlstollen (Glastiefen) | Neben | 50 min | ✅ JSON + 3 Dialoge | ⬜ Level fehlt | ⬜ |
| N215 | Der Kristallwal (Glastiefen) | Neben | 45 min | ✅ JSON + 1 Dialog | ⬜ Level fehlt | ⬜ |
| N216 | Der Glutdachs (Glastiefen) | Neben | 35 min | ✅ JSON | ⬜ Level fehlt | ⬜ |
| N201 | Das große Froschrennen (Graumoor) | Neben | 25 min | ✅ JSON + 2 Dialoge | ⬜ Level fehlt | ⬜ |
| N202 | Die Wilderer am Birkenhain (Graumoor) | Neben | 35 min | ✅ JSON + 2 Dialoge | ⬜ Level fehlt | ⬜ |
| N203 | Bruder Ilmos Bad (Graumoor) | Neben | 25 min | ✅ JSON + 2 Dialoge | ⬜ Level fehlt | ⬜ |
| N204 | Moorwinkel (Graumoor) | Neben | 50 min | ✅ JSON + 4 Dialoge | ⬜ Level fehlt | ⬜ |
| N205 | Die Moorkönigin (Graumoor) | Neben | 45 min | ✅ JSON + 1 Dialog | ⬜ Level fehlt | ⬜ |
| N206 | Die Nebelnacht (Graumoor) | Neben | 30 min | ✅ JSON + 2 Dialoge | ⬜ Level fehlt | ⬜ |
| M201–M210 | Akt 2 (Neue Gesichter bis Masken) | Main | je 45–60 min | ✅ JSON + 29 Dialoge (`Content/Data/Quests/Akt2/`) | ⬜ Level fehlt | ⬜ |
| M301–M510 | Akte 3–5 | Main | je 40–70 min | 🟨 Missionsdesign (docs/missions/AKT3–5.md), JSON folgt pro Akt | ⬜ | ⬜ |
| D_Schule_* | 12 freie Gespräche (Lio, Aurelia, Lehrkräfte, Mitschüler, Ottilie) | – | – | ✅ | ⬜ | ⬜ |
| D_Dorf_* | 5 freie Gespräche (Wenke, Lotte, Agnes, Morchel, Hanne), Regionen: docs/regionen/ | – | – | ✅ | ⬜ | ⬜ |

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
