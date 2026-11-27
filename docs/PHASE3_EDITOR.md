# PHASE3_EDITOR – Editor-Schritte für Phase 3

Was im Unreal-Editor passieren muss, damit aus der Platzhalter-Figur eine echte animierte Figur wird. Alles, was per Konsole oder Skript geht, erledigt Claude (Remote Control oder Unreal MCP). Der Nutzer macht nur die Klicks, die ein Konto oder den Fab-Shop brauchen. **Pro Nachricht kommt immer nur ein Schritt.**

## 0. Was ohne Editor-Arbeit schon geht

Nach dem Bauen: Editor öffnen, eine **leere Karte** (oder `L_Phase3_Test`) starten → die Testarena baut sich selbst auf: Platzhalter-Figur aus Grundformen, 5 Weisen, Übungspuppen, Lio zum Reden (E), ab 20 s Gegnerwellen. **F1** zeigt die Steuerung. Konsole: `Ewigfels.Arena.Wave 3`, `Ewigfels.Arena.Clear`, `Ewigfels.Spells.List`.

## 1. Testkarte und Leucht-Materialien (Claude)

Skript `Scripts/Python/create_phase3_testmap.py` im Editor ausführen (Werkzeuge → Python-Skript ausführen, oder per Unreal MCP). Es legt an:
- `M_EwigfelsGlow` (leuchtend, für Zauber und Augen) und `M_EwigfelsSolid` (Grundmaterial mit Farbe)
- `/Game/Maps/L_Phase3_Test` mit Arena und Startpunkt, als Start-Karte eingetragen

Ohne das Skript läuft alles auch, nur mit einfacheren Materialien.

## 2. Game Animation Sample (Nutzer: 2 Klicks, Rest Claude)

Das Game Animation Sample von Epic enthält ca. 500 Bewegungs-Animationen und fertiges Motion Matching (kostenlos, für Unreal-Projekte freigegeben; Eintrag in CREDITS.md vor dem Einbau).

| Schritt | Wer | Was |
|---|---|---|
| 2.1 | Nutzer | Epic Games Launcher → Fab → „Game Animation Sample“ → **Projekt erstellen** (eigener Ordner, nicht in Ewigfels) |
| 2.2 | Nutzer | Das neue Projekt einmal öffnen und warten, bis die Shader fertig sind |
| 2.3 | Claude | Im Sample-Projekt `CBP_SandboxCharacter` samt Abhängigkeiten per **Migrieren** nach `Ewigfels/Content` kopieren |
| 2.4 | Claude | In Ewigfels: Elternklasse von `CBP_SandboxCharacter` auf `EwigfelsCharacter` umstellen (Reparent) |
| 2.5 | Claude | `DefaultGame.ini`: `PlayerPawnClass` = der umgestellte Blueprint, beim Charakter `bUseBuiltInMovementInput=False` (Laufen/Kamera kommen dann aus dem Sample, Zaubern/Ausweichen/Zielaufschaltung aus unserem C++) |
| 2.6 | Claude | Bauen, Arena starten, prüfen: Laufen mit Motion Matching, Zaubern, Treffer, Tod, Wiederbelebung |

**Stand 2026-11-27: erledigt (2.1–2.6).** Was tatsächlich gemacht wurde (UE-5.8-Version des Samples heißt anders als oben):
- Sample-Projekt liegt unter `Dokumente\Unreal Projects\AnimationenGesichtetc` (bleibt als Quelle liegen).
- Migriert wurde `/Game/Blueprints/SandboxCharacter_CMC` (Character-Variante; `SandboxCharacter_Mover` ist die Mover-Variante) mit allen Abhängigkeiten: ca. 2.900 Dateien, 4,3 GB, ohne das Sample vorher im Editor zu öffnen (headless per Python `migrate_packages`). Pfade wie im Sample (`/Game/Blueprints`, `/Game/Characters/UEFN_Mannequin` …), Content liegt nur lokal + OneDrive.
- 19 zusätzliche Engine-Plugins in `Ewigfels.uproject` (siehe CREDITS.md).
- `DefaultEngine.ini`: Kollisionskanäle `Traversable`, `Mouse`, `Obstacle` (GameTraceChannel1–3) + Profile `CharacterCapsule`, `TraversalObjectPreset`, `ObstaclePreset`, Daten-Konsolenvariablen `DDCvar.*`, PoseSearch-Einstellung. `DefaultGameplayTags.ini` aus dem Sample übernommen (Foley, MotionMatching, SmartObject).
- Elternklasse von `SandboxCharacter_CMC` = `EwigfelsCharacter`, `bUseBuiltInMovementInput=False`, als `PlayerPawnClass` eingetragen. Kamera kommt aus der GameplayCamera des Samples; unsere `FollowCamera`/`CameraBoom` sind zusätzlich da (noch prüfen, ob Zielaufschaltung/Dialogkamera richtig umschalten).
- Bau grün, Tests `Ewigfels.*` grün, Probestart: Testarena + Sample-Kamera aktiv, keine Blueprint-Fehler.

Warum Reparent statt neuer AnimBP: Das Sample-AnimBP liest seine Werte aus dem Sample-Charakter. Wenn dieser Charakter von unserer C++-Klasse erbt, behalten wir das fertige Motion Matching und bekommen trotzdem Zauber, Kampf und Mehrspieler-Vorbereitung.

Später (eigenes AnimBP `ABP_EwigfelsCharacter` nach ANIMATION_BIBLE §2): Werte kommen aus `UEwigfelsAnimInstance` (Geschwindigkeit, Richtung, Neigung, Zielwinkel, Zauber-Schule, Grundton, Handbereitschaft, Erschöpfung).

## 3. Eigene Figur (später, MetaHuman)

Erst wenn 2. läuft: MetaHuman-Figur nach CHARACTER_BIBLE (Nutzer wählt das Gesicht im MetaHuman Creator, Claude hängt sie an den Charakter und richtet Retargeting ein).

## 4. Niagara-Effekte (Claude, Phase 3b)

Jede Weise hat in ihrer JSON schon den Pfad (`vfx.niagara`, z. B. `/Game/VFX/Spells/NS_Funkenstoss`). Sobald es das System an diesem Pfad gibt, wird es statt des Platzhalter-Leuchtens benutzt, ohne Code-Änderung. Parameter `User.Color` bekommt die Farbe der Weise. Liste der Effekte: SPELL_FORMAT.md und Grundweisen.json.

## 5. Prüfliste für den ersten Test

- [ ] Leere Karte → Arena erscheint, Nachricht „Nocturna: Phase 3 – Testarena“
- [ ] WASD, Maus, Springen, Sprinten (Umschalt), Ausweichen (Alt/C)
- [ ] 1–5 zaubern, rechte Maustaste Schild, Parade im richtigen Moment = Zeitlupe + Konter
- [ ] Mittlere Maustaste/Tab: Zielaufschaltung, Mausrad wechselt Ziel
- [ ] Lio ansprechen (E) → Dialogkamera wechselt Einstellungen
- [ ] Gegnerwellen: Novizen greifen höchstens zu zweit an, Fluchsänger halten Abstand
- [ ] Tod → nach 3 s Wiederbelebung
- [ ] Automation-Tests `Ewigfels.*` grün
