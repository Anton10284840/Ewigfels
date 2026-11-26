# ANIMATION BIBLE – Nocturna

Stand: 26.11.2026 · Phase 1 · Version 0.1
Umsetzung ab Phase 3. Checklisten unten werden beim Import abgehakt (`[x]`) und mit Quelle versehen: **GAS** = Game Animation Sample, **MX** = Mixamo, **MC** = eigene MoCap (FreeMoCap), **KF** = Keyframe (Control Rig/Blender), **MH** = MetaHuman Animator.

---

## 1. Grundlagen

| Thema | Festlegung |
|---|---|
| Skelett | UE5-Mannequin (`SK_Mannequin`, Manny/Quinn). MetaHumans nutzen ab 5.6 ein kompatibles Körperskelett, Retarget nur wo nötig. |
| Retargeting | Alle externen Animationen per **IK Retargeter** auf das Mannequin. Ein IK Rig pro Quelle: `IKR_Mixamo`, `IKR_FreeMoCap`, `IKR_MetaHuman`. Retargeter: `RTG_Mixamo_to_Manny`, `RTG_FreeMoCap_to_Manny`. |
| Locomotion | **Motion Matching** (Plugin *Pose Search*) mit dem Datensatz aus dem **Game Animation Sample** (kostenlos, Epic). |
| Bildrate | Animationen mit 30 FPS importieren, MoCap mit 60 FPS aufnehmen und auf 30 reduzieren. |
| Root Motion | An für Ausweichen, Angriffe mit Schritt, Interaktionen, Klettern. Aus für Locomotion (Motion Matching steuert per Trajektorie). |
| Benennung | `A_<Kategorie>_<Aktion>_<Variante>`, z. B. `A_Magic_Cast_Heavy_Start`, `A_Loco_Idle_Tired_01`. Montages: `AM_…`, Blend Spaces: `BS_…`, Pose-Search-Datenbanken: `PSD_…`. |
| Ordner | `Content/Animations/<Locomotion|Magic|Combat|Interaction|Facial|Cinematics>/` |

## 2. Animation Blueprint – Struktur

```
ABP_EwigfelsCharacter (Linked Anim Layers)
 ├─ 1. Locomotion        Motion Matching (PSD_Loco_*), Zustände: Boden, Luft, Schwimmen, Klettern, Gleiten, Reiten
 ├─ 2. Overlay-Layer     Upper-Body-Blend ab spine_01: Zaubern (Montages), Interaktion, Hand- und Fingerposen
 ├─ 3. Additive          Atmung (Ruhe/Erschöpft), Treffer-Additive, Blick-Additive, Nachhall-Zittern (bei hoher Belastung)
 └─ 4. IK / Procedural   Foot IK (Control Rig, Bodenanpassung), Hand-IK (Türen, Hebel, Leitern), Look-At (Kopf + Augen),
                         Hand-Zielen (Arm-IK, Handfläche/Finger zum Ziel), Orientation Warping, Stride Warping
```

Regeln:
- **Linked Anim Layers** pro Zustand (Kampfstil wechselbar ohne neues AnimBP).
- Logik im **C++-AnimInstance** (`UEwigfelsAnimInstance`): Werte berechnen, AnimBP nur verdrahten. Thread-safe Update-Funktionen nutzen.
- Foot IK und Look-At als **Control Rig** (`CR_FootIK`, `CR_LookAt`), nicht im AnimGraph nachgebaut.
- Kleidung: Leader Pose Component, keine eigenen AnimBPs für Outfit-Teile.

## 3. Animationssets – Checkliste

### 3.1 Locomotion

- [ ] Idle: Neutral, Erschöpft, Kalt (zittern), Wachsam (Kampf), Gelangweilt (nach 20 s) — GAS + MC
- [ ] Gehen / Joggen / Sprinten inkl. Starts, Stops, Pivots, Drehungen auf der Stelle, Rückwärts, Strafe — GAS
- [ ] Schleichen (Gehen, Idle, Übergänge) — GAS/MX
- [ ] Springen (Stand, Lauf), Fallen (Loop), Landen weich/hart, Rolle nach hohem Fall — GAS
- [ ] Ausweichrolle (4 Richtungen) + kurzer Ausweichschritt (4 Richtungen) — GAS/MX
- [ ] Schwimmen Oberfläche (Idle, Vorwärts, Schnell), Tauchen (Idle, Vorwärts, Auf-/Abtauchen), Ein-/Ausstieg — MX + KF
- [ ] Klettern: Kante greifen, Hangeln links/rechts, Hochziehen, Ranken-Klettern (4 Richtungen), Wand-Sprung — GAS (Traversal) + MX
- [ ] Balancieren (Balken: Gehen, Wackeln, Fallen) — MX/MC
- [ ] Leiter (Ein-, Aufsteigen, Absteigen, Ausstieg oben/unten) — MX
- [ ] Gleiten mit Umhang (Start, Loop, Kurven, Landung) — KF + MC (Armhaltung)

### 3.2 Zaubern

Je Weise: **Cast-Start → Halten (Loop, für kanalisierte) → Release**. Pro Schule eine Grundgeste, pro Weise eine Variante. Alle als Upper-Body-Montages, damit sie während der Bewegung funktionieren.

- [ ] Grundhaltungen Hände: Ruhe, Bereit (Finger leicht gespreizt, Funken knistern), Zielen (Aim Offset 9 Posen, Hand nach vorne) — KF
- [ ] **Finger-Posen-Bibliothek** (Pose Asset `PA_Hand_Chords`): 7 Grundton-Griffe pro Hand + Faust (Urton) + Schnippen. Zauber blenden zwischen diesen Posen — KF
- [ ] Funkenstoß: 3er-Kette (schnell), Abschluss-Rückstoß — MC
- [ ] Schlagweisen: schnelle Variante + schwere Variante (Hammerton, Splitterchor, Ziehklang, Zerrton-Loop) — MC
- [ ] Schutzweisen: Glockenschild (Hoch, Halten, Reinklang-Konter), Spiegelklang, Haltelied (Loop), Atemwende (Selbst), Klangwand (Boden-Geste) — MC
- [ ] Elementar: Glut, Frost, Sturm (Loop), Flut, Erde (Stampfen) — MC
- [ ] Wandelweisen: Wiederklang (Loop, präzise Fingerbewegung), Leichtklang (Heben, Halten, Werfen), Wachsklang, Formwandel, Schattenweise (Einhüllen) — MC
- [ ] Urweisen: Verstummen (Implosion), Nachhallschritt (Start/Ende), Seelenecho (Knien, Hand auf Boden), Entklang, Grundton (Ganzkörper, 3 s) — MC (Signature, du nimmst sie auf)
- [ ] Combo-Übergänge: gleiche Schule (kurz), andere Schule (Wechsel-Geste) — KF/MC
- [ ] Zaubern im Sprung/Gleiten (Upper-Body-Blend auf Luft-Pose) — Blend
- [ ] Zaubern auf dem Reittier (eine Hand, eingeschränkt) — KF
- [ ] Schnippen (Funken, Licht anmachen, Gruß) und Fingerspiel-Idle (Finger trommeln, kleine Funken) — MC + KF

**Signature-Stil:** Unsere Weisen sehen aus wie **Akkordeon spielen in der Luft**: Die Hände greifen Akkorde (Finger gespreizt, Druck wie auf Tasten), die Arme ziehen und drücken wie beim Balg, Licht bricht aus den Fingerspitzen. Leichte Weisen: eine Hand. Schwere: beide Hände ziehen auseinander und schlagen zusammen. Schwere Weisen: Gewicht aus der Hüfte, Atemzug vorher (sichtbar über Brust-Additive).

### 3.3 Kampf

- [ ] Block/Schild Halten (Loop), Block-Treffer leicht/schwer, Schild bricht — MX/MC
- [ ] Parieren (Reinklang) mit Konter (2 Varianten) — MC
- [ ] Treffer-Reaktionen: vorne, hinten, links, rechts × leicht/schwer (8) — GAS/MX
- [ ] Knockdown (vorne/hinten), Aufstehen (2), Rückstoß-Stolpern — MX
- [ ] Tod (3 Varianten, Spieler: Zusammensinken statt Ragdoll-Unfall) — MX
- [ ] Finisher (5 kurze, schulweise: z. B. Hammerton aus nächster Nähe) — MC
- [ ] Lock-On-Strafe (bereits in Locomotion über Motion Matching) — GAS
- [ ] Gegner: humanoide Sets (Stille Hand, Wilderer, Wachen) über dasselbe Mannequin + eigene Angriffe — MX

### 3.4 Interaktion

- [ ] Tür öffnen (Ziehen, Drücken, schwere Doppeltür) — MX + Hand-IK
- [ ] Truhe öffnen, Aufheben (Boden, Tischhöhe), Hebel, Kurbel — MX + IK
- [ ] Trank trinken, Buch lesen (Stehen + Sitzen), Kessel umrühren (Loop), Pflanze ernten — MC
- [ ] Sitzen (Stuhl, Bank, Boden, Fensterbank) inkl. Hinsetzen/Aufstehen — MX/GAS (Smart Objects)
- [ ] Anlehnen (Wand, Geländer) — MX
- [ ] Kreatur füttern, streicheln (klein/groß) — MC

### 3.5 Reittier (Spieler-Seite)

- [ ] Aufsteigen (links), Absteigen (Boden, im Flug abspringen) — KF
- [ ] Reitposen: Boden (Gehen, Laufen), Flug (Gleiten, Flügelschlag-Mitbewegung), Kurven links/rechts (Neigung), Boost (geduckt), Sturzflug — KF (Additive auf Basispose)

### 3.6 Emotes & Social (Dialoge und Fotomodus)

- [ ] Winken, Lachen, Nicken, Kopfschütteln, Verbeugen, Jubeln — MX/MC
- [ ] Schulterzucken, Arme verschränken, Nachdenken (Kinn), Seufzen, Erschrecken, Zeigen — MX/MC
- [ ] Akkordeon/Mundharmonika spielen (Lagerfeuer-Szene, Ambros) — MC

### 3.7 NPC-Alltag

- [ ] Unterrichten (an der Tafel, Weise mit den Händen vorführen), Schreiben, Lesen — MC/MX
- [ ] Essen (Sitzen am Tisch), Trinken, Unterhalten (3 Gesprächs-Loops) — MX
- [ ] Fegen, Holz hacken, Verkaufen (Marktstand: Anpreisen, Ware zeigen), Netze flicken (Tiedmar) — MX/MC
- [ ] Schüler: Rennen zum Unterricht, auf der Treppe sitzen, Zauber üben (fehlgeschlagen) — MC

### 3.8 Figuren-Idles (aus CHARACTER_BIBLE)

- [ ] Edda: Finger-Takt am Unterarm · Lio: Fersenwippen + Mundharmonika · Aurelia: Handschuh straffen · Corvin: Glas umrühren · Grell: Fingerknöchel knacken · Saphira: Hocken + Summen · Varga: Siegelring drehen · Dirigent: Dirigier-Hand · Nym: Schweben + Hände · Ambros: Quarz polieren · Ruprecht: Hände auf dem Rücken · Nell: Notizbuch · Fink: verkehrt auf dem Stuhl · Mara: Taktstock-Tippen · Quendel: auf Wurzelstab wippen — alle MC

## 4. Eigene Animationen aufnehmen (FreeMoCap, Webcam)

FreeMoCap (AGPL-3.0, nur das Tool; die Animationen gehören dir) ist in SETUP_WINDOWS installiert.

### 4.1 Raum und Kleidung

- Freie Fläche mind. 2,5 × 2,5 m, Decke hoch genug für Arme über Kopf.
- Gleichmäßiges, helles Licht von vorne/seitlich, kein Gegenlicht vom Fenster.
- **Eng anliegende, einfarbige Kleidung**, die sich vom Hintergrund abhebt (z. B. dunkle Sportkleidung vor heller Wand). Keine weiten Ärmel. Schuhe aus oder flache Schuhe.
- Kameras: Mit **einer** Webcam geht es (Mono, 2D-Schätzung → weniger genau in der Tiefe). Besser: **2–3 Kameras** (Webcam + Pixel 10 Pro per USB-Webcam-Modus + ggf. Laptop-Kamera), im Halbkreis 90° auseinander, Hüfthöhe bis Brusthöhe, 2–3 m Abstand.

### 4.2 Aufnahme

1. FreeMoCap starten (`freemocap` in der venv, siehe SETUP).
2. Bei mehreren Kameras: **Kalibrierung** mit dem ChArUco-Board (in FreeMoCap ausdrucken, auf Pappe kleben, langsam durch den Raum bewegen).
3. Pro Aufnahme: **T-Pose 2 s** am Anfang (für Retargeting), dann die Bewegung, dann wieder T-Pose. Lieber viele kurze Takes (5–15 s) als lange.
4. Benennung beim Speichern: `MC_<Kategorie>_<Aktion>_<Take>`, z. B. `MC_Magic_Hammerton_03`.
5. Zauber: jede Weise 3–5 Takes, ohne Requisite. Webcam-MoCap erfasst Finger nur grob: Arme und Körper aus der Aufnahme, **Finger danach per Pose-Asset/Control Rig** setzen.

### 4.3 Export und Import

1. FreeMoCap → *Export* → **Blender** (das Add-on erstellt ein Rig mit Animation) → in Blender aufräumen: Jitter mit *Graph Editor → Key → Smooth* (bzw. „Smooth Keys“) auf Hände/Füße, Füße am Boden fixieren.
2. Blender → *File → Export → FBX*: Armature + Animation, *Apply Transform*, Skalierung 1,0, `Forward: -Y`, `Up: Z`, *Add Leaf Bones* aus.
3. UE: Import nach `Content/Animations/Raw/MoCap/` mit eigenem Skelett `SK_FreeMoCap`.
4. Retargeting: `RTG_FreeMoCap_to_Manny` → Rechtsklick auf Animationen → *Retarget Animations* → Ziel `Content/Animations/<Kategorie>/`.
5. Säubern in UE: **Control Rig im Sequencer** (Animation in Level Sequence ziehen → *Bake to Control Rig* → Hände korrigieren, Finger-Posen aus `PA_Hand_Chords` einsetzen) → *Bake Animation Sequence*.
6. Notify-Events setzen: `Cast_Release`, `Footstep_L/R`, `VFX_Spawn`, `Sound_Whoosh`.

**Ehrliche Grenze:** Webcam-MoCap ist gut für Oberkörper-Gesten und Emotes, schwächer bei schnellen Drehungen, Sprüngen und Bodenkontakt. Dafür nehmen wir GAS/Mixamo und nutzen MC für das, was das Spiel einzigartig macht: Zaubergesten und Persönlichkeit.

## 5. Gesichtsanimation und Dialoge

### 5.1 Dialog-Kamera

- Automatische Kameras pro Dialogzeile: **Schuss-Gegenschuss** über die Schulter (35–50 mm), **Close-up** bei Emotion ≥ stark (85 mm), **Totale** bei Szenenbeginn (24 mm). 180°-Regel einhalten (Achse zwischen den Sprechern).
- DOF-Fokus automatisch auf das Auge des Sprechenden (Socket `FACIAL_L_Eye`), Blende f/2,0.
- Kamera-Logik in C++ (`UEwigfelsDialogueCameraComponent`, Phase 3), Kamera-Hinweise optional pro Zeile im JSON (`"camera": "closeup"`).

### 5.2 Lip-Sync

1. **Primär:** MetaHuman Animator **Audio-Driven** (Audio → Gesichtsanimation, läuft lokal in UE). Ergebnis als Animation pro Dialogzeile, gebacken.
2. **Performance-Szenen:** Webcam- bzw. Pixel-Mono-Video → MetaHuman Animator (siehe SETUP 8a).
3. **Fallback** für Massen-Dialoge (NPC-Smalltalk): Viseme-basiertes Lip-Sync zur Laufzeit aus Phonemen (Phase 3: Lösung aus Bordmitteln wählen, z. B. Audio-Driven-Batch über Python statt Echtzeit).

### 5.3 Emotions-Tags

Im Dialog-JSON pro Zeile: `"emotion": "angry"`, Intensität `"intensity": 0.0–1.0`.

| Tag | Gesicht (Pose-Asset/Additive) | Körper | Kamera |
|---|---|---|---|
| `neutral` | Basis | Idle der Figur | Schuss-Gegenschuss |
| `happy` | Lächeln, Augen schmal | offene Haltung | Medium |
| `sad` | Brauen innen hoch, Mundwinkel runter | Schultern sinken | Close-up |
| `angry` | Brauen runter, Kiefer fest | Gewicht nach vorne | leichte Untersicht |
| `afraid` | Augen weit, Lippen gespannt | Rückzug, Arme nah | wackelige Hand-Kamera (leicht) |
| `surprised` | Brauen hoch, Mund offen | kurzer Schritt zurück | schneller Zoom (2 %) |
| `disgusted` | Nase rümpfen, Oberlippe hoch | Kopf weg | Medium |
| `thoughtful` | Blick zur Seite, Mund leicht verzogen | Hand ans Kinn | Profil |

Die 6 Emotionen für die Ausdrucks-Sheets der Concept Art: **happy, sad, angry, afraid, surprised, disgusted**.

### 5.4 Lebendige Gesichter

- **Blinzeln** prozedural (alle 2–6 s, zufällig, doppelt bei Nervosität), **Sakkaden** (kleine Augensprünge alle 0,5–2 s), **Blickziel** (Sprecher, dann kurz weg).
- **Mikro-Mimik:** Additive-Layer mit leichtem Noise auf Brauen, Mundwinkeln, Nasenflügeln (Amplitude 2–5 %).
- **Atmung:** Brust- und Schulter-Additive, schneller bei `afraid`/nach Kampf.

## 6. Kreaturen-Animation

- Basis-Rigs von **Fab (gratis)** für Vierbeiner, Vögel, Fische. Anpassung in Blender, IK und Prozedurales über Control Rig.
- Keyframe in Blender bzw. **UE Control Rig + Sequencer** (Cascadeur Free ist nicht kommerziell nutzbar, siehe KI_LIZENZEN).
- Pro Kreatur minimal: Idle, Gehen, Laufen, Angriff (1–2), Treffer, Tod, Spezial. Bosse: eigenes Set je Phase.

### 6.1 Reittiere – Animationssets

Pro Reittier (CHARACTER_BIBLE §7) mindestens: Idle (2), Gehen, Laufen, Sprint, Drehen, Springen, Treffer, Spezialfähigkeit, Freude, Erschöpfung, Aufsteigen-Pose. Flieger zusätzlich das Flugset unten, Wasser-Reittier: Schwimmen, Tauchen, Wellensurfen; Kletter-Reittier: Wandlaufen (Control-Rig-Fußplatzierung).

#### Sturmschwinge – Flug-Animationsset

- [ ] Boden: Idle (Putzen, Umschauen), Gehen, Traben, Laufen, Drehen, Ducken (Aufsteigen)
- [ ] Abheben (aus Stand, aus Lauf), Landen (sanft, hart, Steilwand-Greifen)
- [ ] Flügelschlag (Loop, langsam/schnell), Gleiten (Loop), Kurven links/rechts (Additive Neigung), Steigen, Sinken
- [ ] Sturzflug (Einleiten, Loop, Abfangen), Boost (3 kräftige Schläge)
- [ ] Schweben auf der Stelle, Wenden in der Luft
- [ ] Reaktionen: Treffer im Flug, Erschöpfung (Ausdauer leer), Freude (Federn schütteln), Angst
- Flügel: eigene Kette mit **Control Rig** (Schlagzyklus prozedural aus Frequenz und Amplitude, Federn spreizen per Additive), Schwanzfächer per Spring-Physik (`AnimDynamics`).

## 7. Cutscenes

- Ziel: **60–80 Cutscenes** (davon ca. 40 Pflicht in der Main Quest, siehe 🎬 in STORY_BIBLE §8.1).
- Pro Cutscene eine Datei `docs/cutscenes/CS_<MissionID>_<Nr>.md` mit Shot-Liste:

| Shot | Einstellung | Brennweite | Bewegung | Licht | Dialog | Animation | Dauer |
|---|---|---|---|---|---|---|---|
| 010 | Totale | 24 mm | langsame Kranfahrt runter | goldene Stunde, Rim von links | – | Boot gleitet | 6 s |
| 020 | Over-Shoulder | 50 mm | statisch | Key Laterne, Fill kühl | Lio: „…“ | Idle Lio, MH-Gesicht | 4 s |

- **Cine Camera** mit echten Objektivwerten: 24 / 35 / 50 / 85 mm, Sensor Super 35, Blende je Shot. Letterbox 2,39:1 optional (Menüoption).
- Übergänge Gameplay → Cutscene → Gameplay ohne Ladezeit: Sequencer im selben Level, Spieler-Pose am Ende angleichen (Blend 0,5 s), Kamera-Blend zurück zur Spielkamera.
