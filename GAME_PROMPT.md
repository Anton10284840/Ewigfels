MASTER-PROMPT: Open-World-Zauberer-RPG (Arbeitstitel: „Ewigfels“)
Anleitung für mich: Diese Datei in den Projektordner legen und Claude Code sagen: „Lies GAME_PROMPT.md komplett und starte mit Phase 0.“

# 0. Deine Rolle
Du bist mein komplettes Entwicklerteam in einer Person:

* Lead Game Developer (C++, Unreal Engine 5, Architektur)
* Technical Artist (Rendering, Materialien, Shader, Lighting, VFX)
* Character Technical Director (MetaHumans, Rigging, Animation, Facial)
* Narrative Designer (Story, Dialoge, Quests, Lore)
* Level Designer (Open World, PCG, Dungeons)
* Producer (Planung, Fortschritt, Prioritäten)

Ich arbeite alleine mit dir. Du schreibst den Code, die Skripte, die Datenfiles und die Texte. Ich übernehme die Klicks im Editor, die du nicht automatisieren kannst. Dafür bekomme ich von dir immer exakte Schritt-für-Schritt-Anleitungen.

# 1. Vision
Ein Open-World-Action-RPG in einer eigenen Zaubererwelt. Das Spielgefühl ist von Hogwarts Legacy inspiriert, die Welt ist aber 100 % eigenständig: keine Namen, Orte, Figuren, Zauberformeln oder Begriffe aus Harry Potter bzw. der Wizarding World.

Grafik-Ziel: „Cinematic AI-Video-Niveau“ (so wie die Clips aus Higgsfield, Kling oder Veo), aber in Echtzeit spielbar:

* Filmischer Look: weiches, physikalisch korrektes Licht, volumetrischer Nebel, Lichtstrahlen durch Bäume, tiefe Schatten, cineastische Farbgebung
* Fotorealistische Oberflächen: moosiger Stein, nasses Kopfsteinpflaster, Holz mit Maserung, Stoffe mit Faserstruktur
* Lebendige Charaktere: echte Haut mit Subsurface Scattering, Haare aus Strähnen, glaubwürdige Mimik
* Kamera wie im Film: Tiefenschärfe, dezente Bewegungsunschärfe, Kamerafahrten in Zwischensequenzen
* Magie, die „teuer“ aussieht: Partikel, Licht, Verzerrung, Funken, Rauch

Budget: 0 €. Nur kostenlose Software, kostenlose bzw. CC0-Assets und lokal laufende Open-Source-KI. Higgsfield und andere kostenpflichtige Dienste werden nicht benutzt.

Umfang: ca. 100 Stunden Spielzeit.

* Main Story: ca. 45–50 Std.
* Questreihen & Sidequests: ca. 35–40 Std.
* Erkundung & Aktivitäten: ca. 10–15 Std.

Plattform: Windows-PC. Ziel-Performance: 60 FPS in 1440p auf meiner Hardware, mit Skalierungsstufen von Low bis Cinematic.

# 2. Tech-Stack (alles kostenlos)

## Engine & Code

* Unreal Engine 5 (neueste stabile Version): Lumen, Nanite (auch für Foliage), Virtual Shadow Maps, Virtual Textures, PCG-Framework, World Partition, Niagara, MetaSounds, Chaos Physics, Motion Matching, Control Rig, Sequencer, Movie Render Queue
* C++ für alle Kernsysteme, Blueprints nur für Content und schnelles Tuning
* Unreal Python Editor Scripting für automatisierten Weltaufbau, Asset-Import, Massenänderungen und Validierung
* Visual Studio 2022 Community (C++-Workload, Game Development)
* Git + Git LFS (.gitignore und .gitattributes für UE5 anlegen)

## Grafik-Assets

* Fab (kostenlose Assets und Megascans-Inhalte; monatliche Gratis-Assets mitnehmen)
* Poly Haven (HDRIs, Texturen, Modelle, CC0)
* ambientCG (PBR-Materialien, CC0)
* Blender (Modellieren, Aufräumen, UVs, Retopologie, Rigging-Anpassungen)
* Material Maker bzw. ArmorPaint (prozedurale Materialien, Texturieren)

## Lokale KI (Open Source, läuft auf meiner GPU)

* ComfyUI mit FLUX.1-schnell (Apache 2.0) bzw. SDXL: Concept Art, Moodboards, Texturen, Decals, Gemälde in der Akademie, UI-Icons, Ladebildschirme, Kartenillustration
* Hunyuan3D-2 bzw. TRELLIS: 3D-Props aus Bildern (Zauberstäbe, Bücher, Tränke, Kessel, Möbel, Statuen). Danach in Blender aufräumen (Retopo, UV, LODs bzw. Nanite)
* Real-ESRGAN: Texturen hochskalieren
* Kokoro TTS (Apache 2.0) bzw. Piper: Platzhalter-Sprachausgabe für NPCs
* Whisper: Untertitel-Timing aus Audio erzeugen
* Prüfe bei jedem KI-Modell die Lizenz. Ich will das Spiel eventuell später veröffentlichen, also nur Modelle mit kommerziell nutzbarer Lizenz. Ist eine Lizenz nicht kommerziell, sag es mir und schlag eine Alternative vor.

## Charaktere & Animation

* MetaHuman Creator (Haupt- und Story-Charaktere)
* MetaHuman Animator (Gesichtsanimation aus Video bzw. Audio, je nachdem, was in der aktuellen UE-Version mit Webcam oder Android-Handy geht. Ich habe kein iPhone!)
* Epic Game Animation Sample Project (kostenlos, Motion Matching mit hunderten Animationen als Basis-Locomotion)
* Mixamo (zusätzliche Animationen, per IK-Retargeter auf das UE5-Skelett übertragen)
* FreeMoCap (Open Source) bzw. Rokoko Vision (Gratisstufe): eigene Bewegungen mit Webcam aufnehmen, z. B. Zauberbewegungen und Emotes
* Cascadeur (kostenlose Version für Indies): Kampfanimationen per Keyframe mit Physik-Assistenz
* Control Rig + Full Body IK: prozedurale Anpassungen wie Füße auf unebenem Boden, Blickrichtung, Zauberstab-Zielen

## Audio

* Freesound (nur CC0 bzw. CC-BY mit Credits)
* Sonniss GDC Bundles (kostenlose Sound-Pakete)
* MetaSounds für dynamische Sounds (Zauber-Layering, Schritte je nach Untergrund)
* Musik: lizenzfreie bzw. CC0-Musik. Optional schreibe ich selbst etwas (ich spiele Akkordeon, das kann ein Instrument im Soundtrack sein)

## Pflicht: CREDITS.md
Jede externe Ressource bekommt einen Eintrag mit Name, Quelle, Lizenz und Verwendungsort. Ohne Eintrag kommt nichts ins Projekt.

# 3. GRAFIK-BIBEL („Cinematic-Niveau“)
Erstelle daraus als Erstes eine `docs/ART_BIBLE.md` und setze sie technisch um.

## 3.1 Look & Stimmung

* Grundstimmung: „dunkles Märchen trifft Realismus“. Warmes Kerzenlicht innen, kühles Nebelblau außen, goldene Stunde als Signature-Look
* Farbpalette pro Region definieren (Primär-, Sekundär- und Akzentfarbe, Lichttemperatur)
* Mit ComfyUI für jede Region ein Moodboard aus 6–10 Bildern generieren. Workflow und Prompts dokumentierst du in `art/concepts/`

## 3.2 Rendering-Setup

* Lumen für GI und Reflexionen. Hardware-Raytracing aktivieren, falls die GPU es kann, sonst Software-Lumen
* Nanite für alle statischen Meshes inklusive Bäume und Felsen (Nanite Foliage)
* Virtual Shadow Maps, Contact Shadows für kleine Details
* Volumetric Clouds (dynamisch, mit Wettersystem gekoppelt)
* Exponential Height Fog + Volumetric Fog + lokale Fog Volumes (Moor, Höhlen, Wald)
* Sky Atmosphere mit physikalischem Tag-Nacht-Zyklus, Sterne und zwei Monde als eigene Welt-Besonderheit
* Post Process:
   * Filmic Tonemapper, eigene Color-Grading-LUTs pro Region und Tageszeit
   * Bloom (dezent), Lens Flares nur bei Magie und Sonne
   * Depth of Field: im Gameplay nur leicht, in Dialogen und Cutscenes stark (Cine Camera)
   * Motion Blur dezent (im Menü abschaltbar)
   * Film Grain und Chromatic Aberration minimal, für den Filmlook
   * Auto-Exposure abgestimmt auf Innen/Außen-Übergänge
* TSR (Temporal Super Resolution) als Upscaler

## 3.3 Materialien

* Master-Materialien mit Material Instances:
   * `M_Master_Surface` (PBR, Detail-Normal, Makro-Variation gegen Kachelmuster)
   * `M_Master_Landscape` (Auto-Material nach Steigung und Höhe, Runtime Virtual Texture, Blending mit Meshes)
   * `M_Master_Foliage` (Subsurface, Wind via Pivot Painter bzw. WPO)
   * `M_Master_Water` (See, Fluss, Meer; Refraktion, Schaum, Kaustiken)
   * `M_Master_Glass` (Fenster der Akademie, Tränke)
   * `M_Master_Magic` (emissiv, Fresnel, Distortion, Dissolve)
* Wettergesteuerte Materialien: Nässe, Pfützen und Schnee über Material Parameter Collection global steuern
* Decals für Moos, Dreck, Risse, Ruß und magische Runen

## 3.4 Beleuchtung

* Lichtsetzung der Akademie wie im Film: Kerzen, Fackeln und Kronleuchter mit flackernden Light Functions, Lichtstrahlen durch Buntglasfenster (volumetrisch)
* Light-Rigs pro Innenraum-Typ (Halle, Bibliothek, Kerker, Turmzimmer) als wiederverwendbare Blueprints
* Cutscene-Lighting: Key, Fill und Rim Light pro Shot im Sequencer

## 3.5 VFX (Niagara)

* Pro Zauber: Cast-Effekt (Hand/Stab), Projektil bzw. Strahl, Impact, Nachglühen
* Umgebung: Glühwürmchen, Staub im Licht, Blätter im Wind, Regen mit Splashes, Schneeflocken, Funken, schwebende Kerzen, magische Partikel an Ley-Linien
* GPU-Partikel und Skalierbarkeit (Effekte bei Low weniger dicht)

## 3.6 Cinematic-Modus

* Fotomodus (Pause, freie Kamera, DOF, Filter, Posen)
* Movie Render Queue + Path Tracer für Trailer-Aufnahmen in Maximalqualität
* Skalierbarkeits-Preset „Cinematic“ im Grafikmenü

## 3.7 Performance-Regeln

* Profiling mit Unreal Insights nach jedem Meilenstein
* Budgets festlegen: Draw Calls, Partikelzahlen, Lichtquellen mit Schatten pro Szene
* Grafikmenü: Low / Medium / High / Epic / Cinematic + Einzeloptionen

# 4. CHARAKTER-PIPELINE
Lege `docs/CHARACTER_BIBLE.md` an und baue diese Pipeline auf.

## 4.1 Spielercharakter

* Charakter-Editor zu Spielbeginn auf Basis von MetaHuman-Presets: Gesicht (mehrere Presets + Regler), Hautton, Haare (Groom inkl. Locken), Augenfarbe, Narben/Male, Stimme (2–3 Varianten), Körperbau
* Modulares Outfit-System: Kopf, Oberkörper, Umhang, Hände, Beine, Schuhe als einzelne Skeletal Meshes, gemeinsames Skelett (Leader Pose bzw. Modular Rig)
* Umhang und Kleidung mit Chaos Cloth für Wind, Laufen und Fliegen
* Zauberstab bzw. Fokus-Item als austauschbares Attachment mit eigenen Partikel-Effekten

## 4.2 Story-Charaktere
Entwirf 15 Hauptcharaktere, für jeden ein Charakterblatt mit:

* Name, Alter, Rolle, Fraktion, Persönlichkeit, Ziel, Geheimnis, Entwicklungsbogen über die 5 Akte
* Aussehen (Gesicht, Statur, Frisur, Kleidungsstil, Farbpalette)
* Concept Art per ComfyUI: Porträt, Ganzkörper, Ausdrucks-Sheet mit 6 Emotionen
* MetaHuman-Umsetzung: Schritt-für-Schritt-Anleitung, welche Presets und Regler ich im MetaHuman Creator einstellen soll
* Stimm-Beschreibung (für TTS-Platzhalter und spätere echte Sprecher)
* Eigene Idle-Animation bzw. Körpersprache (z. B. verschränkte Arme, nervöses Spielen mit dem Ring)

Pflichtrollen: Mentor/Mentorin, bester Freund bzw. beste Freundin (Begleiter), Rivale, 2–3 Lehrkräfte, Akademie-Leitung, Anführer der geheimen Gruppe (Antagonist), Verräter (wird erst in Akt 4 klar), mysteriöse Figur aus dem Schattenreich, Händler/Dorfbewohner mit Story-Gewicht.

## 4.3 Nebencharaktere & Crowds

* NPC-Generator: prozedurale Kombination aus Gesichtern, Outfits, Frisuren und Farben, damit sich keine NPCs doppeln
* Schüler mit Schuluniform-Varianten je nach Akademie-Haus (eigene Häuser erfinden: 4 Häuser mit Name, Wappen, Farben, Werten)
* Dorfbewohner, Händler, Wachen, Magier-Beamte
* Mass AI bzw. LOD-System für Menschenmengen im Dorf und in der Hauptstadt
* Tagesabläufe (Smart Objects): NPCs gehen zum Unterricht, essen, schlafen, arbeiten

## 4.4 Kreaturen

* 20 eigene magische Kreaturen (keine aus Harry Potter): Beschreibung, Lebensraum, Verhalten, zähmbar ja/nein, Concept Art
* Fliegendes Reittier (z. B. greifartig): eigenes Rig, Flug-Animationsset (Abheben, Gleiten, Flügelschlag, Sturzflug, Landen, Bodenbewegung)
* Ausgangsbasis für Kreaturen: kostenlose Rigs von Fab bzw. Blender-Modelle, Animation über Cascadeur bzw. Control Rig

# 5. ANIMATIONS-PIPELINE
Lege `docs/ANIMATION_BIBLE.md` an.

## 5.1 Basis

* UE5-Mannequin-Skelett als Standard, alle externen Animationen per IK Retargeter darauf übertragen
* Motion Matching (Pose Search) für die Locomotion: Gehen, Joggen, Sprinten, Starts, Stops, Drehungen, Rückwärts, Strafe. Basis ist das Game Animation Sample
* Animation Blueprint mit sauberer Struktur: Locomotion → Overlay-Layer (Zauber, Interaktion) → Additive (Atmung, Treffer) → IK (Füße, Hände, Blick)
* Foot IK auf unebenem Gelände, Hand-IK für Türen, Hebel und Leitern

## 5.2 Benötigte Animationssets (Liste als Checkliste in ANIMATION_BIBLE.md)

* Locomotion: Idle-Varianten, Gehen/Laufen/Sprinten, Schleichen, Springen, Fallen, Landen (weich/hart), Ausweichrolle, Schwimmen (Oberfläche + tauchen), Klettern, Balancieren, Leiter
* Zaubern: pro Zauber Cast-Start, Halten, Release; schnelle und schwere Variante; während der Bewegung (Upper-Body-Blend); Combo-Übergänge
* Kampf: Block/Schild, Treffer-Reaktionen (vorne, hinten, links, rechts, leicht/schwer), Knockdown, Aufstehen, Tod, Finisher
* Interaktion: Tür öffnen, Truhe öffnen, Aufheben, Trank trinken, Buch lesen, Kessel umrühren, Pflanze ernten, Sitzen, Anlehnen
* Reittier: Aufsteigen, Absteigen, Fliegen-Posen, Kurven, Boost
* Emotes & Social: Winken, Lachen, Nicken, Kopfschütteln, Verbeugen, Jubeln (für Dialoge und Fotomodus)
* NPC-Alltag: Unterrichten, Schreiben, Essen, Unterhalten, Fegen, Verkaufen

## 5.3 Eigene Animationen aufnehmen

* Anleitung für FreeMoCap bzw. Rokoko Vision mit Webcam: Raum-Setup, Kleidung, Aufnahme, Export als FBX, Import, Retargeting, Säubern im Sequencer bzw. Control Rig
* Eigene Zauberbewegungen als Signature-Stil (ich nehme sie selbst auf)

## 5.4 Gesichtsanimation & Dialoge

* Dialog-Kamera-System: automatische Schuss-Gegenschuss-Kameras, Close-ups, DOF auf den Sprechenden
* Lip-Sync: Audio → Gesichtsanimation über MetaHuman Animator (Audio-Driven), sonst Fallback auf Viseme-basiertes Lip-Sync
* Emotions-Tags im Dialog-JSON (`"emotion": "angry"`) steuern Gesichtsausdruck, Körpersprache und Kamera
* Blinzeln, Augenbewegung und Mikro-Mimik prozedural, damit Gesichter nie „tot“ wirken

## 5.5 Cutscenes

* Wichtige Story-Momente als Sequencer-Cinematics (Ziel: ca. 60–80 Cutscenes)
* Für jede Cutscene: Skript mit Shot-Liste (Kameraeinstellung, Brennweite, Bewegung, Licht, Dialog, Animation)
* Cine Camera mit echten Objektiv-Werten (24 mm / 35 mm / 50 mm / 85 mm), Letterbox optional
* Übergänge Gameplay → Cutscene → Gameplay ohne Ladezeit

# 6. SPIELWELT

## 6.1 Akademie (zentraler Hub)

* Eigene Zauberakademie auf einer Klippe über einem See. Name, Geschichte und Architektur entwickelst du
* Bereiche: Große Halle, 4 Häuser-Gemeinschaftsräume, Bibliothek (mit verbotener Abteilung), Klassenräume (mind. 8), Gewächshäuser, Kreaturen-Gehege, Observatorium, Kerker, Glockenturm, Innenhöfe, Bootssteg, Geheimgänge (mind. 15)
* Die Akademie verändert sich mit der Story (Feste, Zerstörung in Akt 4, Wiederaufbau)
* Modulares Kit für die Architektur (Wände, Bögen, Säulen, Fenster, Treppen, Dächer), damit schnell gebaut werden kann

## 6.2 Regionen (World Partition, nahtlos)
10 Regionen, schrittweise freigeschaltet:

1. Akademie-Gelände & See
2. Verwunschener Wald
3. Bergdorf mit Markt
4. Moor mit Ruinen
5. Höhlensystem (unterirdische Kristallseen)
6. Küste mit Hafenstadt
7. Hochgebirge (Schnee, Gletscher)
8. Wüsten-Canyon mit alter Zivilisation
9. Schwebende Inseln
10. Hauptstadt der Magierregierung + Schattenreich als Parallelwelt (eigene Optik: entsättigt, invertiertes Licht, schwebende Trümmer)

Pro Region: eigene Optik, Farbpalette, Wetter, Gegner, Musik, 1 großer Dungeon, 1 Regionsboss, Rätselschreine, Sammelobjekte, Schnellreisepunkte.

## 6.3 Weltaufbau-Technik

* Landschaft: Heightmap (selbst erzeugt oder per Python-Skript prozedural), Landscape-Layer automatisch
* PCG-Graphen für Wälder, Felsen, Gras, Pilze, Wege, Ruinen-Streuung, Dorfrandbebauung
* Python-Skripte, die Schlüsselorte, Quest-Marker, Spawnpunkte und Schnellreisepunkte aus JSON-Daten platzieren
* Dynamischer Tag-Nacht-Zyklus (ein Spieltag = 60 Min. Echtzeit, einstellbar)
* Wettersystem: klar, bewölkt, Regen, Gewitter, Nebel, Schnee (regionsabhängig), mit Übergängen und Einfluss auf Materialien, Sound und NPC-Verhalten

# 7. GAMEPLAY-SYSTEME

1. Third-Person-Controller: Laufen, Sprinten, Springen, Ausweichen, Klettern, Schwimmen, Gleiten
2. Zaubersystem: 25 Zauber in 5 Schulen (Angriff, Verteidigung, Elementar, Verwandlung, Verbotene alte Magie), Zauber-Schnellwahlrad, Combos, Cooldowns, Elementar-Interaktionen (Feuer + Öl, Eis + Wasser, Blitz + Nässe)
3. Kampfsystem: Lock-On, Parieren mit perfektem Timing, Schilde, Gegner-Schwachstellen; 25 Gegnertypen, 10 Regionsbosse, 5 Story-Bosse; Behavior Trees bzw. State Trees für die KI
4. Quest-System (komplett datengetrieben): Quests als JSON, Questlog, Wegpunkte, Bedingungen, Verzweigungen, Belohnungen, Weltstatus-Flags
5. Dialogsystem: Verzweigungen, Entscheidungen, Skill- und Ruf-Checks, Emotions-Tags, Kamera-Integration
6. Entscheidungs- und Konsequenzsystem: globale Flags, Beziehungswerte zu NPCs, Ruf bei 3 Fraktionen
7. Inventar & Ausrüstung: Kleidung mit Werten und Optik (Transmog), Zauberstab-Teile, Seltenheitsstufen
8. Crafting: Tränke brauen (Minispiel), Pflanzen anbauen, Ausrüstung verbessern
9. Progression: Level, Talentbaum pro Zauberschule, Zauber über Unterricht und Quests freischalten
10. Begleiter-System: bis zu 1 Begleiter in der Welt, eigene Kampf-KI, Kommentare zu Orten und Entscheidungen
11. Reittier: Fliegen mit Ausdauer, Upgrades, Rennen
12. Housing: eigenes Turmzimmer einrichten und erweitern
13. Karte & Kompass: Weltkarte mit Nebel des Krieges, Marker, Schnellreise
14. Speichern/Laden: mehrere Slots, Autosave, Weltstatus komplett speichern
15. UI/UX: Hauptmenü mit animiertem Hintergrund, HUD (minimalistisch, ausblendbar), Questlog, Inventar, Talentbaum, Kodex/Lore-Buch, Optionen (Grafik, Audio, Steuerung, Barrierefreiheit, Untertitel). Gamepad- und Tastatursupport (Enhanced Input)
16. Lokalisierung: alle Texte über String Tables; Sprachen Deutsch + Englisch

# 8. STORY & QUESTS (ca. 100 Std.)
Lege `docs/STORY_BIBLE.md` an (Lore, Weltgeschichte, Zeitlinie, Magiesystem-Regeln, Fraktionen, Figuren-Beziehungen).

## 8.1 Main Quest (ca. 45–50 Std.)
Grundidee: Der Spieler beginnt als Neuling an der Akademie und entdeckt eine uralte, verbotene Magieform, die in ihm erwacht. Eine geheime Gruppe will diese Kraft für sich.

Struktur: 5 Akte = 5 Akademiejahre

* Akt 1 – Ankunft: Ankunft, erste Geheimnisse, Tutorial-Bogen, erstes Erwachen der alten Magie
* Akt 2 – Schatten im Inneren: Die geheime Gruppe tritt auf, erste Verluste, Spieler muss Kräfte verbergen
* Akt 3 – Die Reise: Reise in fremde Regionen, Wahrheit über die alte Zivilisation und die Magie
* Akt 4 – Verrat: Großer Wendepunkt, Verräter enthüllt, Angriff auf die Akademie
* Akt 5 – Schattenreich: Reise ins Schattenreich, Finale, Endboss

Liefere:

* ca. 50 Hauptmissionen (je 45–60 Min.), jede mit Missionsdesign-Dokument: Ziel, Ablauf, Orte, Gegner, Rätsel, Dialoge, Cutscenes, Entscheidungen, Belohnungen
* mindestens 5 große Entscheidungen mit echten Konsequenzen (Figuren können sterben, Fraktionen sich abwenden, Regionen sich verändern)
* 4 verschiedene Enden + Epilog-Varianten je nach Entscheidungen
* komplette Dialogskripte im JSON-Format

## 8.2 Questreihen & Sidequests (ca. 35–40 Std.)

* 6 große Questreihen mit je 6–10 Quests und eigener Geschichte, z. B.:
   * Begleiter-Freundschaft (mit Entscheidungen, die den Begleiter verändern)
   * Detektivfall in der Hafenstadt
   * Duell-Turnier-Karriere (vom Neuling zum Champion)
   * Kreaturen-Schutzgebiet aufbauen
   * Die verlorene Zivilisation entschlüsseln
   * Untergrund-Widerstand in der Hauptstadt
* 3 Fraktionen mit Ruf-System und eigenen Questlinien
* ca. 60 einzelne Sidequests, verteilt auf alle Regionen
* Regel: keine reinen Hol-und-Bring-Quests. Jede Quest braucht einen Twist, ein Rätsel oder eine Entscheidung
* Jede Quest mit: ID, Name, NPC, Region, Voraussetzungen, Ablauf, Dauer, Belohnung, Dialoge, Konsequenzen

## 8.3 Aktivitäten & Erkundung (ca. 10–15 Std.)

* Rätselschreine (mind. 40, verschiedene Mechaniken)
* Sammelobjekte mit Lore (Seiten eines alten Tagebuchs, verlorene Artefakte, Kreaturen-Kodex)
* Unterrichts-Minispiele (Zaubertränke, Verwandlung, Astronomie, Kreaturenkunde)
* Kreaturen zähmen und pflegen
* Flugrennen mit dem Reittier
* Arena-Herausforderungen
* Housing erweitern

# 9. PROJEKTSTRUKTUR

```
/Source/              C++ (Module: Core, Magic, Combat, Quest, Dialogue, AI, UI, Save, World)
/Content/
  /Characters/        Player, NPCs, Creatures, MetaHumans
  /Animations/        Locomotion, Magic, Combat, Interaction, Facial, Cinematics
  /Environment/       Academy, Regions, Props, Foliage, Materials
  /VFX/               Niagara Systeme
  /Audio/             SFX, Music, Voice, MetaSounds
  /UI/
  /Cinematics/        Sequencer
  /Data/
    /Quests/          JSON
    /Dialogues/       JSON
    /Items/ /Spells/ /Creatures/ /NPCs/
/Scripts/
  /Python/            Unreal-Editor-Skripte
  /ComfyUI/           Workflows (.json) + Prompt-Listen
  /Tools/             Validierungs- und Build-Skripte (PowerShell)
/art/concepts/        Generierte Concept Art
/docs/                GDD, ART_BIBLE, CHARACTER_BIBLE, ANIMATION_BIBLE, STORY_BIBLE
CREDITS.md
PROGRESS.md
```

Dazu ein Validierungs-Skript, das alle Quest- und Dialog-JSONs prüft (fehlende IDs, tote Verzweigungen, unerreichbare Quests, fehlende Übersetzungen) und die geschätzte Gesamtspielzeit ausrechnet.

# 10. VORGEHEN IN PHASEN

* **Phase 0 – Setup:** Frag mich zuerst nach meiner Hardware (CPU, GPU + VRAM, RAM, freier Speicher). Danach eine Installationsanleitung für Windows mit konkreten Schritten und Befehlen: Epic Launcher + UE5, Visual Studio, Git + LFS, Blender, ComfyUI + Modelle, Hunyuan3D bzw. TRELLIS, FreeMoCap. Am Ende ein Testprojekt, das kompiliert.
* **Phase 1 – Bibeln:** GDD.md, ART_BIBLE.md, CHARACTER_BIBLE.md, ANIMATION_BIBLE.md, STORY_BIBLE.md. Moodboards und Charakter-Concepts per ComfyUI.
* **Phase 2 – Kernsysteme (datengetrieben):** Quest-, Dialog-, Entscheidungs- und Speichersystem plus Editor-Tools, damit neue Quests nur noch Daten sind.
* **Phase 3 – Charakter & Animation:** Spieler-MetaHuman, Motion-Matching-Locomotion, Animation Blueprint, 5 Zauber mit VFX, Kampf gegen 2 Gegnertypen, Dialog-Kamera mit Lip-Sync.
* **Phase 4 – Grafik-Benchmark-Szene:** Ein kleines, extrem polishtes Gebiet (Akademie-Innenhof + Waldrand bei goldener Stunde und bei Regennacht) auf finalem Cinematic-Niveau. Dieses Gebiet ist der Qualitätsmaßstab für alles Weitere. Dazu ein 60-Sekunden-Trailer-Shot per Movie Render Queue.
* **Phase 5 – Vertical Slice:** Akt 1 komplett spielbar (ca. 8–10 Std.): Akademie, Wald, Bergdorf, alle Akt-1-Missionen, erste Sidequests, Cutscenes.
* **Phase 6–10 – Akte 2 bis 5:** Je Akt eine Phase: Regionen bauen, Quests umsetzen, Cutscenes, testen, polishen.
* **Phase 11 – Finale:** Performance-Optimierung, Bugfixing, Balancing, Lokalisierung, Packaging als .exe.

# 11. ARBEITSREGELN FÜR DICH

* Arbeite Phase für Phase. Nach jeder Phase: Zusammenfassung, was fertig ist, was ich manuell im Editor tun muss (exakte Klick-Anleitung), was als Nächstes kommt
* Führe `PROGRESS.md`: Status jedes Systems, jeder Quest (geschrieben / umgesetzt / getestet) und jedes Charakters, mit geschätzter Spielzeit
* Qualität vor Menge: lieber eine Szene auf Cinematic-Niveau als zehn halbfertige
* Frag mich vor großen Designentscheidungen und gib mir 2–3 Optionen mit Vor- und Nachteilen
* Sauberer, kommentierter C++-Code, keine Monster-Klassen, Systeme über Interfaces und Komponenten
* Commit nach jedem funktionierenden Feature mit aussagekräftiger Nachricht
* Speicher und Backups über OneDrive (nicht Google Drive)
* Wenn etwas mit Gratis-Tools nicht oder nur schlecht geht, sag es ehrlich und schlag die beste kostenlose Alternative vor
* Antworte mir auf Deutsch, direkt und ohne Floskeln

Starte jetzt mit Phase 0 und frag mich nach meiner Hardware.
