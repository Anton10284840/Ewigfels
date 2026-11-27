# WEISEN – das Zaubersystem von Nocturna

Stand: 26.11.2026 · Version 1.0 · gehört zu [GDD §4](GDD.md#4-magiesystem-spielmechanik) und [STORY_BIBLE §3](STORY_BIBLE.md#3-magiesystem-resonanz)
Vollständige Liste aller Weisen (automatisch erzeugt): [WEISEN_KATALOG.md](WEISEN_KATALOG.md) · Daten: `Content/Data/Spells/Katalog/` · Prüfen: `python Scripts/Tools/validate_spells.py`

---

## 0. Entscheidung E10 – Hunderte Weisen ✅ Option C (Nutzer, 26.11.2026)

Wunsch: *Hunderte Zaubersprüche, die man im Unterricht oder bei Meistern lernt. Es müssen keine Sprüche sein, kreativ sein.*

| Option | Kern | Vorteil | Nachteil |
|---|---|---|---|
| A: Nur Baukasten | Grundton × Wirkart × Ring ergibt automatisch 300+ Weisen | wenig Animationsarbeit | viele Weisen fühlen sich gleich an |
| B: Alle handgemacht | 200+ Weisen mit eigener Geste und eigenem Effekt | jede Weise besonders | allein nicht machbar (Jahre Animation und VFX) |
| **C: Mischung** ⭐ gewählt | **60 handgemachte Kernweisen** für Kampf, Story und Rätsel + **358 Alltagsweisen** aus dem Baukasten + **12 Koop-Weisen** | wichtige Weisen sehen besonders aus, trotzdem Hunderte zum Sammeln; Animation bleibt machbar | Alltagsweisen brauchen gute Ideen, sonst wirken sie wie Füllmaterial (darum hat jede einen eigenen Witz oder Nutzen) |

**Ergebnis: 430 Weisen.** 319 davon sind schon in Akt 1 erreichbar (Akademie, Ilsensee, Raunwald, Kesselbrunn), der Rest kommt mit den neuen Regionen dazu.

## 1. Die drei Arten von Weisen

| Art | Anzahl | Wofür | Wie sie aussieht | Wo lernen |
|---|---|---|---|---|
| **Kernweisen** | 60 (Schlag 12, Schutz 12, Elementar 14, Wandel 14, Ur 8) | Kampf, Story, große Rätsel | eigene Geste (Motion Capture), eigener Niagara-Effekt, eigener Klang, Akkord aus 3 Grundtönen | Unterricht, Meister, Story, Nym |
| **Alltagsweisen** | 358 in 15 Heften | Alltag, Spaß, Streiche, Erkundung, kleine Rätsel, Personalisierung | Baukasten: 1 Grundton (Farbe + Klang) × 1 Wirkart (geteilte Geste) | überall: Unterricht, Meister, Mitschüler, Tiere, Feste, Liedblätter, selbst ausprobieren |
| **Koop-Weisen** | 12 (8 Duette, 2 Dreiklänge, 2 Vierklänge) | zu zweit bis zu viert stärker, im Einzelspiel mit Begleiter | eigene Gesten, teils aus Kerngesten zusammengesetzt | Unterricht, Meister |

## 2. Zaubern ohne Sprüche: die Wirkarten

Niemand muss in Nocturna Zauberworte rufen. Magie ist Klang aus den Fingern (STORY_BIBLE §3.3), und Klang kann man auf viele Arten machen. Jede **Wirkart** ist eine Körperbewegung mit einer festen Bedeutung, die jeder Spieler nach kurzer Zeit versteht.

| Wirkart | Bewegung | Wirkt auf | Gefühl | Steuerung (Alltagsrad) |
|---|---|---|---|---|
| **Schnippen** | Finger schnippen | einen Punkt, schnell | Funke, kleiner Ruck | antippen |
| **Tippen** | Fingerspitze berührt ein Ding | das berührte Ding | verändern, verzaubern | antippen am Objekt |
| **Streichen** | Hand gleitet durch die Luft oder über eine Fläche | eine Linie, eine Fläche | malen, glätten, lenken | halten + Richtung |
| **Kreisen** | Finger kreisen | die Umgebung um dich | Wirbel, Zone | halten |
| **Pusten** | über die Fingerspitzen pusten | eine Wolke vor dir | Duft, Wind, Blasen, Funken | antippen |
| **Summen** | Hand auf der Brust, leises Summen | dich selbst oder einen Begleiter | Dauerwirkung, Gefühl | halten |
| **Pfeifen** | Pfiff, zwei Finger am Mund | in die Ferne | rufen, suchen, wecken | antippen |
| *Sonder:* Klatschen, Stampfen, Singen, Spielen (Instrument), Tanzen | | | nur für wenige besondere Weisen (Feste, Musik, Meister) | Minispiel oder Rhythmus |

Mit **Spielen** ist echtes Musizieren gemeint: Akkordeon, Mundharmonika, Flöte. Jupp Quetschkommode in Kesselbrunn bringt dir bei, dein Akkordeon zum Zaubern zu nutzen.

## 3. Der Baukasten und das Probieren

### 3.1 Grundformen: 7 × 7 = 49 Weisen, die man selbst findet

Jede Kombination aus einem der sieben **Grundtöne** (Glut, Firn, Strom, Quell, Grund, Wachs, Hall) und einer der sieben **Grund-Wirkarten** ist eine eigene kleine Weise, die **Grundform**. Beispiele: Glut × Schnippen = *Funkenschnipp* (Kerze an), Quell × Pusten = *Seifenblasen*, Strom × Pfeifen = *Echopfiff* (zeigt Hohlräume, also Geheimgänge). Die ganze Tabelle steht in [WEISEN_KATALOG §3](WEISEN_KATALOG.md#3-baukasten-grundformen-grundton--wirkart).

### 3.2 Probier-Modus

Ab M103 (erste Klangkunde-Stunde) kann der Spieler jederzeit **frei probieren**: Grundton wählen, Wirkart wählen, los. Trifft die Kombination eine noch unbekannte Grundform, gibt es einen kleinen Entdecker-Moment (Glöckchenklang, Seite fliegt ins Weisenheft, Notiz „Selbst entdeckt!“). Trifft sie nichts, passiert trotzdem etwas Kleines (ein paar Funken in der Grundtonfarbe), damit Probieren nie langweilig ist.

**Hinweise** auf Grundformen stecken überall: Kinderreime in Dörfern, Kritzeleien in Schulbänken, Rätselschreine, und die Alltagsweise **Stimmgabel** (Edda, M103) zeigt bei jedem Ding den Grundton, auf den es reagiert.

### 3.3 Varianten: Meister verfeinern die Grundformen

Alle anderen Alltagsweisen sind **Varianten**: eine Grundform, die jemand verfeinert hat. Hedda Krume in Kesselbrunn macht aus *Wachs × Summen* das *Hefelied*, Lio macht aus *Quell × Schnippen* den *Quakschluckauf*. Man lernt Varianten nur bei jemandem, nicht durch Probieren. So hat jeder Meister etwas, das nur er kann, und Entdecken bleibt trotzdem möglich.

## 4. Lernwege

| Lernweg | Anzahl | Wie es sich anfühlt |
|---|---|---|
| **Unterricht** | 101 | Stundenplan in der Akademie. Jede Stunde ist ein kurzes Minispiel (GDD §7.2). Wer sie besteht, lernt eine Weise. Pro Fach 7 Alltagsweisen + Kernweisen. Wer gut ist, bekommt Bonusaufgaben („Lass drei Kerzen gleichzeitig schweben“). |
| **Meister in der Welt** | 167 | 36 Meisterinnen und Meister: Bäckerin, Straßenmusikant, Kapitänin, Spiegelhexe, Puppenmacher, Drachenfliegerin. Jeder will vorher etwas (Brötchen ausliefern, eine Melodie nachspielen, ein Gruseldorf befreien). Danach lehrt er sein **Liederheft** mit 2–8 Weisen. |
| **Mitschüler** | 47 | Lio (Streiche, Mundharmonika), Aurelia (Stil, Duelle), Ruven und der Geheimclub „Die Schiefen Töne“, Panki (Basteln), Mirela (Echos), Tilda (Tiere). Freundschaft schaltet frei. |
| **Selbst entdecken** | 49 | Probier-Modus (§3.2). |
| **Kael und Nym** | 19 | Rätselschreine, Kael-Echos, Träume von Nym. Uralt, rätselhaft, manchmal leicht unheimlich. |
| **Feste** | 16 | nur während Ilsenlicht, Firnnacht, Wachsfest, Doppelnacht oder Dorffesten. Wer ein Fest verpasst, lernt die Weise im nächsten Jahr. |
| **Kreaturen** | 14 | Ein Tier, das dir vertraut, singt dir sein Lied vor (Laternenkrähe, Moosling, Stelzreiher, Kristallwal). |
| **Liedblätter** | 8 (darunter die Kernweise Schattenweise) | versteckte Notenblätter und Zettel in der Bibliothek, der Stummen Kammer und in der Welt. |
| **Story / Start** | 9 | Funkenstoß, Glockenschild, Leichtklang und die Urweisen. |

Freischalten im Code: Weltstatus-Flag `flag:Weise_<ID>=1` (z. B. `Weise_K39`), damit Quests und Dialoge Weisen vergeben können (docs/QUEST_FORMAT.md).

## 5. Im Spiel benutzen

- **Kernweisen:** wie bisher 4 Weisen auf dem Schnellwahl-Ring, bis zu 4 Ringe (16 Plätze), Wahlrad (GDD §8).
- **Alltagsrad** (Taste `G` halten / Gamepad Steuerkreuz unten halten): 8 Lieblings-Alltagsweisen, frei belegbar.
- **Kontext-Vorschlag:** Schaut man auf ein Ding und hält `E`, schlägt das Spiel bis zu 3 bekannte Alltagsweisen vor, die darauf wirken (Kerze → Funkenschnipp, Tropfenschnipp, Farbtausch). So geht bei Hunderten Weisen nichts verloren.
- **Weisenheft** (Menü): alle Weisen nach Heft, Grundton oder Lehrer sortierbar, mit kleiner Zeichnung, Akkord-Noten, wer sie gelehrt hat, und einem Feld „Was ich damit gemacht habe“ (Statistik: „47 Kerzen angezündet“). Fehlende Weisen sind als Umriss mit Hinweis sichtbar („Eine Bäckerin in Kesselbrunn singt beim Kneten …“).
- **Sammelziele:** Turm-Abzeichen für 50, 100, 200, 300, 430 Weisen; jedes Heft voll = Titel und Kleidungsstück (z. B. „Streichkönig“ + Schellen-Mütze).

## 6. Koop-Weisen (2–4 Spieler, Koop kommt später)

Koop wird vorbereitet, aber noch nicht gebaut. Die Koop-Weisen sind so gestaltet, dass sie **auch allein funktionieren**: Im Einzelspiel übernimmt ein Begleiter (Lio, Aurelia, später das Reittier) den zweiten Part.

| Gruppe | Weisen | Idee |
|---|---|---|
| **Duette** (2) | Zwiegesang, Kanonfeuer, Brückenlied, Schleuderwurf, Spiegelpaar, Tandemgleiten, Wechselatem, Rufklang | Zwei Hälften ergeben ein Ganzes: Kuppel aus zwei Schilden, Lichtbrücke über eine Schlucht, Mitspieler weit werfen |
| **Dreiklang** (3) | Dreiklang, Kreiswacht | Jeder spielt einen Grundton, der Akkord bestimmt die Wirkung: ein eigener Baukasten für Gruppen (84 Akkorde, 12 benannte Meisterakkorde) |
| **Vierklang** (4) | Vierklang, Glockenchor | ein großer Chor-Moment pro Kampf, Akademie-Glocken läuten |

Dazu kommen Kernweisen mit Mitspieler-Bonus: *Schutzchor* (Schild auf Mitspieler), *Zielfunke* (Ziel für alle markieren), *Haltelied + Sturmweise* zweier Spieler ergeben einen Kettenstrom. Technik: Das Zaubersystem ist server-autoritativ gebaut (GDD E5/E11), Koop-Weisen werden normale Weisen mit „Partner-Fenster“ (2 s, in denen der zweite Spieler einsteigen muss).

## 7. Stimmringe und Alltagsweisen

- Die Quarzfarbe der Ringe färbt **jede** Weise, auch Alltagsweisen (Personalisierung, GDD §13). *Funkenfarbe* (Ambros) erlaubt eine andere Farbe nur für Funken.
- Ein Ring mit Grundton-Bonus macht Alltagsweisen dieses Tons **„laut“**: größere Reichweite, längere Dauer (Seifenblasen werden riesig, Blütenkreis wird ein Blumenfeld). Das ist der dritte Teil des Baukastens, ohne neue Animation.

## 8. Kreativ sein: Alltagsweisen und die Welt

Alltagsweisen machen keinen Kampfschaden. Aber sie setzen dieselben **Zustände** wie Kernweisen, und die Welt reagiert darauf. Das ist der Teil, bei dem Spieler selbst kreativ werden:

| Zustand | gesetzt z. B. von | Folge |
|---|---|---|
| Brennend / Licht | Funkenschnipp, Kaminpfiff | zündet Öl, Lunten, Kerzen; Stummlinge scheuen Licht |
| Nass | Tropfenschnipp, Wolkenschäfchen | löscht Feuer, macht Gegner anfällig für Sturmweise |
| Glatt | Rutschsohle, Frosttupfer | Gegner und Wachen rutschen aus |
| Laut | Türklopfer, Kieselschnipp, Echoköder | lockt Gegner und Ottilie weg |
| Leise | Stillkreis, Schweigeschritt | Schleichen |
| Abgelenkt | Kitzelklang, Kohlwolke, Tanzbeine | schwache Gegner und NPCs 2–5 s abgelenkt (Bosse immun) |

Kosten: 2–6 Atem, keine Abklingzeit. Regel gegen Missbrauch: dieselbe Ablenkung wirkt auf denselben Gegner nur einmal pro Kampf.

## 9. Ehrlich: Was das für Animation, VFX und Ton bedeutet

| Teil | Menge | Arbeit | Wer |
|---|---|---|---|
| Kerngesten (Motion Capture) | 60 + 12 Koop | je Cast-Start, ggf. Loop, Release; ca. 3 Aufnahmen pro Weise | du (FreeMoCap, ANIMATION_BIBLE §4), ich bereite Listen und Retargeting vor |
| Wirkart-Gesten | 7 + 5 Sonder | je eine Montage, einhändig im Gehen, gespiegelt | du (MC), einmalig |
| Kern-VFX | 60 Niagara-Systeme | jedes eigen, aber aus einem Baukasten von ca. 15 Modulen (Funken, Strahl, Welle, Splitter, Ranken …) | ich (Parameter, Python), du prüfst im Editor |
| Alltags-VFX | **1** Niagara-System mit Parametern | 7 Grundtonfarben × 12 Wirkart-Formen | ich |
| Ton | 7 Grundton-Klangfarben × 12 Wirkarten + 60 Kernklänge | Samples (CC0) + **deine Akkordeon-Aufnahmen** pro Grundton | du spielst, ich schneide |
| Wirkungen am Ziel | 9 Systemtypen | Material-Farbe, Physik-Impuls, Licht, Ton, UI: allgemein, **einmal** gebaut. *Objekt*, *KI* und *Welt* brauchen pro Zielart eine kleine Reaktion (Stuhl tanzt, Katze kommt) | ich (C++/Blueprint-Interface) |

**Wo die echte Arbeit steckt:** nicht in den Gesten, sondern in den Reaktionen der Welt (Kategorie *objekt*, *ki*, *welt*, zusammen 115 Alltagsweisen). Darum bauen wir in Scheiben:

| Phase | Weisen | Zahl |
|---|---|---|
| 3 (Charakter, Kampf) | Start-Kernweisen + Probier-Modus-Technik | ca. 10 |
| 5 (Vertical Slice Akt 1) | alle Akt-1-Kernweisen, 49 Grundformen, Unterrichtsweisen, Streiche, Haus/Turm, Ilsenlicht | ca. 190 |
| 6–10 (Akte 2–5) | Rest pro Region mit den Meistern | ca. 230 |

Wenn Zeit knapp wird, streichen wir Varianten in späten Heften, nie Grundformen oder Kernweisen. Das Spiel bleibt vollständig.

## 10. Technik (Phase 3)

- **Eigenes Zaubersystem** (E5, kein GAS): Kampfwerte jeder Kernweise stehen in `Content/Data/Spells/*.json` mit `catalog_id` (Format [SPELL_FORMAT.md](SPELL_FORMAT.md), Prüfung `validate_combat.py` inkl. Abgleich von Akkord und Name mit dem Katalog). Alltagsweisen laufen später über **einen** gemeinsamen datengetriebenen Pfad, der Grundton, Wirkart und Wirkung aus dem Katalog liest.
- Ziele reagieren über ein Interface `IEwigfelsWeiseTarget` (`OnWeise(Grundton, Wirkart, Wirkung, WeiseId)`). Ein Stuhl, der auf *Stuhltanz* reagiert, implementiert nur diese eine Funktion.
- Katalog-Dateien liegen in `Content/Data/Spells/Katalog/` und werden vom Kampf-Loader nicht direkt geladen (Absprache mit dem Phase-3-Thread).
- Neue Weise = Eintrag in der passenden JSON-Datei, dann `python Scripts/Tools/validate_spells.py --write-doc` (prüft IDs, Akkorde, Lehrer, Akt, Baukasten-Raster und schreibt den Katalog neu).

### Datenfelder

| Feld | Kern | Alltag | Koop | Bedeutung |
|---|---|---|---|---|
| `id` | K01–K60 | A001–A358 | D/T/V + Nr. | feste ID, auch für Flags `Weise_<ID>` |
| `kind` | kern | alltag | koop | |
| `name{de,en}`, `description{de}` | ✓ | ✓ | ✓ | englische Beschreibungen folgen mit den String Tables |
| `school`, `chord[3]`, `legacy_name` | ✓ | | | Schule, Akkord aus Grundtönen, alter Name aus GDD v0.1 |
| `heft`, `grundton`, `wirkart`, `wirkung` | | ✓ | | Baukasten |
| `unlock` | ✓ | ✓ | ✓ | ab Akt 1–5 |
| `cost`, `cooldown`, `hands`, `delivery_hint` | ✓ | cost | | Richtwerte, endgültig im Kampf-Format |
| `learn{how, who}` | ✓ | ✓ | ✓ | Lernweg + ID aus `Lehrer.json` |
| `combo{de}` | ✓ | | | Kombinationen |
| `koop{min_players, bonus, solo_partner}` | | | ✓ | |
| `anim{gesture, source}` | ✓ | | ✓ | Beschreibung der Geste, MC = Motion Capture |
