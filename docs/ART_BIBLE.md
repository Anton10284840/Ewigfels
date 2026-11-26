# ART BIBLE – Nocturna („Cinematic-Niveau“)

Stand: 26.11.2026 · Phase 1 · Version 0.1
Maschinenlesbare Werte pro Region: [`Content/Data/Art/RegionLooks.json`](../Content/Data/Art/RegionLooks.json) (wird ab Phase 4 vom Wetter-/Look-System gelesen).
Moodboard-Prompts: `Scripts/ComfyUI/prompts/moodboards.json` · Ablauf: [`Scripts/ComfyUI/README.md`](../Scripts/ComfyUI/README.md).

---

## 1. Look in einem Satz

**Dunkles Märchen trifft Realismus:** echte Materialien, echtes Licht, aber die Welt ist ein bisschen zu schön und ein bisschen zu unheimlich. Innen warmes Kerzenlicht (1800–2700 K), außen kühles Nebelblau (6500–9000 K), **goldene Stunde** (3200 K Sonne, blauer Schatten) als Signature-Look für Key-Art und große Momente.

### 1.1 Die fünf Look-Regeln

1. **Licht erzählt.** Jede Szene hat eine klare Hauptlichtquelle und eine Kontrastfarbe (warm gegen kühl). Kein flaches Rundum-Licht.
2. **Luft ist sichtbar.** Fast immer etwas Nebel/Dunst, Staub im Licht, Lichtstrahlen. Tiefe entsteht durch Atmosphäre.
3. **Oberflächen haben Geschichte.** Nichts ist neu: Moos in Fugen, Wachsflecken, abgegriffenes Holz, Ruß über Fackeln, Wasserlaufspuren.
4. **Magie ist die hellste Farbe im Bild.** Umgebung bleibt eher entsättigt und dunkel, Magie darf strahlen. So „teuer“ wirkt sie.
5. **Klang wird sichtbar.** Magie hat Wellen, Ringe, schwingende Partikel, Verzerrung wie Hitzeflimmern im Takt (Leitmotiv Resonanz).

### 1.2 Was wir nicht machen

Comic-Sättigung, überall Bloom, Lens Flares im Gameplay, lila-pinker Fantasy-Kitsch, glänzend-plastische Haut, Neonfarben außerhalb von Magie, Spitzhüte, Zauberstäbe als dünne Holzstäbchen (bei uns: Stimmstab mit Quarz).

## 2. Farbpaletten pro Region

Primär = dominante Flächenfarbe, Sekundär = zweite Fläche, Akzent = kleine Highlights/Magie/Wegführung. Lichttemperatur = Sonne/Hauptlicht tagsüber.

| # | Region | Primär | Sekundär | Akzent | Licht (K) | Wetter | Stimmung |
|---|---|---|---|---|---|---|---|
| 1 | Nocturna & Ilsensee | Granitgrau `#5B5F63` | Kupfergrün `#5E8C7A` | Kerzengold `#E8B04A` | 5200 (Tag), 3200 (gold. Stunde) | klar, Nebel, Regen | erhaben, heimelig |
| 2 | Raunwald | Moosgrün `#3E5A34` | Rindenbraun `#4A3A2A` | Glimmerblau `#7FD3E8` | 5600 gefiltert | Nebel, Regen, klar | geheimnisvoll, lebendig |
| 3 | Kesselbrunn | Fachwerk-Ocker `#B08A5A` | Schieferblau `#4B5866` | Laternenorange `#F08C2E` | 4800 | klar, bewölkt, Schnee (Winter) | gemütlich, eng |
| 4 | Graumoor | Torfbraun `#4C4433` | Nebelgrau `#9AA0A0` | Irrlichtgrün `#B6F27A` | 7000 (bedeckt) | Nebel, Regen | trostlos, unheimlich |
| 5 | Glastiefen | Höhlenschwarz `#15181C` | Kristalltürkis `#2FB5B0` | Pilzviolett `#A46BE0` | – (nur Biolumineszenz 6000–9000) | – | staunend, kalt |
| 6 | Tiedmar | Kalkweiß `#D8D2C4` | Meergrau-Blau `#3F5A6B` | Signalrot `#C0392B` | 6000 | Sturm, Nebel, Regen, klar | rau, salzig |
| 7 | Weiße Zinnen | Schneeweiß `#EEF2F5` | Gletscherblau `#8FB8D8` | Gebetsfahnen-Safran `#E39B2D` | 6500, hart | Schnee, Sturm, klar | einsam, groß |
| 8 | Kaelschlucht | Sandstein-Rot `#B5603A` | Staubbeige `#D9BE95` | Kael-Gold `#E2B23C` | 4500, sehr hart | klar, Sandsturm | uralt, sengend |
| 9 | Treibende Inseln | Himmelblau `#8DB7E0` | Wolkenweiß `#F3F1EA` | Morgenrosa `#F2A28C` | 5800, weich | klar, Wolkenmeer, Gewitter | schwerelos, frei |
| 10 | Aurenburg | Marmorweiß `#E7E2D8` | Kollegiumsblau `#24406B` | Blattgold `#C9A227` | 5400 | klar, bewölkt, Regen | prächtig, kalt, drückend |
| 10b | Der Nachhall | Aschgrau `#7A7A7E` | Negativ-Violett `#B8A7D9` | Urton-Weiß `#F4F0FF` | Licht von **unten**, 9000 | „Trümmerwind“ | entrückt, falsch |

**Akademie-Innenräume:** Holz `#5A3B24`, Stein `#6A645C`, Kerzenlicht 1900 K, Fensterlicht 7000 K blau. Buntglas: Turmfarben.

## 3. Rendering-Setup

Bereits in `Config/DefaultEngine.ini` (Phase 0): Lumen (GI + Reflexionen), **Hardware-Raytracing an** (RTX 5070 kann es), Nanite, Virtual Shadow Maps, Virtual Textures, kein Light-Baking, TSR, Auto-Exposure mit erweitertem Bereich, Custom Depth + Stencil.

Weitere Festlegungen (Umsetzung Phase 4, Benchmark-Szene):

| Bereich | Einstellung |
|---|---|
| Nanite | Alle statischen Meshes, auch Foliage (Nanite Foliage/Assemblies in 5.8) und Felsen. Ausnahmen: transparente/maskierte Meshes mit WPO prüfen |
| Schatten | VSM, Contact Shadows an der Sonne (Länge 0,02) und an Kerzen, Shadow Ray Count für Flächenlichter niedrig halten |
| Wolken | Volumetric Clouds, Parameter über Wetter-MPC (Bedeckung, Dichte, Wind) |
| Nebel | Exponential Height Fog (2 Schichten) + Volumetric Fog + lokale **Local Fog Volumes** (Moor, Höhlen, Wald, Kerker) |
| Himmel | Sky Atmosphere + Sky Light (Echtzeit-Capture), Sterne als eigenes Himmelsmaterial, **zwei Monde** (Ilse groß/silber, Morn klein/rostrot) als Directional-Lights-Paar nachts (nur einer wirft Schatten) |
| Tag-Nacht | 1 Spieltag = 60 Min. Echtzeit (einstellbar), Sonne nach Breitengrad 50° N |
| Upscaler | TSR (Ziel: 1440p Ausgabe aus ~67 % intern auf High) |
| Path Tracer | nur für Trailer/Key-Art über Movie Render Queue |

### 3.1 Post Process (global, im Haupt-Post-Process-Volume)

| Parameter | Gameplay | Dialog | Cutscene |
|---|---|---|---|
| Tonemapper | Filmic (Standard ACES-ähnlich), Kontrast +0,05 | gleich | gleich |
| Color Grading | LUT pro Region + Tageszeit (Blend über Volumes) | gleich | eigene Shot-LUTs erlaubt |
| Bloom | Intensität 0,35, Threshold −1 (Standard) | 0,35 | 0,5 |
| Lens Flares | aus (nur Magie-Partikel und Sonne-Flare-Sprite) | aus | Sonne 0,2 |
| Depth of Field | aus/leicht (nur Fernunschärfe im Fotomodus) | Cine Camera, Blende f/2,0, Fokus auf Sprecher | f/1,4–f/4 je Shot |
| Motion Blur | 0,3 (Menü: aus/0,3/0,5) | 0,3 | 0,5 |
| Film Grain | 0,1 Intensität | 0,1 | 0,15 |
| Chromatic Aberration | 0,05 | 0,05 | 0,1 |
| Vignette | 0,3 | 0,35 | 0,4 |
| Auto-Exposure | Histogramm, Min −1 / Max +2 EV, Speed Up 3 / Down 1 | fixiert auf Sprecher | manuell pro Shot |

**LUT-Namensschema:** `LUT_<RegionID>_<Tageszeit>` mit Tageszeit ∈ `Dawn`, `Day`, `Golden`, `Dusk`, `Night`, `Storm`. Beispiel: `LUT_R02_Golden`. Erstellung: Screenshot in UE → in **GIMP/Krita** (beide gratis) mit neutraler 16er-LUT-Kachel graden → als `.png` importieren → Texture Group `ColorLookupTable`.

## 4. Materialien

### 4.1 Master-Materialien

| Master | Pfad | Kernfeatures | Wichtige Parameter (Instances) |
|---|---|---|---|
| `M_Master_Surface` | `Content/Environment/Materials/Master/` | PBR (Albedo, Normal, ORM), Detail-Normal, **Makro-Variation** (2 Noise-Texturen in Welt-Raum gegen Kachelmuster), Wetter-Nässe, Moos-Blend nach oben zeigender Normale | `BaseTint`, `MacroStrength`, `DetailTiling`, `MossAmount`, `WetnessResponse` |
| `M_Master_Landscape` | gleich | Auto-Material nach Steigung + Höhe (Gras/Erde/Fels/Schnee), Runtime Virtual Texture, Blending mit Meshes (RVT-Höhe), Nanite-Landscape | `SlopeThreshold`, `SnowHeight`, `LayerTiling` |
| `M_Master_Foliage` | gleich | Two-Sided Foliage/Subsurface, Wind über **Pivot Painter 2** oder einfache WPO, Farbvariation pro Instanz | `WindStrength`, `SeasonTint`, `SSSColor` |
| `M_Master_Water` | gleich | Water-Plugin-basiert: Refraktion, Schaum an Ufern (Distance Field), Kaustiken (Light Function an Unterwasser-Szenen) | `DeepColor`, `Clarity`, `FoamAmount` |
| `M_Master_Glass` | gleich | Translucent mit Lumen-Reflexionen, Schmutzmaske, Buntglas-Variante (Farbe färbt volumetrisches Licht über Light Function) | `Tint`, `Dirt`, `IsStained` |
| `M_Master_Magic` | `Content/VFX/Materials/` | Emissiv, Fresnel, Distortion (Refraction), Dissolve mit Rand-Glühen, Panner-Rauschen; **Grundton-Farbe** als Parameter | `ToneColor`, `PulseRate`, `DissolveAmount` |
| `M_Master_Nachhall` | gleich | Post-Process-Material für das Schattenreich: Entsättigung, invertierte Helligkeit in Schatten, Kanten-Flimmern | `Intensity` (0–1, auch für Nachhall-Belastung des Spielers) |

### 4.2 Wetter über Material Parameter Collection

`MPC_Weather` (global): `Wetness` 0–1, `Puddles` 0–1, `SnowCover` 0–1, `WindStrength`, `WindDirection`, `Lightning` (Flash 0–1), `NachhallBlend` 0–1. Alle Master-Materialien lesen diese Werte; das Wettersystem (Phase 4/5) animiert sie mit Übergangszeiten (Regen beginnt: Nässe +1 in 90 s, Pfützen +1 in 5 min).

### 4.3 Decals

Moos, Dreck, Risse, Ruß (über Fackeln), Wachsflecken, Wasserlaufspuren, Blutfrei-Kampfspuren (Brandflecken, Frost), **magische Runen/Kael-Inschriften** (emissiv, pulsierend). Decal-Material `M_Master_Decal` mit DBuffer.

### 4.4 Material-Quellen

Megascans über Fab (kostenlos in UE), ambientCG und Poly Haven (CC0). Eigene Texturen per SDXL (kachelbar) + Material Maker. Jede Quelle in `CREDITS.md`.

## 5. Beleuchtung

### 5.1 Akademie-Licht wie im Film

- **Kerzen/Fackeln/Kronleuchter:** Point Lights mit **flackernder Light Function** (`MF_Flicker`, Noise-Panner) + leichter Intensitäts-Kurve. Kerzen-Partikel (Niagara) mit eigenem Light-Renderer nur bei Nah-Kerzen.
- **Buntglas:** Rect Light / Sonne durch Fenster, **Volumetric Scattering Intensity 2–4**, Fenster-Material mit Light Function färbt die Strahlen.
- **Light-Rigs als Blueprints** (`Content/Environment/Academy/LightRigs/`):

| Rig | Inhalt | Stimmung |
|---|---|---|
| `BP_LightRig_Hall` | 4 Kronleuchter (je 1 Schatten-Licht + 6 schattenlose), Fensterlicht-Paket, schwebende Kerzen | prachtvoll, warm |
| `BP_LightRig_Library` | Lesepult-Lampen (1900 K), Fenster-Godrays, Staubpartikel-Volumen | still, golden |
| `BP_LightRig_Dungeon` | Fackeln mit Flackern, kaltes Fill aus Lüftungsschächten (8000 K), Local Fog Volume | bedrohlich |
| `BP_LightRig_Tower` | Kamin (Key), Fenster (Fill blau), Schreibtisch-Kerze | heimelig |
| `BP_LightRig_Classroom` | große Fenster (Key), Kugellampen | hell, lebendig |

Jedes Rig mit Parameter `TimeOfDayInfluence` (Fenster werden nachts dunkel, Kerzen heller).

### 5.2 Cutscene-Licht

Pro Shot im Sequencer: **Key** (motiviert durch Szene, z. B. Kerze), **Fill** (−2 bis −3 EV, Gegenfarbe), **Rim** (Haar/Schulterkante, oft kühl), optional **Eye Light** (kleines Rect Light für Augenglanz). Lichter nur für Charaktere über **Lighting Channels**, damit sie die Umgebung nicht verfälschen.

## 6. VFX (Niagara)

### 6.1 Zauber

Jede Weise hat vier Teile: **Cast** (am Quarz des Stimmstabs + Hand), **Projektil/Strahl**, **Impact**, **Nachglühen** (Decal + Restpartikel 2–4 s). Farbe = Grundtöne der Weise (STORY_BIBLE §3.2). Gemeinsames Leitmotiv: **Resonanzringe** (konzentrische Schallwellen-Ringe aus Distortion-Partikeln) beim Cast.

| Schule | Visuelle Signatur |
|---|---|
| Schlagweisen | scharfe Splitter, Schockwellen-Distortion, Funken |
| Schutzweisen | Glockenform, Hexagon-Muster aus Schallwellen, silberweiß |
| Elementar | echte Elemente (Feuer, Eis, Blitz, Wasser, Stein) mit Klang-Ringen verwoben |
| Wandelweisen | grüne/goldene Ranken-Linien, Partikel, die Formen nachzeichnen |
| Urweisen | **Negativ:** Farbe wird aus der Umgebung gesaugt (lokales Post-Process-Volume), schwarze Kerne mit weißem Rand, Ton fällt weg |

### 6.2 Umgebung

Glühwürmchen, Glimmermotten, Staub im Licht, Blätter im Wind (Wind aus MPC), Regen mit Splashes (Depth-Buffer-Kollision), Schneeflocken, Funken (Schmiede, Fackeln), schwebende Kerzen (Große Halle), magische Partikel entlang der **Adern** (Ley-Linien, Spline-gesteuert), Sandschleier (Kaelschlucht), Gischt (Tiedmar), Nachhall-Trümmerstaub.

### 6.3 Skalierung

Alle Systeme GPU-Partikel, **Effect Types** mit Scalability: Low 25 % Spawnrate, Medium 50 %, High 100 %, Epic/Cinematic 100 % + Licht-Renderer. Max. 2 Partikel-Lichter mit Schatten gleichzeitig.

## 7. Cinematic-Modus

- **Fotomodus:** Pause, freie Kamera (Radius 15 m um den Spieler), Brennweite 14–200 mm, DOF (Blende, Fokus per Klick), Filter (LUT-Auswahl), Rahmen/Letterbox, Posen + Emotes für Spieler und Begleiter, Uhrzeit-Regler (nur Fotomodus).
- **Trailer:** Movie Render Queue + Path Tracer, 4K, 64 Samples Spatial, Warm-up 32 Frames.
- **Skalierbarkeits-Preset „Cinematic“** im Grafikmenü (Stufe 4 in UE-Scalability): volle Lumen-Qualität, Reflexionen in voller Auflösung, Haar-Strands auch im Gameplay.

## 8. Grafikmenü und Presets

| Option | Low | Medium | High | Epic | Cinematic |
|---|---|---|---|---|---|
| Ziel | 60 FPS 1080p (schwache GPU) | 60 FPS 1440p TSR Performance | **60 FPS 1440p auf RTX 5070** | 45–60 FPS 1440p | 30 FPS / Screenshots / Cutscenes |
| GI | Lumen SW, niedrig | Lumen SW | Lumen HW-RT | Lumen HW-RT hoch | Lumen HW-RT max |
| Schatten | VSM niedrig | VSM mittel | VSM hoch | VSM hoch + Contact | max |
| Volumetrischer Nebel | aus | an, grob | an | an, fein | max |
| Haare | Karten (Cards) | Cards | Strands nah | Strands | Strands immer |
| Partikel | 25 % | 50 % | 100 % | 100 % | 100 % + Lichter |
| TSR-Auflösung | 50 % | 58 % | 67 % | 77 % | 100 % |

Einzeloptionen: Motion Blur, Film Grain, Chromatic Aberration, DOF im Gameplay, Bewegungsunschärfe, FOV (70–100), Frame-Generation falls verfügbar (DLSS ist kostenlos per NVIDIA-Plugin, Lizenz prüfen in Phase 11).

## 9. Performance-Budgets (High, 1440p, RTX 5070 12 GB)

| Budget | Wert |
|---|---|
| Frame | 16,6 ms (GPU ≤ 14 ms, Game-Thread ≤ 10 ms) |
| VRAM | ≤ 10 GB (2 GB Reserve für Windows/Overlay) |
| Schattenwerfende Lichter im Sichtfeld | ≤ 8 (Innenräume), ≤ 4 (außen nachts) |
| Partikel-Lichter mit Schatten | ≤ 2 |
| MetaHumans mit LOD 0 gleichzeitig | ≤ 2 (Dialog), ≤ 4 (Cutscene) |
| Cloth-Meshes aktiv | ≤ 12 |
| NPCs voll animiert (nicht Mass) | ≤ 40 |
| Draw Calls (Nicht-Nanite) | ≤ 3000 |
| Aktive Niagara-Partikel | ≤ 200.000 GPU |

Profiling mit **Unreal Insights** nach jedem Meilenstein (`-trace=cpu,gpu,frame,memory`), Ergebnis-Tabelle in `PROGRESS.md`.

## 10. Moodboards und Concept Art

- Pro Region 8 Bilder (Gesamtansicht, Detail, Material, Licht, Wetter, Nacht, Kreatur/Figur in Szene, Farbschlüssel). Prompts: `Scripts/ComfyUI/prompts/moodboards.json`.
- Figuren: Porträt, Ganzkörper, Ausdrucks-Sheet (6 Emotionen). Prompts: `characters.json`.
- Kreaturen: je 2 Ansichten. Prompts: `creatures.json`.
- Türme: Wappen + Uniform. Prompts: `houses.json`.
- Modell: **FLUX.1-schnell** (Apache 2.0). Fallback SDXL. Kein FLUX.1-dev (nicht kommerziell).
- Ablage: `art/concepts/<kategorie>/<id>/` (Bilder nicht in Git, nur `manifest.json` mit Prompt/Seed/Modell). Alle Bilder gelten als **KI-generiert** und sind Referenz, keine Spiel-Assets, außer wir übernehmen sie bewusst (dann CREDITS-Eintrag).
- Bewertung: Du wählst pro Region die 2 besten Bilder als „Key-Frames“ und legst sie als `KEY_*.png` ab. Die Key-Frames sind dann der Maßstab für die Benchmark-Szene in Phase 4.
