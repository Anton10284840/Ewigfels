# TO-DO – Nocturna (Gesamtüberblick)

Stand: 27.11.2026 · Diese Liste zeigt **alles**, was bis zum fertigen Spiel zu tun ist. Details und Status pro System stehen in [PROGRESS.md](PROGRESS.md).

Legende: ✅ fertig · 🟨 läuft · ⬜ offen · 👤 = du am PC · 🤖 = Claude (Text/Code, geht auch ohne deinen PC)

**Wo wir stehen:** Phase 0–2 fertig. Phase 3 (Spielfigur, Zaubern, Kampf) und der Charakter-Editor sind als Code fertig und warten auf den ersten Bau auf deinem PC. Story, Dialoge, Gegner, Reittiere und die erste Region (Ilsensee) sind als Text und Daten fertig.

---

## Jetzt als Nächstes

| # | Aufgabe | Wer | Status |
|---|---|---|---|
| 1 | Phase 3 bauen und testen (PR #10), Fehler beheben | 👤 + 🤖 | 🟨 |
| 2 | Charakter-Editor bauen (PR #11), Gesichts-Technik entscheiden | 👤 + 🤖 | 🟨 |
| 3 | Story Akt 1 bis Ende ausarbeiten (alle 50 Missionen, Wendungen, Bosse) | 🤖 | ✅ |
| 4 | Dialoge für Lio, Lehrkräfte und Mitschüler | 🤖 | ✅ |
| 5 | Gruselige Gegner + 6 Reittiere mit Fähigkeiten und Verhalten | 🤖 | ✅ |
| 6 | Erste Region: fröhliches + gruseliges Dorf, Bewohner, Quests (`docs/regionen/ILSENSEE.md`) | 🤖 | ✅ |
| 7 | Game Animation Sample einbinden (docs/PHASE3_EDITOR.md, ein Schritt pro Nachricht) | 👤 | ⬜ |

---

## Phase 0 – Einrichtung ✅ (fast)
- ✅ Hardware erfasst, Tools ausgewählt, Lizenzen geprüft
- ✅ Projekt, GitHub-Repo, OneDrive-Backup-Skript
- ✅ Erster Build auf dem PC
- ⬜ 👤 Plugins installieren (Install-Plugins.ps1 per Remote Control)
- ⬜ 👤 Unreal MCP (Editor-Steuerung) einmal testen
- ⬜ 👤 Webcam-Modell nennen (für Gesichts- und Bewegungsaufnahme)

## Phase 1 – Bibeln und Bilder 🟨
- ✅ GDD, Story-, Figuren-, Grafik-, Animations-Bibel
- ✅ 430 Weisen (Zauber) als Daten, Akt-1-Schulleben
- ⬜ 👤 Concept-Bilder mit ComfyUI erzeugen (ca. 1 Std. Rechenzeit), dann die besten auswählen

## Phase 2 – Kernsysteme ✅
- ✅ Weltstatus, Quests aus JSON, Dialoge (SUDS), Speichern, Prüfskript, 9/9 Tests grün

## Phase 3 – Spielfigur, Zaubern, Kampf 🟨
- ✅ 🤖 C++: Figur, 5 Weisen, Kampf, 2 Gegnertypen, Dialogkamera, HUD, Testarena (Koop-fähig)
- ✅ 🤖 Charakter-Editor (Code)
- ⬜ 👤 Bauen + Tests auf dem PC
- ⬜ 👤 Game Animation Sample (Bewegung), Niagara-Effekte für die 5 Weisen
- ⬜ 👤 Spieler-MetaHuman, Lip-Sync-Test
- ⬜ Entscheidung Gesichts-Technik (8 Grundgesichter mischen / fertige Gesichter / Comic-Look)

## Phase 4 – Grafik-Benchmark-Szene ⬜
- ⬜ Akademie-Innenhof + Waldrand, goldene Stunde und Regennacht, finales Niveau
- ⬜ Modulares Architektur-Kit (Wände, Bögen, Fenster, Dächer)
- ⬜ 60-Sekunden-Trailer-Shot (Movie Render Queue)

## Phase 5 – Akt 1 spielbar (Vertical Slice) ⬜
Geschichte und Daten (🤖, geht ohne PC):
- ✅ Missionsdesign aller 50 Missionen + Story-Bosse → `docs/missions/`
- ✅ Quest-JSON M101–M110
- ✅ Dialoge Akt 1: 28 Missionsdialoge + 12 freie Gespräche, Stimmen-Leitfaden `docs/DIALOG_STIMMEN.md`
- ✅ Schulleben-Quests S101–S118 als JSON
- ✅ Region Ilsensee: Uferlicht + Stillwasser, 6 Nebenquests N101–N106 als JSON + 13 Dialoge, Regionsdaten `Content/Data/World/Regions/Ilsensee.json`
- ✅ Region Raunwald: Pilzhausen + Krähenhain, Nebenquests N111–N116 + 13 Dialoge (`docs/regionen/RAUNWALD.md`)
- 🟨 Kesselbrunn (+ Russgrube): Beschreibung fertig (`docs/regionen/KESSELBRUNN.md`), Quests N121–N126 + Dialoge folgen
- ✅ Gegner, Gruseldorf-Herrscher, Bosse und Reittiere als Design-Daten (`docs/BESTIARIUM.md`, `Content/Data/Creatures/Design/`)
- ⬜ Kampfwerte (Leben, Tempo, Weisen) der Akt-1-Gegner im Phase-3-Format ableiten (nach Build von Phase 3)
- ✅ Reittier Moosbock (Zähm-Quest N116)

Bauen im Editor (👤 mit Anleitung):
- ⬜ Landschaft 4 × 4 km um die Akademie, PCG für Wald und Felsen
- ⬜ Akademie (Große Halle, 4 Türme, Bibliothek, Klassenräume, Geheimgänge)
- ⬜ Uferlicht, Stillwasser, Pilzhausen, Krähenhain, Kesselbrunn, Russgrube
- ⬜ Unterrichts-Minispiele, Glockenball, Probier-Modus
- ⬜ Questlog, HUD, Karte, Kompass, Inventar, Läden (UI)
- ⬜ Cutscenes M101, M106, M108
- ⬜ Musik und Ton (Akkordeon-Leitmotiv!)
- ⬜ Akt 1 komplett durchspielen und polishen

## Phase 6–10 – Akte 2 bis 5 ⬜
Pro Akt: Regionen bauen, Quests + Dialoge als Daten, Gegner/Bosse, Cutscenes, testen.
- ⬜ Akt 2: Graumoor, Glastiefen · Boss Mara · Entscheidung 2 · Reittiere Gischtrobbe + Glutdachs
- ⬜ Akt 3: Tiedmar, Weiße Zinnen, Kaelschlucht · Boss Wächter von Kael · Entscheidung 3 · Sturmschwinge + Dünenschreiter
- ⬜ Akt 4: Treibende Inseln, Aurenburg · Boss Corvin · Entscheidungen 4 + 5 · Perlsegler · Akademie zerstört
- ⬜ Akt 5: Nachhall (Schattenreich) · Endboss Dirigent · Entscheidung 6 · 4 Enden + Epiloge
- ⬜ 6 Questreihen (48 Quests), 3 Fraktionslinien (15), 60 Sidequests, 40+ Rätselschreine

## Phase 11 – Finale ⬜
- ⬜ Leistung optimieren (Ziel: flüssig auf deiner RTX 5070)
- ⬜ Fehler beheben, Balancing, Schwierigkeitsstufen
- ⬜ Englische Übersetzung über String Tables
- ⬜ Koop für 2–4 Spieler einschalten und testen
- ⬜ Spiel als .exe packen

---

**So pflegen wir die Liste:** Jeder Thread, der etwas fertig macht, setzt hier den Haken und trägt Details in PROGRESS.md ein.
