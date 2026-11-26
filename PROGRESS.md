# PROGRESS – Ewigfels

Stand: 26.11.2026 · Aktuelle Phase: **1 – Bibeln** (Phase 0: erster Build beim Nutzer offen) · Engine: UE 5.8

Legende: ⬜ offen · 🟨 in Arbeit · ✅ fertig · ❌ blockiert

## Phasen

| Phase | Inhalt | Status |
|---|---|---|
| 0 | Setup, Tools, Testprojekt | 🟨 Dateien fertig, Installation + erster Build beim Nutzer offen |
| 1 | Bibeln + Moodboards | 🟨 Bibeln + ComfyUI-Pipeline fertig, Bilder generieren + Grundsatzentscheidungen E1–E4 bestätigen beim Nutzer |
| 2 | Kernsysteme (Quest, Dialog, Entscheidung, Save) | ⬜ |
| 3 | Charakter & Animation | ⬜ |
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
| C++-Testprojekt | 🟨 | geschrieben, **noch nie kompiliert** (erster Build beim Nutzer) |
| GitHub-Repo | ⬜ | Nutzer legt `koelleprime-afk/ewigfels` (privat) an |
| OneDrive-Backup-Skript | ✅ | Scripts/Tools/Backup-ToOneDrive.ps1 |
| Plugins ausgewählt | ✅ | docs/PLUGINS.md (Runde 2: PCGEx, SUDS, Engine-Plugins für KI/UI/Wasser/Motion Matching); Installation per Install-Plugins.ps1 beim Nutzer offen |
| Unreal MCP + CLAUDE.md | 🟨 | in .uproject/.mcp.json eingerichtet, Verbindung beim Nutzer noch nicht getestet |

## Phase 1 im Detail

| Aufgabe | Status | Notiz |
|---|---|---|
| docs/GDD.md | ✅ | E1 entschieden: Akademie + Spieltitel **Nocturna**; E2–E5 mit Optionen (Empfehlung umgesetzt), 25 Weisen, 25 Gegnertypen, 15 Bosse, Systeme, Steuerung |
| docs/STORY_BIBLE.md | ✅ | Welt Velmark, Zeitlinie, Magie „Resonanz“, Fraktionen, 50 Missionen (Liste), 6 Entscheidungen, 4 Enden, 6 Questreihen |
| docs/CHARACTER_BIBLE.md | ✅ | Pipeline, MetaHuman-Anleitung, 4 Türme, 15 Figurenblätter, NPC-Plan, 20 Kreaturen |
| docs/ART_BIBLE.md | ✅ | Paletten pro Region, Rendering, Post Process, Master-Materialien, Licht-Rigs, VFX, Presets, Budgets |
| Content/Data/Art/RegionLooks.json | ✅ | Look-Werte pro Region für Phase 4 |
| docs/ANIMATION_BIBLE.md | ✅ | AnimBP-Struktur, Checklisten aller Sets, FreeMoCap-Anleitung, Lip-Sync, Emotions-Tags, Reittier, Cutscene-Vorlage |
| ComfyUI-Workflows + Batch-Skript | ✅ | `run_comfy.py` gegen Test-Server geprüft, in echtem ComfyUI **noch nicht** gelaufen |
| Prompt-Listen | ✅ | 110 Moodboard-, 128 Figuren-, 48 Kreaturen-, 12 Turm-Bilder |
| Nutzer-Wünsche 26.11. | ✅ | Ton verspielt + gruselige Gegner, Magie aus den Fingern (kein Stab), Personalisierung über Läden (GDD §13), 6 eigene Reittiere, fröhliche/gruselige Dörfer + Missklinger. **Offen: Weltgröße (GDD E9)** |
| Bilder generiert | ⬜ | beim Nutzer (ca. 40–60 min), danach Key-Frames wählen |
| Missionsdesign-Dokumente | ⬜ | pro Akt in der Umsetzungsphase (Akt 1: Phase 5) |

## Systeme

| System | Geschrieben | Umgesetzt | Getestet |
|---|---|---|---|
| _ab Phase 2_ | | | |

## Quests (geschätzte Spielzeit gesamt: 0 / ~100 Std. umgesetzt · geplant: 50 Hauptmissionen ≈ 45–50 Std., Liste in STORY_BIBLE §8.1)

| ID | Name | Typ | Dauer | Geschrieben | Umgesetzt | Getestet |
|---|---|---|---|---|---|---|

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
